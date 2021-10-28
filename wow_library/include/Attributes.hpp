#ifndef WOW_SIMULATOR_STATS_HPP
#define WOW_SIMULATOR_STATS_HPP

#include <ostream>
#include <vector>
#include <cassert>

double multiplicative_addition(double val1, double val2);

double multiplicative_subtraction(double val1, double val2);

struct Special_stats
{
    Special_stats() = default;

    Special_stats(double critical_strike, double hit, double attack_power, double bonus_attack_power = 0,
                  double haste = 0, double damage_mod_physical = 0, double stat_multiplier = 0,
                  double bonus_damage = 0, double crit_multiplier = 0, double spell_crit = 0,
                  double damage_mod_spell = 0, double expertise = 0, double sword_expertise = 0, double mace_expertise = 0, double axe_expertise = 0,
                  int gear_armor_pen = 0, double ap_multiplier = 0, double attack_speed = 0)
        : critical_strike{critical_strike}
        , hit{hit}
        , attack_power{attack_power}
        , bonus_attack_power(bonus_attack_power)
        , haste(haste)
        , damage_mod_physical(damage_mod_physical)
        , stat_multiplier(stat_multiplier)
        , bonus_damage(bonus_damage)
        , crit_multiplier(crit_multiplier)
        , spell_crit(spell_crit)
        , damage_mod_spell(damage_mod_spell)
        , expertise{expertise}
        , sword_expertise{sword_expertise}
        , mace_expertise{mace_expertise}
        , axe_expertise{axe_expertise}
        , gear_armor_pen{gear_armor_pen}
        , ap_multiplier{ap_multiplier}
        , attack_speed{attack_speed}
    {
    }

    bool operator<(Special_stats other) const
    {
        return (this->hit < other.hit) &&
               (this->critical_strike < other.critical_strike) &&
               (this->attack_power < other.attack_power) &&
               (this->bonus_damage < other.bonus_damage) &&
               (this->damage_mod_physical < other.damage_mod_physical) &&
               (this->expertise < other.expertise) &&
               (this->sword_expertise < other.sword_expertise) &&
               (this->mace_expertise < other.mace_expertise) &&
               (this->axe_expertise < other.axe_expertise) &&
               (this->gear_armor_pen < other.gear_armor_pen);
    }

    Special_stats operator+(const Special_stats& rhs) const
    {
        assert(rhs.haste == 0 || rhs.attack_speed == 0);

        return {
            critical_strike + rhs.critical_strike,
            hit + rhs.hit,
            (attack_power + rhs.attack_power * (1 + ap_multiplier)) * (1 + rhs.ap_multiplier),
            bonus_attack_power + rhs.bonus_attack_power,
            (1 + haste + rhs.haste * (1 + attack_speed)) * (1 + rhs.attack_speed) - 1,
            multiplicative_addition(damage_mod_physical, rhs.damage_mod_physical),
            multiplicative_addition(stat_multiplier, rhs.stat_multiplier),
            bonus_damage + rhs.bonus_damage,
            multiplicative_addition(crit_multiplier, rhs.crit_multiplier),
            spell_crit + rhs.spell_crit,
            multiplicative_addition(damage_mod_spell, rhs.damage_mod_spell),
            expertise + rhs.expertise,
            sword_expertise + rhs.sword_expertise,
            mace_expertise + rhs.mace_expertise,
            axe_expertise + rhs.axe_expertise,
            gear_armor_pen + rhs.gear_armor_pen,
            multiplicative_addition(ap_multiplier, rhs.ap_multiplier),
            multiplicative_addition(attack_speed, rhs.attack_speed),
        };
    }

    Special_stats operator-(const Special_stats& rhs) const
    {
        assert(rhs.haste == 0 || rhs.attack_speed == 0);

        return {
            critical_strike - rhs.critical_strike,
            hit - rhs.hit,
            (attack_power - rhs.attack_power * (1 + ap_multiplier)) / (1 + rhs.ap_multiplier),
            bonus_attack_power - rhs.bonus_attack_power,
            (1 + haste - rhs.haste * (1 + attack_speed)) / (1 + rhs.attack_speed) - 1,
            multiplicative_subtraction(damage_mod_physical, rhs.damage_mod_physical),
            multiplicative_subtraction(stat_multiplier, rhs.stat_multiplier),
            bonus_damage - rhs.bonus_damage,
            multiplicative_subtraction(crit_multiplier, rhs.crit_multiplier),
            spell_crit - rhs.spell_crit,
            multiplicative_subtraction(damage_mod_spell, rhs.damage_mod_spell),
            expertise - rhs.expertise,
            sword_expertise - rhs.sword_expertise,
            mace_expertise - rhs.mace_expertise,
            axe_expertise - rhs.axe_expertise,
            gear_armor_pen - rhs.gear_armor_pen,
            multiplicative_subtraction(ap_multiplier, rhs.ap_multiplier),
            multiplicative_subtraction(attack_speed, rhs.attack_speed),
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
    double bonus_attack_power{}; // was chance_for_extra_hit, unused; bonus_attack_power is for modelling improved hunter's mark and expose armor
    double haste{};
    double damage_mod_physical{};
    double stat_multiplier{};
    double bonus_damage{};
    double crit_multiplier{};
    double spell_crit{};
    double damage_mod_spell{};
    double expertise{};
    double sword_expertise{};
    double mace_expertise{};
    double axe_expertise{};
    int gear_armor_pen{};

    double ap_multiplier{};
    double attack_speed{};
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

    [[nodiscard]] Attributes multiply(const Special_stats& multipliers) const
    {
        const double multiplier = multipliers.stat_multiplier + 1;
        return {strength * multiplier, agility * multiplier};
    }

    [[nodiscard]] Special_stats to_special_stats(const Special_stats& multipliers) const
    {
        const double multiplier = multipliers.stat_multiplier + 1;
        return {agility * multiplier / 33, 0, strength * multiplier * 2};
    }

    Attributes operator+(const Attributes& rhs) const { return {strength + rhs.strength, agility + rhs.agility}; }

    Attributes& operator+=(const Attributes& rhs)
    {
        *this = *this + rhs;
        return *this;
    }

    Attributes operator*(double rhs) const { return {this->strength * rhs, this->agility * rhs}; }

    Attributes& operator*=(double rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    double strength;
    double agility;
};

std::ostream& operator<<(std::ostream& os, const Special_stats& special_stats);

std::ostream& operator<<(std::ostream& os, const Attributes& stats);

#endif // WOW_SIMULATOR_STATS_HPP