#ifndef WOW_SIMULATOR_LOGGER_HPP
#define WOW_SIMULATOR_LOGGER_HPP

#include "time_keeper.hpp"
#include "string_helpers.hpp"

class Logger
{
public:
    Logger() = default;

    explicit Logger(const Time_keeper& time_keeper) : display_combat_debug_(true), time_keeper_(&time_keeper)
    {
        if (display_combat_debug_) debug_topic_.reserve(128 * 1024); // just a hunch ;)
    }

    void reset() { debug_topic_.clear(); }

    [[nodiscard]] bool is_enabled() const { return display_combat_debug_; }

    [[nodiscard]] std::string get_debug_topic() const { return debug_topic_; }

    template <typename... Args>
    void print(Args&&... args)
    {
        if (display_combat_debug_)
        {
            debug_topic_ += "Time: " + String_helpers::string_with_precision(time_keeper_->time * 0.001, 3) + "s. ";
            __attribute__((unused)) int dummy[] = {0, ((void)print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

private:
    void print_statement(const std::string& t) { debug_topic_ += t; }

    void print_statement(int t) { debug_topic_ += std::to_string(t); }

    void print_statement(double t) { debug_topic_ += String_helpers::string_with_precision(t, 3); }

    bool display_combat_debug_{};
    const Time_keeper* time_keeper_{nullptr};

    std::string debug_topic_{};
};

#endif // WOW_SIMULATOR_LOGGER_HPP
