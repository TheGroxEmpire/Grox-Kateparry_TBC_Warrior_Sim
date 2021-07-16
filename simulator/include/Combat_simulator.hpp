#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include "Buff_manager.hpp"
#include "Character.hpp"
#include "Distribution.hpp"
#include "damage_sources.hpp"
#include "find_values.hpp"
#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "string_helpers.hpp"
#include "time_keeper.hpp"
#include "weapon_sim.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <map>
#include <vector>

struct Combat_simulator_config
{
    Combat_simulator_config() = default;

    explicit Combat_simulator_config(const Sim_input& input)
    {
        get_combat_simulator_config(input);

        n_batches = static_cast<int>(
            String_helpers::find_value(input.float_options_string, input.float_options_val, "n_simulations_dd"));
        if (String_helpers::find_string(input.options, "item_strengths") ||
            String_helpers::find_string(input.options, "wep_strengths") || !input.stat_weights.empty() ||
            String_helpers::find_string(input.options, "compute_dpr"))
        {
            if (n_batches < 100000)
            {
                std::cout << "Increased the number of simulations to 100000 to improve later calculations."
                          << std::endl;
                n_batches = 100000;
            }
        }
        seed = 110000;
    };

    explicit Combat_simulator_config(const Sim_input_mult& input)
    {
        get_combat_simulator_config(input);
        seed = clock();
        performance_mode = true;
    };

    template <typename T>
    void get_combat_simulator_config(const T& input);

    // Combat settings
    int n_batches{};
    double sim_time{};

    int main_target_level{};
    int main_target_initial_armor_{};
    int extra_target_level{};
    int number_of_extra_targets{};
    double extra_target_duration{};
    int extra_target_initial_armor_{};
    int n_sunder_armor_stacks = 0;
    bool exposed_armor{false};
    bool curse_of_recklessness_active{false};
    bool faerie_fire_feral_active{false};

    bool take_periodic_damage_{};
    bool can_trigger_enrage_{};
    int periodic_damage_amount_{};
    int periodic_damage_interval_{};
    bool essence_of_the_red_{};

    double execute_phase_percentage_{};

    bool multi_target_mode_{};
    bool first_global_sunder_{};

    double berserking_haste_{};
    double unleashed_rage_start_{};

    // Simulator settings
    bool enable_bloodrage{false};
    bool enable_recklessness{false};
    bool enable_blood_fury{false};
    bool enable_berserking{false};
    bool enable_unleashed_rage{false};

    bool display_combat_debug{false};
    bool performance_mode{false};
    int seed{};

    struct combat_t
    {
        bool use_bt_in_exec_phase{false};
        bool use_ms_in_exec_phase{false};
        bool use_ww_in_exec_phase{false};
        bool use_sl_in_exec_phase{false};
        bool use_hs_in_exec_phase{false};
        double whirlwind_rage_thresh{};
        double overpower_rage_thresh{};
        double whirlwind_bt_cooldown_thresh{};
        double bt_whirlwind_cooldown_thresh{};
        double overpower_bt_cooldown_thresh{};
        double overpower_ww_cooldown_thresh{};
        double heroic_strike_rage_thresh{};
        double cleave_rage_thresh{};
        double heroic_strike_damage{};
        bool cleave_if_adds{false};
        bool use_hamstring{false};
        bool use_slam{false};
        bool use_bloodthirst{false};
        bool use_rampage{false};
        bool use_mortal_strike{false};
        bool use_sweeping_strikes{false};
        bool use_whirlwind{false};
        bool use_overpower{false};
        bool use_heroic_strike{false};
        double hamstring_cd_thresh{};
        bool dont_use_hm_when_ss{false};
        double slam_latency{};
        double rampage_use_thresh{};
        double hamstring_thresh_dd{};
        double initial_rage{};
        bool deep_wounds{false};
        bool first_hit_heroic_strike{false};
        double slam_spam_rage{false};
        double slam_spam_max_time{false};
        double slam_rage_dd{false};
        bool use_death_wish{false};
    } combat;

    struct dpr_t
    {
        bool compute_dpr_sl_{false};
        bool compute_dpr_sl_no_latency_{false};
        bool compute_dpr_ms_{false};
        bool compute_dpr_bt_{false};
        bool compute_dpr_op_{false};
        bool compute_dpr_ww_{false};
        bool compute_dpr_ex_{false};
        bool compute_dpr_ha_{false};
        bool compute_dpr_hs_{false};
        bool compute_dpr_cl_{false};
    } dpr_settings;

