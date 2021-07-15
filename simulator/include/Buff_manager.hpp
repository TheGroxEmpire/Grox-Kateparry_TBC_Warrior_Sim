#ifndef WOW_SIMULATOR_BUFF_MANAGER_HPP
#define WOW_SIMULATOR_BUFF_MANAGER_HPP

#include "Attributes.hpp"
#include "Item.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>
#include <vector>

struct Over_time_buff
{
    Over_time_buff(std::string id, Special_stats special_stats, int init_time, double rage_gain, double damage,
                   int interval, int duration)
        : id(std::move(id))
        , special_stats(special_stats)
        , init_time(init_time)
        , total_ticks(duration / interval)
        , current_ticks(1)
        , rage_gain(rage_gain)
        , damage(damage)
        , interval(interval){};

    std::string id;
    Special_stats special_stats;
    int init_time;
    int total_ticks;
    int current_ticks;
    double rage_gain;
    double damage;
    int interval;
};

struct Combat_buff
{
    Combat_buff() = default;
    ~Combat_buff() = default;

    Combat_buff(Special_stats special_stats, double duration_left, bool is_independent_stacks = false, bool is_shared_stacks = false)
        : special_stats(special_stats), duration_left(duration_left), is_independent_stacks(is_independent_stacks), is_shared_stacks(is_shared_stacks){};

    Special_stats special_stats{};
    double duration_left{};
    bool is_independent_stacks{};
    bool is_shared_stacks{};
};

struct Hit_buff
{
    Hit_buff(std::string id, double duration_left) : id(std::move(id)), duration_left(duration_left){};

    std::string id;
    double duration_left;
};

class Buff_manager
{
public:
    void initialize(Special_stats& special_stats, const std::vector<std::pair<double, Use_effect>>& use_effects_order,
                    std::vector<Hit_effect>& hit_effects_mh_input, std::vector<Hit_effect>& hit_effects_oh_input,
                    double tactical_mastery_rage, bool performance_mode_in)
    {
        performance_mode = performance_mode_in;
        stat_gains.clear();
        hit_gains.clear();
        hit_stacks.clear();
        over_time_buffs.clear();
        simulation_special_stats = &special_stats;
        hit_effects_mh = &hit_effects_mh_input;
        hit_effects_oh = &hit_effects_oh_input;
        use_effect_order = use_effects_order;
        deep_wounds_damage = 0.0;
        deep_wounds_timestamps.clear();
        need_to_recompute_hit_tables = true;
        tactical_mastery_rage_ = tactical_mastery_rage;
    };

    [[nodiscard]] double get_dt(double sim_time) const
    {
        double dt = 1e10;
        dt = std::min(dt, next_event);
        double interval_dt = std::fmod(sim_time, double(min_interval));
        if (interval_dt < 0.0)
        {
            interval_dt = std::abs(interval_dt);
        }
        else
        {
            interval_dt = double(min_interval) - interval_dt;
        }
        dt = std::min(dt, interval_dt);
        return dt;
    }

