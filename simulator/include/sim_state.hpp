#ifndef COMBAT_SIMULATOR_SIM_STATE_HPP
#define COMBAT_SIMULATOR_SIM_STATE_HPP

#include "weapon_sim.hpp"

struct Sim_state
{
    Sim_state(Weapon_sim& main_hand_weapon, Weapon_sim& off_hand_weapon, bool is_dual_wield,
              Special_stats special_stats, Damage_sources damage_sources) :
        main_hand_weapon(main_hand_weapon),
        off_hand_weapon(off_hand_weapon),
        is_dual_wield(is_dual_wield),
        special_stats(special_stats),
        damage_sources(damage_sources),
        flurry_charges(0),
        rampage_stacks(0) {}

    Weapon_sim& main_hand_weapon;
    Weapon_sim& off_hand_weapon;
    bool is_dual_wield;
    Special_stats special_stats;
    Damage_sources damage_sources;
    int flurry_charges;
    int rampage_stacks;
};

#endif // COMBAT_SIMULATOR_SIM_STATE_HPP