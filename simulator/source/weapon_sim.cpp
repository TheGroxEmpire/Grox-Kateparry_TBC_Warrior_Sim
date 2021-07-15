#include "weapon_sim.hpp"

Weapon_sim::Weapon_sim(const Weapon& weapon, const Special_stats& special_stats)
        : swing_speed(weapon.swing_speed)
        , internal_swing_timer(0.0)
        , min_damage(weapon.min_damage + special_stats.bonus_damage)
        , max_damage(weapon.max_damage + special_stats.bonus_damage)
        , average_damage(0.5 * (weapon.min_damage + weapon.max_damage) + special_stats.bonus_damage)
        , socket(weapon.socket)
        , weapon_type(weapon.type)
        , weapon_socket(weapon.weapon_socket)
        , hit_effects(weapon.hit_effects)
{
    socket_name = (socket == Socket::main_hand) ? "main_hand" : "off_hand";

    if (weapon_socket == Weapon_socket::two_hand)
    {
        normalized_swing_speed = 3.3;
    }
    else if (weapon_type == Weapon_type::dagger)
    {
        normalized_swing_speed = 1.7;
    }
    else
    {
        normalized_swing_speed = 2.4;
    }
}
