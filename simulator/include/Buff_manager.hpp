#ifndef WOW_SIMULATOR_BUFF_MANAGER_HPP
#define WOW_SIMULATOR_BUFF_MANAGER_HPP

#include "Attributes.hpp"
#include "Rage_manager.hpp"
#include "Item.hpp"

#include "time_keeper.hpp"
#include "damage_sources.hpp"
#include "logger.hpp"

#include <unordered_map>

struct Over_time_buff
{
    static constexpr double inactive = -1;

    Over_time_buff(const Over_time_effect& effect, double current_time) :
        name(effect.name),
        rage_gain(effect.rage_gain),
        damage(effect.damage),
        special_stats(effect.special_stats),
        interval(effect.interval),
        next_tick(current_time + effect.interval),
        next_fade(current_time + effect.duration),
        uptime(0),
        last_gain(current_time) {}

    std::string name;

    double rage_gain;
    double damage;
    Special_stats special_stats;

    double interval;

    double next_tick;
    double next_fade;

    // statistics
    double uptime;
    double last_gain;
};

struct Combat_buff
{
    Combat_buff(const Hit_effect& hit_effect, const Special_stats& multipliers, double current_time) :
        name(hit_effect.name),
        special_stats_boost(hit_effect.to_special_stats(multipliers)),
        stacks(1),
        next_fade(current_time + hit_effect.duration),
        charges(hit_effect.max_charges),
        uptime(0),
        last_gain(current_time) {}

    const std::string name;
    const Special_stats special_stats_boost;
    int stacks;

    double next_fade;
    int charges; // alternative way of removing buffs

    // statistics
    double uptime;
    double last_gain;
};

struct Hit_aura
{
    static constexpr double inactive = -1;

    Hit_aura(std::string name, double current_time, double duration) :
        name(std::move(name)),
        next_fade(current_time + duration),
        hit_effect_mh(nullptr),
        hit_effect_oh(nullptr) { }

    std::string name;
    double next_fade;

    Hit_effect* hit_effect_mh; // disabled on next_fade
    Hit_effect* hit_effect_oh;
};

class Buff_manager
{
public:
    void initialize(std::vector<Hit_effect>& hit_effects_mh_input, std::vector<Hit_effect>& hit_effects_oh_input,
                    std::vector<std::pair<double, Use_effect>>& use_effects_input, Rage_manager* rage_manager_input)
    {
        hit_effects_mh = &hit_effects_mh_input;
        hit_effects_oh = &hit_effects_oh_input;

        hit_auras.clear();

        use_effects = use_effects_input;

        rage_manager = rage_manager_input;
    }

    void reset(Special_stats& special_stats, Damage_sources& damage_sources)
    {
        simulation_special_stats = &special_stats;
        simulation_damage_sources = &damage_sources;

        for (auto& he : *hit_effects_mh)
        {
            he.time_counter = 0;
        }

        for (auto& he : *hit_effects_oh)
        {
            he.time_counter = 0;
        }

        for (auto& buff : combat_buffs)
        {
            buff.stacks = 0;
            buff.next_fade = std::numeric_limits<double>::max();
        }
        min_combat_buff = std::numeric_limits<double>::max();

        for (auto& buff : over_time_buffs)
        {
            buff.next_tick = Over_time_buff::inactive;
        }
        min_over_time_buff = std::numeric_limits<double>::max();

        for (auto& hit_aura : hit_auras)
        {
            hit_aura.next_fade = Hit_aura::inactive;
            hit_aura.hit_effect_mh->time_counter = std::numeric_limits<double>::max();
            hit_aura.hit_effect_oh->time_counter = std::numeric_limits<double>::max();
        }
        min_hit_aura = std::numeric_limits<double>::max();

        use_effect_index = 0;
        min_use_effect = use_effects.empty() ? std::numeric_limits<double>::max() : use_effects[0].first - 1;

        need_to_recompute_mitigation = true;
        need_to_recompute_hit_tables = true;
    }