    struct talents_t
    {
        bool death_wish{false};
        bool anger_management{false};
        bool endless_rage{false};
        bool rampage{false};
        int improved_heroic_strike = 0;
        int flurry = 0;
        int unbridled_wrath = 0;
        int mace_specialization = 0;
        int impale = 0;
        int overpower = 0;
        int improved_execute = 0;
        int dual_wield_specialization = 0;
        int improved_cleave = 0;
        int improved_slam = 0;
        int tactical_mastery = 0;
        int deep_wounds = 0;
        int bloodthirst = 0;
        int mortal_strike = 0;
        int sweeping_strikes = 0;
        int improved_disciplines = 0;
        int improved_mortal_strike = 0;
        int weapon_mastery = 0;
        int precision = 0;
        int improved_whirlwind = 0;
        int improved_berserker_stance = 0;
    } talents;

    struct set_bonus_effect_t
    {
        int warbringer_2_set{0};
        bool warbringer_4_set{false};
    } set_bonus_effect;
};

class Combat_simulator
{
public:
    Combat_simulator() = default;

    virtual ~Combat_simulator() = default;

    void set_config(const Combat_simulator_config& new_config);

    enum class Hit_result
    {
        miss,
        dodge,
        glancing,
        crit,
        hit,
        TBD
    };

    enum class Hit_type
    {
        white,
        yellow
    };

    struct Ability_queue_manager
    {
        bool is_ability_queued() { return heroic_strike_queued || cleave_queued; }

        void queue_heroic_strike()
        {
            heroic_strike_queued = true;
            cleave_queued = false;
        }

        void queue_cleave()
        {
            heroic_strike_queued = false;
            cleave_queued = true;
        }

        void reset()
        {
            heroic_strike_queued = false;
            cleave_queued = false;
        }

        bool heroic_strike_queued{false};
        bool cleave_queued{false};
    };

    struct Slam_manager
    {
        Slam_manager() = default;

        void reset()
        {
            slam_casting_ = false;
            slam_cast_time_stamp_ = 0.0;
        };

        bool is_slam_casting() { return slam_casting_; }

        void cast_slam(double time_stamp)
        {
            slam_casting_ = true;
            slam_cast_time_stamp_ = time_stamp;
        }

        void finish_slam() { slam_casting_ = false; }

        double time_left(double current_time)
        {
            return slam_casting_ ? slam_cast_time_ - (current_time - slam_cast_time_stamp_) : 100.0;
        }

        double slam_cast_time_ = 0.0;

    private:
        bool slam_casting_{false};
        double slam_cast_time_stamp_ = 0.0;
    };

    struct Hit_outcome
    {
        Hit_outcome() : damage(0), hit_result(Hit_result::TBD), rageDamage(0) {}
        Hit_outcome(double damage, Hit_result hit_result) : damage(damage), hit_result(hit_result), rageDamage(damage) {}
        Hit_outcome(double damage, Hit_result hit_result, double rageDamage) : damage(damage), hit_result(hit_result), rageDamage(rageDamage) {}

        double damage;
        Hit_result hit_result;
        double rageDamage;
    };

    class Damage_multipliers
    {
    public:
        Damage_multipliers(double glance_multiplier, double crit_multiplier, double hit_multiplier)
                : glance_multiplier_(glance_multiplier), crit_multiplier_(crit_multiplier), hit_multiplier_(hit_multiplier)
        {
        }

        Damage_multipliers() = default;

        [[nodiscard]] double glance() const { return glance_multiplier_; }
        [[nodiscard]] double crit() const { return crit_multiplier_; }
        [[nodiscard]] double hit() const { return hit_multiplier_; }

    private:
        double glance_multiplier_;
        double crit_multiplier_;
        double hit_multiplier_;
    };

    class Hit_table
    {
    public:
        Hit_table(std::string name, double miss, double dodge, double glance, double crit, const Damage_multipliers& dm)
                : name_(std::move(name)), miss_(miss), dodge_(miss + dodge), glance_(miss + dodge + glance), crit_(miss + dodge + glance + crit), dm_(dm)
        {
        }

        Hit_table() = default;

        [[nodiscard]] const std::string& name() const { return name_; }

        [[nodiscard]] bool isMissOrDodge() const { return rand() * 100.0 / RAND_MAX < dodge_; }

        [[nodiscard]] double miss() const { return miss_; }
        [[nodiscard]] double dodge() const { return dodge_ - miss_; }
        [[nodiscard]] double glance() const { return glance_ - dodge_; }
        [[nodiscard]] double crit() const { return crit_ <= 100 ? crit_ - glance_ : 100 - glance_; }
        [[nodiscard]] double hit() const { return crit_ <= 100 ? 100 - crit_ : 0; }

