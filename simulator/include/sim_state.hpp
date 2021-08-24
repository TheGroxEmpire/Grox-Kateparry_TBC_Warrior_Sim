#ifndef COMBAT_SIMULATOR_SIM_STATE_HPP
#define COMBAT_SIMULATOR_SIM_STATE_HPP

#include "weapon_sim.hpp"

struct Sim_state
{
    Sim_state(Weapon_sim& main_hand_weapon, Weapon_sim& off_hand_weapon, bool is_dual_wield,
              Special_stats special_stats,
              std::vector<Damage_instance>& damage_instances, bool log_damage_instances) :
        main_hand_weapon(main_hand_weapon),
        off_hand_weapon(off_hand_weapon),
        is_dual_wield(is_dual_wield),
        special_stats(special_stats),
        damage_sources(),
        damage_instances(damage_instances),
        log_damage_instances(log_damage_instances),
        flurry_charges(0),
        rampage_stacks(0)
    {
        if (log_damage_instances) damage_instances.clear();
    }

    Weapon_sim& main_hand_weapon;
    Weapon_sim& off_hand_weapon;
    const bool is_dual_wield;
    Special_stats special_stats;
    Damage_sources damage_sources;
    std::vector<Damage_instance>& damage_instances;
    const bool log_damage_instances;
    int flurry_charges;
    int rampage_stacks;

    void add_damage(Damage_source source, double damage, int current_time)
    {
        damage_sources.add_damage(source, damage);
        if (log_damage_instances) damage_instances.emplace_back(source, damage, current_time);
    }
};

#endif // COMBAT_SIMULATOR_SIM_STATE_HPP