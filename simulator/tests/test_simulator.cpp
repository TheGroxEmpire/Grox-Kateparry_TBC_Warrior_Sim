#include "Armory.hpp"
#include "BinomialDistribution.hpp"
#include "Combat_simulator.hpp"
#include "Statistics.hpp"
#include "simulation_fixture.cpp"

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

    // subtract 'config.n_batches' since each simulation starts with both weapons swinging, which means that one hit per
    // simulation cant be HS influenced
    double hs_uptime_expected = double(distrib.white_oh_count - config.n_batches) / distrib.white_oh_count;
    EXPECT_FLOAT_EQ(hs_uptime, hs_uptime_expected);
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
    Hit_effect test_effect_oh{"test_wep_oh", Hit_effect::Type::windfury_hit, {}, {}, 0, 0, 0, mh_proc_prob};
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
    config.performance_mode = false;

    double mh_proc_prob = 1.0;
    int mh_proc_duration = 1;
    double oh_proc_prob = 1.0;
    int oh_proc_duration = 2;
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
    config.performance_mode = false;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .01;
    int mh_proc_duration = 30;
    double oh_proc_prob = .01;
    int oh_proc_duration = 20;
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
    config.performance_mode = false;

    // Small chance on hit to decrease second order terms, i.e., procing stat buff while a stat buff is already active
    double mh_proc_prob = .95;
    int mh_proc_duration = 17;
    double oh_proc_prob = .85;
    int oh_proc_duration = 19;
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

/*
results on master:

took 8112 ms

white (mh)    = 283.418
mortal strike = 86.3654
whirlwind     = 44.3856
slam          = 296.372
heroic strike = 1.04357
deep wounds   = 50.1407
----------------------
total         = 784.141 / 786.763
rage lost 12509.3

after yellow hit table fixes:

took 8486 ms

white (mh)    = 282.8
mortal strike = 83.5754
whirlwind     = 43.1117
slam          = 286.858
heroic strike = 1.00548
deep wounds   = 48.7256
----------------------
total         = 768.48 / 771.05
rage lost 12686.2

 */
TEST_F(Sim_fixture, test_2h)
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
    sim.set_config(config);

    auto start = std::chrono::steady_clock::now();
    sim.simulate(character);
    auto end = std::chrono::steady_clock::now();
    std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    std::cout << std::endl;

    auto dd = sim.get_damage_distribution();

    auto f = 1 / (config.sim_time * config.n_batches);

    std::cout << "white (mh)    = " << f * dd.white_mh_damage << std::endl;
    if (dd.white_oh_count > 0) std::cout << "white (oh)    = " << f * dd.white_oh_damage << std::endl;
    if (dd.mortal_strike_count > 0) std::cout << "mortal strike = " << f * dd.mortal_strike_damage << std::endl;
    if (dd.whirlwind_count > 0) std::cout << "whirlwind     = " << f * dd.whirlwind_damage << std::endl;
    if (dd.slam_count > 0) std::cout << "slam          = " << f * dd.slam_damage << std::endl;
    if (dd.heroic_strike_count > 0) std::cout << "heroic strike = " << f * dd.heroic_strike_damage << std::endl;
    if (dd.deep_wounds_count > 0) std::cout << "deep wounds   = " << f * dd.deep_wounds_damage << std::endl;
    if (dd.item_hit_effects_count > 0) std::cout << "hit effects   = " << f * dd.item_hit_effects_damage << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << "total         = " << f * dd.sum_damage_sources() << " / " << sim.get_dps_mean() << std::endl;

    std::cout << "rage lost " << sim.get_rage_lost_capped() << std::endl;
    EXPECT_EQ(0, 0);
}

/*
results on master:

took 14402 ms

white (mh)    = 211.653
white (oh)    = 173.392
heroic strike = 102.595
bloodthirst   = 167.043
whirlwind     = 101.591
execute       = 83.6768
deep wounds   = 38.3068
----------------------
total         = 878.257 / 881.195
rage lost 8530.75

after yellow hit table fixes

took 15786 ms

white (mh)    = 211.725
white (oh)    = 173.002
heroic strike = 99.6991
bloodthirst   = 166.751
whirlwind     = 99.0064
execute       = 83.641
deep wounds   = 38.209
----------------------
total         = 872.033 / 874.95
rage lost 8811.75

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

    auto start = std::chrono::steady_clock::now();
    sim.simulate(character);
    auto end = std::chrono::steady_clock::now();
    std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    std::cout << std::endl;

    auto dd = sim.get_damage_distribution();

    auto f = 1.0 / (config.sim_time * config.n_batches);

    std::cout << "white (mh)    = " << f * dd.white_mh_damage << std::endl;
    if (dd.white_oh_count > 0) std::cout << "white (oh)    = " << f * dd.white_oh_damage << std::endl;
    if (dd.heroic_strike_count > 0) std::cout << "heroic strike = " << f * dd.heroic_strike_damage << std::endl;
    if (dd.cleave_count > 0) std::cout << "cleave        = " << f * dd.cleave_damage << std::endl;
    if (dd.bloodthirst_count > 0) std::cout << "bloodthirst   = " << f * dd.bloodthirst_damage << std::endl;
    if (dd.whirlwind_count > 0) std::cout << "whirlwind     = " << f * dd.whirlwind_damage << std::endl;
    if (dd.execute_count > 0) std::cout << "execute       = " << f * dd.execute_damage << std::endl;
    if (dd.deep_wounds_count > 0) std::cout << "deep wounds   = " << f * dd.deep_wounds_damage << std::endl;
    if (dd.item_hit_effects_count > 0) std::cout << "hit effects   = " << f * dd.item_hit_effects_damage << std::endl;
    std::cout << "----------------------" << std::endl;
    std::cout << "total         = " << f * dd.sum_damage_sources() << " / " << sim.get_dps_mean() << std::endl;

    std::cout << "rage lost " << sim.get_rage_lost_capped() << std::endl;

    EXPECT_EQ(0, 0);
}
