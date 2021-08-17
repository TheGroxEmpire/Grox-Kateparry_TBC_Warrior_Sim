#ifndef WOW_SIMULATOR_USE_EFFECTS_H
#define WOW_SIMULATOR_USE_EFFECTS_H

#include "include/Character.hpp"

namespace Use_effects
{
typedef std::reference_wrapper<Use_effect> Use_effect_ref;

typedef std::vector<std::pair<double, Use_effect_ref>> Schedule;



double is_time_available(const Schedule& schedule, double check_time,
                         double duration);

double get_next_available_time(const Schedule& schedule, double check_time,
                               double duration);

Schedule compute_schedule(std::vector<Use_effect>& use_effects, const Special_stats& special_stats, double sim_time,
                          double ap, int number_of_targets, double extra_target_duration);

std::vector<Use_effect_ref> sort_use_effects_by_power_ascending(std::vector<Use_effect_ref>& shared_effects,
                                                                const Special_stats& special_stats, double total_ap);

double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                    double sim_time);

} // namespace Use_effects

#endif // WOW_SIMULATOR_USE_EFFECTS_H
