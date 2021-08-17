
#include "simulation_fixture.cpp"

#include <sim_interface.hpp>

#include <fstream>
#include <regex>
#include <chrono>
#include <filesystem>

TEST_F(Sim_fixture, test_via_config)
{
    std::filesystem::path p;
    for (auto pp = std::filesystem::current_path(); pp.has_parent_path(); pp = pp.parent_path())
    {
        if (pp.filename() == "TBC_DPS_Warrior_Sim")
        {
            p = pp;
            break;
        }
    }

    p = p / "simulator" / "tests" / "config.txt";

    std::ifstream ifs(p.c_str(), std::ios_base::in);
    if (!ifs) {
        std::cerr << "failed to open \"" << p << "\"" << std::endl;
        return;
    }

    std::vector<std::string> race;
    std::vector<std::string> armor;
    std::vector<std::string> weapons;
    std::vector<std::string> buffs;
    std::vector<std::string> enchants;
    std::vector<std::string> gems;
    std::vector<std::string> stat_weights;
    std::vector<std::string> options;
    std::vector<std::string> float_options_string;
    std::vector<double> float_options_val;
    std::vector<std::string> talent_string;
    std::vector<int> talent_val;

    const std::vector<std::string> empty;

    std::regex LINE_RE(R"(^(\w+)\s+(.+)$)");

    std::regex KEY_RACE(R"(race_dd)");
    std::regex KEY_ARMOR(R"(^(helmet|neck|shoulder|back|chest|wrists|hands|belt|legs|boots|ring1|ring2|trinket1|trinket2|ranged)_dd$)");
    std::regex KEY_WEAPONS(R"(^(main|off|two)_hand_dd$)");
    std::regex KEY_ENCHANTS(R"(_ench_dd$)");
    std::regex KEY_GEMS(R"(_gem[123]_dd$)");
    std::regex KEY_STAT_WEIGHTS(R"(^stat_weight_(.+)$)");
    std::regex KEY_TALENTS(R"(_talent$)");

    auto has_seen_talents = false;

    std::string line;
    while (std::getline(ifs, line))
    {
        std::smatch sm;
        if (!std::regex_match(line, sm, LINE_RE)) continue;

        auto key = sm[1].str();
        auto value = sm[2].str();

        if (value == "false" || value == "none") continue;

        if (std::regex_search(key, KEY_ARMOR))
        {
            armor.emplace_back(value);
        }
        else if (std::regex_search(key, KEY_WEAPONS))
        {
            weapons.emplace_back(value);
        }
        else if (std::regex_search(key, KEY_ENCHANTS))
        {
            auto prefix = key[0];
            if (key == "helmet_ench_dd") prefix = 'e';
            if (key == "boots_ench_dd") prefix = 't';
            enchants.emplace_back(prefix + value);
        }
        else if (std::regex_search(key, KEY_GEMS))
        {
            if (key != value) gems.emplace_back(value);
        }
        else if (std::regex_match(key, sm, KEY_STAT_WEIGHTS))
        {
            if (key == value) stat_weights.emplace_back(sm[1].str());
        }
        else if (std::regex_search(key, KEY_TALENTS))
        {
            talent_string.emplace_back(key);
            talent_val.emplace_back(std::stoi(value));
            has_seen_talents = true;
        }
        else if (std::regex_search(key, KEY_RACE))
        {
            race.emplace_back(value);
        }
        else if (key == value && !has_seen_talents)
        {
            buffs.emplace_back(value);
        }
        else
        {
            options.emplace_back(key);
            if (key == value) continue; // bools
            float_options_string.emplace_back(key);
            float_options_val.emplace_back(std::stod(value));
        }
    }

    Sim_input sim_input(race, armor, weapons,
                        buffs, enchants, gems,
                        stat_weights,
                        options, float_options_string, float_options_val,
                        talent_string, talent_val,
                        empty, empty);

    Sim_interface sim_interface;

    auto start = std::chrono::steady_clock::now();
    const auto& sim_output = sim_interface.simulate(sim_input);
    auto end = std::chrono::steady_clock::now();
    std::cout << "took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    std::cout << std::endl;

    std::cout << "dmg_sources" << std::endl;
    for (const auto& ds : sim_output.dmg_sources)
    {
        std::cout << ds << std::endl;
    }
    std::cout << std::endl;

    std::cout << "aura_uptimes" << std::endl;
    for (const auto& s : sim_output.aura_uptimes)
    {
        std::cout << s << std::endl;
    }
    std::cout << std::endl;

    std::cout << "mean_dps" << std::endl;
    for (const auto& v : sim_output.mean_dps)
    {
        std::cout << v << std::endl;
    }
    std::cout << std::endl;

    std::cout << "std_dps" << std::endl;
    for (const auto& v : sim_output.std_dps)
    {
        std::cout << v << std::endl;
    }
    std::cout << std::endl;

    std::cout << "stat_weights" << std::endl;
    for (const auto& v : sim_output.stat_weights)
    {
        std::cout << v << std::endl;
    }
}