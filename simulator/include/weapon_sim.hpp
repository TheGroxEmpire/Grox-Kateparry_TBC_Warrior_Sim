#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "Item.hpp"

class Weapon_sim
{
public:
    explicit Weapon_sim(const Weapon& weapon);

    // bonus_damage has been removed here because:
    //  - it doesn't apply to all swing-damage based attacks (namely: heroic strike, cleave, slam, and devastate)
    //  - it's applied /after/ the 50% offhand penalty
    //  - there's the deep wounds absurdity

    [[nodiscard]] double swing(const Special_stats& special_stats) const
    {
        return average_damage + (special_stats.attack_power + special_stats.bonus_attack_power) * swing_speed / 14;
    }

    [[nodiscard]] double normalized_swing(const Special_stats& special_stats) const
    {
        return average_damage + (special_stats.attack_power + special_stats.bonus_attack_power) * normalized_swing_speed / 14;
    }

    double swing_speed;
    double normalized_swing_speed;
    int next_swing;
    double average_damage;
    Socket socket;
    Weapon_type weapon_type;
    Weapon_socket weapon_socket;
    std::vector<Hit_effect> hit_effects;
};

#endif // WOW_SIMULATOR_WEAPON_SIM_HPP
