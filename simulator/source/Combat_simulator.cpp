#include "Combat_simulator.hpp"

#include "Statistics.hpp"
#include "Use_effects.hpp"
#include "item_heuristics.hpp"

#include <algorithm>

namespace
{
constexpr double rage_factor = 3.75 / 274.7;

constexpr double rage_from_damage_taken(double damage)
{
    return damage * 5.0 / 2.0 / 274.7;
}

constexpr double armor_mitigation(int target_armor, int target_level)
{
    return static_cast<double>(target_armor) / static_cast<double>(target_armor + (467.5 * target_level - 22167.5));
}

std::vector<double> create_hit_table(double miss, double dodge, double glancing, double crit)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {miss, miss + dodge, miss + dodge + glancing, miss + dodge + glancing + crit};
}

std::vector<double> create_hit_table_yellow(double miss, double dodge, double crit, bool double_roll)
{
    double double_roll_factor = double_roll ? (100 - miss - dodge) / 100 : 1.0;
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {miss, miss + dodge, miss + dodge, miss + dodge + double_roll_factor * crit};
}

std::vector<double> create_multipliers(double glancing_factor, double crit_damage_bonus, double bonus_crit_multiplier)
{
    // Order -> Miss, parry, dodge, block, glancing, crit, hit.
    return {0.0, 0.0, glancing_factor, 1.0 + multiplicative_addition(1.0 + crit_damage_bonus, bonus_crit_multiplier),
            1.0};
}
} // namespace

