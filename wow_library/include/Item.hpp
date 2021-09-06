#ifndef WOW_SIMULATOR_ITEM_HPP
#define WOW_SIMULATOR_ITEM_HPP

#include "Attributes.hpp"

#include <cassert>
#include <utility>
#include <cmath>

[[nodiscard]] static int to_millis(double s) { return static_cast<int>(std::rint(1000 * s)); }

enum class Socket
{
    none,
    head,
    neck,
    shoulder,
    back,
    chest,
    wrist,
    hands,
    belt,
    legs,
    boots,
    ring,
    trinket,
    main_hand,
    off_hand,
    ranged,
};

enum class Weapon_socket
{
    main_hand,
    one_hand,
    off_hand,
    two_hand
};

std::ostream& operator<<(std::ostream& os, const Weapon_socket& ws);

enum class Weapon_type
{
    sword,
    axe,
    dagger,
    mace,
    unarmed
};

std::ostream& operator<<(std::ostream& os, const Weapon_type& wt);

enum class Set
{
    none,
    ragesteel,
    wastewalker,
    doomplate,
    warbringer,
    destroyer,
};

/* enum class Gem_bonus
{
    none
};


struct Gem
{
    enum class Type
    {
    strength3, 
    strength5,
    strength4, 
    strength6, 
    strength8,
    agility3, 
    agility4, 
    agility5,
    agility6, 
    agility10, 
    AP20, 
    AP24,
    crit3,  
    crit4,
    crit5, 
    crit6, 
    crit8, 
    crit10, 
    crit12, 
    hit4, 
    hit6, 
    hit8, 
    hit10, 
    hit12,
    dmg3,  
    agi12_critDmg3, 
    haste_proc,
    crit3_str3, 
    crit4_str4, 
    crit4_str5, 
    crit5_str5,
    empty,
    none
    };

    Gem() = default;

    explicit Gem(Type type) : type(type){};

    Type type{};
    Attributes attributes{};
    Special_stats special_stats{};
}; */


struct Over_time_effect
{
    Over_time_effect() = default;

    Over_time_effect(std::string name, Special_stats special_stats, double rage_gain, double damage,
                     int interval, int duration) :
        name(std::move(name)),
        special_stats(special_stats),
        rage_gain(rage_gain),
        damage(damage),
        interval(1000 * interval),
        duration(1000 * duration) {};

    std::string name{};
    Special_stats special_stats{}; // unused (?)
    double rage_gain{};
    double damage{};
    int interval{};
    int duration{};

    int over_time_buff_idx{-1};
};

// TODO(vigo) remove or repurpose some fields
//  Hit_effect (or Proc, for brevity) should describe /how/ something is triggered:
//   - proc chance (ppm, percentage, always)
//   - proc conditions (specific or both weapons, required Hit_result and Hit_type)
//   - whether it stacks or has charges
//   - internal cooldown
//  A proc grants a number of buffs or one-shot-effects (extra attack, rage),
//   but typically only one.
class Hit_effect
{
public:
    enum class Type
    {
        none,
        extra_hit,
        windfury_hit,
        sword_spec,
        stat_boost,
        damage_physical,
        damage_magic,
        reduce_armor, // deprecated
        rage_boost,
    };

    Hit_effect() = default;

    Hit_effect(std::string name, Type type, Attributes attribute_boost, Special_stats special_stats_boost,
               double damage, double duration, double cooldown, double probability, double attack_power_boost = 0, int max_charges = 1,
               int armor_reduction = 0, int max_stacks = 1, double ppm = 0.0, bool affects_both_weapons = false) :
        name(std::move(name)),
        type(type),
        attribute_boost(attribute_boost),
        special_stats_boost(special_stats_boost),
        damage(damage),
        duration(to_millis(duration)),
        cooldown(to_millis(cooldown)),
        probability(probability),
        attack_power_boost(attack_power_boost), // unused
        max_charges(max_charges),
        armor_reduction(armor_reduction), // unused
        ppm(ppm),
        affects_both_weapons(affects_both_weapons), // unused
        max_stacks(max_stacks) {}

    void sanitize()
    {
        if (type == Type::reduce_armor)
        {
            type = Type::stat_boost;
            special_stats_boost.gear_armor_pen = armor_reduction;
        }

        if (max_charges == 0)
        {
            max_charges = 1;
        }

        if (max_stacks == 0)
        {
            max_stacks = 1;
        }
    }

    [[nodiscard]] Special_stats to_special_stats(const Special_stats& multipliers) const
    {
        return special_stats_boost + attribute_boost.to_special_stats(multipliers);
    }

    std::string name{};
    Type type{};
    Attributes attribute_boost{};
    Special_stats special_stats_boost{};
    double damage{};
    int duration{};
    int cooldown{};
    double probability{};
    double attack_power_boost{}; // unused; was in use for windfury_totem only
    int max_charges{1}; // used to be "n_targets", and unused; now used for charges (for windfury attack or flurry)
    int armor_reduction{}; // deprecated, use special_stats_boost.gear_armor_pen instead (3 usages)
    double ppm{};
    bool affects_both_weapons{}; // unused
    int max_stacks{1};

    int time_counter{}; // "next_ready", aka cooldown end

    int procs{}; // statistics

    int combat_buff_idx{-1}; // "link" to combat buff
};

std::ostream& operator<<(std::ostream& os, const Hit_effect::Type& t);

std::ostream& operator<<(std::ostream& os, const Hit_effect& he);

class Use_effect
{
public:
    enum class Effect_socket
    {
        shared,
        unique,
    };

    Use_effect() = default;

