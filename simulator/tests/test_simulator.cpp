#include "Armory.hpp"
#include "BinomialDistribution.hpp"
#include "Combat_simulator.hpp"
#include "Statistics.hpp"
#include "simulation_fixture.cpp"

#include <chrono>

TEST_F(Sim_fixture, test_no_crit_equals_no_flurry_uptime)
{
    config.talents.flurry = 5;
    config.sim_time = 500.0;
    config.main_target_level = 70;

    character.total_special_stats.critical_strike = 0;

    sim.set_config(config);
    sim.simulate(character);

    EXPECT_EQ(sim.get_flurry_uptime(), 0.0);
}

TEST_F(Sim_fixture, test_max_crit_equals_high_flurry_uptime)
{
    config.talents.flurry = 5;
    config.sim_time = 500.0;
    config.main_target_level = 70;

    character.total_special_stats.critical_strike = 100;

    sim.set_config(config);
    sim.simulate(character);

    EXPECT_GT(sim.get_flurry_uptime(), .95);
}

TEST_F(Sim_fixture, test_endless_rage)
{
    config.sim_time = 100000.0;
    config.n_batches = 1;
    config.main_target_initial_armor_ = 0.0;
    config.combat.initial_rage = 100;

    sim.set_config(config);
    sim.simulate(character);

    double rage_normal = sim.get_rage_lost_capped();

    Combat_simulator sim{};
    config.talents.endless_rage = true;
    sim.set_config(config);
    sim.simulate(character);

    double rage_endless = sim.get_rage_lost_capped();

    double rage_ratio = 1 - (rage_endless / rage_normal * 1.25);

    EXPECT_NEAR(rage_ratio, 0.0, 1);
}

TEST_F(Sim_fixture, test_bloodthirst_count)
{
    config.talents.bloodthirst = 1;
    config.combat.use_bloodthirst = true;
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;

    character.total_special_stats.attack_power = 10000;

    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();

    EXPECT_NEAR(distrib.bloodthirst_count, config.sim_time / 6.0 * config.n_batches, 1.0);
}

TEST_F(Sim_fixture, test_that_with_infinite_rage_all_hits_are_heroic_strike)
{
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;
    config.combat.first_hit_heroic_strike = true;
    config.combat.use_heroic_strike = true;
    config.talents.improved_heroic_strike = 3;
    config.sim_time = 500.0;

    character.total_special_stats.attack_power = 10000;
    character.weapons[0].swing_speed = 1.9;
    character.weapons[1].swing_speed = 1.7;

    sim.set_config(config);
    sim.simulate(character);

    auto distrib = sim.get_damage_distribution();
    auto hs_uptime = sim.get_hs_uptime();
    double expected_swings_per_simulation = (config.sim_time - 1.0) / character.weapons[0].swing_speed + 1;
    double tolerance = 1.0 / character.weapons[0].swing_speed;
    EXPECT_NEAR(distrib.heroic_strike_count / double(config.n_batches), expected_swings_per_simulation, tolerance);

    EXPECT_FLOAT_EQ(hs_uptime, 1);
}

TEST_F(Sim_fixture, test_dps_return_matches_heristic_values)
{
    config.main_target_initial_armor_ = 0.0;
    config.sim_time = 1000.0;
    config.n_batches = 500.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    sim.set_config(config);
    sim.simulate(character);

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double glancing_chance = 0.24;
    double hit_chance = (1 - dodge_chance - miss_chance - glancing_chance);
    double dps_white = 50 + 25;
    double expected_dps = dps_white * hit_chance + dps_white * 0.75 * glancing_chance;

    Distribution distribution = sim.get_dps_distribution();
    auto conf_interval = distribution.confidence_interval_of_the_mean(0.99);

    EXPECT_LT(conf_interval.first, expected_dps);
    EXPECT_GT(conf_interval.second, expected_dps);
}