void Combat_simulator::set_config(const Combat_simulator_config& new_config)
{
    config = new_config;

    heroic_strike_rage_cost = 15 - config.talents.improved_heroic_strike;
    p_unbridled_wrath_ = 3 * config.talents.unbridled_wrath;
    p_mace_spec_ = 0.3 * config.talents.mace_specialization;
    execute_rage_cost_ = 15 - static_cast<int>(2.51 * config.talents.improved_execute);

    armor_reduction_from_spells_ = 0.0;
    armor_reduction_from_spells_ += 520 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells_ += 800 * config.curse_of_recklessness_active;
    armor_reduction_from_spells_ += 610 * config.faerie_fire_feral_active;
    if (config.exposed_armor)
    {
        armor_reduction_delayed_ = 3075 - 520 * config.n_sunder_armor_stacks;
    }

    flurry_haste_factor_ = 0.05 * config.talents.flurry;

    dual_wield_damage_factor_ = 0.5 + 0.025 * config.talents.dual_wield_specialization;
    cleave_bonus_damage_ = 70 * (1.0 + 0.4 * config.talents.improved_cleave);
    slam_manager.slam_cast_time_ = 1.5 - 0.5 * config.talents.improved_slam;

    tactical_mastery_rage_ = 5.0 * config.talents.tactical_mastery + 10;
    deep_wounds_ = config.talents.deep_wounds && config.combat.deep_wounds;
    use_rampage_ = config.talents.rampage && config.combat.use_rampage;
    use_bloodthirst_ = config.talents.bloodthirst && config.combat.use_bloodthirst;
    use_mortal_strike_ = config.talents.mortal_strike && config.combat.use_mortal_strike;
    use_sweeping_strikes_ =
        config.talents.sweeping_strikes && config.combat.use_sweeping_strikes && config.multi_target_mode_;

    over_time_effects_.clear();
    use_effects_all_.clear();

    if (config.talents.death_wish && config.combat.use_death_wish)
    {
        use_effects_all_.emplace_back(deathwish);
    }

    if (config.enable_recklessness)
    {
        use_effects_all_.emplace_back(Use_effect{"Recklessness", 
                                                    Use_effect::Effect_socket::unique, 
                                                    {}, 
                                                    {100, 0, 0}, 0, static_cast<double>(15 + (config.talents.improved_disciplines * 2)) , 900, true});
    }

    if (config.enable_bloodrage)
    {
        use_effects_all_.emplace_back(bloodrage);
    }

    if (config.enable_berserking)
    {
        use_effects_all_.emplace_back(Use_effect{"Berserking",
                                                 Use_effect::Effect_socket::unique,
                                                 {},
                                                 {0, 0, 0, 0, config.berserking_haste_ / 100.0},
                                                 0,
                                                 10,
                                                 180,
                                                 false});
    }

    if (config.enable_blood_fury)
    {
        use_effects_all_.emplace_back(
            Use_effect{"Blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, 282}, 0, 15, 120, true});
    }

    if (config.essence_of_the_red_)
    {
        over_time_effects_.push_back(essence_of_the_red);
    }

    if (config.take_periodic_damage_)
    {
        double rage_per_tick = rage_from_damage_taken(config.periodic_damage_amount_);
        Over_time_effect rage_from_damage = {"Rage gained from damage taken",  {}, rage_per_tick, 0,
                                             config.periodic_damage_interval_, 600};
        over_time_effects_.push_back(rage_from_damage);
    }

    if (config.talents.anger_management)
    {
        over_time_effects_.push_back(anger_management);
    }
}

std::string Combat_simulator::hit_result_to_string(const Combat_simulator::Hit_result hit_result)
{
    switch (hit_result)
    {
    case Hit_result::hit:
        return " hit";
    case Hit_result::crit:
        return " crit";
    case Hit_result::dodge:
        return " dodge";
    case Hit_result::miss:
        return " miss";
    default:
        return " bugs";
    }
}

void Combat_simulator::cout_damage_parse(Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                                         Combat_simulator::Hit_outcome hit_outcome)
{
    if (weapon_hand == Socket::main_hand)
    {
        if (hit_type == Hit_type::white)
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                simulator_cout("Mainhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Mainhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Mainhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                simulator_cout("Mainhand hit dodged");
                break;
            case Hit_result::miss:
                simulator_cout("Mainhand hit missed");
                break;
            case Hit_result::TBD:
                // Should never happen
                simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                simulator_cout("BUG: Ability glanced for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Ability hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Ability crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                simulator_cout("Ability dodged");
                break;
            case Hit_result::miss:
                simulator_cout("Ability missed");
                break;
            case Hit_result::TBD:
                simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
    }
    else
    {
        if (hit_type == Hit_type::white)
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                simulator_cout("Offhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Offhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Offhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                simulator_cout("Offhand hit dodged");
                break;
            case Hit_result::miss:
                simulator_cout("Offhand hit missed");
                break;
            case Hit_result::TBD:
                simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                simulator_cout("BUG: Offhand ability glanced for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                simulator_cout("Offhand ability hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                simulator_cout("Offhand ability crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                simulator_cout("Offhand ability dodged");
                break;
            case Hit_result::miss:
                simulator_cout("Offhand ability missed");
                break;
            case Hit_result::TBD:
                simulator_cout("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit_mh(double damage, Hit_type hit_type, bool is_overpower, 
                                                                bool is_melee_spell)
{
    if (hit_type == Hit_type::white)
    {
        simulator_cout("Drawing outcome from MH hit table");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_white_mh_.begin(), hit_table_white_mh_.end(), random_var) -
                      hit_table_white_mh_.begin();
        return {damage * damage_multipliers_white_mh_[outcome], Hit_result(outcome)};
    }
    else
    {
        simulator_cout("Drawing outcome from yellow table");
        double random_var = get_uniform_random(100);
        if (is_melee_spell)
        {
            int outcome = std::lower_bound(hit_table_yellow_spell_.begin(), hit_table_yellow_spell_.end(), random_var) -
                          hit_table_yellow_spell_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
        }
        else if (is_overpower)
        {
            int outcome = std::lower_bound(hit_table_overpower_.begin(), hit_table_overpower_.end(), random_var) -
                          hit_table_overpower_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
        }
        else
        {
            int outcome = std::lower_bound(hit_table_yellow_.begin(), hit_table_yellow_.end(), random_var) -
                          hit_table_yellow_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit_oh(double damage, bool is_whirlwind)
{
    if ((ability_queue_manager.heroic_strike_queued && !config.dpr_settings.compute_dpr_hs_) || (ability_queue_manager.cleave_queued && !config.dpr_settings.compute_dpr_cl_))
    {
        simulator_cout("Drawing outcome from OH hit table (without DW penalty)");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_two_hand_.begin(), hit_table_two_hand_.end(), random_var) -
                      hit_table_two_hand_.begin();
        return {damage * damage_multipliers_white_oh_[outcome], Hit_result(outcome)};
    }
    else if (is_whirlwind)
    {
        simulator_cout("Drawing outcome from OH yellow hit table (Whirlwind)");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_yellow_.begin(), hit_table_yellow_.end(), random_var) -
                          hit_table_yellow_.begin();
            return {damage * damage_multipliers_yellow_[outcome], Hit_result(outcome)};
    }
    else
    {
        simulator_cout("Drawing outcome from OH hit table");
        double random_var = get_uniform_random(100);
        int outcome = std::lower_bound(hit_table_white_oh_.begin(), hit_table_white_oh_.end(), random_var) -
                      hit_table_white_oh_.begin();
        return {damage * damage_multipliers_white_oh_[outcome], Hit_result(outcome)};
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(const Weapon_sim& main_hand_weapon, double damage,
                                                             Combat_simulator::Hit_type hit_type, Socket weapon_hand,
                                                             const Special_stats& special_stats,
                                                             Damage_sources& damage_sources, bool boss_target,
                                                             bool is_overpower, bool can_sweep, bool is_whirlwind, 
                                                             bool is_melee_spell)
{
    Combat_simulator::Hit_outcome hit_outcome;
    if (weapon_hand == Socket::main_hand)
    {
        hit_outcome = generate_hit_mh(damage, hit_type, is_overpower, is_melee_spell);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_mod_physical);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_mod_physical);
        }
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }
    else
    {
        hit_outcome = generate_hit_oh(damage, is_whirlwind);
        if (boss_target)
        {
            hit_outcome.damage *= armor_reduction_factor_ * (1 + special_stats.damage_mod_physical);
        }
        else
        {
            hit_outcome.damage *= armor_reduction_factor_add * (1 + special_stats.damage_mod_physical);
        }
        hit_outcome.damage *= dual_wield_damage_factor_;
        cout_damage_parse(hit_type, weapon_hand, hit_outcome);
    }

    if (sweeping_strikes_charges_ > 0)
    {
        if (hit_outcome.damage > 0.0 && can_sweep)
        {
            double sweeping_strike_damage{};
            if (boss_target)
            {
                sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_ * armor_reduction_factor_add;
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            else
            {
                double rand_var = get_uniform_random(1);
                if (number_of_extra_targets_ > 0 && rand_var > 1.0 - 1.0 / number_of_extra_targets_)
                {
                    sweeping_strike_damage = hit_outcome.damage / armor_reduction_factor_add * armor_reduction_factor_;
                }
                else
                {
                    sweeping_strike_damage = hit_outcome.damage;
                }
                damage_sources.add_damage(Damage_source::sweeping_strikes, sweeping_strike_damage, time_keeper_.time);
            }
            simulator_cout("Sweeping strikes hits a nearby target.");
            cout_damage_parse(Hit_type::yellow, Socket::main_hand,
                              Hit_outcome{sweeping_strike_damage, Hit_result::hit});
            sweeping_strikes_charges_--;
            simulator_cout("Sweeping strikes charges left: ", sweeping_strikes_charges_);
        }
    }

    if (deep_wounds_)
    {
        if (hit_outcome.hit_result == Combat_simulator::Hit_result::crit)
        {
            buff_manager_.add_over_time_effect(
                {"Deep_wounds",
                 {},
                 0,
                 (1 + special_stats.damage_mod_physical) * config.talents.deep_wounds * 0.2 * main_hand_weapon.swing(special_stats.attack_power) / 4,
                 3,
                 12},
                int(time_keeper_.time));
        }
    }

    if (hit_outcome.hit_result == Combat_simulator::Hit_result::dodge)
    {
        simulator_cout("Overpower aura gained!");
        buff_manager_.add("overpower_aura", {}, 5.0);
    }
    return hit_outcome;
}

void Combat_simulator::compute_hit_table(const Special_stats& special_stats, Socket weapon_hand,
                                         Weapon_socket weapon_socket, Weapon_type weapon_type)
{
    int level_difference = config.main_target_level - 70;
    int target_defence_level = config.main_target_level * 5;
    int skill_diff = target_defence_level - 350;
    int base_skill_diff = level_difference * 5;

    // Crit chance
    double crit_chance;
    if (level_difference == 3)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2 - 1.8; // 1.8 flat aura modifier
    }
    else if (level_difference > 0)
    {
        crit_chance = special_stats.critical_strike - base_skill_diff * 0.2;
    }
    else
    {
        crit_chance = special_stats.critical_strike + base_skill_diff * 0.04;
    }
    crit_chance = std::max(crit_chance, 0.0);

    // Miss chance
    double base_miss_chance;
    int hit_penalty = 0;
    if (skill_diff > 10)
    {
        base_miss_chance = 5.0 + skill_diff * 0.2;
        hit_penalty = 1;
    }
    else if (skill_diff > 0)
    {
        base_miss_chance = 5.0 + skill_diff * 0.1;
    }
    else
    {
        base_miss_chance = 5.0;
    }
    double dw_miss_chance =
        (weapon_socket == Weapon_socket::two_hand) ? base_miss_chance : (base_miss_chance + 19.0);
    double corrected_hit = special_stats.hit - hit_penalty;
    double miss_chance = dw_miss_chance - std::max(corrected_hit, 0.0);
    miss_chance = std::max(miss_chance, 0.0);
    double two_hand_miss_chance = std::max(base_miss_chance - corrected_hit, 0.0);

    // Dodge chance
    double dodge_chance;
    if (weapon_type == Weapon_type::sword)
    {
        if (level_difference > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.sword_expertise) * 0.25), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.sword_expertise) * 0.25), 0.0);
        }
    }
    else if (weapon_type == Weapon_type::mace)
    {
        if (level_difference > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.mace_expertise) * 0.25), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.mace_expertise) * 0.25), 0.0);
        }
    }
    else if (weapon_type == Weapon_type::axe)
    {
        if (level_difference > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.axe_expertise) * 0.25), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) - config.talents.weapon_mastery -
            (int(special_stats.expertise + special_stats.axe_expertise) * 0.25), 0.0);
        }
    }
    else
    {
        if (level_difference > 0)
        {
            dodge_chance = std::max(std::max(5 + skill_diff * 0.1, 5.0) -
            (int(special_stats.expertise) * 0.25), 0.0);
        }
        else
        {
            dodge_chance = std::max(std::max(5 - base_skill_diff * 0.04, 0.0) -
            (int(special_stats.expertise) * 0.25), 0.0);
        }
    }
    // Glancing blows
    double glancing_chance = 0.0;
    if (level_difference > 0)
    {
        glancing_chance = 6 + level_difference * 6;
    }

    double glancing_penalty;
    if (level_difference == 3)
    {
        glancing_penalty = 25.0;
    }
    else if (level_difference == 2)
    {
        glancing_penalty = 15;
    }
    else
    {
        glancing_penalty = 5.0;
    }

    if (weapon_hand == Socket::main_hand)
    {
        hit_table_white_mh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_mh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        hit_table_yellow_spell_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance, true);
        hit_table_yellow_ = create_hit_table_yellow(two_hand_miss_chance, dodge_chance, crit_chance, false);
        hit_table_overpower_ =
            create_hit_table_yellow(two_hand_miss_chance, 0, crit_chance + 25 * config.talents.overpower - 3.0, false);
        damage_multipliers_yellow_ =
            create_multipliers(1.0, 0.1 * config.talents.impale, special_stats.crit_multiplier);
    }
    else
    {
        hit_table_white_oh_ = create_hit_table(miss_chance, dodge_chance, glancing_chance, crit_chance);
        damage_multipliers_white_oh_ =
            create_multipliers((100.0 - glancing_penalty) / 100.0, 0.0, special_stats.crit_multiplier);

        hit_table_two_hand_ = create_hit_table(two_hand_miss_chance, dodge_chance, glancing_chance, crit_chance);
    }
}

