#include "find_values.hpp"

#include "gtest/gtest.h"

TEST(TestSuite, test_find_value_class)
{
    std::vector<std::string> mult_armor_vec;
    mult_armor_vec.emplace_back("lionheart_helm");
    mult_armor_vec.emplace_back("stormrages_talisman_of_seething");
    mult_armor_vec.emplace_back("conquerors_spaulders");
    mult_armor_vec.emplace_back("shroud_of_dominion");
    mult_armor_vec.emplace_back("plated_abomination_ribcage");
    mult_armor_vec.emplace_back("hive_defiler_wristguards");
    mult_armor_vec.emplace_back("gauntlets_of_annihilation");
    mult_armor_vec.emplace_back("girdle_of_the_mentor");
    mult_armor_vec.emplace_back("legplates_of_carnage");
    mult_armor_vec.emplace_back("chromatic_boots");

    {
        std::vector<size_t> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        Find_values<size_t> fv(mult_armor_vec, values);
        EXPECT_TRUE(fv.find("lionheart_helm") == 0);
        EXPECT_TRUE(fv.find("stormrages_talisman_of_seething") == 1);
        EXPECT_TRUE(fv.find("conquerors_spaulders") == 2);
        EXPECT_TRUE(fv.find("shroud_of_dominion") == 3);
        EXPECT_TRUE(fv.find("plated_abomination_ribcage") == 4);
        EXPECT_TRUE(fv.find("hive_defiler_wristguards") == 5);
        EXPECT_TRUE(fv.find("gauntlets_of_annihilation") == 6);
        EXPECT_TRUE(fv.find("girdle_of_the_mentor") == 7);
        EXPECT_TRUE(fv.find("legplates_of_carnage") == 8);
        EXPECT_TRUE(fv.find("chromatic_boots") == 9);
    }
    {
        std::vector<double> values{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
        Find_values<double> fv(mult_armor_vec, values);
        EXPECT_TRUE(fv.find("lionheart_helm") == 0.0);
        EXPECT_TRUE(fv.find("stormrages_talisman_of_seething") == 1.0);
        EXPECT_TRUE(fv.find("conquerors_spaulders") == 2.0);
        EXPECT_TRUE(fv.find("shroud_of_dominion") == 3.0);
        EXPECT_TRUE(fv.find("plated_abomination_ribcage") == 4.0);
        EXPECT_TRUE(fv.find("hive_defiler_wristguards") == 5.0);
        EXPECT_TRUE(fv.find("gauntlets_of_annihilation") == 6.0);
        EXPECT_TRUE(fv.find("girdle_of_the_mentor") == 7.0);
        EXPECT_TRUE(fv.find("legplates_of_carnage") == 8.0);
        EXPECT_TRUE(fv.find("chromatic_boots") == 9.0);
    }
}