    void increment_stat_gains(double current_time, double dt, double& rage, double& rage_lost_stance, bool debug,
                              std::vector<std::string>& debug_msg)
    {
        int stacks = 1;
        for (auto it = stat_gains.begin(); it != stat_gains.end();)
        {
            const auto& name = it->first;
            auto& buff = it->second;

            if (!performance_mode && current_time > 0.0)
            {
                aura_uptime[name] += dt;
            }

            buff.duration_left -= dt;
            if (buff.duration_left < 0.0)
            {
                if (debug)
                {
                    debug_msg.emplace_back(name + " fades.");
                }
                if (name == "battle_stance")
                {
                    if (rage > tactical_mastery_rage_)
                    {
                        rage_lost_stance += rage - tactical_mastery_rage_;
                        rage = tactical_mastery_rage_;
                    }
                }

                if (buff.is_independent_stacks)
                {
                    stacks = 0;
                    for (auto& hit_effect_mh : (*hit_effects_mh))
                    {
                        if (name == hit_effect_mh.name)
                        {
                            stacks = hit_effect_mh.stacks_counter;
                            hit_effect_mh.stacks_counter = 0;
                            break;
                        }
                    }
                    if (!(stacks > 0))
                    {
                        for (auto& hit_effect_oh : (*hit_effects_oh))
                        {
                            if (name == hit_effect_oh.name)
                            {
                                stacks = hit_effect_oh.stacks_counter;
                                hit_effect_oh.stacks_counter = 0;
                                break;
                            }
                        }
                    }
                }
                else if (buff.is_shared_stacks)
                {
                    stacks = arpen_stacks_counter;
                    arpen_stacks_counter = 0;
                    need_to_recompute_mitigation = true;
                    // for (auto& hit_effects_mh : (*hit_effects_mh))
                    // {
                    //     if (it->first == hit_effects_mh.name)
                    //     {
                    //         stacks = hit_effects_mh.stacks_counter;
                    //         hit_effects_mh.stacks_counter = 0;
                    //         break;
                    //     }
                    // }
                    // for (auto& hit_effects_oh : (*hit_effects_oh))
                    // {
                    //     if (it->first == hit_effects_oh.name)
                    //     {
                    //         stacks = hit_effects_oh.stacks_counter;
                    //         hit_effects_oh.stacks_counter = 0;
                    //         break;
                    //     }
                    // }
                }

                for (int i = 0; i < stacks; i++)
                {
                    (*simulation_special_stats) -= buff.special_stats;
                }
                // if (it->second.special_stats.gear_armor_pen > 0)
                // {
                //     need_to_recompute_mitigation = true;
                // }
                if (buff.special_stats.hit > 0 || buff.special_stats.critical_strike > 0 || buff.special_stats.expertise > 0)
                {
                    need_to_recompute_hit_tables = true;
                }
                it = stat_gains.erase(it);
            }
            else
            {
                next_event = std::min(next_event, buff.duration_left);
                ++it;
            }
        }
    }

    void increment_hit_gains(double current_time, double dt, bool debug, std::vector<std::string>& debug_msg)
    {
        for (auto it = hit_gains.begin(); it != hit_gains.end();)
        {
            auto& buff = *it;
            if (!performance_mode && current_time > 0.0)
            {
                aura_uptime[buff.id] += dt;
            }
            buff.duration_left -= dt;
            if (buff.duration_left < 0.0)
            {
                if (debug)
                {
                    debug_msg.emplace_back(buff.id + " fades.");
                }

                for (auto jt = hit_effects_mh->begin(); jt != hit_effects_mh->end();)
                {
                    jt->name == buff.id ? jt = hit_effects_mh->erase(jt) : ++jt;
                }
                for (auto jt = hit_effects_oh->begin(); jt != hit_effects_oh->end();)
                {
                    jt->name == buff.id ? jt = hit_effects_oh->erase(jt) : ++jt;
                }

                reset_armor_reduction = true;
                it = hit_gains.erase(it);
            }
            else
            {
                next_event = std::min(next_event, buff.duration_left);
                ++it;
            }
        }
    }

    void increment_use_effects(double current_time, double& rage, double& global_cooldown, bool debug,
                               std::vector<std::string>& debug_msg, double ap_multiplier)
    {
        // Try to use the use effect within one second of what was planned (compensate some for GCD's)
        double margin = 0.0;
        if (current_time > 0.0)
        {
            margin = 1.0;
        }
        if (!use_effect_order.empty() && current_time > use_effect_order.back().first - margin)
        {
            auto& use_effect = use_effect_order.back().second;
            if (!use_effect.triggers_gcd || (global_cooldown < 0.0))
            {
                if (debug)
                {
                    debug_msg.emplace_back("Activating: " + use_effect.name);
                }
                if (ap_multiplier > 0)
                {
                    if (use_effect.name == "unleashed_rage")
                    {
                        use_effect.special_stats_boost.attack_power += use_effect.special_stats_boost.attack_power * (ap_multiplier - 0.1);
                    }
                    else
                    {
                        use_effect.special_stats_boost.attack_power += use_effect.special_stats_boost.attack_power * ap_multiplier;
                    }
                }
                if (!use_effect.hit_effects.empty())
                {
                    add_hit_effect(use_effect.name, use_effect.hit_effects[0], use_effect.hit_effects[0].duration);
                }
                else if (!use_effect.over_time_effects.empty())
                {
                    add_over_time_effect(use_effect.over_time_effects[0], int(current_time + 1));
                }
                else
                {
                    add(use_effect.name, use_effect.get_special_stat_equivalent(*simulation_special_stats, ap_multiplier),
                        use_effect.duration);
                }
                if (use_effect.rage_boost != 0.0)
                {
                    rage += use_effect.rage_boost;
                    rage = std::min(100.0, rage);
                    if (debug)
                    {
                        debug_msg.emplace_back("Current rage: " + std::to_string(int(rage)));
                    }
                }
                if (use_effect.triggers_gcd)
                {
                    global_cooldown = 1.5;
                }
                use_effect_order.pop_back();
            }
        }
    }

