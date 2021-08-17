#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "include/Item.hpp"

class Weapon_sim
{
public:
    explicit Weapon_sim(const Weapon& weapon);

    [[nodiscard]] double swing(const Special_stats& special_stats) const
    {
        return average_damage + special_stats.bonus_damage + special_stats.attack_power * swing_speed / 14;
    }

    [[nodiscard]] double normalized_swing(const Special_stats& special_stats) const
    {
        return average_damage + special_stats.bonus_damage + special_stats.attack_power * normalized_swing_speed / 14;
    }

    double swing_speed;
    double normalized_swing_speed;
    double next_swing;
    double average_damage;
    Socket socket;
    Weapon_type weapon_type;
    Weapon_socket weapon_socket;
    std::vector<Hit_effect> hit_effects;
    std::string socket_name;
};

#endif // WOW_SIMULATOR_WEAPON_SIM_HPP
