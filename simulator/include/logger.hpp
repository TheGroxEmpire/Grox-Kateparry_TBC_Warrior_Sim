#ifndef WOW_SIMULATOR_LOGGER_HPP
#define WOW_SIMULATOR_LOGGER_HPP

class Logger
{
public:
    Logger() = default;

    Logger(const Time_keeper& time_keeper) : display_combat_debug_(true), time_keeper_(&time_keeper) {}

    void print_statement(const std::string& t) { debug_topic_ += t; }

    void print_statement(int t) { debug_topic_ += std::to_string(t); }

    void print_statement(double t) { debug_topic_ += std::to_string(t); }

    template <typename... Args>
    void print(Args&&... args)
    {
        if (display_combat_debug_)
        {
            //            s. Loop idx:" + std::to_string(                    time_keeper_.step_index) +=
            debug_topic_ += "Time: " + std::to_string(time_keeper_->time) + "s. Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void)print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

    [[nodiscard]] std::string get_debug_topic() const { return debug_topic_; }

private:
    bool display_combat_debug_{};
    const Time_keeper* time_keeper_{nullptr};

    std::string debug_topic_{};
};

#endif // WOW_SIMULATOR_LOGGER_HPP