TEST_F(Sim_fixture, test_hit_effects_extra_hit)
{
    config.sim_time = 1000.0;
    config.n_batches = 500.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0, oh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    auto conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    // OH proc's trigger main hand swings
    expected_swings_mh += expected_procs_mh + expected_procs_oh;

    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

// expected to fail - the statistical model is plain wrong
TEST_F(Sim_fixture, test_hit_effects_icd)
{
    config.sim_time = 1000.0;
    config.n_batches = 500.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 1;
    double oh_proc_prob = 1;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 10, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 10, oh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed / 10;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed / 10;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    auto conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    // OH proc's trigger main hand swings
    expected_swings_mh += expected_procs_mh + expected_procs_oh;

    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST_F(Sim_fixture, test_hit_effects_windfury_hit)
{
    config.sim_time = 1000.0;
    config.n_batches = 500.0;
    config.combat.use_hamstring = true;
    config.essence_of_the_red_ = true;
    config.combat.initial_rage = 100;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::windfury_hit, {}, {}, 0, 0, 0, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::windfury_hit, {}, {}, 0, 0, 0, oh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    // WF should only procs from white hit
    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    auto conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    // OH proc's trigger main hand swings
    expected_swings_mh += expected_procs_mh + expected_procs_oh;

    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST_F(Sim_fixture, test_hit_effects_sword_spec)
{
    config.sim_time = 1000.0;
    config.n_batches = 500.0;
    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::sword_spec, {}, {}, 0, 0, 0, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::sword_spec, {}, {}, 0, 0, 0, mh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    // WF should only procs from white hit
    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    auto conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    // OH proc's trigger main hand swings
    expected_swings_mh += expected_procs_mh + expected_procs_oh;

    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST_F(Sim_fixture, test_hit_effects_physical_damage)
{
    config.sim_time = 100000.0;
    config.n_batches = 1.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::damage_physical, {}, {}, 100, 0, 0, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::damage_physical, {}, {}, 100, 0, 0, oh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);
    double yellow_hit_chance = (1 - 0.09);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;

    double second_order_procs =
        (expected_procs_oh + expected_procs_mh) * (Statistics::geometric_series(yellow_hit_chance * mh_proc_prob) - 1);
    expected_procs_mh += second_order_procs;

    double expected_total_procs = expected_procs_oh + expected_procs_mh;
    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(sources.item_hit_effects_count, expected_total_procs, 0.03 * expected_total_procs);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);
}

TEST_F(Sim_fixture, test_hit_effects_magic_damage)
{
    config.sim_time = 1000.0;
    config.n_batches = 100.0;

    character.total_special_stats.critical_strike = 0;
    character.total_special_stats.attack_power = 0;

    double mh_proc_prob = 0.1;
    double oh_proc_prob = 0.2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::damage_magic, {}, {}, 100, 0, 0, mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::damage_magic, {}, {}, 100, 0, 0, oh_proc_prob};
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_swings_oh = config.n_batches * config.sim_time / character.weapons[1].swing_speed;
    BinomialDistribution bin_dist_oh{expected_swings_oh, hit_chance * oh_proc_prob};
    double expected_procs_oh = bin_dist_oh.mean_;
    double conf_interval_oh = bin_dist_oh.confidence_interval_width(0.99);

    double expected_swings_mh = config.n_batches * config.sim_time / character.weapons[0].swing_speed;
    BinomialDistribution bin_dist_mh{expected_swings_mh, hit_chance * mh_proc_prob};
    double expected_procs_mh = bin_dist_mh.mean_;
    double conf_interval_mh = bin_dist_mh.confidence_interval_width(0.99);

    double expected_total_procs = expected_procs_oh + expected_procs_mh;
    EXPECT_NEAR(sources.white_mh_count, expected_swings_mh, 0.01 * expected_swings_mh);
    EXPECT_NEAR(sources.white_oh_count, expected_swings_oh, 0.01 * expected_swings_oh);

    EXPECT_NEAR(sources.item_hit_effects_count, expected_total_procs, 0.03 * expected_total_procs);

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, conf_interval_mh / 2);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, conf_interval_oh / 2);
}

TEST_F(Sim_fixture, test_hit_effects_stat_boost_short_duration)
{
    config.sim_time = 1000.0;
    config.n_batches = 100.0;

    double mh_proc_prob = 1.0;
    double mh_proc_duration = 1;
    double oh_proc_prob = 1.0;
    double oh_proc_duration = 2;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration, 0,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration, 0,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 3.0;
    character.weapons[1].swing_speed = 3.0;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();
    auto aura_uptimes = sim.get_aura_uptimes_map();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_procs_oh = hit_chance * sources.white_oh_count * oh_proc_prob;
    double expected_uptime_oh = expected_procs_oh * oh_proc_duration;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    double expected_uptime_mh = expected_procs_mh * mh_proc_duration;

    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.03 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.03 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"], expected_uptime_mh, 0.03 * expected_uptime_mh);
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"], expected_uptime_oh, 0.03 * expected_uptime_oh);
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"] != aura_uptimes["off_hand_test_wep_mh"]);
}