    void reset_statistics()
    {
        for (auto& he : *hit_effects_mh)
        {
            he.procs = 0;
        }

        for (auto& he : *hit_effects_oh)
        {
            he.procs = 0;
        }

        for (auto& buff : combat_buffs)
        {
            buff.uptime = 0;
        }

        for (auto& buff : over_time_buffs)
        {
            buff.uptime = 0;
        }
    }

    void update_aura_uptimes(double current_time) {
        for (auto& buff : combat_buffs)
        {
            if (buff.stacks > 0) buff.uptime += current_time - (buff.last_gain > 0 ? buff.last_gain : 0);
        }
        for (auto& buff : over_time_buffs)
        {
            if (buff.next_fade > current_time) buff.uptime += current_time - (buff.last_gain > 0 ? buff.last_gain : 0);
        }
    }

    [[nodiscard]] double next_event(double current_time) const
    {
        auto next_event = std::numeric_limits<double>::max();
        if (min_combat_buff >= current_time && min_combat_buff < next_event) next_event = min_combat_buff;
        if (min_over_time_buff >= current_time && min_over_time_buff < next_event) next_event = min_over_time_buff;
        if (min_hit_aura >= current_time && min_hit_aura < next_event) next_event = min_hit_aura;
        if (min_use_effect >= current_time && min_use_effect < next_event) next_event = min_use_effect;
        return next_event;
    }

    void increment(Time_keeper& time_keeper, Logger& logger)
    {
        auto current_time = time_keeper.time;
        increment_combat_buffs(current_time, logger);
        increment_over_time_buffs(current_time, logger);
        increment_hit_auras(current_time, logger);
        increment_use_effects(current_time, time_keeper, logger);
    }

    void increment_combat_buffs(double current_time, Logger& logger)
    {
        if (min_combat_buff >= current_time)
        {
            return;
        }

        min_combat_buff = std::numeric_limits<double>::max();
        for (auto& buff : combat_buffs)
        {
            if (buff.stacks == 0) // inactive
            {
                continue;
            }

            if (buff.next_fade >= current_time) // not ready
            {
                if (buff.next_fade < min_combat_buff) min_combat_buff = buff.next_fade;
                continue;
            }

            assert(current_time - buff.next_fade < 1.01e-5);

            do_fade_buff(buff, logger);
        }
    }

    void remove_charge(const Hit_effect& hit_effect, double current_time, Logger& logger)
    {
        if (hit_effect.combat_buff_idx == -1)
        {
            // it's required that hit_effect can be successfully registered (i.e. combat_buff_idx is eventually set)
            return; // not up
        }

        auto& buff = combat_buffs[hit_effect.combat_buff_idx];
        if (buff.stacks == 0) return;
        buff.charges -= 1;
        if (buff.charges > 0) return;
        buff.next_fade = current_time; // for correct uptime bookkeeping
        do_fade_buff(buff, logger);
    }

    void do_fade_buff(Combat_buff& buff, Logger& logger)
    {
        const auto& ssb = buff.special_stats_boost;
        for (int i = 0; i < buff.stacks; i++)
        {
            (*simulation_special_stats) -= ssb;
        }
        buff.stacks = 0;
        buff.charges = 0;
        need_to_recompute_hit_tables |= (ssb.critical_strike > 0 || ssb.hit > 0 || ssb.expertise > 0);
        need_to_recompute_mitigation |= (ssb.gear_armor_pen > 0);

        // special case, should be removed
        if (buff.name == "battle_stance")
        {
            rage_manager->swap_stance();
        }

        buff.uptime += buff.next_fade - (buff.last_gain > 0 ? buff.last_gain : 0);

        logger.print(buff.name, " fades.");
    }

