#include "Armory.hpp"
#include "item_heuristics.hpp"

#include "gtest/gtest.h"
#include <Combat_simulator.hpp>

namespace
{
Combat_simulator_config get_config_with_everything_deactivated()
{
    Combat_simulator_config config{};

    config.sim_time = 60.0;
    config.main_target_level = 73.0;
    config.main_target_initial_armor_ = 7700.0;
    config.n_sunder_armor_stacks = 5.0;

    config.n_batches = 10;

    config.seed = 110000;

    return config;
}
} // namespace

class Sim_fixture : public ::testing::Test
{
public:
    Character character{Race::gnome, 70};
    Combat_simulator_config config{};

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