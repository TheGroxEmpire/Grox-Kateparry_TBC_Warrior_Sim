#ifndef WOW_SIMULATOR_TIME_KEEPER_HPP
#define WOW_SIMULATOR_TIME_KEEPER_HPP
#include <limits>

class Time_keeper
{
public:
    Time_keeper() = default;

    void increment(int next_event)
    {
        time = next_event;
    }

    void reset()
    {
        mortal_strike_cd_ = -1;
        sweeping_strikes_cd_ = -1;
        blood_thirst_cd_ = -1;
        rampage_cd_ = -1;
        overpower_cd_ = -1;
        whirlwind_cd_ = -1;
        global_cd_ = -1;

        time = -1;

        overpower_aura_ = -1;
        rampage_aura_ = -1;
    }

    void prepare(int prepare_time)
    {
        global_cd_ = prepare_time;
        time = prepare_time;
    }

    [[nodiscard]] int get_next_event(int next_mh_swing, int next_oh_swing,
                                     int next_buff_event, int next_slam_finish, int sim_time) const
    {
        int next_event = std::numeric_limits<int>::max();
        if (overpower_cd_ > time && overpower_cd_ < next_event) next_event = overpower_cd_;
        if (sweeping_strikes_cd_ > time && sweeping_strikes_cd_ < next_event) next_event = sweeping_strikes_cd_;
        if (mortal_strike_cd_ > time && mortal_strike_cd_ < next_event) next_event = mortal_strike_cd_;
        if (blood_thirst_cd_ > time && blood_thirst_cd_ < next_event) next_event = blood_thirst_cd_;
        if (rampage_cd_ > time && rampage_cd_ < next_event) next_event = rampage_cd_;
        if (whirlwind_cd_ > time && whirlwind_cd_ < next_event) next_event = whirlwind_cd_;
        if (global_cd_ > time && global_cd_ < next_event) next_event = global_cd_;
        if (next_mh_swing > time && next_mh_swing < next_event) next_event = next_mh_swing;
        if (next_oh_swing > time && next_oh_swing < next_event) next_event = next_oh_swing;
        if (next_slam_finish > time && next_slam_finish < next_event) next_event = next_slam_finish;
        if (next_buff_event < next_event) next_event = next_buff_event;
        if (sim_time < next_event) next_event = sim_time;
        return next_event;
    }

    void overpower_cast(int cd) { overpower_cd_ = time + cd; }
    [[nodiscard]] bool overpower_ready() const { return overpower_cd_ <= time; }
    [[nodiscard]] int overpower_cd() const { return overpower_cd_ - time; }

    void rampage_cast(int cd) { rampage_cd_ = time + cd; }
    [[nodiscard]] bool rampage_ready() const { return rampage_cd_ <= time; }
    [[nodiscard]] int rampage_cd() const { return rampage_cd_ - time; }

    void sweeping_strikes_cast(int cd) { sweeping_strikes_cd_ = time + cd; }
    [[nodiscard]] bool sweeping_strikes_ready() const { return sweeping_strikes_cd_ <= time; }
    [[nodiscard]] int sweeping_strikes_cd() const { return sweeping_strikes_cd_ - time; }

    void blood_thirst_cast(int cd) { blood_thirst_cd_ = time + cd; }
    [[nodiscard]] bool blood_thirst_ready() const { return blood_thirst_cd_ <= time; }
    [[nodiscard]] int blood_thirst_cd() const { return blood_thirst_cd_ - time; }

    void mortal_strike_cast(int cd) { mortal_strike_cd_ = time + cd; }
    [[nodiscard]] bool mortal_strike_ready() const { return mortal_strike_cd_ <= time; }
    [[nodiscard]] int mortal_strike_cd() const { return mortal_strike_cd_ - time; }

    void whirlwind_cast(int cd) { whirlwind_cd_ = time + cd; }
    [[nodiscard]] bool whirlwind_ready() const { return whirlwind_cd_ <= time; }
    [[nodiscard]] int whirlwind_cd() const { return whirlwind_cd_ - time; }

    void global_cast(int cd) { global_cd_ = time + cd; }
    [[nodiscard]] bool global_ready() const { return global_cd_ <= time; }
    [[nodiscard]] int global_cd() const { return global_cd_ - time; }

    void gain_overpower_aura() { overpower_aura_ = time + 5000; }
    [[nodiscard]] bool can_do_overpower() const { return time <= overpower_aura_; }

    void gain_rampage_aura() { rampage_aura_ = time + 5000; }
    [[nodiscard]] bool can_do_rampage() const { return time <= rampage_aura_; }

    int time;

private:
    int overpower_cd_;
    int rampage_cd_;
    int sweeping_strikes_cd_;
    int blood_thirst_cd_;
    int mortal_strike_cd_;
    int whirlwind_cd_;
    int global_cd_;

    int overpower_aura_;
    int rampage_aura_;
};

#endif // WOW_SIMULATOR_TIME_KEEPER_HPP
