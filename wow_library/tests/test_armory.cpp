#include "Armory.hpp"

#include "gtest/gtest.h"

TEST(TestSuite, test_armory)
{
    Armory armory;

    for (const auto& w : armory.get_weapon_in_socket(Weapon_socket::main_hand))
    {
        ASSERT_TRUE(w.weapon_socket == Weapon_socket::main_hand || w.weapon_socket == Weapon_socket::one_hand);
    }

    for (const auto& a : armory.get_items_in_socket(Socket::chest))
    {
        ASSERT_EQ(a.socket, Socket::chest);
    }

    const auto& x1 = armory.find_weapon(Weapon_socket::main_hand, "blinkstrike");
    ASSERT_EQ(x1.name, "blinkstrike");

    const auto& x2 = armory.find_weapon(Weapon_socket::one_hand, "dragonstrike");
    ASSERT_EQ(x2.name, "dragonstrike");

    const auto& x3 = armory.find_weapon(Weapon_socket::two_hand, "lionheart_executioner");
    ASSERT_EQ(x3.name, "lionheart_executioner");

    // TODO(vigo) could add basic armory consistency checks here

    for (auto& const_by_type : { armory.fists_t })
    {
        auto& by_type = const_cast<std::vector<Weapon>&>(const_by_type);
        by_type.clear();
    }
    ASSERT_FALSE(armory.fists_t.empty());

    std::vector<Weapon> by_types[] = { armory.fists_t };
    for (auto& by_type : by_types)
    {
        by_type.clear();
    }
    ASSERT_FALSE(armory.fists_t.empty());

    for (auto by_type : { &armory.fists_t })
    {
        by_type->clear();
    }
    ASSERT_TRUE(armory.fists_t.empty());
}