void Combat_simulator::manage_flurry_rampage(Hit_result hit_result, Special_stats& special_stats, int& flurry_charges, int& rampage_stacks, bool rampage_active, bool is_ability)
{
    if (config.talents.flurry)
    {
        bool flurry_active = (flurry_charges > 0);
        if (!is_ability)
        {
            flurry_charges--;
            flurry_charges = std::max(0, flurry_charges);
        }
        if (hit_result == Hit_result::crit)
        {
            flurry_charges = 3;
            if (!flurry_active)
            {
                special_stats += {0, 0, 0, 0, flurry_haste_factor_};
            }
        }
        else if (flurry_active && flurry_charges == 0)
        {
            special_stats -= {0, 0, 0, 0, flurry_haste_factor_};
        }
        simulator_cout(flurry_charges, " flurry charges");
    }

    if (use_rampage_ && hit_result != Hit_result::dodge && hit_result != Hit_result::miss)
    {
        double rampage_ap = 50 * (1 + config.talents.improved_berserker_stance * 0.02 + config.enable_unleashed_rage * 0.1);
        if (rampage_active)
        {
            if (rampage_stacks < 5)
            {
                rampage_stacks += 1;
                special_stats += {0, 0, rampage_ap};
                simulator_cout(rampage_stacks, " rampage stacks");
            }
        }
    }
}

bool Combat_simulator::start_cast_slam(bool mh_swing, double rage, double& swing_time_left)
{
    bool use_sl = true;
    if (use_rampage_)
    {
        use_sl &= time_keeper_.rampage_cd > 3.0;
    }
    if (use_sl)
    {
        if (swing_time_left > config.combat.slam_spam_max_time)
        {
            if ((mh_swing && rage > config.combat.slam_rage_dd) || rage > config.combat.slam_spam_rage)
            {
                simulator_cout("Starting to channel slam.", " Latency: ", config.combat.slam_latency,"s");
                slam_manager.queue_slam(time_keeper_.time + config.combat.slam_latency);
                time_keeper_.global_cd = 1.5 + config.combat.slam_latency;
                // We have started 'channeling' so set a value for the swing time for now which is larger than GCD
                swing_time_left = 1.6 + config.combat.slam_latency;
                return true;
            }
        }
    }
    return false;
}

void Combat_simulator::slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                            Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_sl_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 3 : rage -= 15;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Slam!");
    double damage = main_hand_weapon.swing(special_stats.attack_power) + 140.0;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 3;
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            rage += 2;
        }
    }
    else
    {
        rage -= 15;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    }
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::slam, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                     Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_ms_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 6 : rage -= 30;
        time_keeper_.mortal_strike_cd = 6.0 - (config.talents.improved_mortal_strike * 0.2);
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Mortal Strike!");
    double damage = (main_hand_weapon.normalized_swing(special_stats.attack_power) + 210) * (100 + config.talents.improved_mortal_strike) / 100;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 6;
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            rage += 2;
        }
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    }
    time_keeper_.mortal_strike_cd = 6.0 - (config.talents.improved_mortal_strike * 0.2);
    time_keeper_.global_cd = 1.5;
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::mortal_strike, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                   Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_bt_)
    {
        get_uniform_random(100) < hit_table_yellow_spell_[1] ? rage -= 6 : rage -= 30;
        time_keeper_.blood_thirst_cd = 6.0;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Bloodthirst!");
    // simulator_cout("(DEBUG) AP: ", special_stats.attack_power);
    double damage = special_stats.attack_power * 0.45;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, 
                                    damage_sources, true, false, true, false, true);
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 6;
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            rage += 2;
        }
    }
    else
    {
        rage -= 30;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    }
    time_keeper_.blood_thirst_cd = 6.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_op_)
    {
        if (rage > tactical_mastery_rage_)
        {
            rage = tactical_mastery_rage_;
        }
        rage -= 5;
        buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
        time_keeper_.overpower_cd = 5.0;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Changed stance: Battle Stance.");
    simulator_cout("Overpower!");
    buff_manager_.add("battle_stance", {-3.0, 0, 0}, 1.5);
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats,
                                    damage_sources, true, true);
    if (rage > tactical_mastery_rage_)
    {
        rage_lost_stance_swap_ += rage - (tactical_mastery_rage_);
        rage = tactical_mastery_rage_;
    }
    rage -= 5;
    if (hit_outcome.hit_result != Hit_result::miss)
    {
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    }
    time_keeper_.overpower_cd = 5.0;
    time_keeper_.global_cd = 1.5;
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::overpower, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::whirlwind(Weapon_sim& main_hand_weapon, Weapon_sim& off_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active, bool is_dw)
{
    if (config.dpr_settings.compute_dpr_ww_)
    {
        rage -= 25 - (5 * config.set_bonus_effect.warbringer_2_set);
        time_keeper_.whirlwind_cd = 10 - config.talents.improved_whirlwind;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Whirlwind! #targets = boss + ", number_of_extra_targets_, " adds");
    simulator_cout("Whirlwind hits: ", std::min(number_of_extra_targets_ + 1, 4), " targets");
    double mh_damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    double oh_damage = is_dw ? off_hand_weapon.normalized_swing(special_stats.attack_power) : 0;
    std::vector<Hit_outcome> hit_outcomes{};
    for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 4); i++)
    {
        const auto& mh_outcome = hit_outcomes.emplace_back(generate_hit(main_hand_weapon, mh_damage, Hit_type::yellow, Socket::main_hand,
                                               special_stats, damage_sources, i == 0, false, i == 0));
        if (mh_outcome.hit_result != Hit_result::dodge && mh_outcome.hit_result != Hit_result::miss)
        {
            hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
        }
        if (is_dw)
        {
            const auto& oh_outcome = hit_outcomes.emplace_back(generate_hit(main_hand_weapon, oh_damage, Hit_type::yellow, Socket::off_hand,
                                               special_stats, damage_sources, i == 0, false, false, true));
            if (oh_outcome.hit_result != Hit_result::dodge && oh_outcome.hit_result != Hit_result::miss)
            {
                hit_effects(off_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
            }
        }
    }
    rage -= 25 - (5 * config.set_bonus_effect.warbringer_2_set);
    time_keeper_.whirlwind_cd = 10 - config.talents.improved_whirlwind;
    time_keeper_.global_cd = 1.5;
    Hit_result result_used_for_flurry = Hit_result::TBD;
    double total_damage = 0;
    for (const auto& hit_outcome : hit_outcomes)
    {
        total_damage += hit_outcome.damage;
        if (hit_outcome.hit_result == Hit_result::crit)
        {
            result_used_for_flurry = Hit_result::crit;
        }
    }
    manage_flurry_rampage(result_used_for_flurry, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::whirlwind, total_damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                               Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_ex_)
    {
        simulator_cout("Execute (DPR)!");
        rage -= execute_rage_cost_;
        time_keeper_.global_cd = 1.5;
        if (get_uniform_random(100) < hit_table_yellow_spell_[1]) return;
        rage_spent_on_execute_ += rage;
        rage = 0;
        return;
    }
    simulator_cout("Execute!");
    double damage = 925 + (rage - execute_rage_cost_) * 21;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, 
                                    damage_sources, true, false, true, false, true);
    rage -= execute_rage_cost_;
    time_keeper_.global_cd = 1.5;
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            rage += 2;
        }
        simulator_cout("Current rage: ", int(rage));
        return;
    }
    hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    rage_spent_on_execute_ += rage;
    rage = 0;
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
    rage = 0;
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats, double& rage,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active)
{
    if (config.dpr_settings.compute_dpr_ha_)
    {
        get_uniform_random(100) < hit_table_yellow_[1] ? rage -= 2 : rage -= 10;
        time_keeper_.global_cd = 1.5;
        return;
    }
    simulator_cout("Hamstring!");
    double damage = 63;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, Hit_type::yellow, Socket::main_hand, special_stats, damage_sources);
    time_keeper_.global_cd = 1.5;
    if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
    {
        rage -= 2;
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            rage += 2;
        }
    }
    else
    {
        rage -= 10;
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
    }
    manage_flurry_rampage(hit_outcome.hit_result, special_stats, flurry_charges, rampage_stacks, rampage_active, true);
    damage_sources.add_damage(Damage_source::hamstring, hit_outcome.damage, time_keeper_.time);
    simulator_cout("Current rage: ", int(rage));
}