TEST_F(Sim_fixture, test_hit_effects_stat_boost_long_duration)
{
    config.sim_time = 10000.0;
    config.n_batches = 100.0;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .01;
    double mh_proc_duration = 30;
    double oh_proc_prob = .01;
    double oh_proc_duration = 20;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration, 0,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration, 0,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 1.7;
    character.weapons[1].swing_speed = 1.9;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    Damage_sources sources = sim.get_damage_distribution();

    auto proc_data = sim.get_proc_data();
    auto aura_uptimes = sim.get_aura_uptimes_map();

    double miss_chance = (8 + 19) / 100.0;
    double dodge_chance = 6.5 / 100.0;
    double hit_chance = (1 - miss_chance - dodge_chance);

    double expected_procs_oh = hit_chance * sources.white_oh_count * oh_proc_prob;
    double expected_uptime_oh = expected_procs_oh * oh_proc_duration;
    double swings_during_uptime_oh = oh_proc_duration / character.weapons[1].swing_speed;
    double procs_during_uptime_oh = hit_chance * swings_during_uptime_oh * oh_proc_prob;
    double expected_procs_during_uptime_oh = expected_procs_oh * procs_during_uptime_oh;
    double overlap_duration_oh = expected_procs_during_uptime_oh * oh_proc_duration / 2;

    double expected_procs_mh = hit_chance * sources.white_mh_count * mh_proc_prob;
    double expected_uptime_mh = expected_procs_mh * mh_proc_duration;
    double swings_during_uptime_mh = mh_proc_duration / character.weapons[0].swing_speed;
    double procs_during_uptime_mh = hit_chance * swings_during_uptime_mh * mh_proc_prob;
    double expected_procs_during_uptime_mh = expected_procs_mh * procs_during_uptime_mh;
    double overlap_duration_mh = expected_procs_during_uptime_mh * mh_proc_duration / 2;

    // Does not factor in that a buff might end due to the simulation time reaching its end
    EXPECT_NEAR(proc_data["test_wep_mh"], expected_procs_mh, 0.05 * expected_procs_mh);
    EXPECT_NEAR(proc_data["test_wep_oh"], expected_procs_oh, 0.05 * expected_procs_oh);

    EXPECT_NEAR(aura_uptimes["main_hand_test_wep_mh"], expected_uptime_mh - overlap_duration_mh,
                0.05 * expected_uptime_mh);
    EXPECT_NEAR(aura_uptimes["off_hand_test_wep_oh"], expected_uptime_oh - overlap_duration_oh,
                0.05 * expected_uptime_oh);
    EXPECT_TRUE(aura_uptimes["off_hand_test_wep_oh"] != aura_uptimes["off_hand_test_wep_mh"]);
}

TEST_F(Sim_fixture, test_hit_effects_stat_boost_long_duration_overlap)
{
    config.sim_time = 1000.0;
    config.n_batches = 100.0;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .95;
    double mh_proc_duration = 17;
    double oh_proc_prob = .85;
    double oh_proc_duration = 19;
    Hit_effect test_effect_mh{"test_wep_mh", Hit_effect::Type::stat_boost, {50, 0}, {}, 0, mh_proc_duration, 0,
                              mh_proc_prob};
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::stat_boost, {}, {10, 0, 0, 0, 0.1}, 0, oh_proc_duration, 0,
                              oh_proc_prob};
    character.weapons[0].swing_speed = 1.7;
    character.weapons[1].swing_speed = 1.9;
    character.weapons[0].hit_effects.push_back(test_effect_mh);
    character.weapons[1].hit_effects.push_back(test_effect_oh);

    Combat_simulator sim{};
    sim.set_config(config);
    sim.simulate(character);

    auto aura_uptimes = sim.get_aura_uptimes_map();

    // Subtract 2 seconds since it might not always proc on the first hit
    double expected_duration = (config.sim_time - 2.0) * config.n_batches;

    EXPECT_GT(aura_uptimes["main_hand_test_wep_mh"], 0.98 * expected_duration);
    EXPECT_GT(aura_uptimes["off_hand_test_wep_oh"], 0.98 * expected_duration);
}

TEST_F(Sim_fixture, test_deep_wounds)
{
    config.sim_time = 100000.0;
    config.n_batches = 1;
    config.main_target_initial_armor_ = 0.0;

    auto mh = Weapon{"test_mh", {}, {}, 2.6, 130, 260, Weapon_socket::one_hand, Weapon_type::axe};
    auto oh = Weapon{"test_oh", {}, {}, 1.3, 65, 130, Weapon_socket::one_hand, Weapon_type::dagger};
    character.equip_weapon(mh, oh);

    character.base_special_stats.attack_power = 1400;

    Combat_simulator sim{};
    config.talents.deep_wounds = 3;
    config.combat.deep_wounds = true;
    config.combat.use_whirlwind = true;
    config.combat.use_bloodthirst = true;
    config.combat.use_heroic_strike = true;
    sim.set_config(config);
    sim.simulate(character);

    auto ap = character.total_special_stats.attack_power;
    auto w = character.weapons[0];
    auto dwTick = config.talents.deep_wounds * 0.2 * (0.5 * (w.min_damage + w.max_damage) + ap / 14 * w.swing_speed) / 4;

    auto damage_sources = sim.get_damage_distribution();

    EXPECT_GT(damage_sources.deep_wounds_count, 0);
    EXPECT_NEAR(damage_sources.deep_wounds_damage / damage_sources.deep_wounds_count, dwTick, 0.01);
}

