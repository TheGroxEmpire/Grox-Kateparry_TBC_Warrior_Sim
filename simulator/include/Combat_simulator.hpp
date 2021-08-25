#ifndef WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
#define WOW_SIMULATOR_COMBAT_SIMULATOR_HPP

#include "Buff_manager.hpp"
#include "Character.hpp"
#include "Distribution.hpp"
#include "Rage_manager.hpp"
#include "damage_sources.hpp"
#include "find_values.hpp"
#include "logger.hpp"
#include "sim_input.hpp"
#include "sim_input_mult.hpp"
#include "sim_state.hpp"
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
    };

    template <typename T>
    void get_combat_simulator_config(const T& input);

    // Combat settings
    int n_batches{};
    double sim_time{};

    int main_target_level{};
    int main_target_initial_armor_{};
    int extra_target_level{}; // TODO(vigo) this isn't actually supported (would need a second set of hit tables)
    int number_of_extra_targets{};
    double extra_target_duration{};
    int extra_target_initial_armor_{};
    int n_sunder_armor_stacks = 0;
    bool exposed_armor{false};
    bool curse_of_recklessness_active{false};
    bool faerie_fire_feral_active{false};

    bool take_periodic_damage_{};
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
        double ms_whirlwind_cooldown_thresh{};
        double overpower_bt_cooldown_thresh{};
        double overpower_ww_cooldown_thresh{};
        double heroic_strike_rage_thresh{}; // this is now used for execute phase as well
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
        bool warbringer_2_set{false};
        bool warbringer_4_set{false};
        bool destroyer_2_set{false};
        bool destroyer_4_set{false};
    } set_bonus_effect;
};

