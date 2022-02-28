#include "Item_optimizer.hpp"

#include "item_heuristics.hpp"
#include "string_helpers.hpp"

struct Weapon_struct
{
    Weapon_struct(const Weapon& w, const Special_stats& ss) :
        weapon(w),
        special_stats(w.special_stats + w.attributes.to_special_stats(ss)),
        swing_speed(w.swing_speed),
        average_damage((w.min_damage + w.max_damage) / 2),
        can_be_estimated(w.set_name == Set::none && w.hit_effects.empty() && w.use_effects.empty()) {}

    bool operator==(const Weapon_struct& other) { return &weapon.get() == &other.weapon.get(); }
    [[nodiscard]] const std::string& name() const { return weapon.get().name; }
    [[nodiscard]] Weapon_type type() const { return weapon.get().type; }

    const std::reference_wrapper<const Weapon> weapon;
    const Special_stats special_stats;
    const double swing_speed;
    const double average_damage;
    const bool can_be_estimated;
    bool remove{};
};

bool is_strictly_weaker_wep(const Weapon_struct& wep_struct1, const Weapon_struct& wep_struct2, Weapon_socket socket)
{
    auto special_stats1 = wep_struct1.special_stats;
    auto special_stats2 = wep_struct2.special_stats;

    bool greater_eq = (special_stats2.hit >= special_stats1.hit) &&
                      (special_stats2.critical_strike >= special_stats1.critical_strike) &&
                      (special_stats2.attack_power >= special_stats1.attack_power) &&
                      (special_stats2.axe_expertise >= special_stats1.axe_expertise) &&
                      (special_stats2.sword_expertise >= special_stats1.sword_expertise) &&
                      (special_stats2.mace_expertise >= special_stats1.mace_expertise) &&
                      (special_stats2.expertise >= special_stats1.expertise) &&
                      (special_stats2.gear_armor_pen >= special_stats1.gear_armor_pen);

    if (socket == Weapon_socket::main_hand)
    {
        greater_eq &= wep_struct2.swing_speed >= wep_struct1.swing_speed;
    }
    else
    {
        greater_eq &= wep_struct2.swing_speed <= wep_struct1.swing_speed;
    }
    greater_eq &=
        wep_struct2.average_damage / wep_struct2.swing_speed >= wep_struct1.average_damage / wep_struct1.swing_speed;

    bool greater = (special_stats2.hit > special_stats1.hit) ||
                   (special_stats2.critical_strike > special_stats1.critical_strike) ||
                   (special_stats2.attack_power > special_stats1.attack_power) ||
                   (special_stats2.axe_expertise > special_stats1.axe_expertise) ||
                   (special_stats2.sword_expertise > special_stats1.sword_expertise) ||
                   (special_stats2.mace_expertise > special_stats1.mace_expertise) ||
                   (special_stats2.expertise > special_stats1.expertise) ||
                   (special_stats2.gear_armor_pen > special_stats1.gear_armor_pen);

    if (socket == Weapon_socket::main_hand)
    {
        greater |= wep_struct2.swing_speed > wep_struct1.swing_speed;
    }
    else
    {
        greater |= wep_struct2.swing_speed < wep_struct1.swing_speed;
    }
    greater |=
        wep_struct2.average_damage / wep_struct2.swing_speed > wep_struct1.average_damage / wep_struct1.swing_speed;

    return greater_eq && greater;
}

double estimate_wep_stat_diff(const Weapon_struct& wep1, const Weapon_struct& wep2, bool main_hand)
{
    double wep_1_ap = wep1.average_damage / wep1.swing_speed * 14;
    if (main_hand) wep_1_ap += 100 * (wep1.swing_speed - 2.3);
    double wep_2_ap = wep2.average_damage / wep2.swing_speed * 14;
    if (main_hand) wep_2_ap += 100 * (wep2.swing_speed - 2.3);

    return main_hand ? wep_2_ap - wep_1_ap : 0.5 * (wep_2_ap - wep_1_ap);
}