TEST_F(Sim_fixture, test_flurry_uptime)
{
    config.sim_time = 100000.0;
    config.n_batches = 1;
    config.main_target_initial_armor_ = 0.0;

    auto mh = Weapon{"test_mh", {}, {}, 2.6, 260, 260, Weapon_socket::one_hand, Weapon_type::sword};
    auto oh = Weapon{"test_oh", {}, {}, 2.6, 260, 260, Weapon_socket::one_hand, Weapon_type::sword};
    character.equip_weapon(mh, oh);

    character.total_special_stats.attack_power = 2800;
    character.total_special_stats.critical_strike = 35;
    character.total_special_stats.haste = 0.05; // haste should probably use % as well, for consistency

    Combat_simulator sim{};
    config.talents.flurry = 5;
    config.talents.bloodthirst = 1;
    config.combat.heroic_strike_rage_thresh = 60;
    config.combat.use_heroic_strike = true;
    config.combat.use_bloodthirst = true;
    config.combat.use_whirlwind = true;
    sim.set_config(config);
    sim.simulate(character);

    auto flurryUptime = sim.get_flurry_uptime();
    auto flurryHaste = 1 + config.talents.flurry * 0.05;

    auto haste = 1 + character.total_special_stats.haste;

    auto dd = sim.get_damage_distribution();

    EXPECT_NEAR(dd.white_oh_count / (config.sim_time / oh.swing_speed * haste), (1 - flurryUptime) + flurryUptime * flurryHaste, 0.0001);
    EXPECT_NEAR((dd.white_mh_count + dd.heroic_strike_count) / (config.sim_time / mh.swing_speed * haste), (1 - flurryUptime) + flurryUptime * flurryHaste, 0.0001);
}

void time_simulate(Combat_simulator& sim, const Character& character)
{
    auto start = std::chrono::steady_clock::now();
    sim.simulate(character);
    auto end = std::chrono::steady_clock::now();
    std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    std::cout << std::endl;
}

void print_results(const std::string name, const Combat_simulator& sim, bool print_uptimes_and_procs)
{
    std::cout << name << std::endl;
    std::cout << std::endl;

    auto dd = sim.get_damage_distribution();

    auto f = 1.0 / (sim.config.sim_time * sim.config.n_batches);
    auto g = 60 * f;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "white (mh)    = " << f * dd.white_mh_damage << " (" << g * dd.white_mh_count << "x)" << std::endl;
    if (dd.white_oh_count > 0) std::cout << "white (oh)    = " << f * dd.white_oh_damage << " (" << g * dd.white_oh_count << "x)" << std::endl;
    if (dd.mortal_strike_count > 0) std::cout << "mortal strike = " << f * dd.mortal_strike_damage << " (" << g * dd.mortal_strike_count << "x)" << std::endl;
    if (dd.cleave_count > 0) std::cout << "cleave        = " << f * dd.cleave_damage << " (" << g * dd.cleave_count << "x)" << std::endl;
    if (dd.bloodthirst_count > 0) std::cout << "bloodthirst   = " << f * dd.bloodthirst_damage << " (" << g * dd.bloodthirst_count << "x)" << std::endl;
    if (dd.whirlwind_count > 0) std::cout << "whirlwind     = " << f * dd.whirlwind_damage << " (" << g * dd.whirlwind_count << "x)" << std::endl;
    if (dd.slam_count > 0) std::cout << "slam          = " << f * dd.slam_damage << " (" << g * dd.slam_count << "x)" << std::endl;
    if (dd.heroic_strike_count > 0) std::cout << "heroic strike = " << f * dd.heroic_strike_damage << " (" << g * dd.heroic_strike_count << "x)" << std::endl;
    if (dd.execute_count > 0) std::cout << "execute       = " << f * dd.execute_damage << " (" << g * dd.execute_count << "x)" << std::endl;
    if (dd.deep_wounds_count > 0) std::cout << "deep wounds   = " << f * dd.deep_wounds_damage << " (" << g * dd.deep_wounds_count << "x)" << std::endl;
    if (dd.overpower_count > 0) std::cout << "overpower     = " << f * dd.overpower_damage << " (" << g * dd.overpower_count << "x)" << std::endl;
    if (dd.item_hit_effects_count > 0) std::cout << "hit effects   = " << f * dd.item_hit_effects_damage << " (" << g * dd.item_hit_effects_count << "x)" << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << "total         = " << f * dd.sum_damage_sources() << std::endl;
    std::cout << std::endl;

    if (!print_uptimes_and_procs) return;

    for (const auto& e : sim.get_aura_uptimes_map()) {
        std::cout << e.first << " " << 100 * f * e.second << "%" << std::endl;
    }
    std::cout << std::endl;
    for (const auto& e : sim.get_proc_data()) {
        std::cout << e.first << " " << g * e.second << " procs/min" << std::endl;
    }
    std::cout << std::endl;
}