        [[nodiscard]] double glancing_penalty() const { return dm_.glance(); }

        [[nodiscard]] Hit_outcome generate_hit(double damage) const
        {
            auto roll = rand() * 100.0 / RAND_MAX;
            if (roll < miss_) return Hit_outcome(0, Hit_result::miss);
            if (roll < dodge_) return Hit_outcome(0, Hit_result::dodge, damage * dm_.hit());
            if (roll < glance_) return Hit_outcome(damage * dm_.glance(), Hit_result::glancing);
            if (roll < crit_) return Hit_outcome(damage * dm_.crit(), Hit_result::crit);
            return Hit_outcome(damage * dm_.hit(), Hit_result::hit);
        }
    private:
        std::string name_;

        double miss_;
        double dodge_;
        double glance_;
        double crit_;

        Damage_multipliers dm_;
    };

    void manage_flurry_rampage(Hit_result hit_result, Special_stats& special_stats, int& flurry_charges, int& rampage_stacks, bool rampage_active = false,
                       bool is_ability = false);

    void unbridledWrathAndMaceSpec(const Weapon_sim& weapon, double &rage);

    void swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                      Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false, double attack_power_bonus = 0,
                      bool is_extra_attack = false);
   
    void hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false, bool is_extra_attack = false,
                     bool is_instant = true);

    void overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    bool start_cast_slam(bool mh_swing, double rage, double& swing_time_left);

    void slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage, Damage_sources& damage_sources,
              int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    void mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                       Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    void bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                     Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    void whirlwind(Weapon_sim& main_hand_weapon, Weapon_sim& off_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false, bool is_dw = false);

    void execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    void hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                   Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active = false);

    void simulate(const Character& character, size_t n_simulations, double init_mean, double init_variance,
                  size_t init_simulations);

    void simulate(const Character& character, int init_iteration = 0, bool log_data = false, bool reset_dps = true);

    static double get_uniform_random(double r_max) { return rand() * r_max / RAND_MAX; }

    [[nodiscard]] double rage_generation(const Hit_outcome& hit_outcome, const Weapon_sim& weapon) const;

    void cout_damage_parse(const Weapon_sim& weapon, const Hit_table& hit_table, const Combat_simulator::Hit_outcome& hit_outcome);

    Hit_outcome generate_hit(const Weapon_sim& main_hand_weapon, double damage, const Weapon_sim& weapon, const Hit_table& hit_table,
                             const Special_stats& special_stats, Damage_sources& damage_sources, bool boss_target = true, bool can_sweep = true);

    void compute_hit_tables(const Special_stats& special_stats, const Weapon_sim& weapon);

    std::vector<std::pair<double, Use_effect>> get_use_effect_order(const Character& character);

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_mh() const { return hit_table_white_mh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_oh() const { return hit_table_white_oh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_oh_queued() const { return hit_table_white_oh_queued_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_yellow_mh() const { return hit_table_yellow_mh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_yellow_oh() const { return hit_table_yellow_oh_; }

    void add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances);

    [[nodiscard]] std::vector<std::string> get_aura_uptimes() const;

    [[nodiscard]] std::map<std::string, double> get_aura_uptimes_map() const { return buff_manager_.aura_uptime; };

    [[nodiscard]] std::map<std::string, int> get_proc_data() const { return proc_data_; };

    [[nodiscard]] std::vector<std::string> get_proc_statistics() const;

    void reset_time_lapse();

    [[nodiscard]] std::vector<std::vector<double>> get_damage_time_lapse() const;

    [[nodiscard]] std::string get_debug_topic() const;

    Damage_sources get_damage_distribution() { return damage_distribution_; }

    [[nodiscard]] constexpr Distribution get_dps_distribution() const { return dps_distribution_; }

    [[nodiscard]] constexpr double get_dps_mean() const { return dps_distribution_.mean_; }

    [[nodiscard]] constexpr double get_dps_variance() const { return dps_distribution_.variance_; }

    [[nodiscard]] constexpr int get_n_simulations() const { return config.n_batches; }

    [[nodiscard]] constexpr double get_rage_lost_stance() const { return rage_lost_stance_swap_; }

    [[nodiscard]] constexpr double get_rage_lost_capped() const { return rage_lost_capped_; }

    [[nodiscard]] constexpr double get_avg_rage_spent_executing() const { return avg_rage_spent_executing_; }

    std::vector<int>& get_hist_x() { return hist_x; }

    std::vector<int>& get_hist_y() { return hist_y; }

    [[nodiscard]] constexpr double get_flurry_uptime() const { return flurry_uptime_; }

    [[nodiscard]] constexpr double get_hs_uptime() const { return heroic_strike_uptime_; }

    [[nodiscard]] constexpr double get_rampage_uptime() const { return rampage_uptime_; }

    void init_histogram();

    void prune_histogram();

    void normalize_timelapse();

    std::string hit_result_to_string(Combat_simulator::Hit_result hit_result);

    void print_statement(const std::string& t) { debug_topic_ += t; }

    void print_statement(int t) { debug_topic_ += std::to_string(t); }

    void print_statement(double t) { debug_topic_ += std::to_string(t); }

    template <typename... Args>
    void simulator_cout(Args&&... args)
    {
        if (config.display_combat_debug)
        {
            //            s. Loop idx:" + std::to_string(                    time_keeper_.step_index) +=
            debug_topic_ += "Time: " + std::to_string(time_keeper_.time) + "s. Event: ";
            __attribute__((unused)) int dummy[] = {0, ((void)print_statement(std::forward<Args>(args)), 0)...};
            debug_topic_ += "<br>";
        }
    }

    Combat_simulator_config config;

    const Use_effect deathwish = {
        "Death_wish", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, 0, .20}, -10, 30, 180, true};

    const Use_effect recklessness = {
        "Recklessness", Use_effect::Effect_socket::unique, {}, {100, 0, 0}, 0, 15, 900, true};

    const Use_effect bloodrage = {"Bloodrage", Use_effect::Effect_socket::unique, {}, {}, 10, 10, 60, false,
                                  {},          {{"Bloodrage", {}, 1, 0, 1, 10}}};

    const Over_time_effect essence_of_the_red = {"Essence of the Red", {}, 20, 0, 1, 600};

    const Over_time_effect anger_management = {"Anger Management", {}, 1, 0, 3, 600};

private:
    Hit_table hit_table_white_mh_;
    Hit_table hit_table_white_oh_;
    Hit_table hit_table_yellow_mh_;
    Hit_table hit_table_yellow_oh_;
    Hit_table hit_table_overpower_;
    Hit_table hit_table_white_oh_queued_;
    Damage_sources damage_distribution_{};
    Time_keeper time_keeper_{};
    Buff_manager buff_manager_{};
    Ability_queue_manager ability_queue_manager{};
    Slam_manager slam_manager{};
    std::vector<int> hist_x{};
    std::vector<int> hist_y{};
    std::string debug_topic_{};

    Distribution dps_distribution_{};
    double armor_reduction_factor_{};
    double armor_reduction_factor_add{};
    int current_armor_red_stacks_{};
    int armor_penetration_badge_{};
    int armor_reduction_from_spells_{};
    int armor_reduction_delayed_{};
    bool recompute_mitigation_{false};
    int number_of_extra_targets_{};

    double flurry_uptime_{};
    double heroic_strike_uptime_{};
    double rampage_uptime_{};

    int execute_rage_cost_{};
    int heroic_strike_rage_cost{};

    double cleave_bonus_damage_{};
    double rage_spent_on_execute_{};
    double avg_rage_spent_executing_{};
    double rage_lost_stance_swap_{};
    double rage_lost_capped_{};
    double p_mace_spec_{};
    double p_unbridled_wrath_{};
    double flurry_haste_factor_{};
    double dual_wield_damage_factor_{};

    double tactical_mastery_rage_{0};
    bool deep_wounds_{false};
    bool use_bloodthirst_{false};
    bool use_rampage_{false};
    bool use_mortal_strike_{false};
    bool use_sweeping_strikes_{false};
    int sweeping_strikes_charges_ = 0;

    std::vector<std::vector<double>> damage_time_lapse{};
    std::map<std::string, int> proc_data_{};
    std::vector<Use_effect> use_effects_all_{};
    std::vector<Over_time_effect> over_time_effects_{};
    std::map<Damage_source, int> source_map{
        {Damage_source::white_mh, 0},         {Damage_source::white_oh, 1},          {Damage_source::bloodthirst, 2},
        {Damage_source::execute, 3},          {Damage_source::heroic_strike, 4},     {Damage_source::cleave, 5},
        {Damage_source::whirlwind, 6},        {Damage_source::hamstring, 7},         {Damage_source::deep_wounds, 8},
        {Damage_source::item_hit_effects, 9}, {Damage_source::overpower, 10},        {Damage_source::slam, 11},
        {Damage_source::mortal_strike, 12},   {Damage_source::sweeping_strikes, 13}};
};

#include "Combat_simulator.tcc"

#endif // WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
