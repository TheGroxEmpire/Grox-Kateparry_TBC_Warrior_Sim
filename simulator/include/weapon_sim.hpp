#ifndef WOW_SIMULATOR_WEAPON_SIM_HPP
#define WOW_SIMULATOR_WEAPON_SIM_HPP

#include "Item.hpp"

class Weapon_sim
{
public:
    explicit Weapon_sim(const Weapon& weapon);

    [[nodiscard]] double swing(const Special_stats& special_stats) const
    {
        auto damage = average_damage + (special_stats.attack_power + special_stats.bonus_attack_power) / 14 * swing_speed;
        return socket == Socket::main_hand ? damage + special_stats.bonus_damage : damage * 0.5 + special_stats.bonus_damage;
    }

    [[nodiscard]] double normalized_swing(const Special_stats& special_stats) const
    {
        auto damage = average_damage + (special_stats.attack_power + special_stats.bonus_attack_power) / 14 * normalized_swing_speed;
        return socket == Socket::main_hand ? damage + special_stats.bonus_damage : damage * 0.5 + special_stats.bonus_damage;
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