/*
>> results on master:

took 6994 ms

white (mh)    = 282.709
mortal strike = 83.4979
whirlwind     = 43.0987
slam          = 286.997
heroic strike = 0.981576
deep wounds   = 48.7078
----------------------
total         = 768.312 / 770.882
rage lost 12012.1

>> after heroic strike fix & rampage/overpower changes:

took 6163 ms

white (mh)    = 282.746
mortal strike = 83.5375
whirlwind     = 43.15
slam          = 287.001
heroic strike = 0.961978
deep wounds   = 48.7231
----------------------
total         = 768.415 / 770.985
rage lost 12138.6

>> after over_time_buff / deep wound changes:

took 5492 ms

white (mh)    = 282.709 (15.7341x)
slam          = 286.883 (14.0663x)
mortal strike = 83.4875 (4.18832x)
whirlwind     = 43.1253 (2.55086x)
deep wounds   = 42.0281 (13.3422x)
execute       = 22.3852 (1.22389x)
heroic strike = 0.964171 (0.04656x)
----------------------
total         = 761.583 / 764.131
rage lost 11748

>> after sim_time changes (new format)

took 3884 ms

white (mh)    = 283.65 (15.79x)
slam          = 287.80 (14.11x)
mortal strike = 83.80 (4.20x)
whirlwind     = 43.22 (2.56x)
deep wounds   = 42.16 (13.38x)
execute       = 22.49 (1.23x)
heroic strike = 0.97 (0.05x)
----------------------
total         = 764.08

rage lost 0.10 per minute

Deep_wounds 88.15%
Anger Management 100.00%

>> after haste and attack speed split

took 2157 ms

white (mh)    = 283.57 (15.78x)
slam          = 287.85 (14.12x)
mortal strike = 83.64 (4.19x)
whirlwind     = 43.19 (2.55x)
deep wounds   = 42.16 (13.38x)
execute       = 22.60 (1.24x)
heroic strike = 1.04 (0.05x)
----------------------
total         = 764.05

rage lost 0.09 per minute

Deep_wounds 88.15%
Anger Management 100.00%
*/
TEST_F(Sim_fixture, test_arms)
{
    config.sim_time = 5 * 60;
    config.n_batches = 25000;
    config.main_target_initial_armor_ = 6200.0;

    auto mh = Weapon{"test_mh", {}, {}, 3.8, 500, 500, Weapon_socket::two_hand, Weapon_type::mace};
    character.equip_weapon(mh);

    character.total_special_stats.attack_power = 2800;
    character.total_special_stats.critical_strike = 35;
    character.total_special_stats.hit = 3;
    character.total_special_stats.haste = 0.05; // haste should probably use % as well, for consistency
    character.total_special_stats.expertise = 5;
    character.total_special_stats.axe_expertise = 5;
    character.total_special_stats.crit_multiplier = 0.03;

    Combat_simulator sim{};
    config.talents.flurry = 3;
    config.talents.mortal_strike = 1;
    config.talents.improved_slam = 2;
    config.talents.deep_wounds = 3;
    config.talents.anger_management = true;
    config.talents.unbridled_wrath = 5;
    config.combat.use_mortal_strike = true;
    config.combat.use_slam = true;
    config.combat.slam_rage_dd = 15; // this must not be < slam_rage_cost, afaik, but this isn't enforced; what does "dd" stand for?
    config.combat.slam_spam_rage = 100; // default 100 aka never
    config.combat.slam_spam_max_time = 1.5; // rather slam_min_swing_remaining, but this should (simpler) be slam_max_swing_passed
    config.combat.slam_latency = 0.2;
    config.combat.use_whirlwind = true;
    config.combat.use_heroic_strike = true;
    config.combat.heroic_strike_rage_thresh = 80;
    config.execute_phase_percentage_ = 20;
    config.combat.use_sl_in_exec_phase = true;
    config.combat.use_ms_in_exec_phase = true;
    config.combat.deep_wounds = true;
    config.talents.mace_specialization = 0;
    sim.set_config(config);

    time_simulate(sim, character);

    print_results(test_info_->name(), sim, true);

    EXPECT_EQ(0, 0);
}