    void increment_over_time_buffs(double current_time, Logger& logger)
    {
        if (min_over_time_buff >= current_time)
        {
            return;
        }

        min_over_time_buff = std::numeric_limits<double>::max();
        for (auto& buff : over_time_buffs)
        {
            if (buff.next_tick == Over_time_buff::inactive) // inactive
            {
                continue;
            }

            if (buff.next_tick >= current_time) // not ready
            {
                if (buff.next_tick < min_over_time_buff) min_over_time_buff = buff.next_tick;
                continue;
            }

            assert(current_time - (buff.next_tick > 0 ? buff.next_tick : 0) < 1.01e-5);

            // this used to support everything at once, but no over_time_buff actually granted rage, dealt damage, and added stats.
            //  nothing does the latter, afaik
            if (buff.rage_gain > 0)
            {
                rage_manager->gain_rage(buff.rage_gain);
                logger.print("Over time effect: ", buff.name, " tick. Current rage: ", int(rage_manager->get_rage()));
            }
            else if (buff.damage > 0)
            {
                simulation_damage_sources->add_damage(Damage_source::deep_wounds, buff.damage, current_time);
                logger.print("Over time effect: ", buff.name, " tick. Damage: ", int(buff.damage));
            }
            else
            {
                (*simulation_special_stats) += buff.special_stats;
            }

            if (buff.next_fade < current_time)
            {
                buff.next_tick = Over_time_buff::inactive;
                buff.uptime += buff.next_fade - (buff.last_gain > 0 ? buff.last_gain : 0);
                logger.print("Over time effect: ", buff.name, " fades.");
            }
            else
            {
                buff.next_tick += buff.interval;
                if (buff.next_tick < min_over_time_buff) min_over_time_buff = buff.next_tick;
            }
        }
    }

    void increment_hit_auras(double current_time, Logger& logger)
    {
        if (min_hit_aura >= current_time)
        {
            return;
        }

        min_hit_aura = std::numeric_limits<double>::max();
        for (auto& hit_aura : hit_auras)
        {
            if (hit_aura.next_fade == Hit_aura::inactive)
            {
                continue;
            }

            if (hit_aura.next_fade >= current_time)
            {
                if (hit_aura.next_fade < min_hit_aura) min_hit_aura = hit_aura.next_fade;
                continue;
            }

            assert(current_time - hit_aura.next_fade < 1.01e-5);

            assert(hit_aura.hit_effect_mh != nullptr);
            assert(hit_aura.hit_effect_oh != nullptr);

            // or have a specialized add_combat_buff() here, probably
            assert(hit_aura.hit_effect_mh->combat_buff_idx >= 0);
            assert(hit_aura.hit_effect_oh->combat_buff_idx == -1 || hit_aura.hit_effect_oh->combat_buff_idx == hit_aura.hit_effect_mh->combat_buff_idx);

            hit_aura.hit_effect_mh->time_counter = std::numeric_limits<double>::max(); // disable hit_effects, effectively
            hit_aura.hit_effect_oh->time_counter = std::numeric_limits<double>::max();

            auto& buff = combat_buffs[hit_aura.hit_effect_mh->combat_buff_idx];
            buff.next_fade = hit_aura.next_fade; // for correct uptime bookkeeping
            do_fade_buff(buff, logger);

            hit_aura.next_fade = Hit_aura::inactive;
        }
    }

    void increment_use_effects(double current_time, Time_keeper& time_keeper, Logger& logger)
    {
        if (min_use_effect >= current_time)
        {
            return;
        }

        auto& use_effect = use_effects[use_effect_index].second;

        if (use_effect.triggers_gcd && !time_keeper.global_ready())
        {
            min_use_effect = current_time + time_keeper.global_cd();
            return;
        }

        if (use_effect.rage_boost > 0 && rage_manager->get_rage() + use_effect.rage_boost > 100)
        {
            min_use_effect = current_time + 0.5;
            return;
        }

        if (use_effect.rage_boost < 0 && rage_manager->get_rage() + use_effect.rage_boost < 0)
        {
            min_use_effect = current_time + 0.5;
            return;
        }

        if (!use_effect.hit_effects.empty())
        {
            add_hit_aura(use_effect.name, use_effect.hit_effects[0], use_effect.hit_effects[0].duration, current_time);
        }
        else if (!use_effect.over_time_effects.empty())
        {
            add_over_time_buff(use_effect.over_time_effects[0], current_time);
        }
        else
        {
            auto hit_effect = Hit_effect();
            hit_effect.name = use_effect.name;
            hit_effect.attribute_boost = use_effect.attribute_boost;
            hit_effect.special_stats_boost = use_effect.special_stats_boost;
            hit_effect.duration = use_effect.duration;
            add_combat_buff(hit_effect, current_time);
        }

        if (use_effect.rage_boost > 0)
        {
            rage_manager->gain_rage(use_effect.rage_boost);
            logger.print("Current rage: ", int(rage_manager->get_rage()));
        }

        if (use_effect.rage_boost < 0)
        {
            rage_manager->spend_rage(-use_effect.rage_boost);
            logger.print("Current rage: ", int(rage_manager->get_rage()));
        }

        if (use_effect.triggers_gcd)
        {
            time_keeper.global_cast(1.5);
        }

        use_effect_index += 1;

        if (use_effect_index < use_effects.size())
        {
            min_use_effect = use_effects[use_effect_index].first;

            const auto& ue = use_effects[use_effect_index];
            if (ue.second.triggers_gcd || ue.second.rage_boost != 0) min_use_effect -= 1;
        }
        else
        {
            min_use_effect = std::numeric_limits<double>::max();
        }
    }

