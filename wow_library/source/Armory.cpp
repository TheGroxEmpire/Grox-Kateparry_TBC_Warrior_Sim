#include "Armory.hpp"

#include "Character.hpp"
#include "find_values.hpp"
#include "string_helpers.hpp"

Attributes Armory::get_enchant_attributes(Socket socket, Enchant::Type type) const
{
    switch (socket)
    {
    case Socket::head: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 8};
        case Enchant::Type::strength:
            return {8, 0};
        default:
            return {0, 0};
        }
    }
    case Socket::back: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 3};
        case Enchant::Type::greater_agility:
            return {0, 12};
        default:
            return {0, 0};
        }
    }
    case Socket::chest: {
        switch (type)
        {
        case Enchant::Type::minor_stats:
            return {3, 3};
        case Enchant::Type::major_stats:
            return {4, 4};
        case Enchant::Type::exceptional_stats:
            return {6, 6};
        default:
            return {0, 0};
        }
    }
    case Socket::wrist: {
        switch (type)
        {
        case Enchant::Type::strength7:
            return {7, 0};
        case Enchant::Type::strength9:
            return {9, 0};
        case Enchant::Type::strength12:
            return {12, 0};
        default:
            return {0, 0};
        }
    }
    case Socket::hands: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 7};
        case Enchant::Type::greater_agility:
            return {0, 15};
        case Enchant::Type::strength:
            return {7, 0};
        case Enchant::Type::strength15:
            return {15, 0};
        default:
            return {0, 0};
        }
    }
    case Socket::legs: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 8};
        case Enchant::Type::strength:
            return {8, 0};
        default:
            return {0, 0};
        }
    }
    case Socket::boots: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 7};
        case Enchant::Type::agility12:
            return {0, 12};
        case Enchant::Type::cats_swiftness:
            return {0, 6};
        default:
            return {0, 0};
        }
    }
    case Socket::main_hand:
    case Socket::off_hand: {
        switch (type)
        {
        case Enchant::Type::agility:
            return {0, 15};
        case Enchant::Type::strength:
            return {15, 0};
        case Enchant::Type::strength20:
            return {20, 0};
        case Enchant::Type::greater_agility:
            return {0, 20};
        default:
            return {0, 0};
        }
    }
    case Socket::ring: {
        switch (type)
        {
        case Enchant::Type::major_stats:
            return {4, 4};
        case Enchant::Type::ring_stats:
            return {8, 8};
        default:
            return {0, 0};
        }
    }
    default:
        return {0, 0};
    }
}