/*
>>> results on master:

took 9671 ms

white (mh)    = 212.512
white (oh)    = 173.113
heroic strike = 98.4273
bloodthirst   = 166.721
whirlwind     = 99.9492
execute       = 83.4121
deep wounds   = 38.2024
----------------------
total         = 872.337 / 875.255
rage lost 8648.78

>>> after heroic strike fix & rampage/overpower changes:

took 9223 ms

white (mh)    = 212.676
white (oh)    = 172.942
heroic strike = 95.3933
bloodthirst   = 166.764
whirlwind     = 100.027
execute       = 83.2502
deep wounds   = 38.1973
----------------------
total         = 869.249 / 872.157
rage lost 8478.46

>>> after over_time_buffs / deep wound changes:

took 8160 ms

white (mh)    = 212.649 (21.3161x)
white (oh)    = 172.938 (28.3863x)
bloodthirst   = 166.533 (8.36789x)
whirlwind     = 99.9227 (5.24548x)
heroic strike = 95.4492 (6.11997x)
execute       = 83.3244 (4.31656x)
deep wounds   = 19.9497 (9.8517x)
----------------------
total         = 850.766 / 853.611
rage lost 8702.61

>>> after sim_time changes (new format)

took 5846 ms

white (mh)    = 213.40 (21.40x)
white (oh)    = 173.72 (28.48x)
bloodthirst   = 167.18 (8.39x)
whirlwind     = 100.28 (5.26x)
heroic strike = 95.51 (6.13x)
execute       = 84.09 (4.35x)
deep wounds   = 20.01 (9.88x)
----------------------
total         = 854.19

rage lost 0.07 per minute

Deep_wounds 98.31%
Anger Management 100.00%

>>> after haste/attack speed split

took 3433 ms

white (mh)    = 213.53 (21.40x)
white (oh)    = 173.61 (28.48x)
bloodthirst   = 167.30 (8.39x)
whirlwind     = 100.51 (5.26x)
heroic strike = 95.64 (6.13x)
execute       = 84.26 (4.36x)
deep wounds   = 20.00 (9.88x)
----------------------
total         = 854.86

rage lost 0.07 per minute

Deep_wounds 98.31%
Anger Management 100.00%
*/
TEST_F(Sim_fixture, test_fury)
{
    config.sim_time = 5 * 60;
    config.n_batches = 25000;
    config.main_target_initial_armor_ = 6200.0;

    auto mh = Weapon{"test_mh", {}, {}, 2.7, 270, 270, Weapon_socket::one_hand, Weapon_type::axe};
    auto oh = Weapon{"test_oh", {}, {}, 2.6, 260, 260, Weapon_socket::one_hand, Weapon_type::sword};
    character.equip_weapon(mh, oh);

    character.total_special_stats.attack_power = 2800;
    character.total_special_stats.critical_strike = 35;
    character.total_special_stats.hit = 3;
    character.total_special_stats.haste = 0.05; // haste should probably use % as well, for consistency
    character.total_special_stats.crit_multiplier = 0.03;
    character.total_special_stats.expertise = 5;
    character.total_special_stats.axe_expertise = 5;

    config.talents.flurry = 5;
    config.talents.rampage = true;
    config.combat.rampage_use_thresh = 3;
    config.combat.use_rampage = false;
    config.talents.dual_wield_specialization = 5;
    config.talents.deep_wounds = 3;
    config.combat.deep_wounds = true;
    config.talents.improved_heroic_strike = 3;
    config.talents.improved_whirlwind = 1;
    config.talents.impale = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.weapon_mastery = 2;
    config.talents.bloodthirst = 1;
    config.talents.anger_management = true;
    config.combat.heroic_strike_rage_thresh = 60;
    config.combat.use_heroic_strike = true;
    /*
    config.multi_target_mode_ = true;
    config.combat.cleave_if_adds = true;
    config.combat.cleave_rage_thresh = 60;
    config.number_of_extra_targets = 4;
    config.extra_target_duration = config.sim_time;
    config.extra_target_initial_armor_ = config.main_target_initial_armor_;
    config.extra_target_level = config.main_target_level;
    */
    config.combat.use_bt_in_exec_phase = true;
    config.combat.use_bloodthirst = true;
    config.combat.use_whirlwind = true;
    config.execute_phase_percentage_ = 20;

    sim.set_config(config);

    time_simulate(sim, character);
    print_results(test_info_->name(), sim, true);

    EXPECT_EQ(0, 0);
}