    void start_cooldown(const Hit_effect& hit_effect, double current_time) const
    {
        if (hit_effect.cooldown == 0) return;

        for (auto& hit_effect_mh : *hit_effects_mh)
        {
            if (hit_effect_mh.name == hit_effect.name)
            {
                hit_effect_mh.time_counter = current_time + hit_effect.cooldown;
                break;
            }
        }
        for (auto& hit_effect_oh : *hit_effects_oh)
        {
            if (hit_effect_oh.name == hit_effect.name)
            {
                hit_effect_oh.time_counter = current_time + hit_effect.cooldown;
                break;
            }
        }
    }

    void gain_stats(const Special_stats& ssb)
    {
        (*simulation_special_stats) += ssb;
        need_to_recompute_hit_tables |= (ssb.hit > 0 || ssb.critical_strike > 0 || ssb.expertise > 0);
        need_to_recompute_mitigation |= (ssb.gear_armor_pen > 0);
    }

    // Hit_effect's name is used as a Combat_buff name here, so each "stat boost" hit effect
    //  has a corresponding buff.
    // This allows to setup a hit_effect -> buff connection initially, which allows to cut
    //  short buff iteration.
    // however, min_combat_buff might not be updated correctly, so increment_combat_buff()
    //  does more work

    // instead of using hit_effects per weapon, and "sharing" them via combat_buff name,
    //  hit_effects could also be registered "globally" (together with their respective combat_buff,
    //  so each hit_effect would have a one-to-one connection to the corresponding buff),
    //  and would simply apply with different percentages

    // if stacks work correctly (i.e. stacks == 0 || stacks < max_stacks determines whether
    //  it's a gain or a refresh), next_fade could be set to MAX_DOUBLE on buff fade
    void add_combat_buff(Hit_effect& hit_effect, double current_time)
    {
        assert(hit_effect.max_stacks >= 1);
        assert(hit_effect.max_charges >= 1);

        // "registration", essentially - once per hit_effect, connects each hit_effect w/ a combat buff
        if (hit_effect.combat_buff_idx == -1)
        {
            for (size_t i = 0; i < combat_buffs.size(); ++i)
            {
                if (combat_buffs[i].name == hit_effect.name)
                {
                    hit_effect.combat_buff_idx = static_cast<int>(i);
                    return do_add_combat_buff(hit_effect, current_time);
                }
            }

            auto& buff = combat_buffs.emplace_back(hit_effect, *simulation_special_stats, current_time);
            gain_stats(buff.special_stats_boost);
            if (buff.next_fade < min_combat_buff) min_combat_buff = buff.next_fade;
            hit_effect.combat_buff_idx = static_cast<int>(combat_buffs.size()) - 1;
            return;
        }

        do_add_combat_buff(hit_effect, current_time);
    }

    void do_add_combat_buff(Hit_effect& hit_effect, double current_time)
    {
        auto& buff = combat_buffs[hit_effect.combat_buff_idx];
        if (buff.next_fade < current_time || buff.stacks < hit_effect.max_stacks)
        {
            if (buff.next_fade < current_time) assert(buff.stacks == 0 && buff.charges == 0);
            if (buff.stacks == 0) buff.last_gain = current_time;
            gain_stats(buff.special_stats_boost);
            buff.stacks += 1;
        }
        buff.next_fade = current_time + hit_effect.duration; // or keep unchanged for "temporary hit effects"
        buff.charges = hit_effect.max_charges;
        if (buff.next_fade < min_combat_buff) min_combat_buff = buff.next_fade;
    }

