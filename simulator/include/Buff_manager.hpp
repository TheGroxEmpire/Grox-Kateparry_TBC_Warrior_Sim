#ifndef WOW_SIMULATOR_BUFF_MANAGER_HPP
#define WOW_SIMULATOR_BUFF_MANAGER_HPP

#include "Attributes.hpp"
#include "Item.hpp"
#include "Rage_manager.hpp"
#include "Use_effects.hpp"
#include "damage_sources.hpp"
#include "logger.hpp"
#include "sim_state.hpp"
#include "time_keeper.hpp"

#include <unordered_map>

struct Over_time_buff
{
    static constexpr int inactive = -1;

    Over_time_buff(const Over_time_effect& effect, int current_time) :
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

    int interval;

    int next_tick;
    int next_fade;

    // statistics
    int64_t uptime;
    int last_gain;
};

struct Combat_buff
{
    Combat_buff(const Hit_effect& hit_effect, const Special_stats& multipliers, int current_time) :
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

    int next_fade;
    int charges; // alternative way of removing buffs

    // statistics
    int64_t uptime;
    int last_gain;
};

struct Hit_aura
{
    static constexpr int inactive = -1;

    Hit_aura(std::string name, int current_time, int duration) :
        name(std::move(name)),
        next_fade(current_time + duration),
        hit_effect_mh(nullptr),
        hit_effect_oh(nullptr) { }

    std::string name;
    int next_fade;

    Hit_effect* hit_effect_mh; // disabled on next_fade
    Hit_effect* hit_effect_oh;
};

class Buff_manager
{
public:
    void initialize(std::vector<Hit_effect>& hit_effects_mh_input, std::vector<Hit_effect>& hit_effects_oh_input,
                    Use_effects::Schedule& use_effects_schedule_input, Rage_manager* rage_manager_input);

    void reset(Sim_state& state);

    void update_aura_uptimes(int current_time);
    [[nodiscard]] std::unordered_map<std::string, double> get_aura_uptimes_map() const;

    [[nodiscard]] int next_event(int current_time) const
    {
        auto next_event = std::numeric_limits<int>::max();
        if (min_combat_buff > current_time && min_combat_buff < next_event) next_event = min_combat_buff;
        if (min_over_time_buff > current_time && min_over_time_buff < next_event) next_event = min_over_time_buff;
        if (min_hit_aura > current_time && min_hit_aura < next_event) next_event = min_hit_aura;
        if (min_use_effect > current_time && min_use_effect < next_event) next_event = min_use_effect;
        return next_event;
    }

    void increment(Time_keeper& time_keeper, Logger& logger);

    void remove_charge(const Hit_effect& hit_effect, int current_time, Logger& logger);

    void start_cooldown(const Hit_effect& hit_effect, int current_time) const
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

    void add_combat_buff(Hit_effect& hit_effect, int current_time);
    void add_hit_aura(const std::string& name, Hit_effect& hit_effect, int duration, int current_time);
    void add_over_time_buff(Over_time_effect& over_time_effect, int current_time);

    bool need_to_recompute_hit_tables{};
    bool need_to_recompute_mitigation{};

private:
    void increment_combat_buffs(int current_time, Logger& logger);
    void increment_over_time_buffs(int current_time, Logger& logger);
    void increment_hit_auras(int current_time, Logger& logger);
    void increment_use_effects(int current_time, Time_keeper& time_keeper, Logger& logger);

    void do_fade_buff(Combat_buff& buff, Logger& logger);

    void gain_stats(const Special_stats& ssb);
    void do_add_combat_buff(Hit_effect& hit_effect, int current_time);
    void do_add_over_time_buff(const Over_time_effect& over_time_effect, int current_time);

    Sim_state* sim_state{};
    Rage_manager* rage_manager{};

    size_t use_effect_index{};
    int min_use_effect{std::numeric_limits<int>::max()};

    std::vector<Combat_buff> combat_buffs{};
    int min_combat_buff{std::numeric_limits<int>::max()};

    std::vector<Over_time_buff> over_time_buffs{};
    int min_over_time_buff{std::numeric_limits<int>::max()};

    std::vector<Hit_aura> hit_auras{};
    int min_hit_aura{std::numeric_limits<int>::max()};

    std::vector<Hit_effect>* hit_effects_mh{};
    std::vector<Hit_effect>* hit_effects_oh{};
    Use_effects::Schedule use_effects_schedule{};
};

#endif // WOW_SIMULATOR_BUFF_MANAGER_HPP