/*
>>> baseline, after deep wound fixes

took 14558 ms

white (mh)    = 277.576 (25.2471x)
white (oh)    = 200.944 (29.9224x)
bloodthirst   = 189.134 (8.5567x)
whirlwind     = 106.055 (5.17051x)
heroic strike = 164.315 (9.8308x)
execute       = 97.9637 (4.66646x)
deep wounds   = 18.6281 (8.6935x)
----------------------
total         = 1054.62 / 1058.14
rage lost 576971

>>> after lazy add and other perf work

took 8139 ms

white (mh)    = 277.931 (25.263x)
white (oh)    = 201.414 (29.951x)
bloodthirst   = 189.787 (8.56638x)
whirlwind     = 106.299 (5.17456x)
heroic strike = 164.903 (9.85761x)
execute       = 98.0892 (4.67223x)
deep wounds   = 18.5604 (8.66152x)
----------------------
total         = 1056.98 / 1060.52
rage lost 584443

>>> more perf work, sim_time update, new layout

took 7802 ms

white (mh)    = 278.81 (25.35x)
white (oh)    = 202.05 (30.05x)
bloodthirst   = 190.20 (8.59x)
whirlwind     = 106.61 (5.19x)
heroic strike = 165.30 (9.88x)
execute       = 98.79 (4.70x)
deep wounds   = 18.61 (8.69x)
----------------------
total         = 1060.36

rage lost 4.64 per minute

Deep_wounds 98.85%
Anger Management 100.00%
mongoose_oh 25.62%
mongoose_mh 38.86%
dragonmaw 28.01%

dragonmaw 2.02 procs/min
mongoose_mh 2.03 procs/min
windfury_totem 4.82 procs/min
mongoose_oh 1.20 procs/min
sword_specialization 1.38 procs/min

>>> after haste/attack speed split, and other stuff ;)

took 5167 ms

white (mh)    = 276.48 (25.16x)
white (oh)    = 202.47 (29.96x)
bloodthirst   = 191.87 (8.58x)
whirlwind     = 107.23 (5.19x)
heroic strike = 163.29 (9.75x)
execute       = 98.33 (4.69x)
deep wounds   = 18.83 (8.71x)
----------------------
total         = 1058.50

rage lost 4.54 per minute

Anger Management 100.00%
windfury_attack 2.50%
mongoose_oh 25.53%
Deep_wounds 98.84%
mongoose_mh 38.64%
dragonmaw 27.90%

dragonmaw 2.01 procs/min
mongoose_mh 2.01 procs/min
windfury_totem 8.95 procs/min
mongoose_oh 1.19 procs/min
sword_specialization 1.38 procs/min
*/
TEST_F(Sim_fixture, test_procs)
{
    config.sim_time = 120;
    config.n_batches = 25000;
    config.main_target_initial_armor_ = 6200.0;

    auto dmc_crusade = Hit_effect{"dmc_crusade", Hit_effect::Type::stat_boost, {}, {0, 0, 6}, 0, 10, 0, 1, 0, 0, 0, 20};
    auto executioner = Hit_effect{"executioner", Hit_effect::Type::stat_boost, {}, {}, 0, 15, 0, 0, 0, 0, 0, 1, 1};
    executioner.special_stats_boost.gear_armor_pen = 840;

    Special_stats mongoose_buff;
    mongoose_buff.attack_speed = 0.02;

    auto doomplate_4pc = Hit_effect{"doomplate_4pc", Hit_effect::Type::stat_boost, {}, {0, 0, 160}, 0, 15, 0, 0.02, 0, 0, 1, 0};

    Armory armory;

    character.equip_armor(armory.find_armor(Socket::trinket, "badge_of_the_swarmguard"));
    character.equip_armor(armory.find_armor(Socket::trinket, "bloodlust_brooch"));
    character.buffs.emplace_back(armory.buffs.battle_shout);
    character.buffs.emplace_back(armory.buffs.haste_potion);
    character.buffs.emplace_back(armory.buffs.bloodlust);

    armory.compute_total_stats(character);

    auto mh = Weapon{"test_mh", {}, {}, 2.7, 270, 270, Weapon_socket::one_hand, Weapon_type::axe};
    //mh.hit_effects.emplace_back(executioner);
    mh.hit_effects.emplace_back(Hit_effect{"dragonmaw", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .134}, 0, 10, 0, 2.7 / 60});
    mh.hit_effects.emplace_back(Hit_effect{"mongoose_mh", Hit_effect::Type::stat_boost, {0,120}, mongoose_buff, 0, 15, 0, 2.7/60});
    //mh.hit_effects.emplace_back(Hit_effect{dmc_crusade});
    mh.hit_effects.emplace_back(Hit_effect{"windfury_totem", Hit_effect::Type::windfury_hit, {}, {0, 0, 445}, 0, 0, 0, 0.2});
    //mh.hit_effects.emplace_back(doomplate_4pc);

    auto oh = Weapon{"test_oh", {}, {}, 2.6, 260, 260, Weapon_socket::one_hand, Weapon_type::sword};
    //oh.hit_effects.emplace_back(executioner);
    oh.hit_effects.emplace_back(Hit_effect{"mongoose_oh", Hit_effect::Type::stat_boost, {0,120}, mongoose_buff, 0, 15, 0, 2.6/60});
    //oh.hit_effects.emplace_back(Hit_effect{dmc_crusade});
    oh.hit_effects.emplace_back(Hit_effect{"sword_specialization", Hit_effect::Type::sword_spec, {}, {}, 0, 0, 0.5, 0.05});
    //oh.hit_effects.emplace_back(doomplate_4pc);

    character.equip_weapon(mh, oh);

    character.total_special_stats.attack_power = 2800;
    character.total_special_stats.critical_strike = 35;
    character.total_special_stats.hit = 3;
    character.total_special_stats.haste = 0.05; // haste should probably use % as well, for consistency
    character.total_special_stats.crit_multiplier = 0.03;
    character.total_special_stats.expertise = 5;
    character.total_special_stats.axe_expertise = 5;

    Special_stats mult;
    mult.ap_multiplier = 0.1;
    character.total_special_stats += mult;

    // hello, use effects ;)
    config.talents.death_wish = true;
    config.combat.use_death_wish = true;
    config.enable_bloodrage = true;
    config.enable_blood_fury = true;
    config.enable_unleashed_rage = true;

    config.talents.flurry = 5;
    config.talents.rampage = true;
    config.combat.rampage_use_thresh = 3;
    config.combat.use_rampage = true;
    config.talents.dual_wield_specialization = 5;
    config.talents.deep_wounds = 3;
    config.combat.deep_wounds = true;
    config.talents.improved_heroic_strike = 3;
    config.talents.improved_whirlwind = 1;
    config.talents.impale = 2;
    config.talents.unbridled_wrath = 5;
    config.talents.weapon_mastery = 2;
    config.talents.bloodthirst = 1;
    config.talents.anger_management = true;
    config.combat.heroic_strike_rage_thresh = 60;
    config.combat.use_heroic_strike = true;
    /*
    config.multi_target_mode_ = true;
    config.combat.cleave_if_adds = true;
    config.combat.cleave_rage_thresh = 60;
    config.number_of_extra_targets = 4;
    config.extra_target_duration = config.sim_time;
    config.extra_target_initial_armor_ = config.main_target_initial_armor_;
    config.extra_target_level = config.main_target_level;
    */
    config.combat.use_bt_in_exec_phase = true;
    config.combat.use_ww_in_exec_phase = false;
    config.combat.use_bloodthirst = true;
    config.combat.use_whirlwind = true;
    config.execute_phase_percentage_ = 20;

    config.combat.use_overpower = false;
    config.combat.overpower_rage_thresh = 25;

    sim.set_config(config);

    time_simulate(sim, character);
    print_results(test_info_->name(), sim, true);

    time_simulate(sim, character);
    print_results(test_info_->name(), sim, true);

    time_simulate(sim, character);
    print_results(test_info_->name(), sim, true);

    EXPECT_EQ(0, 0);
}

TEST_F(Sim_fixture, base_stats)
{
    std::vector<Race> races = {
        Race::human, Race::gnome, Race::dwarf, Race::night_elf, Race::draenei,
        Race::orc, Race::undead, Race::tauren, Race::troll
    };

    std::vector<int> expected_ap = {
        480, 470, 484, 474, 482,
        486, 478, 490, 482
    };

    std::vector<double> expected_crit = {
        7.05, 7.14, 6.93, 7.20, 6.96,
        6.96, 6.99, 6.90, 7.11
    };

    Armory armory;

    for (std::vector<Race>::size_type i = 0; i < races.size(); ++i)
    {
        auto toon = Character(races[i], 70);
        armory.compute_total_stats(toon);

        std::cout << "race - " << races[i] << std::endl;
        std::cout << toon.total_attributes;
        std::cout << toon.total_special_stats;
        std::cout << std::endl;

        EXPECT_EQ(toon.total_special_stats.attack_power, expected_ap[i]);
        EXPECT_NEAR(toon.total_special_stats.critical_strike, expected_crit[i], 0.002);
    }
}