#ifndef COMBAT_SIMULATOR_CONFIG_HPP
#define COMBAT_SIMULATOR_CONFIG_HPP

#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "string_helpers.hpp"
#include "find_values.hpp"
#include "time_keeper.hpp"

struct Combat_simulator_config
{
    Combat_simulator_config() = default;

    explicit Combat_simulator_config(const Sim_input& input);
    explicit Combat_simulator_config(const Sim_input_mult& input);

    template <typename T>
    void parse_combat_simulator_config(const T& input);

    //void set_display_histogram(bool display_histogram_input) { display_histogram = display_histogram_input; }
    //void set_display_time_lapse(bool display_time_lapse_input) { display_time_lapse = display_time_lapse_input; }

    [[nodiscard]] static int to_millis(double seconds) { return Time_keeper::to_millis(seconds); }

    int n_batches{};

    bool display_combat_debug{};
    //bool display_histogram{};
    //bool display_time_lapse{};
    int seed{};

    double sim_time{};

    int main_target_level{};
    int main_target_initial_armor_{};

    int n_sunder_armor_stacks{};
    bool exposed_armor{};
    bool curse_of_recklessness_active{};
    bool faerie_fire_feral_active{};

    bool multi_target_mode_{};
    int number_of_extra_targets{};
    double extra_target_percentage{};
    int extra_target_initial_armor_{};

    bool take_periodic_damage_{};
    int periodic_damage_amount_{};
    int periodic_damage_interval_{};
    bool essence_of_the_red_{};

    double execute_phase_percentage_{};

    double initial_rage{};
    int sunder_armor_globals_{};

    bool enable_bloodrage{};
    bool enable_recklessness{};
    bool enable_blood_fury{};
    bool enable_berserking{};
    double berserking_haste_{};
    bool use_death_wish{};
    bool use_sweeping_strikes{};

    bool enable_unleashed_rage{};
    double unleashed_rage_start_{};

    bool deep_wounds{};

    struct combat_t
    {
        bool use_bloodthirst{};
        bool use_bt_in_exec_phase{};
        int bt_whirlwind_cooldown_thresh{};

        bool use_mortal_strike{};
        bool use_ms_in_exec_phase{};
        int ms_whirlwind_cooldown_thresh{};

        bool use_whirlwind{};
        bool use_ww_in_exec_phase{};
        double whirlwind_rage_thresh{};
        int whirlwind_bt_cooldown_thresh{};

        bool use_slam{};
        bool use_sl_in_exec_phase{};
        double slam_rage_thresh{};
        int slam_spam_max_time{};
        double slam_spam_rage{};
        int slam_latency{};

        bool use_rampage{};
        bool use_ra_in_exec_phase{};
        int rampage_use_thresh{};

        bool use_heroic_strike{};
        bool use_hs_in_exec_phase{};
        bool first_hit_heroic_strike{};
        double heroic_strike_rage_thresh{}; // this is now used for execute phase as well

        bool cleave_if_adds{};
        double cleave_rage_thresh{};

        bool use_overpower{};
        double overpower_rage_thresh{};
        int overpower_bt_cooldown_thresh{};
        int overpower_ww_cooldown_thresh{};

        bool use_hamstring{};
        double hamstring_rage_thresh{};
        int hamstring_cd_thresh{};
        bool dont_use_hm_when_ss{};

        bool use_sunder_armor{};
        double sunder_armor_rage_thresh{};
        int sunder_armor_cd_thresh{};
    } combat;

    struct dpr_t
    {
        bool compute_dpr_sl_{};
        bool compute_dpr_ms_{};
        bool compute_dpr_bt_{};
        bool compute_dpr_op_{};
        bool compute_dpr_ww_{};
        bool compute_dpr_ex_{};
        bool compute_dpr_ha_{};
        bool compute_dpr_hs_{};
        bool compute_dpr_cl_{};
    } dpr_settings;
};

#include "Config.tcc"

#endif
