#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>
#include <vector>

double multiplicative_addition(double val1, double val2);

double multiplicative_subtraction(double val1, double val2);

struct Special_stats
{
    Special_stats() = default;

    Special_stats(double critical_strike, double hit, double attack_power, double expertise = 0.0, double chance_for_extra_hit = 0.0,
                  double haste = 0.0, double sword_expertise = 0.0, double axe_expertise = 0.0, double mace_expertise = 0.0,
                  double damage_mod_physical = 0, double stat_multiplier = 0,
                  double bonus_damage = 0, double crit_multiplier = 0, double spell_crit = 0,
                  double damage_mod_spell = 0)
        : critical_strike{critical_strike}
        , hit{hit}
        , attack_power{attack_power}
        , expertise{expertise}
        , chance_for_extra_hit(chance_for_extra_hit)
        , haste(haste)
        , sword_expertise(sword_expertise)
        , axe_expertise(axe_expertise)
        , mace_expertise(mace_expertise)
        , damage_mod_physical(damage_mod_physical)
        , stat_multiplier(stat_multiplier)
        , bonus_damage(bonus_damage)
        , crit_multiplier(crit_multiplier)
        , spell_crit(spell_crit)
        , damage_mod_spell(damage_mod_spell)
    {
    }

    Special_stats operator+(const Special_stats& rhs) const
    {
        return {
            critical_strike + rhs.critical_strike,
            hit + rhs.hit,
            attack_power + rhs.attack_power,
            expertise + rhs.expertise,
            chance_for_extra_hit + rhs.chance_for_extra_hit,
            multiplicative_addition(haste, rhs.haste),
            sword_expertise + rhs.sword_expertise,
            axe_expertise + rhs.axe_expertise,
            mace_expertise + rhs.mace_expertise,
            multiplicative_addition(damage_mod_physical, rhs.damage_mod_physical),
            multiplicative_addition(stat_multiplier, rhs.stat_multiplier),
            bonus_damage + rhs.bonus_damage,
            multiplicative_addition(crit_multiplier, rhs.crit_multiplier),
            spell_crit + rhs.spell_crit,
            multiplicative_addition(damage_mod_spell, rhs.damage_mod_spell),
        };
    }

    Special_stats operator-(const Special_stats& rhs) const
    {
        return {
            critical_strike - rhs.critical_strike,
            hit - rhs.hit,
            attack_power - rhs.attack_power,
            expertise - rhs.expertise,
            chance_for_extra_hit - rhs.chance_for_extra_hit,
            multiplicative_subtraction(haste, rhs.haste),
            sword_expertise - rhs.sword_expertise,
            axe_expertise - rhs.axe_expertise,
            mace_expertise - rhs.mace_expertise,
            multiplicative_subtraction(damage_mod_physical, rhs.damage_mod_physical),
            multiplicative_subtraction(stat_multiplier, rhs.stat_multiplier),
            bonus_damage - rhs.bonus_damage,
            multiplicative_subtraction(crit_multiplier, rhs.crit_multiplier),
            spell_crit - rhs.spell_crit,
            multiplicative_subtraction(damage_mod_spell, rhs.damage_mod_spell),
        };
    }

    Special_stats& operator+=(const Special_stats& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Special_stats& operator-=(const Special_stats& rhs)
    {
        *this = *this - rhs;
        return *this;
    }

    double critical_strike{};
    double hit{};
    double attack_power{};
    double expertise{};
    double chance_for_extra_hit{};
    double haste{};
    double sword_expertise{};
    double axe_expertise{};
    double mace_expertise{};
    double damage_mod_physical{};
    double stat_multiplier{};
    double bonus_damage{};
    double crit_multiplier{};
    double spell_crit{};
    double damage_mod_spell{};
};

class Attributes
{
public:
    Attributes() = default;

    Attributes(double strength, double agility) : strength{strength}, agility{agility} {};

    void clear()
    {
        strength = 0;
        agility = 0;
    }

    Attributes multiply(const Special_stats& special_stats) const
    {
        double multiplier = special_stats.stat_multiplier + 1;
        return {strength * multiplier, agility * multiplier};
    }

    Special_stats convert_to_special_stats(const Special_stats& special_stats) const
    {
        double multiplier = special_stats.stat_multiplier + 1;
        return {agility / 33 * multiplier, 0, strength * 2 * multiplier};
    }

    Attributes operator+(const Attributes& rhs) { return {strength + rhs.strength, agility + rhs.agility}; }

    Attributes& operator+=(const Attributes& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Attributes operator*(double rhs) { return {this->strength * rhs, this->agility * rhs}; }

    Attributes& operator*=(double rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    double strength;
    double agility;
};

std::ostream& operator<<(std::ostream& os, Special_stats const& special_stats);

std::ostream& operator<<(std::ostream& os, Attributes const& stats);

#endif // WOW_SIMULATOR_STATS_HPP