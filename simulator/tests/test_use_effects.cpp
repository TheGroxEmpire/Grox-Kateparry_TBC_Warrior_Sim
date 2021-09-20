#include "Combat_simulator.hpp"
#include "Armory.hpp"
#include "Use_effects.hpp"
#include "gtest/gtest.h"

namespace
{
bool is_ascending(const Use_effects::Schedule& schedule)
{
    return std::is_sorted(schedule.begin(), schedule.end(), [](const auto& a, const auto& b) {
       return a.first < b.first;
    });
}
} // namespace

TEST(TestSuite, test_use_effect_time_function)
{
    Use_effect use_effect1{};
    use_effect1.duration = 20000;
    Use_effect use_effect2{};
    use_effect2.duration = 30000;
    Use_effects::Schedule schedule{};
    schedule.emplace_back(0, use_effect1);
    int time = Use_effects::is_time_available(schedule, 0, use_effect2.duration);
    EXPECT_TRUE(time == 20000);
    time = Use_effects::is_time_available(schedule, 10000, use_effect2.duration);
    EXPECT_TRUE(time == 20000);
    time = Use_effects::is_time_available(schedule, 20000, use_effect2.duration);
    EXPECT_TRUE(time == 20000);

    schedule.emplace_back(40000, use_effect1);
    time = Use_effects::get_next_available_time(schedule, 0, use_effect2.duration);
    EXPECT_TRUE(time == 60000);
}

TEST(TestSuite, test_use_effect_ordering)
{
    Use_effect use_effect1{};
    use_effect1.name = "should_not_fit";
    use_effect1.duration = 30000;
    use_effect1.cooldown = 100000;
    use_effect1.combat_buff.special_stats_boost = {12, 0, 0};
    use_effect1.effect_socket = Use_effect::Effect_socket::shared;

    Use_effect use_effect2{};
    use_effect2.duration = 30000;
    use_effect2.cooldown = 80000;
    use_effect2.combat_buff.special_stats_boost = {12, 0, 40};
    use_effect2.effect_socket = Use_effect::Effect_socket::shared;

    Use_effect use_effect3{};
    use_effect3.duration = 30000;
    use_effect3.cooldown = 80000;
    use_effect3.combat_buff.special_stats_boost = {12, 0, 100};
    use_effect3.effect_socket = Use_effect::Effect_socket::shared;

    std::vector<Use_effect> use_effects{use_effect1, use_effect2, use_effect3};
    auto schedule = Use_effects::compute_schedule(use_effects, Special_stats{}, 580000, 1500);
    for (const auto& effect : schedule)
    {
        EXPECT_TRUE(effect.second.get().name != "should_not_fit");
    }
}

TEST(TestSuite, test_use_effects)
{
    Armory armory;
    auto use1 = armory.find_armor(Socket::trinket, "badge_of_the_swarmguard");
    auto use2 = armory.find_armor(Socket::trinket, "icon_of_unyielding_courage");
    auto use3 = armory.find_armor(Socket::chest, "bulwark_of_kings");
    Combat_simulator sim{};

    std::vector<Use_effect> use_effects{};
    use_effects.emplace_back(sim.death_wish);
    use_effects.emplace_back(sim.recklessness);
    use_effects.emplace_back(sim.bloodrage);
    use_effects.emplace_back(
        Use_effect{"blood_fury", Use_effect::Effect_socket::unique, {}, {0, 0, 300}, 0, 15, 120, true});
    use_effects.emplace_back(Use_effect{"berserking", Use_effect::Effect_socket::unique, {}, {}, 0, 10, 180, false});
    use_effects.emplace_back(use1.use_effects[0]);
    use_effects.emplace_back(use2.use_effects[0]);
    use_effects.emplace_back(use3.use_effects[0]);
    int sim_time = 320000;
    auto schedule = Use_effects::compute_schedule(use_effects, Special_stats{}, sim_time, 1500);
    EXPECT_TRUE(is_ascending(schedule));
    int ic = 0;
    int bk = 0;
    int dw = 0;
    int rl = 0;
    int br = 0;
    int bf = 0;
    int bs = 0;
    for (const auto& e : schedule)
    {
        const auto& ue = e.second.get();
        if (ue.name == "icon_of_unyielding_courage")
            ic++;
        if (ue.name == "bulwark_of_kings")
            bk++;
        if (ue.name == "blood_fury")
            bf++;
        if (ue.name == "berserking")
            bs++;
        if (ue.name == "bloodrage")
            br++;
        if (ue.name == "recklessness")
            rl++;
        if (ue.name == "death_wish")
            dw++;
    }
    EXPECT_EQ(ic, 3);
    EXPECT_EQ(bk, 1);
    EXPECT_EQ(dw, 2);
    EXPECT_EQ(rl, 1);
    EXPECT_EQ(br, 6);
    EXPECT_EQ(bf, 3);
    EXPECT_EQ(bs, 2);
}