    void add_hit_aura(const std::string& name, Hit_effect& hit_effect, double duration_left, double current_time)
    {
        for (auto& hit_aura : hit_auras)
        {
            if (hit_aura.name == name)
            {
                hit_aura.hit_effect_mh->time_counter = 0; // re-enable hit_effects, and queue fade
                hit_aura.hit_effect_oh->time_counter = 0;
                hit_aura.next_fade = current_time + duration_left;
                if (hit_aura.next_fade < min_hit_aura) min_hit_aura = hit_aura.next_fade;
                return;
            }
        }

        auto& hit_aura = hit_auras.emplace_back(Hit_aura(name, current_time, duration_left));
        hit_effect.sanitize();
        hit_aura.hit_effect_mh = &hit_effects_mh->emplace_back(hit_effect);
        hit_aura.hit_effect_oh = &hit_effects_oh->emplace_back(hit_effect);
        if (hit_aura.next_fade < min_hit_aura) min_hit_aura = hit_aura.next_fade;
    }

    void add_over_time_buff(Over_time_effect& over_time_effect, double current_time)
    {
        if (over_time_effect.over_time_buff_idx == -1)
        {
            for (size_t i = 0; i < over_time_buffs.size(); ++i)
            {
                if (over_time_buffs[i].name == over_time_effect.name)
                {
                    over_time_effect.over_time_buff_idx = static_cast<int>(i);
                    return do_add_over_time_buff(over_time_effect, current_time);
                }
            }

            auto& buff = over_time_buffs.emplace_back(Over_time_buff(over_time_effect, current_time));
            if (buff.next_tick < min_over_time_buff) min_over_time_buff = buff.next_tick;
            over_time_effect.over_time_buff_idx = static_cast<int>(over_time_buffs.size()) - 1;
            return;
        }

        do_add_over_time_buff(over_time_effect, current_time);
    }

    void do_add_over_time_buff(const Over_time_effect& over_time_effect, double current_time)
    {
        auto& buff = over_time_buffs[over_time_effect.over_time_buff_idx];

        if (buff.next_tick == Over_time_buff::inactive) {
            buff.last_gain = current_time;
        }

        buff.damage = over_time_effect.damage; // nothing else ever changes

        buff.next_tick = current_time + over_time_effect.interval;
        buff.next_fade = current_time + over_time_effect.duration;
        if (buff.next_tick < min_over_time_buff) min_over_time_buff = buff.next_tick;
    }

    [[nodiscard]] std::unordered_map<std::string, double> get_aura_uptimes_map() const
    {
        auto m = std::unordered_map<std::string, double>();
        for (const auto& buff : combat_buffs)
        {
            m[buff.name] = buff.uptime;
        }
        for (const auto& buff : over_time_buffs)
        {
            m[buff.name] = buff.uptime;
        }
        return m;
    }


    bool need_to_recompute_hit_tables{};
    bool need_to_recompute_mitigation{};
    Special_stats* simulation_special_stats{};
    Damage_sources* simulation_damage_sources{};
    Rage_manager* rage_manager{};

    size_t use_effect_index{};
    double min_use_effect{std::numeric_limits<double>::max()};

    std::vector<Combat_buff> combat_buffs{};
    double min_combat_buff{std::numeric_limits<double>::max()};

    std::vector<Over_time_buff> over_time_buffs{};
    double min_over_time_buff{std::numeric_limits<double>::max()};

    std::vector<Hit_aura> hit_auras{};
    double min_hit_aura{std::numeric_limits<double>::max()};

    std::vector<Hit_effect>* hit_effects_mh{};
    std::vector<Hit_effect>* hit_effects_oh{};
    std::vector<std::pair<double, Use_effect>> use_effects{};
};

#endif // WOW_SIMULATOR_BUFF_MANAGER_HPP