std::vector<Weapon> Item_optimizer::remove_weaker_weapons(const Weapon_socket weapon_socket,
                                                          const std::vector<Weapon>& weapon_vec,
                                                          const Special_stats& special_stats,
                                                          std::string& debug_message, int keep_n_stronger_items,
                                                          const std::function<bool(const Weapon&)>& filter)
{
    std::vector<Weapon_struct> wep_structs;
    wep_structs.reserve(weapon_vec.size());

    for (const auto& w : weapon_vec)
    {
        if (filter(w)) continue;
        wep_structs.emplace_back(w, special_stats);
    }

    for (auto& wep1 : wep_structs)
    {
        if (!wep1.can_be_estimated) continue;

        auto stronger_found = 0;
        for (const auto& wep2 : wep_structs)
        {
            if (wep1 == wep2 || !wep2.can_be_estimated) continue;

            if (wep1.type() == wep2.type() && is_strictly_weaker_wep(wep1, wep2, weapon_socket))
            {
                stronger_found++;
                debug_message += String_helpers::string_with_precision(stronger_found) + "/" +
                                 String_helpers::string_with_precision(keep_n_stronger_items) + " since <b>" +
                                 wep2.name() + "</b> is better than <b>" + wep1.name() + "</b> in all aspects.<br>";
            }
            else
            {
                auto stat_diff = estimate_stat_diff(wep1.special_stats, wep2.special_stats);
                auto wep_stat_diff = estimate_wep_stat_diff(wep1, wep2, weapon_socket == Weapon_socket::main_hand);
                if (stat_diff + wep_stat_diff > 0)
                {
                    stronger_found++;
                    debug_message += String_helpers::string_with_precision(stronger_found) + "/" +
                                     String_helpers::string_with_precision(keep_n_stronger_items) +
                                     " since <b>" + wep2.name() + "</b> was estimated to be <b>" +
                                     String_helpers::string_with_precision(stat_diff + wep_stat_diff, 3) +
                                     " AP </b>better than <b>" + wep1.name() + "</b>.<br>";
                }
            }

            if (stronger_found >= keep_n_stronger_items)
            {
                wep1.remove = true;
                debug_message += "REMOVED:<b> " + wep1.name() + "</b>.<br>";
                break;
            }
        }
    }

    debug_message += "Weapons left:<br>";
    std::vector<Weapon> filtered_weapons;
    filtered_weapons.reserve(keep_n_stronger_items);
    for (const auto& w : wep_structs)
    {
        if (w.remove) continue;
        filtered_weapons.push_back(w.weapon.get());
        debug_message += "<b>" + w.name() + "</b><br>";
    }

    return filtered_weapons;
}

struct Armor_struct
{
    explicit Armor_struct(const Armor& a, const Special_stats& ss) :
        armor(a),
        special_stats(a.special_stats + a.attributes.to_special_stats(ss)),
        can_be_estimated(a.set_name == Set::none && a.hit_effects.empty() && a.use_effects.empty() && a.socket != Socket::trinket) {}

    bool operator==(const Armor_struct& other) const { return &armor.get() == &other.armor.get(); }
    [[nodiscard]] const std::string& name() const { return armor.get().name; }

    const std::reference_wrapper<const Armor> armor;
    const Special_stats special_stats;
    const bool can_be_estimated;
    bool remove{};
};

std::vector<Armor> Item_optimizer::remove_weaker_items(const std::vector<Armor>& armors,
                                                       const Special_stats& special_stats, std::string& debug_message,
                                                       int keep_n_stronger_items, const std::function<bool(const Armor&)>& filter)
{
    std::vector<Armor_struct> armor_structs;
    armor_structs.reserve(armors.size());
    
    for (const auto& a : armors)
    {
        if (filter(a)) continue;
        armor_structs.emplace_back(a, special_stats);
    }

    for (auto& armor1 : armor_structs)
    {
        if (!armor1.can_be_estimated) continue;

        int stronger_found = 0;
        for (const auto& armor2 : armor_structs)
        {
            if (armor1 == armor2 || !armor2.can_be_estimated) continue;

            if (armor1.special_stats < armor2.special_stats)
            {
                stronger_found++;
                debug_message += String_helpers::string_with_precision(stronger_found) + "/" +
                                 String_helpers::string_with_precision(keep_n_stronger_items) + " since <b>" +
                                 armor2.name() + "</b> is better than <b>" + armor1.name() +
                                 "</b> in all aspects.<br>";
            }
            else
            {
                auto stat_diff = estimate_stat_diff(armor1.special_stats, armor2.special_stats);
                if (stat_diff > 0)
                {
                    stronger_found++;
                    debug_message += String_helpers::string_with_precision(stronger_found) + "/" +
                                     String_helpers::string_with_precision(keep_n_stronger_items) +
                                     " since <b>" + armor2.name() + "</b> was estimated to be <b>" +
                                     String_helpers::string_with_precision(stat_diff, 3) +
                                     " AP </b>better than <b>" + armor1.name() + "</b>.<br>";
                }
            }
            if (stronger_found >= keep_n_stronger_items)
            {
                armor1.remove = true;
                debug_message += "REMOVED:<b> " + armor1.name() + "</b>.<br>";
                break;
            }
        }
    }

    debug_message += "Armors left:<br>";
    std::vector<Armor> filtered_armors;
    filtered_armors.reserve(keep_n_stronger_items);
    for (const auto& a : armor_structs)
    {
        if (a.remove) continue;

        filtered_armors.push_back(a.armor.get());
        debug_message += "<b>" + a.name() + "</b><br>";
    }
    return filtered_armors;
}