    Use_effect(const std::string& name, Effect_socket effect_socket, Attributes attribute_boost,
               Special_stats special_stats_boost, double rage_boost, double duration, double cooldown,
               bool triggers_gcd, std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
               std::vector<Over_time_effect> over_time_effects = std::vector<Over_time_effect>()) :
        name(name),
        effect_socket(effect_socket),
        rage_boost(rage_boost),
        duration(to_millis(duration)),
        cooldown(to_millis(cooldown)),
        triggers_gcd(triggers_gcd),
        hit_effects(std::move(hit_effects)),
        over_time_effects(std::move(over_time_effects)),
        combat_buff({name, Hit_effect::Type::stat_boost, attribute_boost, special_stats_boost, 0, duration, 0, 0})
    {}

    [[nodiscard]] Special_stats to_special_stats(const Special_stats& multipliers) const
    {
        return combat_buff.to_special_stats(multipliers);
    }

    std::string name{};
    Effect_socket effect_socket{};
    double rage_boost{};
    int duration{};
    int cooldown{};
    bool triggers_gcd{};
    std::vector<Hit_effect> hit_effects{};
    std::vector<Over_time_effect> over_time_effects{};
    Hit_effect combat_buff{};
};

struct Enchant
{
    enum class Type
    {
        none,
        strength,
        strength7,
        strength9,
        strength12,
        strength15,
        strength20,
        agility,
        agility12,
        greater_agility,
        haste,
        crusader,
        minor_stats,
        major_stats,
        ring_stats,
        ring_damage,
        attack_power,
        naxxramas,
        damage,
        ferocity,
        greater_vengeance,
        greater_blade,
        exceptional_stats,
        cobrahide,
        nethercobra,
        mongoose,
        hit,
        cats_swiftness,
        executioner,
    };

    Enchant() = default;

    explicit Enchant(Type type) : type(type){};

    Type type{};
    Attributes attributes{};
    Special_stats special_stats{};
};

struct Set_bonus
{
    Set_bonus(Set set, int pieces, std::string name, Attributes attributes = {}, Special_stats special_stats = {}, Hit_effect hit_effect = {})
        : set(set), pieces(pieces), name(std::move(name)), attributes(attributes), special_stats(special_stats), hit_effect(std::move(hit_effect)) { }

    Set set;
    int pieces;
    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    Hit_effect hit_effect;
};

struct Buff
{
    Buff(std::string name, Attributes attributes, Special_stats special_stats, double bonus_damage = 0,
         std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
         std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , bonus_damage(bonus_damage)
        , hit_effects(std::move(hit_effects))
        , use_effects(std::move(use_effects)){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    double bonus_damage;
    std::vector<Hit_effect> hit_effects{};
    std::vector<Use_effect> use_effects{};
};

struct Weapon_buff
{
    Weapon_buff() = default;

    Weapon_buff(std::string name, Attributes attributes, Special_stats special_stats, double bonus_damage = 0, Hit_effect hit_effect = {})
        : name(std::move(name)), attributes(attributes), special_stats(special_stats), bonus_damage(bonus_damage), hit_effect(std::move(hit_effect)) {};

    std::string name{};
    Attributes attributes{};
    Special_stats special_stats{};
    double bonus_damage{};
    Hit_effect hit_effect{};
};

struct Gem
{
    Gem(std::string name, Attributes attributes, Special_stats special_stats, Hit_effect hit_effect = {}) :
        name(std::move(name)),
        attributes(attributes),
        special_stats(special_stats),
        hit_effect(std::move(hit_effect)) {}

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    Hit_effect hit_effect;
};

struct Armor
{
    Armor(std::string name, Attributes attributes, Special_stats special_stats, Socket socket, Set set_name = Set::none,
          std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(),
          std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , socket(socket)
        , set_name(set_name)
        , hit_effects(std::move(hit_effects))
        , use_effects(std::move(use_effects)){};
    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    Socket socket;
    Set set_name;
    Enchant enchant{};
    std::vector<Hit_effect> hit_effects{};
    std::vector<Use_effect> use_effects{};
};

struct Weapon
{
    Weapon(std::string name, Attributes attributes, Special_stats special_stats, double swing_speed, double min_damage,
           double max_damage, Weapon_socket weapon_socket, Weapon_type weapon_type,
           std::vector<Hit_effect> hit_effects = std::vector<Hit_effect>(), Set set_name = Set::none,
           std::vector<Use_effect> use_effects = std::vector<Use_effect>())
        : name(std::move(name))
        , attributes(attributes)
        , special_stats(special_stats)
        , swing_speed(swing_speed)
        , min_damage(min_damage)
        , max_damage(max_damage)
        , weapon_socket(weapon_socket)
        , type(weapon_type)
        , hit_effects(std::move(hit_effects))
        , set_name(set_name)
        , use_effects(std::move(use_effects)){};

    std::string name;
    Attributes attributes;
    Special_stats special_stats;
    double swing_speed;
    double min_damage;
    double max_damage;
    Weapon_socket weapon_socket;
    Weapon_type type;
    std::vector<Hit_effect> hit_effects;
    Set set_name;
    std::vector<Use_effect> use_effects;
    Socket socket;
    Enchant enchant;
    Weapon_buff buff; // TODO(vigo) - temporary weapon enhancements; they should at least add bonus damage
};

std::ostream& operator<<(std::ostream& os, const Socket& socket);

std::string operator+(std::string& string, const Socket& socket);

std::string operator+(std::string& string, const Weapon_socket& socket);

#endif // WOW_SIMULATOR_ITEM_HPP