    void increment_over_time_buffs(double current_time, double& rage, bool debug, std::vector<std::string>& debug_msg)
    {
        for (auto it = over_time_buffs.begin(); it != over_time_buffs.end();)
        {
            auto& buff = *it;
            if ((int(current_time) - buff.init_time) / buff.interval >= buff.current_ticks)
            {
                rage += buff.rage_gain;
                rage = std::min(100.0, rage);
                (*simulation_special_stats) += buff.special_stats;
                buff.current_ticks++;
                if (buff.damage > 0.0)
                {
                    deep_wounds_damage += buff.damage;
                    deep_wounds_timestamps.push_back(current_time);
                }
                if (debug)
                {
                    if (buff.rage_gain > 0)
                    {
                        debug_msg.emplace_back("Over time effect: " + buff.id +
                                               " tick. Current rage: " + std::to_string(int(rage)));
                    }
                    else if (buff.damage > 0)
                    {
                        debug_msg.emplace_back("Over time effect: " + buff.id +
                                               " tick. Damage: " + std::to_string(int(buff.damage)));
                    }
                    else
                    {
                        debug_msg.emplace_back("Over time effect: " + buff.id + " tick.");
                    }
                }
            }
            if (buff.current_ticks == buff.total_ticks)
            {
                if (debug)
                {
                    debug_msg.emplace_back("Over time effect: " + buff.id + " fades.");
                }
                it = over_time_buffs.erase(it);
            }
            else
            {
                ++it;
            }
        }

        min_interval = 100000;
        for (const auto& buff : over_time_buffs)
        {
            min_interval = std::min(buff.interval, min_interval);
        }
    }

    void increment_icd(double dt) const
    {
        for (auto& hit_effect_mh : *hit_effects_mh)
        {
            hit_effect_mh.time_counter -= dt;
        }
        for (auto& hit_effect_oh : *hit_effects_oh)
        {
            hit_effect_oh.time_counter -= dt;
        }
    }

    void reset_icd(const Hit_effect& hit_effect) const
    {
        for (auto& hit_effect_mh : *hit_effects_mh)
        {
            if (hit_effect_mh.name == hit_effect.name)
            {
                hit_effect_mh.time_counter = hit_effect.cooldown;
                break;
            }
        }
        for (auto& hit_effect_oh : *hit_effects_oh)
        {
            if (hit_effect_oh.name == hit_effect.name)
            {
                hit_effect_oh.time_counter = hit_effect.cooldown;
                break;
            }
        }
    }

    void modify_arpen_stacks(Hit_effect& hit_effect, Socket weapon_hand, bool at_max_stacks = false)
    {
        Special_stats no_reduction = {};
        Special_stats armor_reduction = {};
        armor_reduction.gear_armor_pen = hit_effect.armor_reduction;

        need_to_recompute_mitigation = true;
        if (weapon_hand == Socket::main_hand)
        {
            for (auto& hit_effect_mh : *hit_effects_mh)
            {
                if (hit_effect_mh.name == hit_effect.name)
                {
                    add(hit_effect_mh.name, at_max_stacks ? no_reduction : armor_reduction, hit_effect_mh.duration, false, true);
                    break;
                }
            }
        }
        else
        {
            for (auto& hit_effect_oh : *hit_effects_oh)
            {
                if (hit_effect_oh.name == hit_effect.name)
                {
                    armor_reduction.gear_armor_pen = hit_effect.armor_reduction;
                    add(hit_effect_oh.name, at_max_stacks ? no_reduction : armor_reduction, hit_effect_oh.duration, false, true);
                    break;
                }
            }
        }
    }

