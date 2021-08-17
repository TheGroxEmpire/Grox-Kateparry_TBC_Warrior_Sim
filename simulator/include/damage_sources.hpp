#ifndef WOW_SIMULATOR_DAMAGE_SOURCES_HPP
#define WOW_SIMULATOR_DAMAGE_SOURCES_HPP

#include <string>
#include <vector>
#include <cassert>

enum class Damage_source
{
    white_mh, // order is important, cp. damage_names
    white_oh,
    bloodthirst,
    execute,
    heroic_strike,
    cleave,
    whirlwind,
    hamstring,
    deep_wounds,
    item_hit_effects,
    overpower,
    slam,
    mortal_strike,
    sweeping_strikes,
    size, // convenience ;)
};

struct Damage_instance
{
    Damage_instance(Damage_source source, double damage, int time_stamp)
        : damage_source(source), damage(damage), time_stamp(time_stamp){};
    Damage_source damage_source;
    double damage{};
    int time_stamp{};
};

struct Damage_sources
{
    explicit Damage_sources(bool keep_history);

    ~Damage_sources() = default;

    Damage_sources& operator+(const Damage_sources& rhs);

    [[nodiscard]] double sum_damage_sources() const
    {
        return white_mh_damage + white_oh_damage + bloodthirst_damage + mortal_strike_damage + slam_damage +
               overpower_damage + heroic_strike_damage + cleave_damage + whirlwind_damage + hamstring_damage +
               execute_damage + deep_wounds_damage + item_hit_effects_damage + sweeping_strikes_damage;
    }

    [[nodiscard]] int sum_counts() const
    {
        return white_mh_count + white_oh_count + bloodthirst_count + mortal_strike_count + slam_count +
               overpower_count + heroic_strike_count + cleave_count + whirlwind_count + hamstring_count +
               execute_count + deep_wounds_count + item_hit_effects_count + sweeping_strikes_count;
    }

    void add_damage(Damage_source source, double damage, int time_stamp);

    double white_mh_damage{};
    double white_oh_damage{};
    double slam_damage{};
    double overpower_damage{};
    double bloodthirst_damage{};
    double mortal_strike_damage{};
    double sweeping_strikes_damage{};
    double execute_damage{};
    double heroic_strike_damage{};
    double cleave_damage{};
    double whirlwind_damage{};
    double hamstring_damage{};
    double deep_wounds_damage{};
    double item_hit_effects_damage{};

    int white_mh_count{};
    int white_oh_count{};
    int slam_count{};
    int overpower_count{};
    int bloodthirst_count{};
    int mortal_strike_count{};
    int sweeping_strikes_count{};
    int execute_count{};
    int heroic_strike_count{};
    int cleave_count{};
    int whirlwind_count{};
    int hamstring_count{};
    int deep_wounds_count{};
    int item_hit_effects_count{};

    bool keep_history{};
    std::vector<Damage_instance> damage_instances{};
};

#endif // WOW_SIMULATOR_DAMAGE_SOURCES_HPP
