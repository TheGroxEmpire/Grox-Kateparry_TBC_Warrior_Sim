#include "Armory.hpp"
#include "item_heuristics.hpp"

#include "gtest/gtest.h"
#include <Combat_simulator.hpp>

namespace
{
Combat_simulator_config get_config_with_everything_deactivated()
{
    Combat_simulator_config config{};
    //    config.exposed_armor = true;
    //    config.curse_of_recklessness_active = true;
    //    config.faerie_fire_feral_active = true;
    //    config.use_death_wish = true;
    //    config.enable_recklessness = true;
    //    config.enable_blood_fury = true;
    //    config.enable_berserking = true;
    //    config.combat.use_bt_in_exec_phase = true;
    //    config.combat.use_ms_in_exec_phase = true;
    //    config.combat.use_hs_in_exec_phase = true;
    //    config.combat.cleave_if_adds = true;
    //    config.combat.use_sweeping_strikes = true;
    //    config.combat.dont_use_hm_when_ss = true;
    //    config.combat.use_hamstring = true;
    //    config.combat.use_bloodthirst = true;
    //    config.combat.use_mortal_strike = true;
    //    config.combat.use_slam = true;
    //    config.combat.use_whirlwind = true;
    //    config.combat.use_overpower = true;
    //    config.combat.use_heroic_strike = true;
    //    config.deep_wounds = true;
    //    config.combat.first_hit_heroic_strike = true;
    //    config.multi_target_mode_ = true;
    //    config.essence_of_the_red_ = true;
    //    config.can_trigger_enrage_ = true;
    //    config.first_global_sunder_ = true;
    //    config.take_periodic_damage_ = true;
    //    config.ability_queue_ = true;

    config.combat.heroic_strike_rage_thresh = 0.0;
    config.combat.cleave_rage_thresh = 0.0;
    config.combat.whirlwind_rage_thresh = 0.0;
    config.combat.hamstring_cd_thresh = 0.0;
    config.combat.hamstring_rage_thresh = 0.0;
    config.initial_rage = 0.0;
    config.combat.whirlwind_bt_cooldown_thresh = 0.0;
    config.combat.overpower_rage_thresh = 0.0;
    config.combat.overpower_bt_cooldown_thresh = 0.0;
    config.combat.overpower_ww_cooldown_thresh = 0.0;
    config.combat.slam_latency = 0.0;
    config.combat.slam_spam_max_time = 0.0;
    config.combat.slam_spam_rage = 0.0;
    config.combat.slam_rage_thresh = 0.0;

    config.sim_time = 60.0;
    config.main_target_level = 73.0;
    config.main_target_initial_armor_ = 7700.0;
    config.n_sunder_armor_stacks = 5.0;
    config.number_of_extra_targets = 0.0;
    config.extra_target_percentage = 0.0;
    config.extra_target_initial_armor_ = 0.0;
    config.periodic_damage_interval_ = 0.0;
    config.periodic_damage_amount_ = 0.0;
    config.execute_phase_percentage_ = 0.0;
    config.berserking_haste_ = 0.0;

    config.n_batches = 10;

    config.seed = 110000;

    return config;
}
} // namespace

class Sim_fixture : public ::testing::Test
{
public:
    Character character;
    Combat_simulator_config config{};
    Combat_simulator sim{};
    Sim_fixture() : character{Race::gnome, 70} {}

    void SetUp() override
    {
        config = get_config_with_everything_deactivated();

        auto wep = Weapon{"test_wep", {}, {}, 2.0, 100, 100, Weapon_socket::one_hand, Weapon_type::axe};
        character.equip_weapon(wep, wep);

        Armory armory{};
        armory.compute_total_stats(character);
    }

    void TearDown() override
    {
        // Nothing to do since no memory was allocated
    }
};