void Combat_simulator::hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                   double& rage, Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active,
                                   bool is_extra_attack, bool is_instant)
{
    for (auto& hit_effect : weapon.hit_effects)
    {
        double r = get_uniform_random(1);
        double probability;
        double ap_multiplier = config.talents.improved_berserker_stance * 0.02 + config.enable_unleashed_rage * 0.1;
        if (hit_effect.ppm != 0)
        {
            probability = hit_effect.ppm / (60.0 / weapon.swing_speed);
        }
        else
        {
            probability = hit_effect.probability;
        }
        if (r < probability && hit_effect.time_counter <= 0)
        {
            //simulator_cout("Proc PPM: ", hit_effect.ppm, " Proc chance: ", probability, "Proc ICD: ");
            buff_manager_.reset_icd(hit_effect);
            proc_data_[hit_effect.name]++;
            switch (hit_effect.type)
            {
            case Hit_effect::Type::extra_hit:
                if (!is_extra_attack)
                {
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                 flurry_charges, rampage_stacks, rampage_active, hit_effect.attack_power_boost, true);
                }
                else
                {
                    // Decrement the proc statistics for extra hit if it got triggered by an extra hit
                    proc_data_[hit_effect.name]--;
                }
                break;
            case Hit_effect::Type::windfury_hit:
                if (!is_extra_attack  && !is_instant)
                {
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                 flurry_charges, rampage_stacks, rampage_active, hit_effect.attack_power_boost, true);
                }
                else
                {
                    // Decrement the proc statistics for extra hit if it got triggered by an extra hit
                    proc_data_[hit_effect.name]--;
                }
                break;
            case Hit_effect::Type::sword_spec: {
                    simulator_cout("PROC: extra hit from: ", hit_effect.name);
                    swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources,
                                        flurry_charges, rampage_stacks, rampage_active, hit_effect.attack_power_boost, false);
                break;
            }
            case Hit_effect::Type::damage_magic: {
                // * 0.83 Assumes a static 17% chance to resist.
                // (100 + special_stats.spell_crit / 2) / 100 is the average damage gained from a x1.5 spell crit
                double effect_damage = hit_effect.damage * 0.83 * (100 + special_stats.spell_crit / 2) / 100 *
                                       (1 + special_stats.damage_mod_spell);
                damage_sources.add_damage(Damage_source::item_hit_effects, effect_damage, time_keeper_.time);
                simulator_cout("PROC: ", hit_effect.name, " does ", effect_damage, " magic damage.");
                break;
            }
            case Hit_effect::Type::damage_physical: {
                auto hit = generate_hit(main_hand_weapon, hit_effect.damage, Hit_type::yellow, Socket::main_hand,
                                        special_stats, damage_sources);
                damage_sources.add_damage(Damage_source::item_hit_effects, hit.damage, time_keeper_.time);
                if (config.display_combat_debug)
                {
                    simulator_cout("PROC: ", hit_effect.name, hit_result_to_string(hit.hit_result), " does ",
                                   int(hit.damage), " physical damage");
                }
                if (hit.hit_result != Hit_result::miss && hit.hit_result != Hit_result::dodge)
                {
                    hit_effects(main_hand_weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active,
                            is_extra_attack);
                }
            }
            break;
            case Hit_effect::Type::stat_boost:
                simulator_cout("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration, "s");
                buff_manager_.add(weapon.socket_name + "_" + hit_effect.name,
                                hit_effect.get_special_stat_equivalent(special_stats, ap_multiplier), hit_effect.duration);
                break;
            case Hit_effect::Type::reduce_armor: {
                if (hit_effect.name == "badge_of_the_swarmguard")
                {
                    if (current_armor_red_stacks_ < hit_effect.max_stacks)
                    {
                        recompute_mitigation_ = true;
                        current_armor_red_stacks_++;
                        armor_penetration_badge_ = current_armor_red_stacks_ * hit_effect.armor_reduction;
                        simulator_cout("PROC: ", hit_effect.name, ", current stacks: ", current_armor_red_stacks_);
                    }
                    else
                    {
                        simulator_cout("PROC: ", hit_effect.name,
                                    ". At max stacks. Current stacks: ", current_armor_red_stacks_);
                    }
                }
                else
                {
                    if (buff_manager_.arpen_stacks_counter < hit_effect.max_stacks)
                    {
                        buff_manager_.arpen_stacks_counter++;
                        buff_manager_.modify_arpen_stacks(hit_effect, weapon.socket);
                        simulator_cout("PROC: ", hit_effect.name, ", current stacks: ", buff_manager_.arpen_stacks_counter);
                    }
                    else
                    {
                        buff_manager_.modify_arpen_stacks(hit_effect, weapon.socket, true);
                        simulator_cout("PROC: ", hit_effect.name,
                                    ". At max stacks. Current stacks: ", buff_manager_.arpen_stacks_counter);
                    }
                }
            }
            break;
            default:
                std::cout << ":::::::::::FAULTY HIT EFFECT IN SIMULATION!!!:::::::::";
                break;
            }
        }
    }
}

double Combat_simulator::rage_generation(double damage, const Weapon_sim& weapon, const Hit_result hit_result)
{
    auto hit_factor = weapon.socket == Socket::main_hand ? 3.5/2 : 1.75/2;
    if (hit_result == Hit_result::crit) hit_factor *= 2;
    auto rage = damage * rage_factor + hit_factor * weapon.swing_speed;
    if (config.talents.endless_rage) rage *= 1.25;
    return rage;
}