    void increment(double dt, double current_time, double& rage, double& rage_lost_stance, double& global_cooldown,
                   bool debug, std::vector<std::string>& debug_msg, double ap_multiplier)
    {
        next_event = 100000;
        increment_icd(dt);
        increment_stat_gains(current_time, dt, rage, rage_lost_stance, debug, debug_msg);
        increment_hit_gains(current_time, dt, debug, debug_msg);
        // increment_hit_stacks(current_time, dt, debug, debug_msg);
        increment_use_effects(current_time, rage, global_cooldown, debug, debug_msg, ap_multiplier);
        increment_over_time_buffs(current_time, rage, debug, debug_msg);
    }

    void add(const std::string& name, const Special_stats& special_stats, double duration_left, bool is_independent_stacks = false, bool is_shared_stacks = false)
    {
        // Insert the buff in stat gains vector. If it exists increase the duration of the buff
        auto kv_pair = stat_gains.insert({name, {special_stats, duration_left, is_independent_stacks, is_shared_stacks}});
        if (!kv_pair.second)
        {
            kv_pair.first->second.duration_left = duration_left;
            if (!is_independent_stacks && !is_shared_stacks)
            {
                return;
            }
        }
        (*simulation_special_stats) += special_stats;
        if (special_stats.hit > 0 || special_stats.critical_strike > 0 || special_stats.expertise > 0)
        {
            need_to_recompute_hit_tables = true;
        }
        // if (special_stats.gear_armor_pen > 0)
        // {
        //     need_to_recompute_mitigation = true;
        // }
        if (duration_left < next_event)
        {
            next_event = duration_left;
        }
    }

    void add_hit_effect(const std::string& name, const Hit_effect& hit_effect, double duration_left)
    {
        (*hit_effects_mh).emplace_back(hit_effect);
        (*hit_effects_oh).emplace_back(hit_effect);
        hit_gains.emplace_back(name, duration_left);
    }

    void add_over_time_effect(const Over_time_effect& over_time_effect, int init_time)
    {
        if (over_time_effect.name == "Deep_wounds")
        {
            for (auto& over_time_buff : over_time_buffs)
            {
                if (over_time_buff.id == "Deep_wounds")
                {
                    over_time_buff.damage = over_time_effect.damage;
                    over_time_buff.total_ticks =
                        (over_time_effect.duration + init_time - over_time_buff.init_time) / over_time_buff.interval;
                    return;
                }
            }
        }
        min_interval = std::min(over_time_effect.interval, min_interval);
        over_time_buffs.emplace_back(over_time_effect.name, over_time_effect.special_stats, init_time,
                                     over_time_effect.rage_gain, over_time_effect.damage, over_time_effect.interval,
                                     over_time_effect.duration);
    }

    bool can_do_overpower()
    {
        auto it = stat_gains.find("overpower_aura");
        return it != stat_gains.end();
    }

    bool need_to_recompute_hit_tables{false};
    bool reset_armor_reduction{false};
    bool need_to_recompute_mitigation{false};
    bool performance_mode{false};
    Special_stats* simulation_special_stats;
    std::map<std::string, Combat_buff> stat_gains;
    std::vector<Hit_buff> hit_gains;
    std::vector<Hit_buff> hit_stacks;
    std::vector<Over_time_buff> over_time_buffs;
    std::vector<Hit_effect>* hit_effects_mh;
    std::vector<Hit_effect>* hit_effects_oh;
    std::vector<std::pair<double, Use_effect>> use_effect_order;
    double next_event = 100000;
    int min_interval = 100000;
    std::map<std::string, double> aura_uptime;
    double deep_wounds_damage{};
    double tactical_mastery_rage_{};
    std::vector<double> deep_wounds_timestamps{};
    int arpen_stacks_counter{0};
};

#endif // WOW_SIMULATOR_BUFF_MANAGER_HPP
