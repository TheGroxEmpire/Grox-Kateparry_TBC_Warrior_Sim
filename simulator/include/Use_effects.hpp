#ifndef WOW_SIMULATOR_USE_EFFECTS_H
#define WOW_SIMULATOR_USE_EFFECTS_H

#include "Item.hpp"

class Use_effects
{
public:
    typedef std::reference_wrapper<Use_effect> Use_effect_ref;
    typedef std::vector<std::pair<int, Use_effect_ref>> Schedule;

    static Schedule compute_schedule(std::vector<Use_effect>& use_effects, const Special_stats& special_stats,
                                     int sim_time, double ap);

    static double get_use_effect_ap_equivalent(const Use_effect& use_effect, const Special_stats& special_stats, double total_ap,
                                               int sim_time);

    static int is_time_available(const Schedule& schedule, int check_time, int duration);

    static int get_next_available_time(const Schedule& schedule, int check_time, int duration);

private:
    static std::vector<Use_effect_ref> sort_use_effects_by_power_ascending(std::vector<Use_effect_ref>& shared_effects,
                                                                const Special_stats& special_stats, double total_ap);
};

#endif // WOW_SIMULATOR_USE_EFFECTS_H
