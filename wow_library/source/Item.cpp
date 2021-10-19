#include "Item.hpp"

#include "iostream"

std::ostream& operator<<(std::ostream& os, const Weapon_socket& ws)
{
    static const std::string m[]{"main_hand", "one_hand", "off_hand", "two_hand"};
    return os << m[static_cast<size_t>(ws)];
}

std::ostream& operator<<(std::ostream& os, const Weapon_type& wt)
{
    static const std::string m[]{"sword", "axe", "dagger", "mace", "unarmed"};
    return os << m[static_cast<size_t>(wt)];
}

std::ostream& operator<<(std::ostream& os, const Socket& socket)
{
    static const std::string m[]{"none", "head", "neck", "shoulder", "back", "chest", "wrist", "hands",
        "belt", "legs", "boots", "ring", "trinket", "main_hand", "off_hand", "ranged"};
    return os << m[static_cast<size_t>(socket)];
}

std::string friendly_name(const Socket& socket)
{
    static const std::string m[]{"None", "Helmet", "Neck", "Shoulder", "Back", "Chest", "Wrist", "Hands",
                                 "Belt", "Legs", "Boots", "Ring", "Trinket", "Main hand", "Off hand", "Ranged"};
    return m[static_cast<size_t>(socket)];
}

std::string operator+(std::string& string, const Socket& socket)
{
    return string + friendly_name(socket);
}

std::string friendly_name(const Weapon_socket& weapon_socket)
{
    static const std::string m[]{"main-hand", "one-hand", "off-hand", "two-hand"};
    return m[static_cast<size_t>(weapon_socket)];
}

std::string operator+(std::string& string, const Weapon_socket& socket)
{
    return string + friendly_name(socket);
}

std::ostream& operator<<(std::ostream& os, const Hit_effect::Type& t)
{
    switch (t)
    {
    case Hit_effect::Type::none:
        return os << "none";
    case Hit_effect::Type::extra_hit:
        return os << "extra_hit";
    case Hit_effect::Type::windfury_hit:
        return os << "windfury_hit";
    case Hit_effect::Type::sword_spec:
        return os << "sword_spec";
    case Hit_effect::Type::stat_boost:
        return os << "stat_boost";
    case Hit_effect::Type::damage_physical:
        return os << "damage_physical";
    case Hit_effect::Type::damage_magic:
        return os << "damage_magic";
    case Hit_effect::Type::reduce_armor:
        return os << "reduce_armor";
    case Hit_effect::Type::rage_boost:
        return os << "rage_boost";
    default:
        return os;
    }
}

std::ostream& operator<<(std::ostream& os, const Hit_effect& he)
{
    os << he.name << " " << he.type << " affects_both_weapons = " << std::boolalpha << he.affects_both_weapons
    << ", proc_type = " << he.proc_type << ", max_charges = " << he.max_charges
       << ", armor_reduction = " << he.armor_reduction << ", max_stacks = " << he.max_stacks;
    return os;
}