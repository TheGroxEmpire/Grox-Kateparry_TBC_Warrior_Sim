#include "Attributes.hpp"

#include <cmath>
#include <iostream>

double multiplicative_addition(double val1, double val2)
{
    return (1 + val1) * (1 + val2) - 1;
}

double multiplicative_subtraction(double val1, double val2)
{
    return (1 + val1) / (1 + val2) - 1;
}

std::ostream& operator<<(std::ostream& os, Attributes const& a)
{
    if (a.strength > 0) os << "strength = " << a.strength << " ";
    if (a.agility > 0) os << "agility = " << a.agility << " ";
    return os;
}

double as_rating(double raw, double factor)
{
    auto x = raw * factor * 82 / 52.0;
    auto rv = std::rint(x);
    if (std::abs(x - rv) > 0.05) std::cerr << "raw = " << raw << ", factor = " << factor << " seems off (" << x << " vs. " << rv << ")" << std::endl;
    return rv;
}

std::ostream& operator<<(std::ostream& os, const Special_stats& ss)
{
    // this covers about 99% of all cases
    if (ss.attack_power > 0) os << "attack power = " << ss.attack_power << " ";
    if (ss.critical_strike > 0) os << "crit = " << as_rating(ss.critical_strike, 14) << " ";
    if (ss.hit > 0) os << "hit = " << as_rating(ss.hit, 10) << " ";
    if (ss.expertise > 0) os << "expertise = " << as_rating(ss.expertise, 2.5) << " ";
    if (ss.haste > 0) os << "haste = " << as_rating(ss.haste, 10) << " ";
    if (ss.gear_armor_pen > 0) os << "arpen = " << ss.gear_armor_pen << " ";
    return os;
}