void Combat_simulator::swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                    double& rage, Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool rampage_active,
                                    double attack_power_bonus, bool is_extra_attack)
{
    std::vector<Hit_outcome> hit_outcomes{};
    double swing_damage = weapon.swing(special_stats.attack_power + attack_power_bonus);

    auto white_replaced = false;
    if (ability_queue_manager.heroic_strike_queued && weapon.socket == Socket::main_hand)
    {
        if (rage >= heroic_strike_rage_cost && config.dpr_settings.compute_dpr_hs_)
        {
            simulator_cout("Performing Heroic Strike (DPR)");
            get_uniform_random(100) < hit_table_yellow_[1] ? rage -= heroic_strike_rage_cost : rage -= 0.2 * heroic_strike_rage_cost;
        }
        else if (rage >= heroic_strike_rage_cost)
        {
            simulator_cout("Performing Heroic Strike");
            swing_damage += config.combat.heroic_strike_damage;
            const auto& hit_outcome = hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket,
                                                   special_stats, damage_sources));
            if (hit_outcome.hit_result == Hit_result::dodge || hit_outcome.hit_result == Hit_result::miss)
            {
                rage -= 0.2 * heroic_strike_rage_cost; // Refund rage for missed/dodged heroic strikes.
                if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set) rage += 2;
            }
            else
            {
                rage -= heroic_strike_rage_cost;
            }
            damage_sources.add_damage(Damage_source::heroic_strike, hit_outcome.damage, time_keeper_.time);
            white_replaced = true;
        }
        else
        {
            // Failed to pay rage for heroic strike
            simulator_cout("Failed to pay rage for Heroic Strike");
        }
        ability_queue_manager.heroic_strike_queued = false;
        simulator_cout("Current rage: ", int(rage));
    }
    else if (ability_queue_manager.cleave_queued && weapon.socket == Socket::main_hand)
    {
        if (rage >= 20 && config.dpr_settings.compute_dpr_cl_)
        {
            simulator_cout("Performing Cleave (DPR)");
            rage -= 20;
        }
        else if (rage >= 20)
        {
            simulator_cout("Performing Cleave! #targets = boss + ", number_of_extra_targets_, " adds");
            simulator_cout("Cleave hits: ", std::min(number_of_extra_targets_ + 1, 2), " targets");
            swing_damage += cleave_bonus_damage_;
            for (int i = 0, n = number_of_extra_targets_ > 0 ? 2 : 1; i < n; i++)
            {
                hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::yellow, weapon.socket,
                                                       special_stats, damage_sources, i == 0));
            }
            rage -= 20;
            double total_damage = 0;
            for (const auto& hit_outcome : hit_outcomes)
            {
                if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set) rage += 2;
                total_damage += hit_outcome.damage;
            }
            damage_sources.add_damage(Damage_source::cleave, total_damage, time_keeper_.time);
            white_replaced = true;
        }
        else
        {
            simulator_cout("Failed to pay rage for Cleave");
        }
        ability_queue_manager.cleave_queued = false;
        simulator_cout("Current rage: ", int(rage));
    }

    if (!white_replaced)
    {
        // Otherwise do white hit
        const auto& hit_outcome = hit_outcomes.emplace_back(generate_hit(main_hand_weapon, swing_damage, Hit_type::white, weapon.socket,
                                               special_stats, damage_sources));

        if (hit_outcome.hit_result != Hit_result::dodge && hit_outcome.hit_result != Hit_result::miss)
        {
            rage += rage_generation(hit_outcome.damage, weapon, hit_outcome.hit_result);
        }
        else if (hit_outcome.hit_result == Hit_result::dodge)
        {
            if (config.set_bonus_effect.warbringer_4_set)
            {
                rage += 2;
            }
            rage += rage_generation(swing_damage * armor_reduction_factor_ * (1 + special_stats.damage_mod_physical), weapon, hit_outcome.hit_result);
            simulator_cout("Rage gained since the enemy dodged.");
        }

        if (rage > 100.0)
        {
            rage_lost_capped_ += rage - 100.0;
            rage = 100.0;
        }
        simulator_cout("Current rage: ", int(rage));
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcome.damage, time_keeper_.time);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcome.damage, time_keeper_.time);
        }
    }

    Hit_result result_used_for_flurry = Hit_result::TBD;
    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_result == Hit_result::crit)
        {
            result_used_for_flurry = Hit_result::crit;
            break;
        }
    }
    manage_flurry_rampage(result_used_for_flurry, special_stats, flurry_charges, rampage_stacks, rampage_active);

    for (const auto& hit_outcome : hit_outcomes)
    {
        if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
        {
            hit_effects(weapon, main_hand_weapon, special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active, is_extra_attack, 
            false);
            // Unbridled wrath
            if (get_uniform_random(1) < (p_unbridled_wrath_ * weapon.swing_speed / 60))
            {
                rage += 1;
                if (rage > 100.0)
                {
                    rage_lost_capped_ += rage - 100.0;
                    rage = 100.0;
                }
                simulator_cout("Unbridled wrath. Current rage: ", int(rage));
            }
            // Mace specialization
            if (get_uniform_random(1) < (p_mace_spec_ * weapon.swing_speed / 60) && weapon.weapon_type == Weapon_type::mace)
            {
                rage += 7;
                if (rage > 100.0)
                {
                    rage_lost_capped_ += rage - 100.0;
                    rage = 100.0;
                }
                simulator_cout("Mace specialization. Current rage: ", int(rage));
            }
        }
    }
}

void Combat_simulator::simulate(const Character& character, size_t n_simulations, double init_mean,
                                double init_variance, size_t init_simulations)
{
    dps_distribution_.mean_ = init_mean;
    dps_distribution_.variance_ = init_variance;
    dps_distribution_.n_samples_ = init_simulations;
    config.n_batches = n_simulations;
    simulate(character, init_simulations, false, false);
}

