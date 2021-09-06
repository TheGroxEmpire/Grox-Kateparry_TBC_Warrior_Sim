#ifndef WOW_SIMULATOR_CHARACTER_HPP
#define WOW_SIMULATOR_CHARACTER_HPP

#include "Armory.hpp"
#include "Item.hpp"

#include <cassert>
#include <iostream>
#include <vector>

enum class Race
{
    human,
    dwarf,
    night_elf,
    gnome,
    draenei,
    orc,
    tauren,
    troll,
    undead
};

class Character
{
public:
    Character(const Race& race, int level);

    void equip_armor(const Armor& piece) { armor.emplace_back(piece); }

    void equip_weapon(std::vector<Weapon> weapon_vec)
    {
        if (weapon_vec.size() == 1)
        {
            equip_weapon(weapon_vec[0]);
        }
        else if (weapon_vec.size() == 2)
        {
            equip_weapon(weapon_vec[0], weapon_vec[1]);
        }
        else
        {
            std::cout << "ERROR: trying to equip weapon vec of size: " << weapon_vec.size() << "\n";
        }
    }

    void equip_weapon(Weapon weapon)
    {
        if (weapon.weapon_socket != Weapon_socket::two_hand)
        {
            std::cout << "WARN: Wielding single weapon that is not two handed.\n";
        }
        weapon.socket = Socket::main_hand;
        weapons.clear();
        weapons.emplace_back(weapon);
    }

    void equip_weapon(Weapon weapon1, Weapon weapon2)
    {
        if (weapon1.weapon_socket == Weapon_socket::two_hand || weapon2.weapon_socket == Weapon_socket::two_hand)
        {
            std::cout << "Cant dual wield with a two-hand.";
            assert(false);
        }
        weapon1.socket = Socket::main_hand;
        weapon2.socket = Socket::off_hand;
        weapons.clear();
        weapons.emplace_back(weapon1);
        weapons.emplace_back(weapon2);
    }

    void add_enchant(const Socket socket, const Enchant::Type type)
    {
        for (auto& wep : weapons)
        {
            if (socket == wep.socket)
            {
                wep.enchant = Enchant{type};
                return;
            }
        }
        for (auto& item : armor)
        {
            if (socket == item.socket)
            {
                item.enchant = Enchant{type};
                return;
            }
        }
    }

    void add_gem(const Gem& gem) { gems.emplace_back(gem); }

    [[nodiscard]] bool has_buff(const Buff& buff) const
    {
        for (const auto& b : buffs)
        {
            if (b.name == buff.name) return true;
        }
        return false;
    }

    void add_buff(const Buff& buff) { buffs.emplace_back(buff); }

    void add_weapon_buff(const Socket socket, const Weapon_buff& buff)
    {
        for (auto& wep : weapons)
        {
            if (socket == wep.socket)
            {
                wep.buff = buff;
                return;
            }
        }
    }

    [[nodiscard]] bool is_dual_wield() const { return weapons.size() == 2; }

    [[nodiscard]] bool has_weapon_of_type(Weapon_type weapon_type) const
    {
        for (const auto& wep : weapons)
        {
            if (wep.type == weapon_type) return true;
        }
        return false;
    }

    [[nodiscard]] bool has_item(const std::string& item_name) const
    {
        for (const auto& a : armor)
        {
            if (a.name == item_name) return true;
        }
        for (const auto& w : weapons)
        {
            if (w.name == item_name) return true;
        }
        return false;
    }

    Armor get_item_from_socket(const Socket socket, bool first_slot = true)
    {
        for (const auto& armor_piece : armor)
        {
            if (armor_piece.socket == socket)
            {
                if (first_slot)
                {
                    return armor_piece;
                }
                first_slot = true; // Take the next item
            }
        }
        std::cout << "Error did not find item in socket: " << socket << "\n";
        return armor[0];
    }

    Weapon get_weapon_from_socket(const Socket socket)
    {
        for (const auto& wep : weapons)
        {
            if (wep.socket == socket) return wep;
        }
        std::cout << "Error did not find item in socket: " << socket << "\n";
        return weapons[0];
    }

    [[nodiscard]] bool has_set_bonus(Set set, int pieces) const
    {
        for (const auto& sb : set_bonuses)
        {
            if (sb.set == set && sb.pieces == pieces) return true;
        }
        return false;
    }

    struct talents_t
    {
        int improved_heroic_strike{};
        //int deflection{};
        //int improved_rend{};
        //int improved_charge{};
        //int iron_will{};
        //int improved_thunder_clap{};
        int improved_overpower{};
        int anger_management{};
        int deep_wounds{};
        int two_handed_weapon_specialization{};
        int impale{};
        int poleaxe_specialization{};
        int death_wish{};
        int mace_specialization{};
        int sword_specialization{};
        //int improved_intercept{};
        //int improved_hamstring{};
        int improved_disciplines{};
        //int blood_frenzy{};
        int mortal_strike{};
        //int second_wind{};
        int improved_mortal_strike{};
        int endless_rage{};
        int booming_voice{};
        int cruelty{};
        //int improved_demoralizing_shout{};
        int unbridled_wrath{};
        int improved_cleave{};
        //int peircing_howl{};
        //int blood_craze{};
        int commanding_presence{};
        int dual_wield_specialization{};
        int improved_execute{};
        //int enrage{};
        int improved_slam{};
        int sweeping_strikes{};
        int weapon_mastery{};
        //int improved_berserker_rage{};
        int flurry{};
        int precision{};
        int bloodthirst{};
        int improved_whirlwind{};
        int improved_berserker_stance{};
        int rampage{};
        //int improved_bloodrage{};
        int tactical_mastery{};
        //int anticipation{};
        //int shield_specialization{};
        //int toughness{};
        //int last_stand{};
        //int improved_shield_block{};
        //int improved_revenge{};
        int defiance{};
        //int improved_sunder_armor{};
        //int improved_disarm{};
        //int improved_taunt{};
        //int improved_shield_wall{};
        //int concussion_blow{};
        //int improved_shield_bash{};
        //int shield_mastery{};
        int one_handed_weapon_specialization{};
        //int improved_defensive_stance{};
        //int shield_slam{};
        //int focused_rage{};
        //int vitality{};
        //int devastate{};
    } talents;

    Attributes base_attributes;
    Special_stats base_special_stats;

    Attributes total_attributes;
    Special_stats total_special_stats;

    std::vector<Armor> armor;
    std::vector<Weapon> weapons;
    std::vector<Gem> gems;
    std::vector<Set_bonus> set_bonuses;

    std::vector<Buff> buffs;
    std::vector<Use_effect> use_effects;

    Race race;
    int level;
};

Character character_setup(const Armory& armory, const std::string& race, const std::vector<std::string>& armor_vec,
                          const std::vector<std::string>& weapons_vec, const std::vector<std::string>& buffs_vec,
                          const std::vector<std::string>& talent_string, const std::vector<int>& talent_val,
                          const std::vector<std::string>& ench_vec, const std::vector<std::string>& gem_vec);

Race get_race(const std::string& race);

Character get_character_of_race(const std::string& race);

std::ostream& operator<<(std::ostream& os, const Race& race);

std::ostream& operator<<(std::ostream& os, const Character& character);

#endif // WOW_SIMULATOR_CHARACTER_HPP