Special_stats Armory::get_enchant_special_stats(Socket socket, Enchant::Type type) const
{
    switch (socket)
    {
    case Socket::head: {
        switch (type)
        {
        case Enchant::Type::haste:
            return {0, 0, 0, 0, .0063};
        case Enchant::Type::ferocity:
            return {0, 1.01, 34};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::shoulder: {
        switch (type)
        {
        case Enchant::Type::attack_power:
            return {0, 0, 30};
        case Enchant::Type::naxxramas:
            return {0.63, 0, 26};
        case Enchant::Type::greater_vengeance:
            return {0.45, 0, 30};
        case Enchant::Type::greater_blade:
            return {0.67, 0, 20};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::hands: {
        switch (type)
        {
        case Enchant::Type::haste:
            return {0, 0, 0, 0, .0063};
        case Enchant::Type::attack_power:
            return {0, 0, 26};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::legs: {
        switch (type)
        {
        case Enchant::Type::haste:
            return {0, 0, 0, 0, .0063};
        case Enchant::Type::cobrahide:
            return {0.45, 0, 40};
        case Enchant::Type::nethercobra:
            return {0.54, 0, 50};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::boots: {
        switch (type)
        {
        case Enchant::Type::hit:
            return {0, 0.63, 0};
        default:
            return {0, 0, 0};
        }
    }
    case Socket::ring: {
        switch (type)
        {
        case Enchant::Type::damage:
            return {0, 0, 0, 0, 0, 0, 0, 2};
        case Enchant::Type::ring_damage:
            return {0, 0, 0, 0, 0, 0, 0, 4};
        default:
            return {0, 0, 0};
        }
    }
    default:
        return {0, 0, 0};
    }
}

Hit_effect Armory::enchant_hit_effect(double weapon_speed, Enchant::Type type) const
{
    switch (type)
    {
    case Enchant::Type::crusader:
        return {"crusader", Hit_effect::Type::stat_boost, {60, 0}, {0, 0, 0}, 0, 15, 0, weapon_speed / 60};
    case Enchant::Type::mongoose:
        return {"mongoose", Hit_effect::Type::stat_boost, {0, 120}, {0, 0, 0, 0, 0.02}, 0, 15, 0, weapon_speed / 60};
    default:
        return {"none", Hit_effect::Type::none, {}, {}, 0, 0, 0, 0};
    }
}

void Armory::clean_weapon(Weapon& weapon) const
{
    // TODO remove from armory.
    if (!weapon.hit_effects.empty())
    {
        auto temp = weapon.hit_effects;
        weapon.hit_effects = {};
        for (const auto& hit_effect : temp)
        {
            if (hit_effect.name == weapon.name)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }
}

void Armory::compute_total_stats(Character& character) const
{
    if (!check_if_weapons_valid(character.weapons))
    {
        std::cout << "invalid weapon setup";
        assert(false);
    }
    if (!check_if_armor_valid(character.armor))
    {
        std::cout << "invalid armor setup";
        assert(false);
    }
    character.total_attributes = {};
    character.total_special_stats = {};
    character.set_bonuses = {};
    for (auto& wep : character.weapons)
    {
        clean_weapon(wep);
    }
    Attributes total_attributes{};
    Special_stats total_special_stats{};

    total_attributes += character.base_attributes;
    total_special_stats += character.base_special_stats;
    std::vector<Set> set_names{};
    std::vector<Use_effect> use_effects{};
    for (const Armor& armor : character.armor)
    {
        total_attributes += armor.attributes;
        total_special_stats += armor.special_stats;

        total_attributes += get_enchant_attributes(armor.socket, armor.enchant.type);
        total_special_stats += get_enchant_special_stats(armor.socket, armor.enchant.type);

        set_names.emplace_back(armor.set_name);
        for (const auto& use_effect : armor.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }
        for (const auto& hit_effect : armor.hit_effects)
        {
            for (Weapon& weapon : character.weapons)
            {
                weapon.hit_effects.emplace_back(hit_effect);
            }
        }
    }

    for (Weapon& weapon : character.weapons)
    {
        total_attributes += weapon.attributes;
        total_special_stats += weapon.special_stats;

        total_attributes += get_enchant_attributes(weapon.socket, weapon.enchant.type);
        total_special_stats += get_enchant_special_stats(weapon.socket, weapon.enchant.type);

        for (const auto& use_effect : weapon.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }
        auto hit_effect = enchant_hit_effect(weapon.swing_speed, weapon.enchant.type);
        if (hit_effect.type != Hit_effect::Type::none)
        {
            weapon.hit_effects.emplace_back(hit_effect);
        }

        set_names.emplace_back(weapon.set_name);
    }

    std::vector<Set> unique_set_names{};
    for (const Set& set_name : set_names)
    {
        if (set_name != Set::none)
        {
            bool unique = true;
            for (const Set& set_name_unique : unique_set_names)
            {
                if (set_name == set_name_unique)
                {
                    unique = false;
                }
            }
            if (unique)
            {
                unique_set_names.emplace_back(set_name);
            }
        }
    }

    for (const Set& unique_set_name : unique_set_names)
    {
        int count = 0;
        for (const Set& set_name : set_names)
        {
            if (set_name == unique_set_name)
            {
                count++;
            }
        }
        for (const Set_bonus& set_bonus : set_bonuses)
        {
            if (set_bonus.set == unique_set_name && count >= set_bonus.pieces)
            {
                total_attributes += set_bonus.attributes;
                total_special_stats += set_bonus.special_stats;
                character.set_bonuses.emplace_back(set_bonus);
            }
        }
    }

    for (const auto& buff : character.buffs)
    {
        total_attributes += buff.attributes;
        total_special_stats += buff.special_stats;

        for (const auto& use_effect : buff.use_effects)
        {
            use_effects.emplace_back(use_effect);
        }

        for (const auto& hit_effect : buff.hit_effects)
        {
            if (hit_effect.name != "windfury_totem")
            {
                for (Weapon& weapon : character.weapons)
                {
                    weapon.hit_effects.emplace_back(hit_effect);
                }
            }
            else
            {
                character.weapons[0].hit_effects.emplace_back(hit_effect);
            }
        }
    }

    // Effects gained from talents
    for (auto& use_effect : use_effects)
    {
        if (use_effect.name == "battle_shout")
        {
            if (character.talents.booming_voice_talent)
            {
                use_effect.duration = 180.0;
            }
            if (character.talents.commanding_presence_talent > 0)
            {
                use_effect.special_stats_boost.attack_power *=
                    1.0 + 0.05 * character.talents.commanding_presence_talent;
            }
            break;
        }
    }

    if (character.talents.sword_specialization > 0)
    {
        for (auto& wep : character.weapons)
        {
            if (wep.type == Weapon_type::sword)
            {
                double prob = double(character.talents.sword_specialization) / 100.0;
                wep.hit_effects.push_back({"sword_specialization", Hit_effect::Type::sword_spec, {}, {}, 0, 0, 0.5, prob});
            }
        }
    }

    if (character.talents.one_handed_weapon_specialization > 0 && character.is_dual_wield())
    {
        double multiplier = double(character.talents.one_handed_weapon_specialization) / 50.0;
        character.talent_special_stats += {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, multiplier};
    }

    // Cruelty etc.
    total_special_stats += character.talent_special_stats;
    total_special_stats.critical_strike += 3; // crit from berserker stance
    if (character.talents.improved_berserker_stance > 0)
    {
        double ap_multiplier = double(character.talents.improved_berserker_stance) * 0.02;
        total_special_stats.attack_power += total_special_stats.attack_power * ap_multiplier;
    }

    total_special_stats += total_attributes.convert_to_special_stats(total_special_stats, character.talents.improved_berserker_stance * 0.02 + 1);
    character.total_attributes = total_attributes.multiply(total_special_stats);
    character.total_special_stats = total_special_stats;
    character.use_effects = use_effects;
}

bool Armory::check_if_armor_valid(const std::vector<Armor>& armor) const
{
    std::vector<Socket> sockets;
    bool one_ring{false};
    bool one_trinket{false};
    for (auto const& armor_piece : armor)
    {
        for (auto const& socket : sockets)
        {
            if (armor_piece.socket == socket)
            {
                if (armor_piece.socket == Socket::ring)
                {
                    if (armor_piece.socket == Socket::ring && one_ring)
                    {
                        std::cout << "extra copy of " << armor_piece.socket << "\n";
                        return false;
                    }
                    one_ring = true;
                }
                else if (armor_piece.socket == Socket::trinket)
                {
                    if (armor_piece.socket == Socket::trinket && one_trinket)
                    {
                        std::cout << "extra copy of " << armor_piece.socket << "\n";
                        return false;
                    }
                    one_trinket = true;
                }
                else
                {
                    std::cout << "extra copy of " << armor_piece.socket << "\n";
                    return false;
                }
            }
        }
        sockets.emplace_back(armor_piece.socket);
    }
    return true;
}

bool Armory::check_if_weapons_valid(std::vector<Weapon>& weapons) const
{
    bool is_valid{true};
    is_valid &= weapons.size() <= 2;
    is_valid &= (weapons[0].socket != Socket::off_hand);
    if (weapons.size() == 2)
    {
        is_valid &= (weapons[1].socket != Socket::main_hand);
    }
    if (weapons.size() == 1)
    {
        is_valid &= (weapons[0].weapon_socket == Weapon_socket::two_hand);
    }
    return is_valid;
}

void Armory::change_weapon(std::vector<Weapon>& current_weapons, const Weapon& equip_weapon, const Socket& socket) const
{
    // TODO fix twohanded -> dual wield item swap!
    if (equip_weapon.weapon_socket == Weapon_socket::two_hand)
    {
        Weapon& current_wep = current_weapons[0];
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
    else
    {
        Weapon& current_wep = (socket == Socket::main_hand) ? current_weapons[0] : current_weapons[1];
        Weapon weapon_copy = equip_weapon;
        weapon_copy.buff = current_wep.buff;
        weapon_copy.enchant = current_wep.enchant;
        weapon_copy.socket = socket;
        current_wep = weapon_copy;
    }
}

void Armory::change_armor(std::vector<Armor>& armor_vec, const Armor& armor, bool first_misc_slot) const
{
    auto socket = armor.socket;
    for (auto& armor_piece : armor_vec)
    {
        if (armor_piece.socket == socket)
        {
            if (socket == Socket::ring || socket == Socket::trinket)
            {
                if (first_misc_slot)
                {
                    armor_piece = armor;
                    return;
                }
                first_misc_slot = true; // Will trigger on the second hit instead
            }
            else
            {
                // Reuse the same enchant
                auto enchant = armor_piece.enchant;
                armor_piece = armor;
                armor_piece.enchant = enchant;
                return;
            }
        }
    }
}

std::vector<Weapon> Armory::get_weapon_in_socket(const Weapon_socket socket) const
{
    switch (socket)
    {
    case Weapon_socket::main_hand: {
        std::vector<Weapon> mh_weapons{};
        for (const auto& wep : swords_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : axes_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : maces_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : daggers_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : fists_t)
        {
            if (wep.weapon_socket == Weapon_socket::main_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                mh_weapons.emplace_back(wep);
            }
        }
        return mh_weapons;
    }
    case Weapon_socket::off_hand: {
        std::vector<Weapon> oh_weapons{};
        for (const auto& wep : swords_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : axes_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : maces_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : daggers_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        for (const auto& wep : fists_t)
        {
            if (wep.weapon_socket == Weapon_socket::off_hand || wep.weapon_socket == Weapon_socket::one_hand)
            {
                oh_weapons.emplace_back(wep);
            }
        }
        return oh_weapons;
    }
    case Weapon_socket ::two_hand: {
        std::vector<Weapon> th_weapons{};
        for (const auto& wep : two_handed_swords_t)
        {
            th_weapons.emplace_back(wep);
        }
        for (const auto& wep : two_handed_maces_t)
        {
            th_weapons.emplace_back(wep);
        }
        for (const auto& wep : two_handed_axes_polearm_t)
        {
            th_weapons.emplace_back(wep);
        }
        return th_weapons;
    }
    default:
        std::cout << "ERROR: incorrect weapon socket provided!\n";
        return swords_t;
    }
}
std::vector<Armor> Armory::get_items_in_socket(const Socket socket) const
{
    switch (socket)
    {
    case Socket::head:
        return helmet_t;
    case Socket::neck:
        return neck_t;
    case Socket::shoulder:
        return shoulder_t;
    case Socket::back:
        return back_t;
    case Socket::chest:
        return chest_t;
    case Socket::wrist:
        return wrists_t;
    case Socket::hands:
        return hands_t;
    case Socket::belt:
        return belt_t;
    case Socket::legs:
        return legs_t;
    case Socket::boots:
        return boots_t;
    case Socket::ring:
        return ring_t;
    case Socket::trinket:
        return trinket_t;
    case Socket::ranged:
        return ranged_t;
    default:
        std::cout << "ERROR: incorrect item socket provided: " << socket << "\n";
        assert(false);
        return ranged_t;
    }
}

Armor Armory::find_armor(const Socket socket, const std::string& name) const
{
    std::vector<Armor> items;
    switch (socket)
    {
    case Socket::head:
        items = helmet_t;
        break;
    case Socket::neck:
        items = neck_t;
        break;
    case Socket::shoulder:
        items = shoulder_t;
        break;
    case Socket::back:
        items = back_t;
        break;
    case Socket::chest:
        items = chest_t;
        break;
    case Socket::wrist:
        items = wrists_t;
        break;
    case Socket::hands:
        items = hands_t;
        break;
    case Socket::belt:
        items = belt_t;
        break;
    case Socket::legs:
        items = legs_t;
        break;
    case Socket::boots:
        items = boots_t;
        break;
    case Socket::ring:
        items = ring_t;
        break;
    case Socket::trinket:
        items = trinket_t;
        break;
    case Socket::ranged:
        items = ranged_t;
        break;
    default:
        std::cout << "ERROR: incorrect item socket provided: " << socket << "\n";
        assert(false);
        break;
    }
    for (const auto& item : items)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    return {"item_not_found: " + name, {}, {}, socket};
}

Weapon Armory::find_weapon(Weapon_socket socket, const std::string& name) const
{
    if (socket == Weapon_socket::two_hand)
    {
        for (const auto& item : two_handed_swords_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        for (const auto& item : two_handed_maces_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        for (const auto& item : two_handed_axes_polearm_t)
        {
            if (item.name == name)
            {
                return item;
            }
        }
        return {"item_not_found: " + name, {}, {}, 3.0, 0, 0, Weapon_socket::two_hand, Weapon_type::sword};
    }
    for (const auto& item : swords_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : maces_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : axes_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : daggers_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    for (const auto& item : fists_t)
    {
        if (item.name == name)
        {
            return item;
        }
    }
    return {"item_not_found: " + name, {}, {}, 2.0, 0, 0, Weapon_socket::one_hand, Weapon_type::unarmed};
}

void Armory::add_enchants_to_character(Character& character, const std::vector<std::string>& ench_vec) const
{
    if (String_helpers::find_string(ench_vec, "e+8 strength"))
    {
        character.add_enchant(Socket::head, Enchant::Type::strength);
    }
    else if (String_helpers::find_string(ench_vec, "e+10 haste"))
    {
        character.add_enchant(Socket::head, Enchant::Type::haste);
    }
    else if (String_helpers::find_string(ench_vec, "eferocity"))
    {
        character.add_enchant(Socket::head, Enchant::Type::ferocity);
    }

    if (String_helpers::find_string(ench_vec, "s+30 attack_power"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::attack_power);
    }
    else if (String_helpers::find_string(ench_vec, "snaxxramas"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::naxxramas);
    }
    else if (String_helpers::find_string(ench_vec, "sgreater_vengeance"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::greater_vengeance);
    }
    else if (String_helpers::find_string(ench_vec, "sgreater_blade"))
    {
        character.add_enchant(Socket::shoulder, Enchant::Type::greater_blade);
    }

    if (String_helpers::find_string(ench_vec, "b+3 agility"))
    {
        character.add_enchant(Socket::back, Enchant::Type::agility);
    }
    else if (String_helpers::find_string(ench_vec, "b+12 agility"))
    {
        character.add_enchant(Socket::back, Enchant::Type::greater_agility);
    }

    if (String_helpers::find_string(ench_vec, "c+3 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::minor_stats);
    }
    else if (String_helpers::find_string(ench_vec, "c+4 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::major_stats);
    }
    else if (String_helpers::find_string(ench_vec, "c+6 stats"))
    {
        character.add_enchant(Socket::chest, Enchant::Type::exceptional_stats);
    }

    if (String_helpers::find_string(ench_vec, "w+7 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength7);
    }
    else if (String_helpers::find_string(ench_vec, "w+9 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength9);
    }
    else if (String_helpers::find_string(ench_vec, "w+12 strength"))
    {
        character.add_enchant(Socket::wrist, Enchant::Type::strength12);
    }

    if (String_helpers::find_string(ench_vec, "h+7 strength"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::strength);
    }
    else if (String_helpers::find_string(ench_vec, "h+15 strength"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::strength15);
    }
    else if (String_helpers::find_string(ench_vec, "h+7 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::agility);
    }
    else if (String_helpers::find_string(ench_vec, "h+15 agility"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::greater_agility);
    }
    else if (String_helpers::find_string(ench_vec, "h+10 haste"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::haste);
    }
    else if (String_helpers::find_string(ench_vec, "h+26 attack_power"))
    {
        character.add_enchant(Socket::hands, Enchant::Type::attack_power);
    }

    if (String_helpers::find_string(ench_vec, "l+8 strength"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::strength);
    }
    else if (String_helpers::find_string(ench_vec, "l+10 haste"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::haste);
    }
    else if (String_helpers::find_string(ench_vec, "lcobrahide"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::cobrahide);
    }
    else if (String_helpers::find_string(ench_vec, "lnethercobra"))
    {
        character.add_enchant(Socket::legs, Enchant::Type::nethercobra);
    }

    if (String_helpers::find_string(ench_vec, "t+7 agility"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::agility);
    }
    else if (String_helpers::find_string(ench_vec, "t+12 agility"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::agility12);
    }
    else if (String_helpers::find_string(ench_vec, "tcats_swiftness"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::cats_swiftness);
    }
    else if (String_helpers::find_string(ench_vec, "t+10 hit"))
    {
        character.add_enchant(Socket::boots, Enchant::Type::hit);
    }

    if (String_helpers::find_string(ench_vec, "mcrusader"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::crusader);
    }
    else if (String_helpers::find_string(ench_vec, "mmongoose"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::mongoose);
    }
    else if (String_helpers::find_string(ench_vec, "m+15 agility"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::agility);
    }
    else if (String_helpers::find_string(ench_vec, "m+20 agility"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::greater_agility);
    }
    else if (String_helpers::find_string(ench_vec, "m+15 strength"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::strength);
    }
    else if (String_helpers::find_string(ench_vec, "m+20 strength"))
    {
        character.add_enchant(Socket::main_hand, Enchant::Type::strength20);
    }

    if (String_helpers::find_string(ench_vec, "ocrusader"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::crusader);
    }
    else if (String_helpers::find_string(ench_vec, "omongoose"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::mongoose);
    }
    else if (String_helpers::find_string(ench_vec, "o+15 agility"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::agility);
    }
    else if (String_helpers::find_string(ench_vec, "o+20 agility"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::greater_agility);
    }
    else if (String_helpers::find_string(ench_vec, "o+15 strength"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::strength);
    }
    else if (String_helpers::find_string(ench_vec, "o+20 strength"))
    {
        character.add_enchant(Socket::off_hand, Enchant::Type::strength20);
    }

    if (String_helpers::find_string(ench_vec, "r+4 stats") && String_helpers::find_string(ench_vec, "f+4 stats"))
    {
        character.add_enchant(Socket::ring, Enchant::Type::ring_stats);
    }
    else if (String_helpers::find_string(ench_vec, "r+4 stats") || String_helpers::find_string(ench_vec, "f+4 stats"))
    {
        character.add_enchant(Socket::ring, Enchant::Type::major_stats);
    }

    if (String_helpers::find_string(ench_vec, "r+2 damage") && String_helpers::find_string(ench_vec, "f+2 damage"))
    {
        character.add_enchant(Socket::ring, Enchant::Type::ring_damage);
    }
    else if (String_helpers::find_string(ench_vec, "r+2 damage") || String_helpers::find_string(ench_vec, "f+2 damage"))
    {
        character.add_enchant(Socket::ring, Enchant::Type::damage);
    }
}

void Armory::add_gems_to_character(Character& character, const std::vector<std::string>& gem_vec) const
{
    int gem_counter, i;

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+3 strength");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.strength_3);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 strength");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.strength_4);
    }
    
    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+5 strength");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.strength_5);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+6 strength");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.strength_6);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+8 strength");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.strength_8);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+3 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_3);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_4);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+5 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_5);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+6 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_6);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+8 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_8);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+10 agility");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agility_10);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+3 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_3);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_4);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+5 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_5);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+6 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_6);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+8 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_8);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+10 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_10);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+12 crit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_12);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+20 AP");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.ap_20);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+24 AP");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.ap_24);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 hit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.hit_4);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+6 hit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.hit_6);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+8 hit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.hit_8);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+10 hit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.hit_10);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+12 hit");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.hit_12);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+3 crit +3 str");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_3_str_3);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 crit +4 str");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_4_str_4);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+4 crit +5 str");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_4_str_5);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+5 crit +5 str");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.crit_5_str_5);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "haste proc");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.gem_haste);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+3 dmg");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.dmg_3);
    }

    gem_counter = std::count(gem_vec.begin(), gem_vec.end(), "+12 agi +3% crit dmg");
    for (i = 0; i < gem_counter; i++)
    {
        character.add_gem(gems.agi_12_critDmg_3);
    }
}