void Combat_simulator::simulate(const Character& character, int init_iteration, bool log_data, bool reset_dps)
{
    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        debug_topic_ = "";
        n_damage_batches = 1;
    }
    if (log_data)
    {
        reset_time_lapse();
        init_histogram();
    }
    buff_manager_.aura_uptime.clear();
    damage_distribution_ = Damage_sources{};
    if (reset_dps)
    {
        dps_distribution_.reset();
    }
    flurry_uptime_ = 0;
    rage_lost_stance_swap_ = 0;
    rage_lost_capped_ = 0;
    heroic_strike_uptime_ = 0;
    rampage_uptime_ = 0;
    const auto starting_special_stats = character.total_special_stats;
    std::vector<Weapon_sim> weapons;
    for (const auto& wep : character.weapons)
    {
        weapons.emplace_back(wep.swing_speed, wep.min_damage, wep.max_damage, wep.socket, wep.type, wep.weapon_socket,
                             wep.hit_effects);
        weapons.back().compute_weapon_damage(wep.buff.bonus_damage + starting_special_stats.bonus_damage);
        compute_hit_table(starting_special_stats, wep.socket, wep.weapon_socket, wep.type);
    }

    // TODO can move this to armory::compute_total_stats method
    if (character.is_dual_wield())
    {
        for (size_t i = 0; i < 2; i++)
        {
            size_t j = (i == 0) ? 1 : 0;
            for (const auto& hit_effect : character.weapons[i].hit_effects)
            {
                if (hit_effect.affects_both_weapons)
                {
                    auto new_hit_effect = hit_effect;
                    if (hit_effect.ppm != 0)
                    {
                        new_hit_effect.probability = hit_effect.ppm / (60.0 / weapons[j].swing_speed);
                    }
                    else
                    {
                        std::cout << "missing PPM for hit effect that affects both weapons!\n";
                    }
                    weapons[j].hit_effects.emplace_back(new_hit_effect);
                }
            }
        }
    }
    // Passing set bonuses that are not stats from character to config
    config.set_bonus_effect.warbringer_2_set = character.set_bonus_effect.warbringer_2_set;
    config.set_bonus_effect.warbringer_4_set = character.set_bonus_effect.warbringer_4_set;

    const bool is_dual_wield = character.is_dual_wield();
    const auto hit_effects_mh_copy = weapons[0].hit_effects;
    auto hit_effects_oh_copy = is_dual_wield ? weapons[1].hit_effects : std::vector<Hit_effect>{};

    double sim_time = config.sim_time;
    const double averaging_interval = 2.0; // Duration of the uniform interval that is evaluated
    sim_time -= averaging_interval;

    // Configure use effects
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }

    
    if (config.enable_unleashed_rage > 0)
    {
        double ap_boost = 
            character.total_special_stats.attack_power * 0.1; 
        use_effects_all.emplace_back(
            Use_effect{"unleashed_rage", Use_effect::Effect_socket::unique, {}, {0, 0, ap_boost}, 0, sim_time - config.unleashed_rage_start_, sim_time + 5, false});
    }

    double ap_equiv{};
    if (is_dual_wield)
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], character.weapons[1],
                                               sim_time, {});
    }
    else
    {
        ap_equiv = get_character_ap_equivalent(starting_special_stats, character.weapons[0], sim_time, {});
    }

    auto use_effect_order = Use_effects::compute_use_effect_order(use_effects_all, starting_special_stats,
                                                                  sim_time + averaging_interval / 2, ap_equiv, 0, 0,
                                                                  config.combat.initial_rage);

    for (int iter = init_iteration; iter < n_damage_batches + init_iteration; iter++)
    {
        time_keeper_.reset(); // Class variable that keeps track of the time spent, cooldowns, iteration number
        ability_queue_manager.reset();
        slam_manager.reset();
        auto special_stats = starting_special_stats;
        Damage_sources damage_sources{};
        double rage = config.combat.initial_rage;

        // Reset hit effects
        weapons[0].hit_effects = hit_effects_mh_copy;
        if (is_dual_wield)
        {
            weapons[1].hit_effects = hit_effects_oh_copy;
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, weapons[1].hit_effects,
                                     tactical_mastery_rage_, config.performance_mode);
        }
        else
        {
            buff_manager_.initialize(special_stats, use_effect_order, weapons[0].hit_effects, hit_effects_oh_copy,
                                     tactical_mastery_rage_, config.performance_mode);
        }

        recompute_mitigation_ = true;
        buff_manager_.arpen_stacks_counter = 0;
        current_armor_red_stacks_ = 0;
        armor_penetration_badge_ = 0;
        rage_spent_on_execute_ = 0;

        int flurry_charges = 0; 
        bool crit_for_rampage = false;
        bool rampage_active = false;
        int rampage_stacks = 0;
        bool apply_delayed_armor_reduction = false;
        bool execute_phase = false;

        double flurry_uptime = 0.0;

        int mh_hits = 0;
        int oh_hits = 0;
        int oh_hits_w_heroic = 0;
        int mh_hits_w_rampage = 0;

        weapons[0].internal_swing_timer = 0.0;
        if (is_dual_wield) weapons[1].internal_swing_timer = 0.5 * (weapons[1].swing_speed / (1 + special_stats.haste)); // de-sync mh/oh swing timers

        // To avoid local max/min results from running a specific run time
        sim_time += averaging_interval / n_damage_batches;
        double time_execute_phase = sim_time * (100.0 - config.execute_phase_percentage_) / 100.0;

        // Combat configuration
        if (!config.multi_target_mode_)
        {
            number_of_extra_targets_ = 0;
        }
        else
        {
            number_of_extra_targets_ = config.number_of_extra_targets;
        }

        // Check if the simulator should use any use effects before the fight
        if (!use_effect_order.empty() && use_effect_order.back().first < 0.0)
        {
            time_keeper_.time = use_effect_order.back().first;
            while (time_keeper_.time < 0.0)
            {
                double buff_dt = buff_manager_.get_dt(time_keeper_.time);
                double use_effect_dt = buff_manager_.use_effect_order.back().first - time_keeper_.time;
                if (use_effect_dt <= 0.0)
                {
                    use_effect_dt = 1e-5;
                    if (buff_manager_.use_effect_order.back().second.triggers_gcd)
                    {
                        use_effect_dt = (time_keeper_.global_cd > 0.0) ? time_keeper_.global_cd : use_effect_dt;
                    }
                }
                double dt =
                    time_keeper_.get_dynamic_time_step(100.0, 100.0, buff_dt, 0.0 - time_keeper_.time, use_effect_dt);
                time_keeper_.increment(dt);
                std::vector<std::string> debug_msg{};
                double ap_multiplier = config.talents.improved_berserker_stance * 0.02 + config.enable_unleashed_rage * 0.1;
                buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_, time_keeper_.global_cd,
                                        config.display_combat_debug, debug_msg, ap_multiplier);
                for (const auto& msg : debug_msg)
                {
                    simulator_cout(msg);
                }
            }
        }

        for (const auto& over_time_effect : over_time_effects_)
        {
            buff_manager_.add_over_time_effect(over_time_effect, 0);
        }

        // First global sunder
        bool first_global_sunder = config.first_global_sunder_;

        if (config.combat.first_hit_heroic_strike && rage > heroic_strike_rage_cost)
        {
            ability_queue_manager.queue_heroic_strike();
        }

        while (time_keeper_.time < sim_time)
        {
            double mh_dt = weapons[0].internal_swing_timer;
            double oh_dt = is_dual_wield ? weapons[1].internal_swing_timer : 1000.0;
            double oldHaste = special_stats.haste;
            double buff_dt = buff_manager_.get_dt(time_keeper_.time);
            double slam_dt = slam_manager.time_left(time_keeper_.time);
            double dt = time_keeper_.get_dynamic_time_step(mh_dt, oh_dt, buff_dt, sim_time - time_keeper_.time, slam_dt);
            if (flurry_charges > 0) flurry_uptime += dt;
            time_keeper_.increment(dt);
            std::vector<std::string> debug_msg{};
            double ap_multiplier = config.talents.improved_berserker_stance * 0.02 + config.enable_unleashed_rage * 0.1;
            buff_manager_.increment(dt, time_keeper_.time, rage, rage_lost_stance_swap_, time_keeper_.global_cd,
                                    config.display_combat_debug, debug_msg, ap_multiplier);
            for (const auto& msg : debug_msg)
            {
                simulator_cout(msg);
            }

            if (sim_time - time_keeper_.time < 0.0)
            {
                break;
            }

            if (buff_manager_.need_to_recompute_hittables)
            {
                for (const auto& weapon : weapons)
                {
                    compute_hit_table(special_stats, weapon.socket, weapon.weapon_socket, weapon.weapon_type);
                }
                buff_manager_.need_to_recompute_hittables = false;
            }

            if (buff_manager_.need_to_recompute_mitigation)
            {
                recompute_mitigation_ = true;
                buff_manager_.need_to_recompute_mitigation = false;
            }

            if (buff_manager_.reset_armor_reduction)
            {
                recompute_mitigation_ = true;
                current_armor_red_stacks_ = 0;
                armor_penetration_badge_ = 0;
                buff_manager_.reset_armor_reduction = false;
            }

            if (!apply_delayed_armor_reduction && time_keeper_.time > 6.0 && config.exposed_armor)
            {
                apply_delayed_armor_reduction = true;
                recompute_mitigation_ = true;
                simulator_cout("Applying improved exposed armor!");
            }

            if (recompute_mitigation_)
            {
                int target_armor =
                    config.main_target_initial_armor_ - armor_reduction_from_spells_ - armor_penetration_badge_ - special_stats.gear_armor_pen;
                if (apply_delayed_armor_reduction)
                {
                    target_armor -= armor_reduction_delayed_;
                }
                target_armor = std::max(target_armor, 0);
                armor_reduction_factor_ = 1 - armor_mitigation(target_armor, 70);
                simulator_cout("Target armor: ", target_armor, ". Mitigation factor: ", 1 - armor_reduction_factor_,
                               "%.");
                if (config.multi_target_mode_)
                {
                    int extra_target_armor = config.extra_target_initial_armor_ - armor_penetration_badge_ - special_stats.gear_armor_pen;
                    extra_target_armor = std::max(extra_target_armor, 0);
                    armor_reduction_factor_add = 1 - armor_mitigation(extra_target_armor, 70);

                    simulator_cout("Extra targets armor: ", extra_target_armor,
                                   ". Mitigation factor: ", 1 - armor_reduction_factor_add, "%.");
                }
                recompute_mitigation_ = false;
            }

            if (config.multi_target_mode_ && number_of_extra_targets_ > 0 &&
                time_keeper_.time / sim_time > config.extra_target_duration)
            {
                simulator_cout("Extra targets die.");
                number_of_extra_targets_ = 0;
            }

            bool mh_swing = weapons[0].time_for_swing(dt);
            bool oh_swing = is_dual_wield ? weapons[1].time_for_swing(dt) : false;

            if (mh_swing)
            {
                mh_hits++;
                if (rampage_stacks > 0)
                {
                    mh_hits_w_rampage++;
                }
                swing_weapon(weapons[0], weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
            }

            if (oh_swing)
            {
                oh_hits++;
                if (ability_queue_manager.is_ability_queued())
                {
                    oh_hits_w_heroic++;
                }
                swing_weapon(weapons[1], weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
            }

            if (!execute_phase)
            {
                if (time_keeper_.time > time_execute_phase)
                {
                    simulator_cout("------------ Execute phase! ------------");
                    execute_phase = true;
                }
            }

            if (use_sweeping_strikes_)
            {
                if (rage > 30.0 && time_keeper_.global_cd < 0.0 && time_keeper_.sweeping_strikes_cd < 0.0)
                {
                    simulator_cout("Sweeping strikes!");
                    sweeping_strikes_charges_ = 10;
                    time_keeper_.sweeping_strikes_cd = 30;
                    time_keeper_.global_cd = 1.5;
                }
            }

            if (first_global_sunder)
            {
                if (time_keeper_.global_cd < 0 && rage > 15)
                {
                    simulator_cout("Sunder Armor!");
                    time_keeper_.global_cd = 1.5;
                    rage -= 15;
                    first_global_sunder = false;
                }
            }
            else if (execute_phase)
            {
                if (weapons[0].weapon_socket == Weapon_socket::two_hand && config.combat.use_slam && config.combat.use_sl_in_exec_phase)
                {
                    if (!slam_manager.is_slam_queued() && time_keeper_.global_cd < 0.0)
                    {
                        if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                        {
                            continue;
                        }
                    }
                    else if (slam_manager.time_left(time_keeper_.time) <= 0.0)
                    {
                        slam(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                        slam_manager.un_queue_slam();
                        weapons[0].internal_swing_timer = weapons[0].swing_speed / (1 + special_stats.haste);
                        if (time_keeper_.global_cd < 0.0)
                        {
                            if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
                if (use_mortal_strike_ && config.combat.use_ms_in_exec_phase)
                {
                    bool ms_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        ms_ww = std::max(time_keeper_.whirlwind_cd, 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.mortal_strike_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30 && ms_ww)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }
                if (use_bloodthirst_ && config.combat.use_bt_in_exec_phase)
                {
                    bool bt_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        bt_ww = std::max(time_keeper_.whirlwind_cd, 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30 && bt_ww)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }
                if (config.combat.use_whirlwind && config.combat.use_ww_in_exec_phase)
                {
                    bool use_ww = true;
                    if (use_rampage_)
                    {
                        use_ww = time_keeper_.rampage_cd > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ww = std::max(time_keeper_.blood_thirst_cd, 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww = std::max(time_keeper_.mortal_strike_cd, 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (time_keeper_.whirlwind_cd < 0.0 && rage > config.combat.whirlwind_rage_thresh && rage > 25 - (5 * config.set_bonus_effect.warbringer_2_set) &&
                        time_keeper_.global_cd < 0 && use_ww)
                    {
                        if(weapons[0].weapon_socket == Weapon_socket::two_hand)
                        {
                            whirlwind(weapons[0], weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                        }
                        else
                        {
                            whirlwind(weapons[0], weapons[1], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active, 1);
                        }
                        
                    }
                }
                if (time_keeper_.global_cd < 0 && rage > execute_rage_cost_)
                {
                    execute(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                }
                if (rage > heroic_strike_rage_cost && !ability_queue_manager.heroic_strike_queued &&
                    config.combat.use_hs_in_exec_phase)
                {
                    if (config.combat.use_heroic_strike)
                    {
                        if (config.dpr_settings.compute_dpr_hs_)
                        {
                            if (mh_swing)
                            {
                                rage -= config.combat.heroic_strike_rage_thresh;
                            }
                        }
                        else
                        {
                            ability_queue_manager.queue_heroic_strike();
                            simulator_cout("Heroic strike activated");
                        }
                    }
                }
            }
            else
            {
                if (weapons[0].weapon_socket == Weapon_socket::two_hand && config.combat.use_slam)
                {
                    if (!slam_manager.is_slam_queued() && time_keeper_.global_cd < 0.0)
                    {
                        if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                        {
                            continue;
                        }
                    }
                    else if (slam_manager.time_left(time_keeper_.time) <= 0.0)
                    {
                        slam(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                        slam_manager.un_queue_slam();
                        weapons[0].internal_swing_timer = weapons[0].swing_speed / (1 + special_stats.haste);
                        if (time_keeper_.global_cd < 0.0)
                        {
                            if (start_cast_slam(mh_swing, rage, weapons[0].internal_swing_timer))
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        continue;
                    }
                }

                if (use_rampage_)
                {
                    // TODO create a proper crit detection without using flurry charges
                    if (flurry_charges > 0)
                    {
                        crit_for_rampage = true;
                    }
                    if (time_keeper_.rampage_cd < (0 + config.combat.rampage_use_thresh) && time_keeper_.global_cd < 0 && rage > 20 && crit_for_rampage)
                    {
                        time_keeper_.rampage_cd = 30.0;
                        time_keeper_.global_cd = 1.5;
                        rage -= 20;
                        crit_for_rampage = false;
                        rampage_active = true;
                        if (rampage_stacks < 1)
                        {
                            rampage_stacks = 1;
                        }
                        simulator_cout("Rampage!");
                        simulator_cout("Current rage: ", int(rage));
                    }
                    else if (time_keeper_.rampage_cd < 0.0 && rampage_active)
                    {
                        double rampage_ap = 50 * (1 + config.talents.improved_berserker_stance * 0.02 + config.enable_unleashed_rage * 0.1) * rampage_stacks;
                        special_stats -= {0, 0, rampage_ap};
                        rampage_active = false;
                        rampage_stacks = 0;
                        simulator_cout("Rampage fades.");
                    }
                }

                if (use_bloodthirst_)
                {
                    bool bt_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        bt_ww = std::max(time_keeper_.whirlwind_cd, 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.blood_thirst_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30 && bt_ww)
                    {
                        bloodthirst(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }

                if (use_mortal_strike_)
                {
                    bool ms_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        ms_ww = std::max(time_keeper_.whirlwind_cd, 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.mortal_strike_cd < 0.0 && time_keeper_.global_cd < 0 && rage > 30 && ms_ww)
                    {
                        mortal_strike(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }

                if (config.combat.use_whirlwind)
                {
                    bool use_ww = true;
                    if (use_rampage_)
                    {
                        use_ww = time_keeper_.rampage_cd > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ww = std::max(time_keeper_.blood_thirst_cd, 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww = std::max(time_keeper_.mortal_strike_cd, 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (time_keeper_.whirlwind_cd < 0.0 && rage > config.combat.whirlwind_rage_thresh && rage > 25 - (5 * config.set_bonus_effect.warbringer_2_set) &&
                        time_keeper_.global_cd < 0 && use_ww)
                    {
                        if (weapons[0].weapon_socket == Weapon_socket::two_hand)
                        {
                            whirlwind(weapons[0], weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                        }
                        else
                        {
                            whirlwind(weapons[0], weapons[1], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active, 1);
                        }
                        
                    }
                }

                if (config.combat.use_overpower)
                {
                    bool use_op = true;
                    if (use_rampage_)
                    {
                        use_op &= time_keeper_.rampage_cd > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_op &= time_keeper_.blood_thirst_cd > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_op &= time_keeper_.mortal_strike_cd > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_op &= time_keeper_.whirlwind_cd > config.combat.overpower_ww_cooldown_thresh;
                    }
                    if (time_keeper_.overpower_cd < 0.0 && rage < config.combat.overpower_rage_thresh && rage > 5 &&
                        time_keeper_.global_cd < 0 && buff_manager_.can_do_overpower() && use_op)
                    {
                        overpower(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }

                if (config.combat.use_hamstring)
                {
                    bool use_ham = true;
                    if (config.combat.dont_use_hm_when_ss && sweeping_strikes_charges_ > 0)
                    {
                        use_ham = false;
                    }
                    if (use_rampage_)
                    {
                        use_ham &= time_keeper_.rampage_cd > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ham &= time_keeper_.blood_thirst_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ham &= time_keeper_.mortal_strike_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_ham &= time_keeper_.whirlwind_cd > config.combat.hamstring_cd_thresh;
                    }
                    if (rage > config.combat.hamstring_thresh_dd && time_keeper_.global_cd < 0 && use_ham)
                    {
                        hamstring(weapons[0], special_stats, rage, damage_sources, flurry_charges, rampage_stacks, rampage_active);
                    }
                }

                // Heroic strike or Cleave
                // TODO move to the top of sim loop, enables 'continue' after casting spells for performance
                if (number_of_extra_targets_ > 0 && config.combat.cleave_if_adds)
                {
                    if (rage > config.combat.cleave_rage_thresh && !ability_queue_manager.cleave_queued)
                    {
                        ability_queue_manager.queue_cleave();
                        simulator_cout("Cleave activated");
                    }
                }
                else
                {
                    if (rage > config.combat.heroic_strike_rage_thresh && !ability_queue_manager.heroic_strike_queued &&
                        rage > heroic_strike_rage_cost)
                    {
                        if (config.combat.use_heroic_strike)
                        {
                            ability_queue_manager.queue_heroic_strike();
                            simulator_cout("Heroic strike activated");
                        }
                    }
                }
            }

            // end of turn - update swing timers if necessary
            if (mh_swing)
            {
                weapons[0].internal_swing_timer = weapons[0].swing_speed / (1 + special_stats.haste);
            }
            else if (special_stats.haste != oldHaste)
            {
                weapons[0].internal_swing_timer *= (1 + oldHaste) / (1 + special_stats.haste);
            }

            if (oh_swing)
            {
                weapons[1].internal_swing_timer = weapons[1].swing_speed / (1 + special_stats.haste);
            }
            else if (is_dual_wield && special_stats.haste != oldHaste)
            {
                weapons[1].internal_swing_timer *= (1 + oldHaste) / (1 + special_stats.haste);
            }
        }
        if (deep_wounds_)
        {
            double dw_average_damage = buff_manager_.deep_wounds_damage / buff_manager_.deep_wounds_timestamps.size();
            for (double deep_wounds_timestamp : buff_manager_.deep_wounds_timestamps)
            {
                damage_sources.add_damage(Damage_source::deep_wounds, dw_average_damage, deep_wounds_timestamp);
            }
        }
        double new_sample = damage_sources.sum_damage_sources() / sim_time;
        dps_distribution_.add_sample(new_sample);
        damage_distribution_ = damage_distribution_ + damage_sources;
        rampage_uptime_ = Statistics::update_mean(rampage_uptime_, iter + 1, double(mh_hits_w_rampage) / mh_hits);
        if (weapons.size() > 1)
        {
            heroic_strike_uptime_ =
                Statistics::update_mean(heroic_strike_uptime_, iter + 1, double(oh_hits_w_heroic) / oh_hits);
        }
        flurry_uptime_ = Statistics::update_mean(flurry_uptime_, iter + 1, flurry_uptime / time_keeper_.time);
        avg_rage_spent_executing_ =
            Statistics::update_mean(avg_rage_spent_executing_, iter + 1, rage_spent_on_execute_);
        if (log_data)
        {
            add_damage_source_to_time_lapse(damage_sources.damage_instances);
            hist_y[new_sample / 20]++;
        }
    }
    if (log_data)
    {
        normalize_timelapse();
        prune_histogram();
    }
}

std::vector<std::pair<double, Use_effect>> Combat_simulator::get_use_effect_order(const Character& character)
{
    // Copy paste from the simulate function
    std::vector<Use_effect> use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }

    double ap_equiv{};
    if (character.is_dual_wield())
    {
        ap_equiv = get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                               character.weapons[1], config.sim_time, {});
    }
    else
    {
        ap_equiv =
            get_character_ap_equivalent(character.total_special_stats, character.weapons[0], config.sim_time, {});
    }
    return Use_effects::compute_use_effect_order(use_effects_all, character.total_special_stats, config.sim_time,
                                                 ap_equiv, 0, 0, config.combat.initial_rage);
}

void Combat_simulator::init_histogram()
{
    double res = 20;
    for (int i = 0; i < 1000; i++)
    {
        hist_x.push_back(i * res);
        hist_y.push_back(0);
    }
}

void Combat_simulator::normalize_timelapse()
{
    for (auto& damage_time_lapse_i : damage_time_lapse)
    {
        for (auto& singe_damage_instance : damage_time_lapse_i)
        {
            singe_damage_instance /= config.n_batches;
        }
    }
}

void Combat_simulator::prune_histogram()
{
    int start_idx{};
    int end_idx{};
    for (size_t i = 0; i < hist_x.size(); i++)
    {
        if (hist_y[i] != 0 && !start_idx)
        {
            start_idx = i;
            break;
        }
    }
    for (size_t i = hist_x.size() - 1; i > 0; i--)
    {
        if (hist_y[i] != 0 && !end_idx)
        {
            end_idx = i;
            break;
        }
    }
    {
        auto first = hist_x.begin() + start_idx;
        auto last = hist_x.begin() + end_idx + 1;
        hist_x = std::vector<int>(first, last);
    }
    {
        auto first = hist_y.begin() + start_idx;
        auto last = hist_y.begin() + end_idx + 1;
        hist_y = std::vector<int>(first, last);
    }
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_mh() const
{
    return hit_table_white_mh_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_oh() const
{
    return hit_table_white_oh_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_white_2h() const
{
    return hit_table_two_hand_;
}

const std::vector<double>& Combat_simulator::get_hit_probabilities_yellow() const
{
    return hit_table_yellow_;
}

double Combat_simulator::get_glancing_penalty_mh() const
{
    return damage_multipliers_white_mh_[2];
}

double Combat_simulator::get_glancing_penalty_oh() const
{
    return damage_multipliers_white_oh_[2];
}

std::vector<std::string> Combat_simulator::get_aura_uptimes() const
{
    std::vector<std::string> aura_uptimes;
    double total_sim_time = config.n_batches * (config.sim_time - 1);
    for (const auto& aura : buff_manager_.aura_uptime)
    {
        double uptime = aura.second / total_sim_time;
        aura_uptimes.emplace_back(aura.first + " " + std::to_string(100 * uptime));
    }
    if (flurry_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry " + std::to_string(100 * flurry_uptime_));
    }
    if (heroic_strike_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("'Heroic_strike_bug' " + std::to_string(100 * heroic_strike_uptime_));
    }
    if (rampage_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("Rampage " + std::to_string(100 * rampage_uptime_));
    }
    return aura_uptimes;
}

std::vector<std::string> Combat_simulator::get_proc_statistics() const
{
    std::vector<std::string> proc_counter;
    for (const auto& proc : proc_data_)
    {
        double counter = static_cast<double>(proc.second) / config.n_batches;
        proc_counter.emplace_back(proc.first + " " + std::to_string(counter));
    }
    return proc_counter;
}

void Combat_simulator::reset_time_lapse()
{
    double resolution = .50;
    std::vector<double> history;
    history.reserve(config.sim_time / resolution);
    for (double t = 0; t < config.sim_time; t += resolution)
    {
        history.push_back(0);
    }
    history.push_back(0); // This extra bin might be used for last second dot effects
    if (!(damage_time_lapse.empty()))
    {
        damage_time_lapse.clear();
    }
    for (size_t i = 0; i < source_map.size(); i++)
    {
        damage_time_lapse.push_back(history);
    }
}

void Combat_simulator::add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances)
{
    double resolution = .50;
    for (const auto& damage_instance : damage_instances)
    {
        size_t first_idx = source_map.at(damage_instance.damage_source);
        size_t second_idx = damage_instance.time_stamp / resolution; // automatically floored
        damage_time_lapse[first_idx][second_idx] += damage_instance.damage;
    }
}

std::vector<std::vector<double>> Combat_simulator::get_damage_time_lapse() const
{
    return damage_time_lapse;
}

std::string Combat_simulator::get_debug_topic() const
{
    return debug_topic_;
}
