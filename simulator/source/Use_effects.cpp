#include "Use_effects.hpp"

#include <algorithm>

namespace Use_effects
{
double is_time_available(const Schedule& schedule, double check_time,
                         double duration)
{
    for (const auto& pair : schedule)
    {
        if (check_time >= pair.first && check_time < (pair.first + pair.second.get().duration))
        {
            return pair.first + pair.second.get().duration;
        }
        if ((check_time + duration >= pair.first) && (check_time + duration) < (pair.first + pair.second.get().duration))
        {
            return pair.first + pair.second.get().duration;
        }
    }
    return check_time;
}

double get_next_available_time(const Schedule& schedule, double check_time,
                               double duration)
{
    while (true)
    {
        double next_available = is_time_available(schedule, check_time, duration);
        if (next_available == check_time)
        {
            return next_available;
        }
        check_time = next_available;
    }
}

Schedule compute_schedule(std::vector<Use_effect>& use_effects,
                          const Special_stats& special_stats, double sim_time,
                          double total_ap, int number_of_targets,
                          double extra_target_duration)
{
    Schedule schedule;
    std::vector<Use_effect_ref> shared_effects{};
    std::vector<Use_effect_ref> unique_effects{};

    for (auto& use_effect : use_effects)
    {
        if (use_effect.effect_socket == Use_effect::Effect_socket::shared)
        {
            shared_effects.emplace_back(use_effect);
        }
        else
        {
            unique_effects.emplace_back(use_effect);
        }
    }

    if (number_of_targets > 0)
    {
    }
    else
    {
        auto sorted_shared_use_effects = sort_use_effects_by_power_ascending(shared_effects, special_stats, total_ap);

        for (auto& use_effect : sorted_shared_use_effects)
        {
            double test_time = 0.0 * extra_target_duration;
            for (int i = 0; i < 10; i++)
            {
                test_time = get_next_available_time(schedule, test_time, use_effect.get().duration);
                if (test_time >= sim_time)
                {
                    break;
                }
                schedule.emplace_back(test_time, use_effect);
                test_time += use_effect.get().cooldown;
            }
        }

        for (auto& use_effect : unique_effects)
        {
            double test_time = 0.0;
            for (int i = 0; i < 10; i++)
            {
                if (test_time >= sim_time)
                {
                    break;
                }
                schedule.emplace_back(test_time, use_effect);
                test_time += use_effect.get().cooldown;
            }
        }
    }

    for (auto& use_effect : schedule)
    {
        use_effect.first = sim_time - use_effect.first - use_effect.second.get().duration;
    }

    std::sort(schedule.begin(), schedule.end(),[](const auto& a, const auto& b) {
      return a.first < b.first;
    });

    return schedule;
}

// TODO(vigo) this should use pre-defined or -calculated stat values, and consider uptime
double estimate_power(const Use_effect& use_effect, const Special_stats& special_stats,
                                           double total_ap)
{
    double use_effect_ap_boost = use_effect.to_special_stats(special_stats).attack_power;

    double use_effect_haste_boost = total_ap * use_effect.combat_buff.special_stats_boost.haste;

    double use_effect_armor_boost{};
    if (use_effect.name == "badge_of_the_swarmguard")
    {
        // TODO this should be based on armor of the boss
        use_effect_armor_boost = total_ap * 0.07;
    }
    return use_effect_ap_boost + use_effect_haste_boost + use_effect_armor_boost;
}

std::vector<Use_effect_ref> sort_use_effects_by_power_ascending(std::vector<Use_effect_ref>& shared_effects,
                                                                const Special_stats& special_stats, double total_ap)
{
    std::sort(shared_effects.begin(), shared_effects.end(), [special_stats,total_ap](const auto& a, const auto& b) {
        return estimate_power(a.get(), special_stats, total_ap) > estimate_power(b.get(), special_stats, total_ap);
    });
    return shared_effects;
}

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time)
{
    double ap_during_active = estimate_power(use_effect, special_stats, total_ap);
    return ap_during_active * std::min(use_effect.duration / sim_time, 1.0);
}

} // namespace Use_effects