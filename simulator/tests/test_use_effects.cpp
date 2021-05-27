#include "Combat_simulator.hpp"
#include "Armory.hpp"
#include "Use_effects.hpp"
#include "gtest/gtest.h"

namespace
{
bool is_descending(const std::vector<std::pair<double, Use_effect>>& use_effects)
{
    double activate_time = 1000000.0;
    bool is_descending{true};
    for (const auto& effect : use_effects)
    {
        is_descending &= (effect.first <= activate_time);
        activate_time = effect.first;
    }
    return is_descending;
}
} // namespace

TEST(TestSuite, test_use_effect_time_function)
{
    Use_effect use_effect1{};
    use_effect1.duration = 20;
    Use_effect use_effect2{};
    use_effect2.duration = 30;
    std::vector<std::pair<double, Use_effect>> use_effect_timers;
    use_effect_timers.emplace_back(0.0, use_effect1);
    double time = Use_effects::is_time_available(use_effect_timers, 0.0, use_effect2.duration);
    EXPECT_TRUE(time == 20.0);
    time = Use_effects::is_time_available(use_effect_timers, 10.0, use_effect2.duration);
    EXPECT_TRUE(time == 20.0);
    time = Use_effects::is_time_available(use_effect_timers, 20.0, use_effect2.duration);
    EXPECT_TRUE(time == 20.0);

    use_effect_timers.emplace_back(40.0, use_effect1);
    time = Use_effects::get_next_available_time(use_effect_timers, 0.0, use_effect2.duration);
    EXPECT_TRUE(time == 60.0);
}

TEST(TestSuite, test_use_effect_ordering)
{
    Use_effect use_effect1{};
    use_effect1.name = "should_not_fit";
    use_effect1.duration = 30;
    use_effect1.cooldown = 100;
    use_effect1.special_stats_boost = {12, 0, 0};
    use_effect1.effect_socket = Use_effect::Effect_socket::shared;

    Use_effect use_effect2{};
    use_effect2.duration = 30;
    use_effect2.cooldown = 80;
    use_effect2.special_stats_boost = {12, 0, 40};
    use_effect2.effect_socket = Use_effect::Effect_socket::shared;

    Use_effect use_effect3{};
    use_effect3.duration = 30;
    use_effect3.cooldown = 80;
    use_effect3.special_stats_boost = {12, 0, 100};
    use_effect3.effect_socket = Use_effect::Effect_socket::shared;

    std::vector<Use_effect> use_effects{use_effect1, use_effect2, use_effect3};
    auto order = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, 580, 1500, 0, 0, 0);
    for (const auto& effect : order)
    {
        EXPECT_TRUE(effect.second.name != "should_not_fit");
    }
}

TEST(TestSuite, test_use_effect_shuffle)
{
    Combat_simulator sim{};
    std::vector<Use_effect> use_effects{};
    use_effects.emplace_back(sim.deathwish);
    use_effects.emplace_back(sim.bloodrage);
    double sim_time = 20.0;
    auto order_with_rage = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, sim_time, 1500, 0, 0, 10);
    auto order_without_rage = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, sim_time, 1500, 0, 0, 0);

    EXPECT_TRUE(order_with_rage[0].second.name == "Bloodrage");
    EXPECT_TRUE(order_with_rage[1].second.name == "Death_wish");

    EXPECT_TRUE(order_without_rage[0].second.name == "Death_wish");
    EXPECT_TRUE(order_without_rage[1].second.name == "Bloodrage");

    EXPECT_TRUE(is_descending(order_with_rage));
    EXPECT_TRUE(is_descending(order_without_rage));

    Armory armory;
    auto use1 = armory.find_armor(Socket::trinket, "figurine_nightseye_panther");
    use_effects.clear();
    use_effects.emplace_back(sim.deathwish);
    use_effects.emplace_back(sim.bloodrage);
    use_effects.emplace_back(sim.recklessness);
    use_effects.emplace_back(use1.use_effects[0]);
    order_with_rage = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, sim_time, 1500, 0, 0, 10);
    order_without_rage = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, sim_time, 1500, 0, 0, 0);

    EXPECT_TRUE(order_with_rage[0].second.name == "Bloodrage");
    EXPECT_TRUE(order_with_rage[1].second.name == "Recklessness");
    EXPECT_TRUE(order_with_rage[2].second.name == "Death_wish");
    EXPECT_TRUE(order_with_rage[3].second.name == "figurine_nightseye_panther");

    EXPECT_TRUE(order_without_rage[0].second.name == "Recklessness");
    EXPECT_TRUE(order_without_rage[1].second.name == "Death_wish");
    EXPECT_TRUE(order_without_rage[2].second.name == "Bloodrage");
    EXPECT_TRUE(order_without_rage[3].second.name == "figurine_nightseye_panther");

    EXPECT_TRUE(is_descending(order_with_rage));
    EXPECT_TRUE(is_descending(order_without_rage));
}

TEST(TestSuite, test_use_effects)
{
    Armory armory;
    auto use1 = armory.find_armor(Socket::trinket, "figurine_nightseye_panther");
    auto use2 = armory.find_armor(Socket::trinket, "icon_of_unyeilding_courage");
    auto use3 = armory.find_armor(Socket::legs, "bulwark_of_kings");
    Combat_simulator sim{};

    std::vector<Use_effect> use_effects{};
    use_effects.emplace_back(sim.deathwish);
    use_effects.emplace_back(sim.recklessness);
    use_effects.emplace_back(sim.bloodrage);
    use_effects.emplace_back(
        Use_effect{"Blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, 300}, 0, 15, 120, true});
    use_effects.emplace_back(Use_effect{"Berserking", Use_effect::Effect_socket::unique, {}, {}, 0, 10, 180, false});
    use_effects.emplace_back(use1.use_effects[0]);
    use_effects.emplace_back(use2.use_effects[0]);
    use_effects.emplace_back(use3.use_effects[0]);
    double sim_time = 320.0;
    auto order = Use_effects::compute_use_effect_order(use_effects, Special_stats{}, sim_time, 1500, 0, 0, 0);
    EXPECT_TRUE(is_descending(order));
    int fg = 0;
    int ic = 0;
    int bk = 0;
    int dw = 0;
    int rl = 0;
    int br = 0;
    int bf = 0;
    int bs = 0;
    for (const auto& effect : order)
    {
        if (effect.second.name == "figurine_nightseye_panther")
            fg++;
        if (effect.second.name == "icon_of_unyeilding_courage")
            ic++;
        if (effect.second.name == "bulwark_of_kings")
            bk++;
        if (effect.second.name == "Blood_fury")
            bf++;
        if (effect.second.name == "Berserking")
            bs++;
        if (effect.second.name == "Bloodrage")
            br++;
        if (effect.second.name == "Recklessness")
            rl++;
        if (effect.second.name == "Death_wish")
            dw++;
    }
    EXPECT_TRUE(fg == 1);
    EXPECT_TRUE(ic == 3);
    EXPECT_TRUE(bk == 1);
    EXPECT_TRUE(dw == 2);
    EXPECT_TRUE(rl == 1);
    EXPECT_TRUE(br == 6);
    EXPECT_TRUE(bf == 3);
    EXPECT_TRUE(bs == 2);
}