class Combat_simulator : Rage_manager
{
public:
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
        melee,
        next_melee,
        spell,
    };

    enum class Extra_attack_type
    {
        none, // may proc no extra attacks (Sword Spec, Windfury Totem)
        self, // may proc itself (e.g. Blinkstrike on a melee/next melee hit)
        all, // may proc anything (e.g. Blinkstrike on a spell hit)
    };

    struct Ability_queue_manager
    {
        [[nodiscard]] bool is_ability_queued() const { return heroic_strike_queued || cleave_queued; }

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
        explicit Slam_manager(int slam_cast_time) : slam_cast_time_(slam_cast_time) { }

        [[nodiscard]] bool is_slam_casting() const { return is_casting_; }

        [[nodiscard]] int next_finish() const { return next_finish_; }

        void cast_slam(int time_stamp)
        {
            is_casting_ = true;
            next_finish_ = time_stamp + slam_cast_time_;
        }

        void finish_slam()
        {
            is_casting_ = false;
            next_finish_ = std::numeric_limits<int>::max();
        }

        [[nodiscard]] bool ready(double current_time) const
        {
            assert(next_finish_ >= current_time);
            return next_finish_ == current_time;
        }


    private:
        int slam_cast_time_;
        bool is_casting_{false};
        int next_finish_{std::numeric_limits<int>::max()};
    };

    struct Hit_outcome
    {
        Hit_outcome() : damage(0), hit_result(Hit_result::TBD), rage_damage(0) {}
        Hit_outcome(double damage, Hit_result hit_result) : damage(damage), hit_result(hit_result), rage_damage(damage) {}
        Hit_outcome(double damage, Hit_result hit_result, double rage_damage) : damage(damage), hit_result(hit_result), rage_damage(rage_damage) {}

        double damage;
        Hit_result hit_result;
        double rage_damage;
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

        Hit_table() : name_(), miss_(0), dodge_(0), glance_(0), crit_(0), dm_() {}

        void alter_white_crit(double crit_delta) { crit_ += crit_delta; }
        void alter_yellow_crit(double crit_delta) { crit_ += (100 - dodge_) / 100 * crit_delta; }

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
            if (roll < miss_) return {0, Hit_result::miss};
            if (roll < dodge_) return {0, Hit_result::dodge, damage * dm_.hit()};
            if (roll < glance_) return {damage * dm_.glance(), Hit_result::glancing};
            if (roll < crit_) return {damage * dm_.crit(), Hit_result::crit};
            return {damage * dm_.hit(), Hit_result::hit};
        }
    private:
        std::string name_;

        double miss_;
        double dodge_;
        double glance_;
        double crit_;

        Damage_multipliers dm_;
    };

    void gain_rage(double amount) final
    {
        rage_gained_ += amount;
        rage += amount;
        if (rage > 100)
        {
            rage_lost_capped_ += rage - 100;
            rage = 100;
        }
    }

    void spend_rage(double amount) final
    {
        assert(rage - amount >= 0);
        rage_spent_ += amount;
        rage -= amount;
    }

    void spend_all_rage() final
    {
        rage_spent_on_execute_ += rage;
        rage = 0;
    }

    void swap_stance() final
    {
        if (rage > tactical_mastery_rage_)
        {
            rage_lost_stance_swap_ += rage - tactical_mastery_rage_;
            rage = tactical_mastery_rage_;
        }
    }

    [[nodiscard]] double get_rage() const final { return rage; }

    // TODO turn us into hit effects :)
    void maybe_gain_flurry(Hit_result hit_result, int& flurry_charges, Special_stats& special_stats) const;
    void maybe_remove_flurry(int& flurry_charges, Special_stats& special_stats) const;
    void maybe_add_rampage_stack(Hit_result hit_result, int& rampage_stacks, Special_stats& special_stats);
    void unbridled_wrath(const Weapon_sim& weapon);

    void swing_weapon(Sim_state& state, Weapon_sim& weapon, Extra_attack_type extra_attack_type = Extra_attack_type::all);

    void hit_effects(Sim_state& state, Weapon_sim& weapon, Hit_type hit_type = Hit_type::spell, Extra_attack_type extra_attack_type = Extra_attack_type::all);

    void overpower(Sim_state& state);

    bool start_cast_slam(bool mh_swing, const Weapon_sim& weapon);

    void slam(Sim_state& state);

    void mortal_strike(Sim_state& state);

    void bloodthirst(Sim_state& state);

    void whirlwind(Sim_state& state);

    void execute(Sim_state& state);

    void hamstring(Sim_state& state);

    void simulate(const Character& character, int n_simulations, double init_mean, double init_variance,
                  int init_simulations);

    void simulate(const Character& character, int init_iteration = 0, bool log_data = false, bool reset_dps = true);

    void normal_phase(Sim_state& state, bool mh_swing);
    void execute_phase(Sim_state& state, bool mh_swing);
    void queue_next_melee();

    void update_swing_timers(Sim_state& state, double oldHaste);

    static double get_uniform_random(double r_max) { return rand() * r_max / RAND_MAX; }

    [[nodiscard]] double rage_generation(const Hit_outcome& hit_outcome, const Weapon_sim& weapon) const;

    void cout_damage_parse(const Weapon_sim& weapon, const Hit_table& hit_table, const Hit_outcome& hit_outcome);

    Hit_outcome generate_hit(Sim_state& state, const Weapon_sim& weapon, const Hit_table& hit_table, double damage,
                             bool boss_target = true, bool can_sweep = true);

    void compute_hit_tables(const Special_stats& special_stats, const Weapon_sim& weapon);

    void add_use_effects(const Character& character);

    void add_over_time_effects();

    Use_effects::Schedule compute_use_effects_schedule(const Character& character);

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_mh() const { return hit_table_white_mh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_oh() const { return hit_table_white_oh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_white_oh_queued() const { return hit_table_white_oh_queued_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_yellow_mh() const { return hit_table_yellow_mh_; }

    [[nodiscard]] const Hit_table& get_hit_probabilities_yellow_oh() const { return hit_table_yellow_oh_; }

    void add_damage_source_to_time_lapse(const std::vector<Damage_instance>& damage_instances);

    [[nodiscard]] std::vector<std::string> get_aura_uptimes() const;

    [[nodiscard]] std::unordered_map<std::string, double> get_aura_uptimes_map() const { return buff_manager_.get_aura_uptimes_map(); }

    [[nodiscard]] const std::unordered_map<std::string, int>& get_proc_data() const { return proc_data_; }

    [[nodiscard]] std::vector<std::string> get_proc_statistics() const;

    void reset_time_lapse();

    [[nodiscard]] const std::vector<std::vector<double>>& get_damage_time_lapse() const { return damage_time_lapse_; };

    [[nodiscard]] std::string get_debug_topic() const;

    [[nodiscard]] const Damage_sources& get_damage_distribution() const { return damage_distribution_; }

    [[nodiscard]] const Distribution& get_dps_distribution() const { return dps_distribution_; }

    [[nodiscard]] double get_dps_mean() const { return dps_distribution_.mean(); }

    [[nodiscard]] double get_dps_variance() const { return dps_distribution_.variance(); }

    [[nodiscard]] int get_n_simulations() const { return config.n_batches; }

    [[nodiscard]] double get_rage_lost_stance() const { return rage_lost_stance_swap_; }

    [[nodiscard]] double get_rage_lost_capped() const { return rage_lost_capped_; }

    [[nodiscard]] double get_avg_rage_spent_executing() const { return avg_rage_spent_executing_; }

    [[nodiscard]] const std::vector<int>& get_hist_x() const { return hist_x; }

    [[nodiscard]] const std::vector<int>& get_hist_y() const { return hist_y; }

    [[nodiscard]] double get_flurry_uptime() const { return flurry_uptime_; }

    [[nodiscard]] double get_hs_uptime() const { return oh_queued_uptime_; }

    [[nodiscard]] double get_rampage_uptime() const { return rampage_uptime_; }

    void init_histogram();

    void prune_histogram();

    void normalize_timelapse();

    static std::string hit_result_to_string(const Hit_result& hit_result);

    Combat_simulator_config config;

    const Use_effect death_wish = {
        "death_wish", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, 0, .20}, -10, 30, 180, true};

    const Use_effect recklessness = {
        "recklessness", Use_effect::Effect_socket::unique, {}, {100, 0, 0}, 0, 15, 900, true};

    const Use_effect bloodrage = {"bloodrage", Use_effect::Effect_socket::unique, {}, {}, 10, 10, 60, false,
                                  {},          {{"bloodrage", {}, 1, 0, 1, 10}}};

    const Over_time_effect essence_of_the_red = {"essence_of_the_red", {}, 20, 0, 1, 600};

    const Over_time_effect anger_management = {"anger_management", {}, 1, 0, 3, 600};

private:
    [[nodiscard]] static int to_millis(double seconds) { return Time_keeper::to_millis(seconds); }
    [[nodiscard]] int from_offset(double offset) const { return time_keeper_.from_offset(offset); }

    Hit_table hit_table_white_mh_{};
    Hit_table hit_table_white_oh_{};
    Hit_table hit_table_yellow_mh_{};
    Hit_table hit_table_yellow_oh_{};
    Hit_table hit_table_overpower_{};
    Hit_table hit_table_white_oh_queued_{};

    Special_stats compute_hit_table_stats_{};

    Time_keeper time_keeper_{};
    Buff_manager buff_manager_{};
    Ability_queue_manager ability_queue_manager{};
    Slam_manager slam_manager{1500};
    double rage{};

    Logger logger_{};

    // config related
    double armor_reduction_factor_{};
    double armor_reduction_factor_add{};
    int armor_reduction_from_spells_{};
    int armor_reduction_delayed_{};
    bool recompute_mitigation_{};
    int number_of_extra_targets_{};

    int execute_rage_cost_{};
    int heroic_strike_rage_cost_{};
    int whirlwind_rage_cost_{};
    int mortal_strike_rage_cost_{};
    int bloodthirst_rage_cost_{};
    int tactical_mastery_rage_{};

    double cleave_bonus_damage_{};
    Special_stats flurry_{};

    bool deep_wounds_{};
    bool use_bloodthirst_{};
    bool use_rampage_{};
    bool use_mortal_strike_{};
    bool use_sweeping_strikes_{};
    int sweeping_strikes_charges_{};

    std::vector<Use_effect> use_effects_{};
    std::vector<Over_time_effect> over_time_effects_{};

    Over_time_effect deep_wound_effect_{"deep_wound", {}, 0, 0, 3, 12};
    Hit_effect battle_stance_{"battle_stance", Hit_effect::Type::stat_boost, {}, {-3.0, 0, 0}, 0, 1.5, 0, 0};
    Hit_effect destroyer_2_set_{"destroyer_2_set", Hit_effect::Type::stat_boost, {}, {0, 0, 100}, 0, 5, 0, 0};
    Hit_effect windfury_attack_{"windfury_attack", Hit_effect::Type::stat_boost, {}, {0, 0, 445}, 0, 1.5, 0, 0, 0, 2};

    // statistics
    Damage_sources damage_distribution_{};
    Distribution dps_distribution_{};

    double flurry_uptime_{};
    double oh_queued_uptime_{};
    double rampage_uptime_{};

    double rage_gained_{};
    double rage_spent_{};
    double rage_spent_on_execute_{};
    double rage_lost_stance_swap_{};
    double rage_lost_capped_{};

    double avg_rage_spent_executing_{};

    std::unordered_map<std::string, int> proc_data_{};

    static constexpr int time_lapse_resolution = 500; // time lapse bucket size (in ms)
    static constexpr int histogram_dps_resolution = 20; // histogram bucket size (in dps)

    std::vector<std::vector<double>> damage_time_lapse_{};
    std::vector<int> hist_x{};
    std::vector<int> hist_y{};
};

#include "Combat_simulator.tcc"

#endif // WOW_SIMULATOR_COMBAT_SIMULATOR_HPP