void Armory::add_buffs_to_character(Character& character, const std::vector<std::string>& buffs_vec) const
{
    if (String_helpers::find_string(buffs_vec, "fungal_bloom"))
    {
        character.add_buff(buffs.fungal_bloom);
    }
    if (String_helpers::find_string(buffs_vec, "full_polarity"))
    {
        character.add_buff(buffs.full_polarity);
    }
    if (String_helpers::find_string(buffs_vec, "ferocious_inspiration"))
    {
        character.add_buff(buffs.ferocious_inspiration);
    }
    if (String_helpers::find_string(buffs_vec, "battle_squawk"))
    {
        character.add_buff(buffs.battle_squawk);
    }

    // Player buffs
    if (String_helpers::find_string(buffs_vec, "battle_shout"))
    {

        character.add_buff(buffs.battle_shout);

    }
    if (String_helpers::find_string(buffs_vec, "blessing_of_kings"))
    {
        character.add_buff(buffs.blessing_of_kings);
    }
    if (String_helpers::find_string(buffs_vec, "blessing_of_might"))
    {
        character.add_buff(buffs.blessing_of_might);

    }
    if (String_helpers::find_string(buffs_vec, "windfury_totem"))
    {
        Buff totem = buffs.windfury_totem;
        if (String_helpers::find_string(buffs_vec, "improved_weapon_totems"))
        {
            totem.hit_effects[0].attack_power_boost *= 1.3;
        }
        character.add_buff(totem);
    }
    if (String_helpers::find_string(buffs_vec, "strength_of_earth_totem"))
    {
        Buff totem = buffs.strength_of_earth_totem;
        if (String_helpers::find_string(buffs_vec, "enhancing_totems"))
        {
            totem.attributes.strength *= 1.15;
        }
        character.add_buff(totem);
    }
    if (String_helpers::find_string(buffs_vec, "grace_of_air_totem"))
    {
        Buff totem = buffs.grace_of_air_totem;
        if (String_helpers::find_string(buffs_vec, "enhancing_totems"))
        {
            totem.attributes.agility *= 1.15;
        }
        character.add_buff(totem);
    }
    if (String_helpers::find_string(buffs_vec, "gift_of_the_wild"))
    {
        character.add_buff(buffs.gift_of_the_wild);
    }
    if (String_helpers::find_string(buffs_vec, "leader_of_the_pack"))
    {
        character.add_buff(buffs.leader_of_the_pack);
    }
    if (String_helpers::find_string(buffs_vec, "improved_seal_of_the_crusader"))
    {
        character.add_buff(buffs.improved_seal_of_the_crusader);
    }
    if (String_helpers::find_string(buffs_vec, "blood_frenzy"))
    {
        character.add_buff(buffs.blood_frenzy);
    }
    if (String_helpers::find_string(buffs_vec, "improved_sanctity_aura"))
    {
        character.add_buff(buffs.improved_sanctity_aura);
    }
    if (String_helpers::find_string(buffs_vec, "heroic_presence"))
    {
        character.add_buff(buffs.heroic_presence);
    }
    if (String_helpers::find_string(buffs_vec, "improved_faerie_fire"))
    {
        character.add_buff(buffs.improved_faerie_fire);
    }
    if (String_helpers::find_string(buffs_vec, "trueshot_aura"))
    {
        character.add_buff(buffs.trueshot_aura);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_mongoose"))
    {
        character.add_buff(buffs.elixir_mongoose);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_major_agility"))
    {
        character.add_buff(buffs.elixir_of_major_agility);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_mastery_bloodberry_elixir"))
    {
        character.add_buff(buffs.elixir_of_mastery_bloodberry_elixir);
    }
    if (String_helpers::find_string(buffs_vec, "blessed_sunfruit"))
    {
        character.add_buff(buffs.blessed_sunfruit);
    }
    if (String_helpers::find_string(buffs_vec, "roasted_clefthoof"))
    {
        character.add_buff(buffs.roasted_clefthoof);
    }
    if (String_helpers::find_string(buffs_vec, "spicy_hot_talbuk"))
    {
        character.add_buff(buffs.spicy_hot_talbuk);
    }
    if (String_helpers::find_string(buffs_vec, "grilled_mudfish"))
    {
        character.add_buff(buffs.grilled_mudfish);
    }
    if (String_helpers::find_string(buffs_vec, "ravager_dog"))
    {
        character.add_buff(buffs.ravager_dog);
    }
    if (String_helpers::find_string(buffs_vec, "charred_bear_kabobs"))
    {
        character.add_buff(buffs.charred_bear_kabobs);
    }
    if (String_helpers::find_string(buffs_vec, "juju_power"))
    {
        character.add_buff(buffs.juju_power);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_giants"))
    {
        character.add_buff(buffs.elixir_of_giants);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_major_strength"))
    {
        character.add_buff(buffs.elixir_of_major_strength);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_brute_force"))
    {
        character.add_buff(buffs.elixir_of_brute_force);
    }
    if (String_helpers::find_string(buffs_vec, "juju_might"))
    {
        character.add_buff(buffs.juju_might);
    }
    if (String_helpers::find_string(buffs_vec, "winterfall_firewater"))
    {
        character.add_buff(buffs.winterfall_firewater);
    }
    if (String_helpers::find_string(buffs_vec, "fel_strength_elixir"))
    {
        character.add_buff(buffs.fel_strength_elixir);
    }
    if (String_helpers::find_string(buffs_vec, "onslaught_elixir"))
    {
        character.add_buff(buffs.onslaught_elixir);
    }
    if (String_helpers::find_string(buffs_vec, "elixir_of_demonslaying"))
    {
        character.add_buff(buffs.elixir_of_demonslaying);
    }
    if (String_helpers::find_string(buffs_vec, "flask_of_relentless_assault"))
    {
        character.add_buff(buffs.flask_of_relentless_assault);
    }
    if (String_helpers::find_string(buffs_vec, "unstable_flask_of_the_bandit"))
    {
        character.add_buff(buffs.unstable_flask_of_the_bandit);
    }
    if (String_helpers::find_string(buffs_vec, "unstable_flask_of_the_beast"))
    {
        character.add_buff(buffs.unstable_flask_of_the_beast);
    }
    if (String_helpers::find_string(buffs_vec, "unstable_flask_of_the_soldier"))
    {
        character.add_buff(buffs.unstable_flask_of_the_soldier);
    }
    if (String_helpers::find_string(buffs_vec, "roids"))
    {
        character.add_buff(buffs.roids);
    }
    if (String_helpers::find_string(buffs_vec, "scroll_of_strength_v"))
    {
        character.add_buff(buffs.scroll_of_strength_v);
    }
    if (String_helpers::find_string(buffs_vec, "scroll_of_agility_v"))
    {
        character.add_buff(buffs.scroll_of_agility_v);
    }
    if (String_helpers::find_string(buffs_vec, "mighty_rage_potion"))
    {
        character.add_buff(buffs.mighty_rage_potion);
    }
    if (String_helpers::find_string(buffs_vec, "drums_of_battle"))
    {
        character.add_buff(buffs.drums_of_battle);
    }
    if (String_helpers::find_string(buffs_vec, "bloodlust"))
    {
        character.add_buff(buffs.bloodlust);
    }
    if (String_helpers::find_string(buffs_vec, "haste_potion"))
    {
        character.add_buff(buffs.haste_potion);
    }
    if (String_helpers::find_string(buffs_vec, "insane_strength_potion"))
    {
        character.add_buff(buffs.insane_strength_potion);
    }
    if (String_helpers::find_string(buffs_vec, "heroic_potion"))
    {
        character.add_buff(buffs.heroic_potion);
    }
    if (String_helpers::find_string(buffs_vec, "dense_stone_main_hand"))
    {
        character.add_weapon_buff(Socket::main_hand, buffs.dense_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "elemental_stone_main_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "consecrated_sharpening_stone_main_hand"))
    {
        character.add_buff(buffs.consecrated_sharpening_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "adamantite_stone_main_hand"))
    {
        character.add_weapon_buff(Socket::main_hand, buffs.adamantite_stone);
    }

    if (String_helpers::find_string(buffs_vec, "dense_stone_off_hand"))
    {
        character.add_weapon_buff(Socket::off_hand, buffs.dense_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "elemental_stone_off_hand"))
    {
        character.add_buff(buffs.elemental_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "consecrated_sharpening_stone_off_hand"))
    {
        character.add_buff(buffs.consecrated_sharpening_stone);
    }
    else if (String_helpers::find_string(buffs_vec, "adamantite_stone_off_hand"))
    {
        character.add_weapon_buff(Socket::off_hand, buffs.adamantite_stone);
    }
}

void Armory::add_talents_to_character(Character& character, const std::vector<std::string>& talent_string,
                                      const std::vector<int>& talent_val) const
{
    Find_values<int> fv{talent_string, talent_val};
    int val = fv.find("cruelty_talent");
    if (val > 0)
    {
        character.talent_special_stats.critical_strike += val;
    }
    val = fv.find("defiance_talent");
    if (val > 0)
    {
        character.talent_special_stats.expertise += val * 0.5;
    }
    val = fv.find("precision_talent");
    if (val > 0)
    {
        character.talent_special_stats.hit += val;
    }
    val = fv.find("two_handed_weapon_specialization_talent");
    if (val > 0)
    {
        if (!character.is_dual_wield())
        {
            double amount = double(val) / 100.0;
            character.talent_special_stats += Special_stats{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, amount};
        }
    }
    val = fv.find("poleaxe_specialization_talent");
    if (val > 0)
    {
        if (character.weapons[0].type == Weapon_type::axe)
        {
            character.talent_special_stats.critical_strike += val;
        }
    }
    val = fv.find("booming_voice_talent");
    if (val > 0)
    {
        character.talents.booming_voice_talent = true;
    }
    val = fv.find("commanding_presence_talent");
    if (val > 0)
    {
        character.talents.commanding_presence_talent = val;
    }
    val = fv.find("sword_specialization_talent");
    if (val > 0)
    {
        character.talents.sword_specialization = val;
    }

    val = fv.find("one_handed_weapon_specialization_talent");
    if (val > 0)
    {
        character.talents.one_handed_weapon_specialization = val;
    }

    val = fv.find("improved_berserker_stance_talent");
    if (val > 0)
    {
        character.talents.improved_berserker_stance = val;
    }
}