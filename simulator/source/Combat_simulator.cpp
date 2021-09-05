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

constexpr double armor_reduction_factor(int target_armor)
{
    return 10557.5 / (10557.5 + target_armor);
}
} // namespace

void Combat_simulator::set_config(const Combat_simulator_config& new_config)
{
    config = new_config;

    heroic_strike_rage_cost_ = 15 - config.talents.improved_heroic_strike;
    execute_rage_cost_ = 15 - static_cast<int>(2.51 * config.talents.improved_execute);
    whirlwind_rage_cost_ = 25;
    mortal_strike_rage_cost_ = 30;
    bloodthirst_rage_cost_ = 30;

    armor_reduction_from_spells_ = 0.0;
    armor_reduction_from_spells_ += 520 * config.n_sunder_armor_stacks;
    armor_reduction_from_spells_ += 800 * config.curse_of_recklessness_active;
    armor_reduction_from_spells_ += 610 * config.faerie_fire_feral_active;
    if (config.exposed_armor)
    {
        armor_reduction_delayed_ = 3075 - 520 * config.n_sunder_armor_stacks;
    }

    flurry_.attack_speed = 0.05 * config.talents.flurry;

    cleave_bonus_damage_ = 70 * (1.0 + 0.4 * config.talents.improved_cleave);

    tactical_mastery_rage_ = 5 * config.talents.tactical_mastery + 10;
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
        use_effects_all_.emplace_back(death_wish);
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
            Use_effect{"blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, 282}, 0, 15, 120, false});
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

std::string Combat_simulator::hit_result_to_string(const Hit_result& hit_result)
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

void Combat_simulator::cout_damage_parse(const Weapon_sim& weapon, const Hit_table& hit_table, const Combat_simulator::Hit_outcome& hit_outcome)
{
    if (!logger_.is_enabled()) return;

    if (weapon.socket == Socket::main_hand)
    {
        if (hit_table.glance() > 0)
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                logger_.print("Mainhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                logger_.print("Mainhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                logger_.print("Mainhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                logger_.print("Mainhand hit dodged");
                break;
            case Hit_result::miss:
                logger_.print("Mainhand hit missed");
                break;
            case Hit_result::TBD:
                // Should never happen
                logger_.print("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                logger_.print("BUG: Ability glanced for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                logger_.print("Ability hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                logger_.print("Ability crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                logger_.print("Ability dodged");
                break;
            case Hit_result::miss:
                logger_.print("Ability missed");
                break;
            case Hit_result::TBD:
                logger_.print("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
    }
    else
    {
        if (hit_table.glance() > 0)
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                logger_.print("Offhand glancing hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                logger_.print("Offhand white hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                logger_.print("Offhand crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                logger_.print("Offhand hit dodged");
                break;
            case Hit_result::miss:
                logger_.print("Offhand hit missed");
                break;
            case Hit_result::TBD:
                logger_.print("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
        else
        {
            switch (hit_outcome.hit_result)
            {
            case Hit_result::glancing:
                logger_.print("BUG: Offhand ability glanced for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::hit:
                logger_.print("Offhand ability hit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::crit:
                logger_.print("Offhand ability crit for: ", int(hit_outcome.damage), " damage.");
                break;
            case Hit_result::dodge:
                logger_.print("Offhand ability dodged");
                break;
            case Hit_result::miss:
                logger_.print("Offhand ability missed");
                break;
            case Hit_result::TBD:
                logger_.print("BUUUUUUUUUUGGGGGGGGG");
                break;
            }
        }
    }
}

Combat_simulator::Hit_outcome Combat_simulator::generate_hit(const Weapon_sim& main_hand_weapon, double damage,
                                                             const Weapon_sim& weapon, const Hit_table& hit_table,
                                                             const Special_stats& special_stats, Damage_sources& damage_sources,
                                                             bool boss_target, bool can_sweep)
{
    logger_.print("Drawing outcome from ", hit_table.name());

    if (boss_target)
    {
        damage *= armor_reduction_factor_ * (1 + special_stats.damage_mod_physical);
    }
    else
    {
        damage *= armor_reduction_factor_add * (1 + special_stats.damage_mod_physical);
    }

    auto hit_outcome = hit_table.generate_hit(damage);

    cout_damage_parse(weapon, hit_table, hit_outcome);

    if (sweeping_strikes_charges_ > 0)
    {
        // TODO(vigo) can_sweep is actually either "clone damage" (can't crit; fully dependent on hit_outcome)
        //  or "normalized swing" (can't miss, can't crit; regular swing, independent of hit_outcome, happens for execute or ww)
        if (hit_outcome.damage > 0.0 && can_sweep)
        {
            double sweeping_strike_damage;
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
            logger_.print("Sweeping strikes hits a nearby target.");
            cout_damage_parse(main_hand_weapon, hit_table_yellow_mh_, {sweeping_strike_damage, Hit_result::hit});
            sweeping_strikes_charges_--;
            logger_.print("Sweeping strikes charges left: ", sweeping_strikes_charges_);
        }
    }

    if (deep_wounds_)
    {
        if (hit_outcome.hit_result == Combat_simulator::Hit_result::crit)
        {
            deep_wound_effect_.damage = 0.25 * (1 + special_stats.damage_mod_physical) * config.talents.deep_wounds * 0.2 * main_hand_weapon.swing(special_stats.attack_power);
            buff_manager_.add_over_time_buff(deep_wound_effect_, time_keeper_.time);
        }
    }

    if (config.combat.use_overpower && hit_outcome.hit_result == Combat_simulator::Hit_result::dodge)
    {
        logger_.print("Overpower aura gained!");
        time_keeper_.gain_overpower_aura();
    }

    if (use_rampage_ && hit_outcome.hit_result == Combat_simulator::Hit_result::crit)
    {
        logger_.print("Rampage aura gained!");
        time_keeper_.gain_rampage_aura();
    }

    return hit_outcome;
}

void Combat_simulator::compute_hit_tables(const Special_stats& special_stats, const Weapon_sim& weapon)
{
    auto miss = 8.0;
    auto hit_suppression = 1.0;
    auto dodge = 6.5;
    auto glance = 24.0;
    auto glance_dr = 25.0;
    auto crit_suppression = 4.8;

    if (config.main_target_level == 72)
    {
        miss = 6.0;
        hit_suppression = 0.0;
        dodge = 6.0;
        glance = 18.0;
        glance_dr = 15.0;
        crit_suppression = 2.0;
    }
    else if (config.main_target_level == 71)
    {
        miss = 5.5;
        hit_suppression = 0.0;
        dodge = 5.5;
        glance = 12.0;
        glance_dr = 5.0;
        crit_suppression = 1.0;
    }
    else if (config.main_target_level == 70)
    {
        miss = 5.0;
        hit_suppression = 0.0;
        dodge = 5.0;
        glance = 6.0;
        glance_dr = 5.0;
        crit_suppression = 0.0;
    }

    auto sw_miss = std::max(miss - std::max(special_stats.hit - hit_suppression, 0.0), 0.0);
    auto dw_miss = std::max(miss + 19.0 - std::max(special_stats.hit - hit_suppression, 0.0), 0.0);
    miss = weapon.weapon_socket == Weapon_socket::two_hand ? sw_miss : dw_miss;

    auto expertise = special_stats.expertise;
    if (weapon.weapon_type == Weapon_type::axe) expertise += special_stats.axe_expertise;
    if (weapon.weapon_type == Weapon_type::mace) expertise += special_stats.mace_expertise;
    if (weapon.weapon_type == Weapon_type::sword) expertise += special_stats.sword_expertise;

    dodge = std::max(dodge - (int)expertise * 0.25 - config.talents.weapon_mastery, 0.0);

    auto crit = std::max(special_stats.critical_strike - crit_suppression, 0.0);

    // just using the EJ-approved crit formula here ;)
    auto yellow_crit_dm = 1 + (2 * (1 + special_stats.crit_multiplier) - 1) * (1 + 0.1 * config.talents.impale);
    // auto yellow_crit_dm = (2 + 0.1 * config.talents.impale) * (1 + special_stats.crit_multiplier);

    if (weapon.socket == Socket::main_hand)
    {
        int axe_crit = (weapon.weapon_type == Weapon_type::axe) ? config.talents.poleaxe_specialization : 0;
        crit += axe_crit;
        
        auto white_dm = Damage_multipliers((100 - glance_dr) / 100, 2 * (1 + special_stats.crit_multiplier), 1);

        hit_table_white_mh_ = Hit_table("white (mh)", miss, dodge, glance, crit, white_dm);

        auto yellow_dm = Damage_multipliers(0, yellow_crit_dm, 1);
        hit_table_yellow_mh_ = Hit_table("yellow (mh)", sw_miss, dodge, 0, (100 - sw_miss - dodge) / 100 * crit, yellow_dm);

        auto overpower_crit_chance = crit + config.talents.overpower * 25; // this might be capped at 100%, but presumably nobody ever tested this
        hit_table_overpower_ = Hit_table("overpower", sw_miss, 0, 0, (100 - sw_miss) / 100 * overpower_crit_chance, yellow_dm);

        //auto hit_table_ss_ = Hit_table("sweeping strikes", 0, dodge, 0, 0, yellow_dm);
    }
    else
    {
        int axe_crit = (weapon.weapon_type == Weapon_type::axe) ? config.talents.poleaxe_specialization : 0;
        crit += axe_crit;

        auto oh_factor = 0.5 * (1 + 0.05 * config.talents.dual_wield_specialization);
        auto white_dm = Damage_multipliers(oh_factor * (100 - glance_dr) / 100, oh_factor * 2 * (1 + special_stats.crit_multiplier), oh_factor);

        hit_table_white_oh_ = Hit_table("white (oh)", miss, dodge, glance, crit, white_dm);
        hit_table_white_oh_queued_ = Hit_table("white (oh queued)", sw_miss, dodge, glance, crit, white_dm);

        auto yellow_dm = Damage_multipliers(0, oh_factor * yellow_crit_dm, oh_factor);
        hit_table_yellow_oh_ = Hit_table("yellow (oh)", sw_miss, dodge, 0, (100 - sw_miss - dodge) / 100 * crit, yellow_dm);
    }
}

void Combat_simulator::maybe_gain_flurry(Hit_result hit_result, int& flurry_charges, Special_stats& special_stats) const
{
    if (config.talents.flurry == 0 || flurry_charges == 3 || hit_result != Hit_result::crit) return;

    if (flurry_charges == 0) special_stats += flurry_;
    flurry_charges = 3;
}

void Combat_simulator::maybe_remove_flurry(int& flurry_charges, Special_stats& special_stats) const
{
    if (config.talents.flurry == 0 || flurry_charges == 0) return;

    if (flurry_charges == 1) special_stats -= flurry_;
    flurry_charges -= 1;
}

void Combat_simulator::maybe_add_rampage_stack(Hit_result hit_result, int& rampage_stacks, Special_stats& special_stats)
{
    if (!use_rampage_ || rampage_stacks == 5 || rampage_stacks == 0 || hit_result == Hit_result::miss || hit_result == Hit_result::dodge) return;

    rampage_stacks += 1;
    special_stats += {0, 0, 50};
    logger_.print(rampage_stacks, " rampage stacks");
}

void Combat_simulator::unbridled_wrath(const Weapon_sim& weapon)
{
    if (config.talents.unbridled_wrath > 0 &&
        get_uniform_random(60) < config.talents.unbridled_wrath * 3.0 * weapon.swing_speed)
    {
        gain_rage(1);
        logger_.print("Unbridled wrath. Current rage: ", int(rage));
    }
}

bool Combat_simulator::start_cast_slam(bool mh_swing, const Weapon_sim& weapon)
{
    if (use_rampage_ && time_keeper_.rampage_cd() <= 3)
    {
        return false;
    }

    if (mh_swing || weapon.next_swing - time_keeper_.time > config.combat.slam_spam_max_time)
    {
        if ((mh_swing && rage > config.combat.slam_rage_dd) || rage > config.combat.slam_spam_rage)
        {
            logger_.print("Starting to cast slam.", " Latency: ", config.combat.slam_latency, "s");
            slam_manager.cast_slam(time_keeper_.time + config.combat.slam_latency);
            time_keeper_.global_cast(1.5 + config.combat.slam_latency);
            return true;
        }
    }
    return false;
}

void Combat_simulator::slam(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                            Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_sl_)
    {
        spend_rage(hit_table_yellow_mh_.isMissOrDodge() ? 3 : 15);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Slam!");
    double damage = main_hand_weapon.swing(special_stats.attack_power) + 140.0;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_yellow_mh_, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
    {
        spend_rage(3);
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            gain_rage(2);
        }
    }
    else
    {
        spend_rage(15);
        maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    }
    damage_sources.add_damage(Damage_source::slam, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::mortal_strike(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                     Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_ms_)
    {
        spend_rage(hit_table_yellow_mh_.isMissOrDodge() ? 0.2 * mortal_strike_rage_cost_ : mortal_strike_rage_cost_);
        time_keeper_.mortal_strike_cast(6.0 - config.talents.improved_mortal_strike * 0.2);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Mortal Strike!");
    double damage = (main_hand_weapon.normalized_swing(special_stats.attack_power) + 210) * (100 + config.talents.improved_mortal_strike) / 100;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_yellow_mh_, special_stats, damage_sources);
    if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
    {
        spend_rage(0.2 * mortal_strike_rage_cost_);
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            gain_rage(2);
        }
    }
    else
    {
        spend_rage(mortal_strike_rage_cost_);
        maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    }
    time_keeper_.mortal_strike_cast(6.0 - config.talents.improved_mortal_strike * 0.2);
    time_keeper_.global_cast(1.5);
    damage_sources.add_damage(Damage_source::mortal_strike, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::bloodthirst(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                   Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_bt_)
    {
        spend_rage(hit_table_yellow_mh_.isMissOrDodge() ? 0.2 * bloodthirst_rage_cost_ : bloodthirst_rage_cost_);
        time_keeper_.blood_thirst_cast(6.0);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Bloodthirst!");
    // logger_.print("(DEBUG) AP: ", special_stats.attack_power);
    double damage = special_stats.attack_power * 0.45;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_yellow_mh_, special_stats,
                                    damage_sources);
    if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
    {
        spend_rage(0.2 * bloodthirst_rage_cost_);
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            gain_rage(2);
        }
    }
    else
    {
        spend_rage(bloodthirst_rage_cost_);
        maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    }
    time_keeper_.blood_thirst_cast(6.0);
    time_keeper_.global_cast(1.5);
    damage_sources.add_damage(Damage_source::bloodthirst, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::overpower(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_op_)
    {
        swap_stance();
        spend_rage(5);
        buff_manager_.add_combat_buff(battle_stance_, time_keeper_.time);
        time_keeper_.overpower_cast(5.0);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Changed stance: Battle Stance.");
    logger_.print("Overpower!");
    buff_manager_.add_combat_buff(battle_stance_, time_keeper_.time);
    double damage = main_hand_weapon.normalized_swing(special_stats.attack_power) + 35;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_overpower_, special_stats,
                                    damage_sources);
    swap_stance();
    spend_rage(5);
    if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
    {
        maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    }
    if (config.set_bonus_effect.destroyer_2_set)
    {
        buff_manager_.add_combat_buff(destroyer_2_set_, time_keeper_.time);
    }
    time_keeper_.overpower_cast(5.0);
    time_keeper_.global_cast(1.5);
    damage_sources.add_damage(Damage_source::overpower, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::whirlwind(Weapon_sim& main_hand_weapon, Weapon_sim& off_hand_weapon, Special_stats& special_stats,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks, bool is_dw)
{
    if (config.dpr_settings.compute_dpr_ww_)
    {
        spend_rage(whirlwind_rage_cost_);
        time_keeper_.whirlwind_cast(10 - config.talents.improved_whirlwind);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Whirlwind! #targets = boss + ", number_of_extra_targets_, " adds");
    logger_.print("Whirlwind hits: ", std::min(number_of_extra_targets_ + 1, 4), " targets");
    spend_rage(whirlwind_rage_cost_); // spend rage before hit_effects
    double mh_damage = main_hand_weapon.normalized_swing(special_stats.attack_power);
    double oh_damage = is_dw ? off_hand_weapon.normalized_swing(special_stats.attack_power) : 0;
    double total_damage = 0;
    for (int i = 0; i < std::min(number_of_extra_targets_ + 1, 4); i++)
    {
        const auto& mh_outcome = generate_hit(main_hand_weapon, mh_damage, main_hand_weapon, hit_table_yellow_mh_,
                                               special_stats, damage_sources, i == 0, i == 0);
        total_damage += mh_outcome.damage;
        if (mh_outcome.hit_result != Hit_result::miss && mh_outcome.hit_result != Hit_result::dodge)
        {
            maybe_gain_flurry(mh_outcome.hit_result, flurry_charges, special_stats);
            hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
        }
        else if (mh_outcome.hit_result == Hit_result::dodge)
        {
            if (config.set_bonus_effect.warbringer_4_set) gain_rage(2);
        }
        if (is_dw)
        {
            const auto& oh_outcome = generate_hit(main_hand_weapon, oh_damage, off_hand_weapon, hit_table_yellow_oh_,
                                               special_stats, damage_sources, i == 0, false);
            total_damage += oh_outcome.damage;
            if (oh_outcome.hit_result != Hit_result::miss && oh_outcome.hit_result != Hit_result::dodge)
            {
                maybe_gain_flurry(oh_outcome.hit_result, flurry_charges, special_stats);
                // most likely doesn't proc any non-weapon-specific hit effect
                hit_effects(off_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
            }
        }
    }
    time_keeper_.whirlwind_cast(10 - config.talents.improved_whirlwind);
    time_keeper_.global_cast(1.5);
    damage_sources.add_damage(Damage_source::whirlwind, total_damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::execute(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                               Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_ex_)
    {
        logger_.print("Execute (DPR)!");
        spend_rage(execute_rage_cost_);
        time_keeper_.global_cast(1.5);
        if (hit_table_yellow_mh_.isMissOrDodge()) return;
        spend_all_rage();
        return;
    }
    logger_.print("Execute!");
    double damage = 925 + (rage - execute_rage_cost_) * 21;
    auto hit_outcome = generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_yellow_mh_, special_stats,
                                    damage_sources);
    spend_rage(execute_rage_cost_);
    time_keeper_.global_cast(1.5);
    if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
    {
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            gain_rage(2);
        }
        logger_.print("Current rage: ", int(rage));
        return;
    }
    spend_all_rage();
    maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
    hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    damage_sources.add_damage(Damage_source::execute, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::hamstring(Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                 Damage_sources& damage_sources, int& flurry_charges, int& rampage_stacks)
{
    if (config.dpr_settings.compute_dpr_ha_)
    {
        spend_rage(hit_table_yellow_mh_.isMissOrDodge() ? 2 : 10);
        time_keeper_.global_cast(1.5);
        return;
    }
    logger_.print("Hamstring!");
    double damage = 63;
    auto hit_outcome =
        generate_hit(main_hand_weapon, damage, main_hand_weapon, hit_table_yellow_mh_, special_stats, damage_sources);
    time_keeper_.global_cast(1.5);
    if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
    {
        spend_rage(2);
        if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set)
        {
            gain_rage(2);
        }
    }
    else
    {
        spend_rage(10);
        maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
        hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks);
    }
    damage_sources.add_damage(Damage_source::hamstring, hit_outcome.damage, time_keeper_.time);
    logger_.print("Current rage: ", int(rage));
}

void Combat_simulator::hit_effects(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                   Damage_sources& damage_sources, int& flurry_charges,
                                   int& rampage_stacks, Hit_type hit_type, Extra_attack_type extra_attack_type)
{
    maybe_add_rampage_stack(Hit_result::hit, rampage_stacks, special_stats);

    if (config.talents.mace_specialization > 0 && weapon.weapon_type == Weapon_type::mace && get_uniform_random(60) < config.talents.mace_specialization * 0.3 * weapon.swing_speed)
    {
        gain_rage(7);
        logger_.print("Mace specialization. Current rage: ", int(rage));
    }

    for (auto& hit_effect : weapon.hit_effects)
    {
        if (hit_effect.time_counter >= time_keeper_.time)
        {
            // on cooldown
            continue;
        }

        auto probability = hit_effect.ppm > 0 ? hit_effect.ppm * weapon.swing_speed / 60 : hit_effect.probability;
        if (probability < 1 && get_uniform_random(1) >= probability)
        {
            continue;
        }

        //logger_.print("Proc PPM: ", hit_effect.ppm, " Proc chance: ", probability, "Proc ICD: ");
        buff_manager_.start_cooldown(hit_effect, time_keeper_.time);
            switch (hit_effect.type)
                {
        case Hit_effect::Type::windfury_hit: { // only triggered by melee or next_melee; can _not_ proc itself or (presumably) other extra attacks
            if (hit_type == Hit_type::spell || extra_attack_type != Extra_attack_type::all) break;

            hit_effect.procs++;
            logger_.print("PROC: extra hit from: ", hit_effect.name);
            windfury_attack_.duration = hit_type == Hit_type::next_melee ? 1.5 : 0.01;
            buff_manager_.add_combat_buff(windfury_attack_, time_keeper_.time);
            swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                         rampage_stacks, Extra_attack_type::none);
            break;
        }
        case Hit_effect::Type::sword_spec: { // can _not_ proc itself or other extra attacks
            if (extra_attack_type != Extra_attack_type::all) break;

            hit_effect.procs++;
            logger_.print("PROC: extra hit from: ", hit_effect.name);
            swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                         rampage_stacks, Extra_attack_type::none);
            break;
        }
        case Hit_effect::Type::extra_hit: { // _can_ proc itself and other extra attacks on instant attacks
            if (extra_attack_type == Extra_attack_type::none) break;

            hit_effect.procs++;
            logger_.print("PROC: extra hit from: ", hit_effect.name);
            swing_weapon(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                         rampage_stacks, hit_type == Hit_type::spell ? Extra_attack_type::all : Extra_attack_type::self);
            break;
        }
        case Hit_effect::Type::stat_boost: {
            hit_effect.procs++;
            logger_.print("PROC: ", hit_effect.name, " stats increased for ", hit_effect.duration, "s");
            buff_manager_.add_combat_buff(hit_effect, time_keeper_.time);
            break;
        }
        case Hit_effect::Type::rage_boost: {
            hit_effect.procs++;
            logger_.print("PROC: ", hit_effect.name, ". Current rage: ", int(rage));
            gain_rage(hit_effect.damage);
                break;
            }
            case Hit_effect::Type::damage_magic: {
            hit_effect.procs++;
                // * 0.83 Assumes a static 17% chance to resist.
                // (100 + special_stats.spell_crit / 2) / 100 is the average damage gained from a x1.5 spell crit
                double effect_damage = hit_effect.damage * 0.83 * (100 + special_stats.spell_crit / 2) / 100 *
                                       (1 + special_stats.damage_mod_spell);
                damage_sources.add_damage(Damage_source::item_hit_effects, effect_damage, time_keeper_.time);
            logger_.print("PROC: ", hit_effect.name, " does ", effect_damage, " magic damage.");
                break;
            }
            case Hit_effect::Type::damage_physical: {
            hit_effect.procs++;
                auto hit = generate_hit(main_hand_weapon, hit_effect.damage, main_hand_weapon, hit_table_yellow_mh_,
                                        special_stats, damage_sources);
                damage_sources.add_damage(Damage_source::item_hit_effects, hit.damage, time_keeper_.time);
                if (config.display_combat_debug)
                {
                logger_.print("PROC: ", hit_effect.name, hit_result_to_string(hit.hit_result), " does ",
                                   int(hit.damage), " physical damage");
                }
                if (hit.hit_result != Hit_result::miss && hit.hit_result != Hit_result::dodge)
                {
                hit_effects(main_hand_weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                            rampage_stacks);
                }
                break;
            }
            default:
            logger_.print("PROC: ", hit_effect.name, " has unknown type ", static_cast<int>(hit_effect.type));
            assert(false);
                break;
            }
        }
    }

double Combat_simulator::rage_generation(const Hit_outcome& hit_outcome, const Weapon_sim& weapon) const
{
    auto hit_factor = weapon.socket == Socket::main_hand ? 3.5/2 : 1.75/2;
    if (hit_outcome.hit_result == Hit_result::crit) hit_factor *= 2;
    auto rage_gain = hit_outcome.rage_damage * rage_factor + hit_factor * weapon.swing_speed;
    if (config.talents.endless_rage) rage_gain *= 1.25;
    return rage_gain;
}

void Combat_simulator::swing_weapon(Weapon_sim& weapon, Weapon_sim& main_hand_weapon, Special_stats& special_stats,
                                    Damage_sources& damage_sources, int& flurry_charges,
                                    int& rampage_stacks, Extra_attack_type extra_attack_type)
{
    double swing_damage = weapon.swing(special_stats.attack_power);

    maybe_remove_flurry(flurry_charges, special_stats);

    auto white_replaced = false;
    if (ability_queue_manager.heroic_strike_queued && weapon.socket == Socket::main_hand)
    {
        ability_queue_manager.heroic_strike_queued = false;
        if (rage >= heroic_strike_rage_cost_ && config.dpr_settings.compute_dpr_hs_)
        {
            logger_.print("Performing Heroic Strike (DPR)");
            spend_rage(hit_table_yellow_mh_.isMissOrDodge() ? heroic_strike_rage_cost_ :
                                                              0.2 * heroic_strike_rage_cost_);
        }
        else if (rage >= heroic_strike_rage_cost_)
        {
            logger_.print("Performing Heroic Strike");
            swing_damage += config.combat.heroic_strike_damage;
            const auto& hit_outcome = generate_hit(main_hand_weapon, swing_damage, main_hand_weapon, hit_table_yellow_mh_,
                                                   special_stats, damage_sources);
            if (hit_outcome.hit_result == Hit_result::miss || hit_outcome.hit_result == Hit_result::dodge)
            {
                spend_rage(0.2 * heroic_strike_rage_cost_); // Refund rage for missed/dodged heroic strikes.
                if (hit_outcome.hit_result == Hit_result::dodge && config.set_bonus_effect.warbringer_4_set) gain_rage(2);
            }
            else
            {
                spend_rage(heroic_strike_rage_cost_);
                maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
                unbridled_wrath(weapon);
                hit_effects(weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                            rampage_stacks, Hit_type::next_melee, extra_attack_type);
            }
            damage_sources.add_damage(Damage_source::heroic_strike, hit_outcome.damage, time_keeper_.time);
            white_replaced = true;
        }
        else
        {
            // Failed to pay rage for heroic strike
            logger_.print("Failed to pay rage for Heroic Strike");
        }
        logger_.print("Current rage: ", int(rage));
    }
    else if (ability_queue_manager.cleave_queued && weapon.socket == Socket::main_hand)
    {
        ability_queue_manager.cleave_queued = false;
        if (rage >= 20 && config.dpr_settings.compute_dpr_cl_)
        {
            logger_.print("Performing Cleave (DPR)");
            spend_rage(20);
        }
        else if (rage >= 20)
        {
            logger_.print("Performing Cleave! #targets = boss + ", number_of_extra_targets_, " adds");
            logger_.print("Cleave hits: ", std::min(number_of_extra_targets_ + 1, 2), " targets");
            spend_rage(20);
            swing_damage += cleave_bonus_damage_;
            double total_damage = 0;
            for (int i = 0, n = number_of_extra_targets_ > 0 ? 2 : 1; i < n; i++)
            {
                const auto& hit_outcome = generate_hit(main_hand_weapon, swing_damage, main_hand_weapon, hit_table_yellow_mh_,
                                                       special_stats, damage_sources, i == 0);
                total_damage += hit_outcome.damage;
                if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
                {
                    maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
                    unbridled_wrath(weapon);
                    hit_effects(weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges,
                                rampage_stacks, Hit_type::next_melee, extra_attack_type);
                }
                else if (hit_outcome.hit_result == Hit_result::dodge)
                {
                    if (config.set_bonus_effect.warbringer_4_set) gain_rage(2);
                }
            }
            damage_sources.add_damage(Damage_source::cleave, total_damage, time_keeper_.time);
            white_replaced = true;
        }
        else
        {
            logger_.print("Failed to pay rage for Cleave");
        }
        logger_.print("Current rage: ", int(rage));
    }

    if (!white_replaced)
    {
        auto is_queued = (ability_queue_manager.heroic_strike_queued && !config.dpr_settings.compute_dpr_hs_) || (ability_queue_manager.cleave_queued && !config.dpr_settings.compute_dpr_cl_);

        auto hit_table = weapon.socket == Socket::main_hand ? hit_table_white_mh_ : is_queued ? hit_table_white_oh_queued_ : hit_table_white_oh_;

        const auto& hit_outcome = generate_hit(main_hand_weapon, swing_damage, weapon, hit_table, special_stats, damage_sources);
        buff_manager_.remove_charge(windfury_attack_, time_keeper_.time, logger_);
        if (hit_outcome.hit_result != Hit_result::miss && hit_outcome.hit_result != Hit_result::dodge)
        {
            gain_rage(rage_generation(hit_outcome, weapon));
            maybe_gain_flurry(hit_outcome.hit_result, flurry_charges, special_stats);
            unbridled_wrath(weapon);
            hit_effects(weapon, main_hand_weapon, special_stats, damage_sources, flurry_charges, rampage_stacks,
                        Hit_type::melee, extra_attack_type);
        }
        else if (hit_outcome.hit_result == Hit_result::dodge)
        {
            if (config.set_bonus_effect.warbringer_4_set)
            {
                gain_rage(2);
            }
            gain_rage(rage_generation(hit_outcome, weapon));
            logger_.print("Rage gained since the enemy dodged.");
        }

        logger_.print("Current rage: ", int(rage));
        if (weapon.socket == Socket::main_hand)
        {
            damage_sources.add_damage(Damage_source::white_mh, hit_outcome.damage, time_keeper_.time);
        }
        else
        {
            damage_sources.add_damage(Damage_source::white_oh, hit_outcome.damage, time_keeper_.time);
        }
    }
}

void update_swing_timers(double current_time, Weapon_sim& mh, Weapon_sim& oh, double oldHaste, double haste)
{
    //TODO(vigo) check for == current_time circumstances
    if (mh.next_swing <= current_time)
    {
        mh.next_swing = current_time + mh.swing_speed / (1 + haste);
    }
    else if (haste != oldHaste)
    {
        mh.next_swing = current_time + (mh.next_swing - current_time) * (1 + oldHaste) / (1 + haste);
    }

    if (oh.socket != Socket::off_hand) return;

    if (oh.next_swing <= current_time)
    {
        oh.next_swing = current_time + oh.swing_speed / (1 + haste);
    }
    else if (haste != oldHaste)
    {
        oh.next_swing = current_time + (oh.next_swing - current_time) * (1 + oldHaste) / (1 + haste);
    }
}

void Combat_simulator::simulate(const Character& character, int n_simulations, double init_mean,
                                double init_variance, int init_simulations)
{
    dps_distribution_.mean_ = init_mean;
    dps_distribution_.variance_ = init_variance;
    dps_distribution_.n_samples_ = init_simulations;
    config.n_batches = n_simulations;
    simulate(character, init_simulations, false, false);
}

void Combat_simulator::simulate(const Character& character, int init_iteration, bool log_data, bool reset_dps)
{
    if (log_data)
    {
        reset_time_lapse();
        init_histogram();
    }
    damage_distribution_ = Damage_sources(false);

    int n_damage_batches = config.n_batches;
    if (config.display_combat_debug)
    {
        logger_ = Logger(time_keeper_);
        n_damage_batches = 1;
    }

    flurry_uptime_ = 0;
    rage_lost_stance_swap_ = 0;
    rage_lost_capped_ = 0;
    oh_queued_uptime_ = 0;
    rampage_uptime_ = 0;
    const auto starting_special_stats = character.total_special_stats;

    std::vector<Weapon_sim> weapons;
    for (const auto& wep : character.weapons)
    {
        auto& weapon = weapons.emplace_back(wep, starting_special_stats);
        compute_hit_tables(starting_special_stats, weapon);

        // sanitize hit_effects
        for (auto& e : weapon.hit_effects)
        {
            // TODO(vigo) this is... not very nice
            if (e.name == "windfury_totem")
            {
                windfury_attack_.special_stats_boost.attack_power = e.special_stats_boost.attack_power;
            }

            if (e.probability == 0)
            {
                e.probability = e.ppm * weapon.swing_speed / 60;
            }

            e.sanitize();
        }
    }

    // TODO(vigo) just make character part of the "sim context"
    // Passing set bonuses that are not stats from character to config
    config.set_bonus_effect.warbringer_2_set = character.set_bonus_effect.warbringer_2_set;
    config.set_bonus_effect.warbringer_4_set = character.set_bonus_effect.warbringer_4_set;
    config.set_bonus_effect.destroyer_2_set = character.set_bonus_effect.destroyer_2_set;
    config.set_bonus_effect.destroyer_4_set = character.set_bonus_effect.destroyer_4_set;

    if (config.set_bonus_effect.warbringer_2_set)
    {
        whirlwind_rage_cost_ = 20;
    }

    if (config.set_bonus_effect.destroyer_4_set)
    {
        mortal_strike_rage_cost_ = 25;
        bloodthirst_rage_cost_ = 25;
    }

    const bool is_dual_wield = character.is_dual_wield();

    const double sim_time = config.sim_time;
    const double time_execute_phase = sim_time * (100.0 - config.execute_phase_percentage_) / 100.0;

    auto use_effect_order = get_use_effect_order(character);

    auto empty_hit_effects = std::vector<Hit_effect>();
    if (is_dual_wield)
    {
        buff_manager_.initialize(weapons[0].hit_effects,weapons[1].hit_effects, use_effect_order, this);
    }
    else
    {
        buff_manager_.initialize(weapons[0].hit_effects,empty_hit_effects, use_effect_order, this);
    }

    if (reset_dps)
    {
        dps_distribution_.reset();
        proc_data_.clear();
        buff_manager_.reset_statistics();
    }

    for (int iter = init_iteration; iter < n_damage_batches + init_iteration; iter++)
    {
        ability_queue_manager.reset();
        slam_manager = Slam_manager(1.5 - 0.5 * config.talents.improved_slam);
        rage = config.combat.initial_rage;
        auto special_stats = starting_special_stats;
        Damage_sources damage_sources(log_data);

        buff_manager_.reset(special_stats, damage_sources);

        rage_spent_on_execute_ = 0;

        int flurry_charges = 0; 
        int rampage_stacks = 0;
        bool apply_delayed_armor_reduction = false;
        bool execute_phase = false;

        double flurry_uptime = 0.0;

        int mh_hits = 0;
        int oh_hits = 0;
        int oh_hits_w_queued = 0;
        int mh_hits_w_rampage = 0;

        weapons[0].next_swing = 0;
        if (is_dual_wield) weapons[1].next_swing = 0.5 * (weapons[1].swing_speed / (1 + special_stats.haste)); // de-sync mh/oh swing timers

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
        for (const auto& ue : use_effect_order)
                {
            if (ue.first >= 0) break;

            // set everything up so it works ;)
            time_keeper_.prepare(ue.first);
            rage -= ue.second.rage_boost;

            buff_manager_.increment(time_keeper_, logger_);
        }

        time_keeper_.reset();

        for (auto& over_time_effect : over_time_effects_)
        {
            buff_manager_.add_over_time_buff(over_time_effect, 0);
        }

        // First global sunder
        bool first_global_sunder = config.first_global_sunder_;

        if (config.combat.first_hit_heroic_strike && rage >= heroic_strike_rage_cost_)
        {
            ability_queue_manager.queue_heroic_strike();
        }

        while (time_keeper_.time < sim_time)
        {
            double next_mh_swing = weapons[0].next_swing;
            double next_oh_swing = is_dual_wield ? weapons[1].next_swing : std::numeric_limits<double>::max();
            double next_buff_event = buff_manager_.next_event(time_keeper_.time);
            double next_slam_finish = slam_manager.next_finish();
            double next_event = time_keeper_.get_next_event(next_mh_swing, next_oh_swing,
                                                            next_buff_event, next_slam_finish, sim_time);
            if (flurry_charges > 0) flurry_uptime += next_event - time_keeper_.time;
            time_keeper_.increment(next_event);

            if (time_keeper_.time > sim_time)
            {
                break;
            }

            double oldHaste = special_stats.haste;

            buff_manager_.increment(time_keeper_, logger_);

            if (buff_manager_.need_to_recompute_hit_tables)
            {
                for (const auto& weapon : weapons)
                {
                    compute_hit_tables(special_stats, weapon);
                }
                buff_manager_.need_to_recompute_hit_tables = false;
            }

            if (buff_manager_.need_to_recompute_mitigation)
            {
                recompute_mitigation_ = true;
                buff_manager_.need_to_recompute_mitigation = false;
            }

            if (!apply_delayed_armor_reduction && time_keeper_.time > 6.0 && config.exposed_armor)
            {
                apply_delayed_armor_reduction = true;
                recompute_mitigation_ = true;
                logger_.print("Applying improved exposed armor!");
            }

            if (recompute_mitigation_)
            {
                int target_armor =
                    config.main_target_initial_armor_ - armor_reduction_from_spells_ - special_stats.gear_armor_pen;
                if (apply_delayed_armor_reduction)
                {
                    target_armor -= armor_reduction_delayed_;
                }
                target_armor = std::max(target_armor, 0);
                armor_reduction_factor_ = armor_reduction_factor(target_armor);
                logger_.print("Target armor: ", target_armor, ". Mitigation factor: ", 1 - armor_reduction_factor_, "%.");
                if (config.multi_target_mode_)
                {
                    int extra_target_armor = config.extra_target_initial_armor_ - special_stats.gear_armor_pen;
                    extra_target_armor = std::max(extra_target_armor, 0);
                    armor_reduction_factor_add = armor_reduction_factor(extra_target_armor);

                    logger_.print("Extra targets armor: ", extra_target_armor,
                                   ". Mitigation factor: ", 1 - armor_reduction_factor_add, "%.");
                }
                recompute_mitigation_ = false;
            }

            if (config.multi_target_mode_ && number_of_extra_targets_ > 0 &&
                time_keeper_.time / sim_time > config.extra_target_duration)
            {
                logger_.print("Extra targets die.");
                number_of_extra_targets_ = 0;
            }

            if (slam_manager.is_slam_casting())
            {
                if (!slam_manager.ready(time_keeper_.time))
                {
                    continue;
                }

                slam(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                slam_manager.finish_slam();

                // reset MH
                weapons[0].next_swing = time_keeper_.time + weapons[0].swing_speed / (1 + special_stats.haste);

                // ... and now OH as well
                if (is_dual_wield) {
                    weapons[1].next_swing = time_keeper_.time + weapons[1].swing_speed / (1 + special_stats.haste);
                }

                // keep update_swing_timer() from applying haste changes again
                oldHaste = special_stats.haste;
            }

            bool mh_swing = weapons[0].next_swing < time_keeper_.time;
            bool oh_swing = is_dual_wield ? weapons[1].next_swing < time_keeper_.time : false;

            if (mh_swing)
            {
                mh_hits++;
                if (rampage_stacks > 0)
                {
                    mh_hits_w_rampage++;
                }
                swing_weapon(weapons[0], weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
            }

            if (oh_swing)
            {
                oh_hits++;
                if (ability_queue_manager.is_ability_queued())
                {
                    oh_hits_w_queued++;
                }
                swing_weapon(weapons[1], weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
            }

            if (!execute_phase)
            {
                if (time_keeper_.time > time_execute_phase)
                {
                    logger_.print("------------ Execute phase! ------------");
                    execute_phase = true;
                }
            }

            if (use_sweeping_strikes_)
            {
                if (time_keeper_.sweeping_strikes_ready() && time_keeper_.global_ready() && rage >= 30)
                {
                    logger_.print("Sweeping strikes!");
                    time_keeper_.global_cast(1.5);
                    spend_rage(30);
                    time_keeper_.sweeping_strikes_cast(30);
                    sweeping_strikes_charges_ = 10;
                }
            }

            if (first_global_sunder)
            {
                if (time_keeper_.global_ready() && rage >= 15)
                {
                    logger_.print("Sunder Armor!");
                    time_keeper_.global_cast(1.5);
                    spend_rage(15);
                    first_global_sunder = false;
                }
            }
            else if (execute_phase)
            {
                if (config.combat.use_slam && config.combat.use_sl_in_exec_phase)
                {
                    if (!slam_manager.is_slam_casting() && time_keeper_.global_ready() && rage >= 15)
                    {
                        if (start_cast_slam(mh_swing, weapons[0]))
                        {
                            continue;
                        }
                    }
                }
                if (use_mortal_strike_ && config.combat.use_ms_in_exec_phase)
                {
                    bool ms_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        ms_ww = std::max(time_keeper_.whirlwind_cd(), 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.mortal_strike_ready() && time_keeper_.global_ready() && rage >= 30 && ms_ww)
                    {
                        mortal_strike(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                    }
                }
                if (use_bloodthirst_ && config.combat.use_bt_in_exec_phase)
                {
                    bool bt_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        bt_ww = std::max(time_keeper_.whirlwind_cd(), 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.blood_thirst_ready() && time_keeper_.global_ready() && rage >= 30 && bt_ww)
                    {
                        bloodthirst(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                    }
                }
                if (config.combat.use_whirlwind && config.combat.use_ww_in_exec_phase)
                {
                    bool use_ww = true;
                    if (use_rampage_)
                    {
                        use_ww = time_keeper_.rampage_cd() > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ww = std::max(time_keeper_.blood_thirst_cd(), 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww = std::max(time_keeper_.mortal_strike_cd(), 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (time_keeper_.whirlwind_ready() && rage > config.combat.whirlwind_rage_thresh && rage >= whirlwind_rage_cost_ &&
                        time_keeper_.global_ready() && use_ww)
                    {
                        if (is_dual_wield)
                        {
                            whirlwind(weapons[0], weapons[1], special_stats, damage_sources, flurry_charges, rampage_stacks, true);
                        }
                        else
                        {
                            whirlwind(weapons[0], weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                        }

                    }
                }
                if (time_keeper_.global_ready() && rage >= execute_rage_cost_)
                {
                    execute(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                }

                // Heroic strike or Cleave
                if (config.combat.use_hs_in_exec_phase && config.combat.use_heroic_strike)
                {
                    if (number_of_extra_targets_ > 0 && config.combat.cleave_if_adds)
                    {
                        if (rage > config.combat.cleave_rage_thresh && !ability_queue_manager.cleave_queued && rage >= 20)
                        {
                            ability_queue_manager.queue_cleave();
                            logger_.print("Cleave activated");
                        }
                    }
                    else
                    {
                        if (rage > config.combat.hs_rage_thresh_exec_phase && !ability_queue_manager.heroic_strike_queued &&
                            rage >= heroic_strike_rage_cost_)
                        {
                            ability_queue_manager.queue_heroic_strike();
                            logger_.print("Heroic strike activated");
                        }
                    }
                }
            }
            else
            {
                if (config.combat.use_slam)
                {
                    if (!slam_manager.is_slam_casting() && time_keeper_.global_ready() && rage >= 15)
                    {
                        if (start_cast_slam(mh_swing, weapons[0]))
                        {
                            continue;
                        }
                    }
                }

                if (use_rampage_)
                {
                    if (time_keeper_.rampage_cd() < config.combat.rampage_use_thresh && time_keeper_.global_ready() && rage >= 20 && time_keeper_.can_do_rampage())
                    {
                        time_keeper_.rampage_cast(30.0);
                        time_keeper_.global_cast(1.5);
                        spend_rage(20);
                        if (rampage_stacks == 0)
                        {
                            special_stats += {0, 0, 50};
                            rampage_stacks = 1;
                        }
                        logger_.print("Rampage!");
                        logger_.print("Current rage: ", int(rage));
                    }
                    else if (time_keeper_.rampage_cd() < 0.0 && rampage_stacks > 0)
                    {
                        special_stats -= {0, 0, 50.0 * rampage_stacks};
                        rampage_stacks = 0;
                        logger_.print("Rampage fades.");
                    }
                }

                if (use_bloodthirst_)
                {
                    bool bt_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        bt_ww = std::max(time_keeper_.whirlwind_cd(), 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.blood_thirst_ready() && time_keeper_.global_ready() && rage >= 30 && bt_ww)
                    {
                        bloodthirst(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                    }
                }

                if (use_mortal_strike_)
                {
                    bool ms_ww = true;
                    if (config.combat.use_whirlwind)
                    {
                        ms_ww = std::max(time_keeper_.whirlwind_cd(), 0.1) > config.combat.bt_whirlwind_cooldown_thresh;
                    }
                    if (time_keeper_.mortal_strike_ready() && time_keeper_.global_ready() && rage >= 30 && ms_ww)
                    {
                        mortal_strike(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                    }
                }

                if (config.combat.use_whirlwind)
                {
                    bool use_ww = true;
                    if (use_rampage_)
                    {
                        use_ww = time_keeper_.rampage_cd() > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ww = std::max(time_keeper_.blood_thirst_cd(), 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ww = std::max(time_keeper_.mortal_strike_cd(), 0.1) > config.combat.whirlwind_bt_cooldown_thresh;
                    }
                    if (time_keeper_.whirlwind_ready() && rage > config.combat.whirlwind_rage_thresh && rage >= whirlwind_rage_cost_ &&
                        time_keeper_.global_ready() && use_ww)
                    {
                        if (is_dual_wield)
                        {
                            whirlwind(weapons[0], weapons[1], special_stats, damage_sources, flurry_charges, rampage_stacks, true);
                        }
                        else
                        {
                            whirlwind(weapons[0], weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                        }

                    }
                }

                if (config.combat.use_overpower)
                {
                    bool use_op = true;
                    if (use_rampage_)
                    {
                        use_op &= time_keeper_.rampage_cd() > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_op &= time_keeper_.blood_thirst_cd() > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_op &= time_keeper_.mortal_strike_cd() > config.combat.overpower_bt_cooldown_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_op &= time_keeper_.whirlwind_cd() > config.combat.overpower_ww_cooldown_thresh;
                    }
                    if (time_keeper_.overpower_ready() && rage < config.combat.overpower_rage_thresh && rage >= 5 &&
                        time_keeper_.global_ready() && time_keeper_.can_do_overpower() && use_op)
                    {
                        overpower(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
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
                        use_ham &= time_keeper_.rampage_cd() > 3.0;
                    }
                    if (use_bloodthirst_)
                    {
                        use_ham &= time_keeper_.blood_thirst_cd() > config.combat.hamstring_cd_thresh;
                    }
                    if (use_mortal_strike_)
                    {
                        use_ham &= time_keeper_.mortal_strike_cd() > config.combat.hamstring_cd_thresh;
                    }
                    if (config.combat.use_whirlwind)
                    {
                        use_ham &= time_keeper_.whirlwind_cd() > config.combat.hamstring_cd_thresh;
                    }
                    if (rage > config.combat.hamstring_thresh_dd && time_keeper_.global_ready() && rage >= 10 && use_ham)
                    {
                        hamstring(weapons[0], special_stats, damage_sources, flurry_charges, rampage_stacks);
                    }
                }

                // Heroic strike or Cleave
                if (config.combat.use_heroic_strike)
                {
                    if (number_of_extra_targets_ > 0 && config.combat.cleave_if_adds)
                    {
                        if (rage > config.combat.cleave_rage_thresh && !ability_queue_manager.cleave_queued && rage >= 20)
                        {
                            ability_queue_manager.queue_cleave();
                            logger_.print("Cleave activated");
                        }
                    }
                    else
                    {
                    if (rage > config.combat.heroic_strike_rage_thresh && !ability_queue_manager.heroic_strike_queued &&
                            rage >= heroic_strike_rage_cost_)
                        {
                            ability_queue_manager.queue_heroic_strike();
                            logger_.print("Heroic strike activated");
                        }
                    }
                }
            }

            // end of turn - update swing timers if necessary
            update_swing_timers(time_keeper_.time, weapons[0], is_dual_wield ? weapons[1] : weapons[0], oldHaste, special_stats.haste);
        }
        // end of batch

        buff_manager_.update_aura_uptimes(sim_time);

        double new_sample = damage_sources.sum_damage_sources() / sim_time;
        dps_distribution_.add_sample(new_sample);
        damage_distribution_ = damage_distribution_ + damage_sources;

        rampage_uptime_ = Statistics::update_mean(rampage_uptime_, iter + 1, double(mh_hits_w_rampage) / mh_hits);
        if (is_dual_wield)
        {
            oh_queued_uptime_ =
                Statistics::update_mean(oh_queued_uptime_, iter + 1, double(oh_hits_w_queued) / oh_hits);
        }
        flurry_uptime_ = Statistics::update_mean(flurry_uptime_, iter + 1, flurry_uptime / time_keeper_.time);
        avg_rage_spent_executing_ =
            Statistics::update_mean(avg_rage_spent_executing_, iter + 1, rage_spent_on_execute_);
        if (log_data)
        {
            add_damage_source_to_time_lapse(damage_sources.damage_instances);
            hist_y[static_cast<int>(new_sample / hist_resolution)]++;
        }
    }

    for (const auto& he : weapons[0].hit_effects)
    {
        proc_data_[he.name] += he.procs;
    }
    if (is_dual_wield)
    {
        for (const auto& he : weapons[1].hit_effects)
        {
            proc_data_[he.name] += he.procs;
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
    auto use_effects_all = use_effects_all_;
    for (const auto& use_effect : character.use_effects)
    {
        use_effects_all.push_back(use_effect);
    }

    if (config.enable_unleashed_rage > 0)
    {
        Special_stats ss;
        ss.ap_multiplier = 0.1;
        use_effects_all.emplace_back(Use_effect{"unleashed_rage", Use_effect::Effect_socket::unique, {}, ss, 0, config.sim_time - config.unleashed_rage_start_, config.sim_time + 5, false});
    }

    double ap_equiv;
    if (character.is_dual_wield())
    {
        ap_equiv = get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                               character.weapons[1], config.sim_time, {});
    }
    else
    {
        ap_equiv = get_character_ap_equivalent(character.total_special_stats, character.weapons[0],
                                               config.sim_time, {});
    }
    return Use_effects::compute_use_effect_order(use_effects_all, character.total_special_stats, config.sim_time, ap_equiv, 0, 0);
}

void Combat_simulator::init_histogram()
{
    for (int i = 0; i < 1000; i++)
    {
        hist_x.push_back(static_cast<int>(i * hist_resolution));
        hist_y.push_back(0);
    }
}

void Combat_simulator::normalize_timelapse()
{
    for (auto& damage_time_lapse_i : damage_time_lapse_)
    {
        for (auto& singe_damage_instance : damage_time_lapse_i)
        {
            singe_damage_instance /= config.n_batches;
        }
    }
}

void Combat_simulator::prune_histogram()
{
    size_t start_idx = 0;
    for (; start_idx < hist_x.size(); ++start_idx)
    {
        if (hist_y[start_idx] != 0) break;
    }

    int end_idx = static_cast<int>(hist_x.size()) - 1;
    for (; end_idx >= 0; --end_idx)
    {
        if (hist_y[end_idx] != 0) break;
    }

    hist_x = std::vector<int>(&hist_x[start_idx], &hist_x[end_idx] + 1);
    hist_y = std::vector<int>(&hist_y[start_idx], &hist_y[end_idx] + 1);
}

std::vector<std::string> Combat_simulator::get_aura_uptimes() const
{
    std::vector<std::string> aura_uptimes;
    double total_sim_time = config.n_batches * config.sim_time;
    for (const auto& aura : buff_manager_.get_aura_uptimes_map())
    {
        double uptime = aura.second / total_sim_time;
        aura_uptimes.emplace_back(aura.first + " " + std::to_string(100 * uptime));
    }
    if (flurry_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("Flurry " + std::to_string(100 * flurry_uptime_));
    }
    if (oh_queued_uptime_ != 0.0)
    {
        aura_uptimes.emplace_back("'Heroic_strike_bug' " + std::to_string(100 * oh_queued_uptime_));
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
    std::vector<double> history(static_cast<size_t>(std::ceil(config.sim_time / time_lapse_resolution)));
    damage_time_lapse_.assign(static_cast<size_t>(Damage_source::size), history);
}

void Combat_simulator::add_damage_source_to_time_lapse(std::vector<Damage_instance>& damage_instances)
{
    for (const auto& damage_instance : damage_instances)
    {
        auto first_idx = static_cast<size_t>(damage_instance.damage_source);
        auto second_idx = static_cast<size_t>(damage_instance.time_stamp / time_lapse_resolution);
        damage_time_lapse_[first_idx][second_idx] += damage_instance.damage;
    }
}

std::string Combat_simulator::get_debug_topic() const
{
    return logger_.get_debug_topic();
}
