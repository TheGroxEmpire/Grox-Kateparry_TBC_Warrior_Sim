#ifndef WOW_SIMULATOR_ARMORY_HPP
#define WOW_SIMULATOR_ARMORY_HPP

#include "Item.hpp"
#include <unordered_map>

class Character;

struct Buffs
{
    // World buffs
    Buff fungal_bloom{"fungal_bloom", Attributes{0, 0}, Special_stats{50, 0, 0}};

    // these are set in "sim_interface.cpp" or "sim_interface_mult.cpp" depending on what's specified
    Buff full_polarity{"full_polarity", {}, {}};
    Buff battle_squawk{"battle_squawk", {}, {}};
    Buff expose_weakness{"expose_weakness", {}, {}};
    Buff ferocious_inspiration{"ferocious_inspiration", {}, {}};

    // Player_buffs
    Buff battle_shout{"battle_shout", {}, {}, 0, {}, {{"battle_shout", Use_effect::Effect_socket::unique, {}, {0, 0, 306}, -10, 120, 120, true}}};
    // battle_shout_preshout_bonus AP is applied in Combat_simulator.cpp. Cooldown is set at 360s to never cast it more than once
    Buff battle_shout_preshout_bonus{"battle_shout_preshout_bonus", {}, {}, 0, {}, {{"battle_shout_preshout_bonus", Use_effect::Effect_socket::unique, {}, {0, 0, 0}, 0, 120, 360, true}}};
    Buff blessing_of_kings{"blessing_of_kings", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0, 0, 0.1}};
    Buff blessing_of_might{"blessing_of_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 264}};
    Buff gift_of_the_wild{"gift_of_the_wild", Attributes{18.9, 18.9}, Special_stats{0.0, 0.0, 0.0}};
    Buff leader_of_the_pack{"leader_of_the_pack", Attributes{0.0, 0.0}, Special_stats{5.0, 0.0, 0.0}};
    Buff improved_seal_of_the_crusader{"improved_seal_of_the_crusader", Attributes{0.0, 0.0}, Special_stats{3.0, 0.0, 0.0}};
    Buff blood_frenzy{"blood_frenzy", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0, .04}};
    Buff improved_sanctity_aura{"improved_sanctity_aura", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0, .02, 0, 0, 0, 0, .02}};
    Buff heroic_presence{"heroic_presence", Attributes{0.0, 0.0}, Special_stats{0.0, 1.0, 0.0}};
    Buff braided_eternium_chain{"braided_eternium_chain", Attributes{0.0, 0.0}, Special_stats{1.2683, 0.0, 0.0}};
    Buff improved_faerie_fire{"improved_faerie_fire", Attributes{0.0, 0.0}, Special_stats{0.0, 3.0, 0.0}};
    Buff trueshot_aura{"trueshot_aura", {}, {0, 0, 125}};
    Buff improved_hunters_mark{"improved_hunters_mark", {}, {0, 0, 0, 110}};
    Buff strength_of_earth_totem{"strength_of_earth_totem", Attributes{86.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff grace_of_air_totem{"grace_of_air_totem", Attributes{0.0, 77.0}, Special_stats{0.0, 0.0, 0.0}};

    // Consumables
    Buff elixir_mongoose{"elixir_mongoose", Attributes{0.0, 25.0}, Special_stats{1.26, 0.0, 0.0}};
    Buff elixir_of_major_agility{"elixir_of_major_agility", Attributes{0.0, 35.0}, Special_stats{0.9, 0.0, 0.0}};
    Buff elixir_of_mastery_bloodberry_elixir{"elixir_of_mastery_bloodberry_elixir", Attributes{15.0, 15.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff blessed_sunfruit{"blessed_sunfruit", Attributes{10.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff roasted_clefthoof{"roasted_clefthoof", Attributes{20.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff spicy_hot_talbuk{"spicy_hot_talbuk", Attributes{0.0, 0.0}, Special_stats{0.0, 1.26, 0.0}};
    Buff grilled_mudfish{"grilled_mudfish", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff ravager_dog{"ravager_dog", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Buff charred_bear_kabobs{"charred_bear_kabobs", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 24}};
    Buff juju_power{"juju_power", Attributes{30.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff elixir_of_giants{"elixir_of_giants", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff elixir_of_major_strength{"elixir_of_major_strength", Attributes{35.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff elixir_of_brute_force{"elixir_of_brute_force", Attributes{18.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff juju_might{"juju_might", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40}};
    Buff winterfall_firewater{"winterfall_firewater", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 35}};
    Buff fel_strength_elixir{"fel_strength_elixir", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 90}};
    Buff onslaught_elixir{"onslaught_elixir", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 60}};
    Buff elixir_of_demonslaying{"elixir_of_demonslaying", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 265}};
    Buff flask_of_relentless_assault{"flask_of_relentless_assault", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 120}};
    Buff unstable_flask_of_the_bandit{"unstable_flask_of_the_bandit", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 40}};
    Buff unstable_flask_of_the_beast{"unstable_flask_of_the_beast", Attributes{20.0, 20.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff unstable_flask_of_the_soldier{"unstable_flask_of_the_soldier", Attributes{20.0, 0.0}, Special_stats{0.9, 0.0, 0.0}};
    Buff roids{"roids", Attributes{25.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff scroll_of_strength_v{"scroll_of_strength_v", Attributes{20.0, 0.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff scroll_of_agility_v{"scroll_of_agility_v", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 0.0}};
    Buff mighty_rage_potion{"mighty_rage_potion", {}, {}, 0, {},
                            {{"mighty_rage_potion", Use_effect::Effect_socket::unique, {60, 0}, {0, 0, 0}, 60, 20,
                              120, false}}};
    Buff drums_of_battle{"drums_of_battle", {}, {}, 0, {},
                         {{"drums_of_battle", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, .05},
                          0, 30, 120, false}}};
    Buff bloodlust{"bloodlust", {}, {}, 0, {},
                         {{"bloodlust", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, .3},
                          0, 40, 600, false}}};
    Buff haste_potion{"haste_potion", {}, {}, 0, {},
                         {{"haste_potion", Use_effect::Effect_socket::unique, {}, {0, 0, 0, 0, .25},
                          0, 15, 120, false}}};
    Buff insane_strength_potion{"insane_strength_potion", {}, {}, 0, {},
                         {{"insane_strength_potion", Use_effect::Effect_socket::unique, {120, 0.0}, {},
                          0, 15, 120, false}}};
    Buff heroic_potion{"heroic_potion", {}, {}, 0, {},
                         {{"heroic_potion", Use_effect::Effect_socket::unique, {70, 0.0}, {},
                          0, 15, 120, false}}};

    Weapon_buff windfury_totem{"windfury_totem", {}, {}, 0, {"windfury_totem", Hit_effect::Type::windfury_hit, {}, {0, 0, 445}, 0, 0, 0, 0.2}};
    Weapon_buff flametongue_totem{"flametongue_totem", {}, {}, 0, {"flametongue_totem", Hit_effect::Type::damage_magic, {}, {}, 59.7, 0, 0, 1}};
    Weapon_buff dense_stone{"dense_stone", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0, 0, 0, 8}};
    Weapon_buff adamantite_stone{"adamantite_stone", Attributes{0.0, 0.0}, Special_stats{0.6341, 0.0, 0.0, 0, 0, 0, 0, 12}};
    Weapon_buff elemental_stone{"elemental_stone", Attributes{0.0, 0.0}, Special_stats{1.2683, 0.0, 0.0}};
    Weapon_buff consecrated_stone{"consecrated_stone", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 100.0}};
};

struct Gems
{
    Gem strength_2{"strength_2", Attributes {2, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_3{"strength_3", Attributes {3, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_4{"strength_4", Attributes {4, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_5{"strength_5", Attributes {5, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_6{"strength_6", Attributes {6, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_8{"strength_8", Attributes {8, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem strength_10{"strength_10", Attributes {10, 0}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_2{"agility_2", Attributes {0, 2}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_3{"agility_3", Attributes {0, 3}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_4{"agility_4", Attributes {0, 4}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_5{"agility_5", Attributes {0, 5}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_6{"agility_6", Attributes {0, 6}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_8{"agility_8", Attributes {0, 8}, Special_stats{0.0, 0.0, 0.0}};
    Gem agility_10{"agility_10", Attributes {0, 10}, Special_stats{0.0, 0.0, 0.0}};
    Gem crit_2{"crit_2", Attributes {0, 0}, Special_stats {0.09057971014, 0.0, 0.0}};
    Gem crit_3{"crit_3", Attributes {0, 0}, Special_stats {0.1358695652, 0.0, 0.0}};
    Gem crit_4{"crit_4", Attributes {0, 0}, Special_stats {0.1811594203, 0.0, 0.0}};
    Gem crit_5{"crit_5", Attributes {0, 0}, Special_stats {0.2264492754, 0.0, 0.0}};
    Gem crit_6{"crit_6", Attributes {0, 0}, Special_stats {0.2717391304, 0.0, 0.0}};
    Gem crit_8{"crit_8", Attributes {0, 0}, Special_stats {0.3623188406, 0.0, 0.0}};
    Gem crit_10{"crit_10", Attributes {0, 0}, Special_stats {0.4528985507, 0.0, 0.0}};
    Gem crit_12{"crit_12", Attributes {0, 0}, Special_stats {0.5434782609, 0.0, 0.0}};
    Gem hit_2{"hit_2", Attributes {0, 0}, Special_stats {0.0, 0.1268230818, 0.0}};
    Gem hit_4{"hit_4", Attributes {0, 0}, Special_stats {0.0, 0.2536461636, 0.0}};
    Gem hit_6{"hit_6", Attributes {0, 0}, Special_stats {0.0, 0.3804692454, 0.0}};
    Gem hit_8{"hit_8", Attributes {0, 0}, Special_stats {0.0, 0.5072923272, 0.0}};
    Gem hit_10{"hit_10", Attributes {0, 0}, Special_stats {0.0, 0.634115409, 0.0}};
    Gem hit_12{"hit_12", Attributes {0, 0}, Special_stats {0.0, 0.7609384908, 0.0}};
    Gem ap_20{"ap_20", Attributes {0, 0}, Special_stats {0.0, 0.0, 20}};
    Gem ap_24{"ap_24", Attributes {0, 0}, Special_stats {0.0, 0.0, 24}};
    Gem ap_32{"ap_32", Attributes {0, 0}, Special_stats {0.0, 0.0, 32}};
    Gem dmg_3{"dmg_3", Attributes {0, 0}, Special_stats {0.0, 0.0, 0.0, 0, 0, 0, 0, 3}};
    Gem crit_3_str_3{"crit_3_str_3", Attributes {3, 0}, Special_stats {0.1358695652, 0.0, 0.0}};
    Gem crit_4_str_4{"crit_4_str_4", Attributes {4, 0}, Special_stats {0.1811594203, 0.0, 0.0}};
    Gem crit_4_str_5{"crit_4_str_5", Attributes {5, 0}, Special_stats {0.1811594203, 0.0, 0.0}};
    Gem crit_5_str_5{"crit_5_str_5", Attributes {5, 0}, Special_stats {0.2264492754, 0.0, 0.0}};
    Gem agi_12_critDmg_3{"agi_12_critDmg_3", Attributes {0, 12}, Special_stats {0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0.03}};
    Gem gem_haste{"gem_haste", {}, {}, {"gem_haste", Hit_effect::Type::stat_boost, {0, 0}, {0, 0, 0, 0, 0.1522}, 0, 6, 40, 0, 0, 0, 0, 1, 1}};
};

struct Armory
{
    std::vector<Armor> helmet_t
            {
                {"empty_head", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::head},
                
                // Phase 1
                {"mask_of_the_deceiver", Attributes{0.0, 32.0}, Special_stats{0.0, 1.0145846544071022, 64.0, 0, 0.0}, Socket::head}, 
                {"cowl_of_beastly_rage", Attributes{29.0, 26.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"warbringer_battle-helm", Attributes{45.0, 0.0}, Special_stats{1.0869565217391306, 0.8877615726062144, 0.0, 0, 0.0}, Socket::head, Set::warbringer},
                {"wastewalker_helm", Attributes{0.0, 0.0}, Special_stats{0.996376811594203, 1.14140773620799, 56.0, 0, 0.0}, Socket::head, Set::wastewalker}, 
                {"helm_of_the_claw", Attributes{0.0, 25.0}, Special_stats{0.0, 0.8877615726062144, 66.0, 0, 0.0}, Socket::head}, 
                {"helm_of_assassination", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 0.0, 66.0, 0, 0.0}, Socket::head}, 
                {"gladiators_plate_helm", Attributes{30.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"malefic_mask_of_the_shadows", Attributes{0.0, 0.0}, Special_stats{1.9021739130434785, 1.9657577679137603, 84.0, 0, 0.0}, Socket::head}, 
                {"helm_of_desolation", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 66.0, 0, 0.0}, Socket::head}, 
                {"beast_lord_helm", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, Socket::head}, 
                {"grand_marshal_plate_helm", Attributes{20.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"warpstalker_helm", Attributes{0.0, 24.0}, Special_stats{0.0, 0.0, 48.0, 0, 0.0}, Socket::head}, 
                {"cowl_of_defiance", Attributes{0.0, 34.0}, Special_stats{0.0, 1.5218769816106532, 100.0, 0, 0.0}, Socket::head}, 
                {"overlords_helmet_of_second_sight", Attributes{29.0, 0.0}, Special_stats{1.0869565217391306, 0.8243500317057705, 0.0, 0, 0.0}, Socket::head}, 
                {"warhelm_of_the_bold", Attributes{24.0, 22.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"exorcists_plate_helm", Attributes{25.0, 0.0}, Special_stats{1.1322463768115942, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"stealthers_helmet_of_second_sight", Attributes{0.0, 24.0}, Special_stats{0.0, 0.8243500317057705, 60.0, 0, 0.0}, Socket::head}, 
                {"exorcists_scaled_helm", Attributes{29.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"hope_bearer_helm", Attributes{30.0, 23.0}, Special_stats{0.0, 0.8243500317057705, 0.0, 0, 0.0}, Socket::head}, 
                {"earthwardens_coif", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 34.0, 0, 0.0}, Socket::head}, 
                {"doomplate_warhelm", Attributes{34.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head, Set::doomplate}, 
                {"exorcists_linked_helm", Attributes{24.0, 0.0}, Special_stats{0.996376811594203, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"stalkers_helmet_of_second_sight", Attributes{0.0, 31.0}, Special_stats{0.0, 0.0, 60.0, 0, 0.0}, Socket::head}, 
                {"maulgars_warhelm", Attributes{0.0, 0.0}, Special_stats{1.9021739130434785, 0.0, 86.0, 0, 0.0}, Socket::head}, 
                {"mask_of_veiled_death", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 1.2048192771084338, 80.0, 0, 0.0}, Socket::head}, 
                {"thundering_greathelm", Attributes{50.0, 43.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"moonglade_cowl", Attributes{24.0, 8.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"ragesteel_helm", Attributes{45.0, 0.0}, Special_stats{1.6757246376811596, 0.0, 0.0, 0, 0.0}, Socket::head, Set::ragesteel}, 
                {"exorcists_leather_helm", Attributes{0.0, 27.0}, Special_stats{0.4528985507246377, 0.0, 20.0, 0, 0.0}, Socket::head}, 
                {"warbringer_greathelm", Attributes{15.0, 17.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"cobrascale_hood", Attributes{0.0, 37.0}, Special_stats{0.0, 1.14140773620799, 74.0, 0, 0.0}, Socket::head}, 
                {"gnomish_battle_goggles", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 100.0, 0, 0.0}, Socket::head}, 
                {"lionheart_helm", Attributes{18.0, 0.0}, Special_stats{1.2681159420289856, 1.2682308180088777, 0.0, 0, 0.0}, Socket::head}, 
                {"helm_of_endless_rage", Attributes{26.0, 26.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head}, 
                {"conquerors_crown", Attributes{29.0, 18.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::head},

                // Phase 2
                {"destroyer_battlehelm", Attributes{47, 0.0}, Special_stats{1.6304347826086958, 1.3316423589093216, 0.0, 0, 0.0}, Socket::head, Set::destroyer}, 
                {"merciless_gladiators_plate_helm", Attributes{35, 0.0}, Special_stats{1.403985507246377, 0.8877615726062144, 0.0, 0, 0.0}, Socket::head}, 
                {"furious_gizmatic_goggles", Attributes{48, 0.0}, Special_stats{1.7210144927536233, 0.8243500317057705, 0.0, 0, 0.0}, Socket::head}, 
                {"felsteel_warhelm", Attributes{46, 0.0}, Special_stats{1.992753623188406, 1.9023462270133165, 0.0, 0, 0.0}, Socket::head}, 

                // Phase 3
                {"forest_prowlers_helm", Attributes{0.0, 42.0}, Special_stats{0.9057971014492754, 0.0, 100.0, 0, 0.0}, Socket::head}, 
                {"helm_of_the_illidari_shatterer", Attributes{51.0, 0.0}, Special_stats{1.9021739130434785, 2.155992390615092, 0.0, 0, 0.0}, Socket::head}, 
                {"cursed_vision_of_sargeras", Attributes{0.0, 39.0}, Special_stats{1.7210144927536233, 1.3316423589093216, 108.0, 0, 0.0}, Socket::head}, 
                {"onslaught_battle-helm", Attributes{54.0, 41.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 105}, Socket::head, Set::onslaught}, 
                {"vengeful_gladiators_plate_helm", Attributes{39.0, 0.0}, Special_stats{1.585144927536232, 0.7609384908053266, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::head}, 
            
                // Phase 4
                {"amani_mask_of_death", Attributes{46.0, 0.0}, Special_stats{0.0, 2.09, 0.0, 0, 0.0}, Socket::head},
                {"coif_of_the_jungle_stalker", Attributes{0.0, 36.0}, Special_stats{0.0, 0.0, 72, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, Socket::head},
                {"grimgrin_faceguard", Attributes{0.0, 40.0}, Special_stats{0.0, 1.52, 82, 0, 0.0}, Socket::head}, 

                // Phase 5
                {"coif_of_alleria", Attributes{0.0, 43.0}, Special_stats{1.5398550724637683, 0.0, 126.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 182}, Socket::head}, 
                {"crown_of_anasterian", Attributes{61.0, 0.0}, Special_stats{1.8115942028985508, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 231}, Socket::head}, 
                {"duplicitous_guise", Attributes{0.0, 43.0}, Special_stats{0.0, 1.9023462270133165, 126.0, 0, 0.021559923906150923}, Socket::head}, 
                {"mayhem_projection_goggles", Attributes{62.0, 0.0}, Special_stats{2.3097826086956523, 1.648700063411541, 0.0, 0, 0.0}, Socket::head}, 
                {"brutal_gladiators_plate_helm", Attributes{45.0, 0.0}, Special_stats{1.855203619909502, 1.139240506329114, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::head}, 
            };

    std::vector<Armor> neck_t
            {
                {"empty_neck", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::neck},
                
                // Phase 1
                {"choker_of_vile_intent", Attributes{0.0, 20.0}, Special_stats{0.0, 1.14140773620799, 42.0, 0, 0.0}, Socket::neck}, 
                {"worgen_claw_necklace", Attributes{0.0, 20.0}, Special_stats{0.0, 1.077996195307546, 42.0, 0, 0.0}, Socket::neck}, 
                {"haramads_bargain", Attributes{26.0, 25.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::neck}, 
                {"mithril_chain_of_heroism", Attributes{28.0, 22.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::neck}, 
                {"natashas_choker", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.6341154090044389, 50.0, 0, 0.0}, Socket::neck}, 
                {"adamantite_chain_of_the_unbroken", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 0.0, 34.0, 0, 0.0}, Socket::neck}, 
                {"necklace_of_trophies", Attributes{22.0, 15.0}, Special_stats{0.0, 0.8877615726062144, 0.0, 0, 0.0}, Socket::neck}, 
                {"traitors_noose", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.7609384908053266, 38.0, 0, 0.0}, Socket::neck}, 
                {"blood_guards_necklace_of_ferocity", Attributes{0.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 54.0, 0, 0.0}, Socket::neck}, 
                {"the_savages_choker", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.015852885225110972}, Socket::neck}, 
                {"bone_chain_necklace", Attributes{0.0, 19.0}, Special_stats{0.0, 0.8243500317057705, 36.0, 0, 0.0}, Socket::neck}, 
                {"stormrages_talisman_of_seething", Attributes{0.0, 0.0}, Special_stats{1.2683, 0.0, 26.0, 0, 0.0}, Socket::neck},
                {"insignia_of_the_maghari_hero", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.8877615726062144, 30.0, 0, 0.0}, Socket::neck}, 
                {"saberclaw_talisman", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 46.0, 0, 0.0}, Socket::neck},
                {"braided_eternium_chain", Attributes{0.0, 0.0}, Special_stats{0.0, 1.3317, 0, 0, 0, 0, 0, 5}, Socket::neck},

                //Phase 2
                {"pendant_of_the_perilous", Attributes{32.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 0.0, 0, 0.0}, Socket::neck}, 
                {"choker_of_animalistic_fury", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 64.0, 0, 0.0}, Socket::neck}, 
                {"telonicuss_pendant_of_mayhem", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 70.0, 0, 0.0}, Socket::neck}, 
                {"veterans_pendant_of_triumph", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 42.0, 0, 0.0}, Socket::neck}, 

                // Phase 3
                {"choker_of_serrated_blades", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 70, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 175}, Socket::neck}, 
                {"choker_of_endless_nightmares", Attributes{0.0, 0.0}, Special_stats{1.2228260869565217, 1.3316423589093216, 72, 0, 0.0}, Socket::neck}, 
                {"vindicators_pendant_of_triumph", Attributes{0.0, 0.0}, Special_stats{0.996376811594203, 0.0, 50, 0, 0.0}, Socket::neck}, 
                
                // Phase 4
                {"brooch_of_deftness", Attributes{0.0, 0.0}, Special_stats{0, 1.40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5.326569435637287}, Socket::neck}, 

                // Phase 5
                {"hard_khorium_choker", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 58, 0, 0.0183893468611287250, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 150}, Socket::neck}, 
                {"clutch_of_demise", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 64, 0, 0.019023462270133164}, Socket::neck}, 
                {"sindorei_pendant_of_triumph", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 58, 0, 0.0}, Socket::neck}, 
                {"guardians_pendant_of_triumph", Attributes{0.0, 0.0}, Special_stats{1.176470588235294, 0.0, 58, 0, 0.0}, Socket::neck},
                {"shattered_sun_pendant_of_might_aldor", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 64, 0, 0.0}, Socket::neck, Set::none, {{"shattered_sun_pendant_of_might_aldor", Hit_effect::Type::stat_boost, {}, {0.0, 0.0, 200}, 0, 10, 45, .15}}},
                {"shattered_sun_pendant_of_might_scryers", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 64, 0, 0.0}, Socket::neck, Set::none, {{"shattered_sun_pendant_of_might_scryers", Hit_effect::Type::damage_magic, {}, {}, 350, 10, 45, .15}}},
            };

    std::vector<Armor> shoulder_t
            {
                {"empty_shoulder", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::shoulder},
                
                // Phase 1
                {"ripfiend_shoulderplates", Attributes{39.0, 0.0}, Special_stats{0.996376811594203, 0.8243500317057705, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"ragesteel_shoulders", Attributes{32.0, 0.0}, Special_stats{0.7699275362318841, 0.570703868103995, 0.0, 0, 0.0}, Socket::shoulder, Set::ragesteel}, 
                {"bladed_shoulderpads_of_the_merciless", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.8243500317057705, 58.0, 0, 0.0}, Socket::shoulder}, 
                {"warbringer_shoulderplates", Attributes{32.0, 22.0}, Special_stats{0.0, 0.8243500317057705, 0.0, 0, 0.0}, Socket::shoulder, Set::warbringer}, 
                {"gladiators_plate_shoulders", Attributes{22.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"mantle_of_perenolde", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 1.4584654407102093, 20.0, 0, 0.0}, Socket::shoulder}, 
                {"wastewalker_shoulderpads", Attributes{0.0, 25.0}, Special_stats{0.0, 1.0145846544071022, 34.0, 0, 0.0}, Socket::shoulder, Set::wastewalker}, 
                {"expedition_scouts_epaulets", Attributes{0.0, 25.0}, Special_stats{0.0, 1.5218769816106532, 50.0, 0, 0.0}, Socket::shoulder}, 
                {"talbuk_hide_shoulders", Attributes{0.0, 20.0}, Special_stats{0.0, 0.9511731135066582, 70.0, 0, 0.0}, Socket::shoulder}, 
                {"spaulders_of_slaughter", Attributes{30.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"doomplate_shoulderguards", Attributes{19.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 0.0, 0, 0.0}, Socket::shoulder, Set::doomplate}, 
                {"shoulderpads_of_assassination", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 42.0, 0, 0.0}, Socket::shoulder}, 
                {"high_warlord_plate_shoulders", Attributes{14.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"conquerors_spaulders", Attributes{20.0, 16.0}, Special_stats{0.0, 0.6341154090044389, 0.0, 0, 0.0}, Socket::shoulder}, 

                // Phase 2
                {"shoulderpads_of_the_stranger", Attributes{0.0, 33.0}, Special_stats{0.7246376811594204, 0.0, 60.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 2.5364616360177554}, Socket::shoulder}, 
                {"destroyer_shoulderblades", Attributes{36.0, 0.0}, Special_stats{0.9057971014492754, 1.14140773620799, 0.0, 0, 0.0}, Socket::shoulder, Set::destroyer}, 
                {"pauldrons_of_the_wardancer", Attributes{38.0, 0.0}, Special_stats{1.3134057971014494, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"merciless_gladiators_plate_shoulders", Attributes{29.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 

                // Phase 3
                {"mantle_of_darkness", Attributes{0.0, 0.0}, Special_stats{1.4945652173913044, 1.3950538998097655, 94.0, 0, 0.0}, Socket::shoulder}, 
                {"shoulders_of_the_hidden_predator", Attributes{0.0, 38.0}, Special_stats{1.177536231884058, 0.0, 76.0, 0, 0.0}, Socket::shoulder}, 
                {"blood-stained_pauldrons", Attributes{47.0, 0.0}, Special_stats{1.4492753623188408, 1.4584654407102093, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"razorfury_mantle", Attributes{0.0, 28.0}, Special_stats{1.0416666666666667, 0.0, 76.0, 0, 0.0}, Socket::shoulder}, 
                {"swiftstrike_shoulders", Attributes{0.0, 29.0}, Special_stats{0.0, 0.0, 74.0, 0, 0.024096385542168676}, Socket::shoulder}, 
                {"shoulders_of_lighting_reflexes", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 74.0, 0, 0.023462270133164237}, Socket::shoulder}, 
                {"swiftsteel_shoulders", Attributes{47.0, 0.0}, Special_stats{0.0, 1.2048192771084338, 0.0, 0, 0.02219403931515536}, Socket::shoulder}, 
                {"onslaught_shoulderblades", Attributes{39.0, 39.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 91}, Socket::shoulder, Set::onslaught}, 
                {"vengeful_gladiators_plate_shoulders", Attributes{33.0, 0.0}, Special_stats{1.358695652, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
            
                // Phase 4
                {"pauldrons_of_gruesome_fate", Attributes{42.0, 0.0}, Special_stats{0.0, 1.46, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 154}, Socket::shoulder}, 
                {"shoulderpads_of_dancing_blades", Attributes{0.0, 33.0}, Special_stats{0.0, 1.01, 68.0, 0, 0.0}, Socket::shoulder}, 
                {"pauldrons_of_primal_fury", Attributes{0.0, 28.0}, Special_stats{0.0, 1.27, 58.0, 0, 0.0}, Socket::shoulder}, 

                // Phase 5
                {"pauldrons_of_berserking", Attributes{45.0, 0.0}, Special_stats{1.358695652173913, 0.0, 0.0, 0, 0.017121116043119847}, Socket::shoulder}, 
                {"demontooth_shoulderpads", Attributes{38.0, 38.0}, Special_stats{0.9057971014492754, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 105}, Socket::shoulder}, 
                {"mantle_of_the_golden_forest", Attributes{0.0, 40.0}, Special_stats{0.0, 0.0, 90.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 154}, Socket::shoulder}, 
                {"shoulderpads_of_vehemence", Attributes{0.0, 33.0}, Special_stats{0.0, 1.648700063411541, 90.0, 0, 0.019023462270133164}, Socket::shoulder}, 
                {"brutal_gladiators_plate_shoulders", Attributes{38.0, 0.0}, Special_stats{1.583710407239819, 0.0, 0.0, 0, 0.0}, Socket::shoulder}, 
                {"shoulderpads_of_the_silvermoon_retainer", Attributes{0.0, 35.0}, Special_stats{0.0, 1.458465441, 62, 0, 0.0}, Socket::shoulder}, 
                {"shoulderpads_of_the_everlasting_pain", Attributes{30.0, 0.0}, Special_stats{0.0, 0.0, 0, 0, 0.01331642359}, Socket::shoulder}, 
            };

    std::vector<Armor> back_t
            {
                {"empty_back", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::back},
                
                // Phase 1
                {"black_iron_battlecloak", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 0.0, 60.0, 0, 0.0}, Socket::back}, 
                {"vengeance_wrap", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 52.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_the_pit_stalker", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 56.0, 0, 0.0}, Socket::back}, 
                {"drape_of_the_dark_reavers", Attributes{0.0, 24.0}, Special_stats{0.0, 1.077996195307546, 34.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_the_inciter", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 1.0145846544071022, 30.0, 0, 0.0}, Socket::back}, 
                {"netherfury_cape", Attributes{19.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::back}, 
                {"blood_knight_war_cloak", Attributes{0.0, 23.0}, Special_stats{0.0, 0.0, 48.0, 0, 0.0}, Socket::back}, 
                {"royal_cloak_of_arathi_kings", Attributes{26.0, 0.0}, Special_stats{0.0, 1.0145846544071022, 0.0, 0, 0.0}, Socket::back}, 
                {"shroud_of_frenzy", Attributes{21.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 0.0, 0, 0.0}, Socket::back}, 
                {"shroud_of_dominion", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 50.0, 0, 0.0}, Socket::back}, 
                {"sergeants_heavy_cloak", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 38.0, 0, 0.0}, Socket::back}, 
                {"dawnstrikes_cloak", Attributes{0.0, 12.0}, Special_stats{0.0, 0.570703868103995, 42.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_the_craft", Attributes{0.0, 25.0}, Special_stats{0.0, 0.8243500317057705, 20.0, 0, 0.0}, Socket::back}, 
                {"capacitus_cloak_of_calibration", Attributes{22.0, 18.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::back}, 
                {"nomads_woven_cloak", Attributes{14.0, 14.0}, Special_stats{0.0, 0.8243500317057705, 0.0, 0, 0.0}, Socket::back}, 
                {"farstrider_wildercloak", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 70.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_malice", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.0}, Socket::back}, 
                {"delicate_green_poncho", Attributes{0.0, 14.0}, Special_stats{0.0, 0.8877615726062144, 28.0, 0, 0.0}, Socket::back}, 
                {"consortium_cloak_of_the_quick", Attributes{0.0, 14.0}, Special_stats{0.0, 0.8877615726062144, 26.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_impulsiveness", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 40.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_the_fallen_god", Attributes{11.0, 26.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::back}, 

                // Phase 2
                {"thalassian_wildercloak", Attributes{0.0, 28.0}, Special_stats{0.0, 0.0, 68.0, 0, 0.0}, Socket::back}, 
                {"razor_scale_battlecloak", Attributes{33.0, 23.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::back}, 
                {"crystalweave_cape", Attributes{0.0, 15.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.01648700063}, Socket::back}, 

                // Phase 3
                {"shadowmoon_destroyers_drape", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 1.077996195307546, 72.0, 0, 0.0}, Socket::back}, 
                {"cloak_of_darkness", Attributes{23.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 0.0, 0, 0.0}, Socket::back}, 

                // Phase 4
                {"dorys_embrace", Attributes{0.0, 0.0}, Special_stats{0.9057971014, 0.0, 46.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 112}, Socket::back},
                {"cloak_of_fiends", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 46.0, 0, 0.01585288523}, Socket::back},

                // Phase 5
                {"cloak_of_unforgivable_sin", Attributes{0.0, 26}, Special_stats{0.0, 0.0, 72, 0, 0.020291693088142045}, Socket::back}, 
            };

    std::vector<Armor> chest_t
            {
                {"empty_chest", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::chest},
                
                // Phase 1
                {"bulwark_of_kings", Attributes{35.0, 0.0}, Special_stats{1.6757246376811596, 1.4584654407102093, 0.0, 0, 0.0}, Socket::chest, Set::none, {}, {{"bulwark_of_kings", Use_effect::Effect_socket::unique, {150, 0}, {}, 0, 15, 900, false}}},
                {"bulwark_of_kings_no_cd", Attributes{35.0, 0.0}, Special_stats{1.6757246376811596, 1.4584654407102093, 0.0, 0, 0.0}, Socket::chest}, 
                {"terrorweave_tunic", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 1.3316423589093216, 96.0, 0, 0.0}, Socket::chest}, 
                {"breastplate_of_kings", Attributes{31.0, 0.0}, Special_stats{1.403985507246377, 1.2682308180088777, 0.0, 0, 0.0}, Socket::chest}, 
                {"warbringer_breastplate", Attributes{44.0, 0.0}, Special_stats{1.177536231884058, 0.0, 0.0, 0, 0.0}, Socket::chest, Set::warbringer}, 
                {"ebon_netherscale_breastplate", Attributes{0.0, 0.0}, Special_stats{1.403985507246377, 0.0, 80.0, 0, 0.0}, Socket::chest}, 
                {"gladiators_plate_chestpiece", Attributes{23.0, 0.0}, Special_stats{1.358695652173913, 0.7609384908053266, 0.0, 0, 0.0}, Socket::chest}, 
                {"twisting_nether_chain_shirt", Attributes{0.0, 0.0}, Special_stats{1.4945652173913044, 0.0, 68.0, 0, 0.0}, Socket::chest}, 
                {"primalstrike_vest", Attributes{0.0, 38.0}, Special_stats{0.0, 0.7609384908053266, 108.0, 0, 0.0}, Socket::chest}, 
                {"chestguard_of_the_conniver", Attributes{0.0, 37.0}, Special_stats{0.0, 1.3950538998097655, 90.0, 0, 0.0}, Socket::chest}, 
                {"chestguard_of_exile", Attributes{44.0, 0.0}, Special_stats{1.6304347826086958, 0.0, 0.0, 0, 0.0}, Socket::chest}, 
                {"ragesteel_breastplate", Attributes{50.0, 0.0}, Special_stats{1.3134057971014494, 0.0, 0.0, 0, 0.0}, Socket::chest, Set::ragesteel}, 
                {"chestplate_of_adal", Attributes{35.0, 0.0}, Special_stats{1.1322463768115942, 1.3950538998097655, 0.0, 0, 0.0}, Socket::chest}, 
                {"doomplate_chestguard", Attributes{30.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 0.0, 0, 0.0}, Socket::chest, Set::doomplate}, 
                {"the_exarchs_protector", Attributes{30.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 0.0, 0, 0.0}, Socket::chest}, 
                {"crimsonforge_breastplate", Attributes{36.0, 0.0}, Special_stats{1.177536231884058, 1.0145846544071022, 0.0, 0, 0.0}, Socket::chest}, 
                {"wastewalker_tunic", Attributes{0.0, 28.0}, Special_stats{0.0, 0.0, 56.0, 0, 0.0}, Socket::chest, Set::wastewalker}, 
                {"durotans_battle_harness", Attributes{31.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 0.0, 0, 0.0}, Socket::chest}, 
                {"vest_of_vengeance", Attributes{0.0, 27.0}, Special_stats{0.0, 0.6975269499048827, 42.0, 0, 0.0}, Socket::chest}, 
                {"nether_chain_shirt", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 40.0, 0, 0.0}, Socket::chest}, 
                {"chestguard_of_the_dark_stalker", Attributes{0.0, 0.0}, Special_stats{1.4492753623188408, 0.0, 84.0, 0, 0.0}, Socket::chest}, 
                {"ghoul_skin_tunic", Attributes{40.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::chest}, 
                {"tunic_of_assassination", Attributes{0.0, 28.0}, Special_stats{0.0, 0.0, 54.0, 0, 0.0}, Socket::chest}, 
                {"blackened_chestplate", Attributes{30.0, 30.0}, Special_stats{0.0, 1.585288522511097, 0.0, 0, 0.0}, Socket::chest}, 
                {"plated_abomination_ribcage", Attributes{45.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 0.0, 0, 0.0}, Socket::chest}, 
                {"maghari_scouts_tunic", Attributes{0.0, 23.0}, Special_stats{0.0, 0.7609384908053266, 86.0, 0, 0.0}, Socket::chest}, 
                {"high_warlords_plate_chestpiece", Attributes{12.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 0.0, 0, 0.0}, Socket::chest}, 
                {"chestguard_of_no_remorse", Attributes{0.0, 0.0}, Special_stats{0.9510869565, 0.0, 92.0, 0, 0.0}, Socket::chest},

                // Phase 2
                {"bulwark_of_ancient_kings", Attributes{40.0, 0.0}, Special_stats{1.8568840579710146, 1.585288522511097, 0.0, 0, 0.0}, Socket::chest, Set::none, {}, {{"bulwark_of_ancient_kings", Use_effect::Effect_socket::unique, {150, 0}, {}, 0, 15, 900, false}}},
                {"bulwark_of_ancient_kings_no_cd", Attributes{40.0, 0.0}, Special_stats{1.8568840579710146, 1.585288522511097, 0.0, 0, 0.0}, Socket::chest}, 
                {"bloodsea_brigand_s_vest", Attributes{0.0, 0.0}, Special_stats{1.6304347826086958, 1.7121116043119848, 92.0, 0, 0.0}, Socket::chest}, 
                {"destroyer_breastplate", Attributes{50.0, 0.0}, Special_stats{1.4945652173913044, 0.9511731135066582, 0.0, 0, 0.0}, Socket::chest, Set::destroyer}, 
                {"krakken_heart_breastplate", Attributes{61.0, 43.0}, Special_stats{0.0, 1.9023462270133165, 0.0, 0, 0.0}, Socket::chest}, 
                {"merciless_gladiators_plate_chestpiece", Attributes{35.0, 0.0}, Special_stats{1.585144927536232, 0.8877615726062144, 0.0, 0, 0.0}, Socket::chest}, 
                {"ranger_generals_chestguard", Attributes{0.0, 36.0}, Special_stats{0.6340579710144928, 0.0, 88.0, 0, 0.0}, Socket::chest}, 

                // Phase 3
                {"nether_shadow_tunic", Attributes{0.0, 36.0}, Special_stats{0.0, 2.219403931515536, 86.0, 0, 0.0}, Socket::chest}, 
                {"vest_of_mounting_assault", Attributes{0.0, 58.0}, Special_stats{0.0, 0.0, 116.0, 0, 0.0}, Socket::chest}, 
                {"heartshatter_breastplate", Attributes{63.0, 0.0}, Special_stats{1.992753623188406, 1.9023462270133165, 0.0, 0, 0.0}, Socket::chest}, 
                {"onslaught_breastplate", Attributes{53.0, 34.0}, Special_stats{0.0, 1.0145846544071022, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::chest, Set::onslaught}, 
                {"shadowprowlers_chestguard", Attributes{32.0, 40.0}, Special_stats{0.0, 0.5072923272035511, 0.0, 0, 0.0}, Socket::chest}, 
                {"vengeful_gladiators_plate_chestpiece", Attributes{39.0, 0.0}, Special_stats{1.7663043478260871, 0.7609384908053266, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::chest}, 
                {"midnight_chestguard", Attributes{0.0, 0.0}, Special_stats{2.083333333, 1.838934686, 106}, Socket::chest},

                // Phase 4
                {"bloodstained_elven_battlevest", Attributes{46.0, 0.0}, Special_stats{1.59, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 161}, Socket::chest}, 
                {"shadowtooth_trollskin_cuirass", Attributes{0.0, 45.0}, Special_stats{0.0, 0.0, 92, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 210}, Socket::chest},
                {"arrow-fall_chestguard", Attributes{0.0, 39.0}, Special_stats{0.0, 0.0, 78, 0.0, 0.0190}, Socket::chest},

                // Phase 5
                {"hard_khorium_breastplate", Attributes{61.0, 0.0}, Special_stats{1.8568840579710146, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 280}, Socket::chest}, 
                {"bladed_chaos_tunic", Attributes{0.0, 42.0}, Special_stats{1.7210144927536233, 0.0, 120.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 210}, Socket::chest}, 
                {"carapace_of_sun_and_shadow", Attributes{0.0, 42.0}, Special_stats{0.0, 1.9023462270133165, 120.0, 0, 0.024096385542168676}, Socket::chest}, 
                {"warharness_of_reckless_fury", Attributes{61.0, 0.0}, Special_stats{1.8568840579710146, 0.0, 0.0, 0, 0.020291693088142045}, Socket::chest}, 
                {"embrace_of_the_phoenix", Attributes{0.0, 44.0}, Special_stats{0.0, 0.0, 104.0, 0, 0.02790107799619531}, Socket::chest}, 
                {"harness_of_carnal_instinct", Attributes{52.0, 44.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 196}, Socket::chest}, 
                {"vicious_hawkstrider_hauberk", Attributes{0.0, 47.0}, Special_stats{0.0, 0.0, 104.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 182}, Socket::chest},
                {"brutal_gladiators_plate_chestpiece", Attributes{45.0, 0.0}, Special_stats{2.036199095022624, 1.139240506329114, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::chest}, 
                {"netherforce_chestplate", Attributes{44.0, 0.0}, Special_stats{1.358695652, 1.141407736, 0.0}, Socket::chest},
                {"breastplate_of_ire", Attributes{52.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.032278481012658228}, Socket::chest}, 
            };

    std::vector<Armor> wrists_t
            {
                {"empty_wrist", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::wrist},
                
                // Phase 1
                {"bladespire_warbands", Attributes{20.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"black_felsteel_bracers", Attributes{26.0, 0.0}, Special_stats{0.996376811594203, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"bracers_of_maliciousness", Attributes{0.0, 0.0}, Special_stats{0.996376811594203, 0.0, 50.0, 0, 0.0}, Socket::wrist}, 
                {"ebon_netherscale_bracers", Attributes{0.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 38.0, 0, 0.0}, Socket::wrist}, 
                {"marshals_linked_bracers", Attributes{19.0, 0.0}, Special_stats{0.6793478260869565, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"marshals_scaled_bracers", Attributes{26.0, 0.0}, Special_stats{0.3170289855072464, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"marshals_plate_bracers", Attributes{19.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"primalstrike_bracers", Attributes{0.0, 15.0}, Special_stats{0.0, 0.0, 64.0, 0, 0.0}, Socket::wrist}, 
                {"felstalker_bracers", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.0}, Socket::wrist}, 
                {"nightfall_wristguards", Attributes{0.0, 24.0}, Special_stats{0.0, 0.0, 46.0, 0, 0.0}, Socket::wrist}, 
                {"wristguards_of_vengeance", Attributes{24.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"aged_leather_bindings", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 40.0, 0, 0.0}, Socket::wrist}, 
                {"armwraps_of_disdain", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 38.0, 0, 0.0}, Socket::wrist}, 
                {"demolishers_bracers", Attributes{14.0, 0.0}, Special_stats{0.6340579710144928, 0.8243500317057705, 0.0, 0, 0.0}, Socket::wrist}, 
                {"hive_defiler_wristguards", Attributes{23.0, 18.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"marshals_leather_bracers", Attributes{0.0, 19.0}, Special_stats{0.3170289855072464, 0.0, 16.0, 0, 0.0}, Socket::wrist}, 
                {"stalkers_war_bands", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 44.0, 0, 0.0}, Socket::wrist}, 
                {"amber_bands_of_the_aggressor", Attributes{22.0, 18.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"bands_of_syth", Attributes{21.0, 19.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"nightstalkers_wristguards", Attributes{0.0, 12.0}, Special_stats{0.0, 0.570703868103995, 42.0, 0, 0.0}, Socket::wrist}, 
                {"spymistress_wristguards", Attributes{0.0, 18.0}, Special_stats{0.0, 0.9511731135066582, 0.0, 0, 0.0}, Socket::wrist}, 

                // Phase 2
                {"bracers_of_eradication", Attributes{25.0, 0.0}, Special_stats{1.0869565217391306, 1.077996195307546, 0.0, 0, 0.0}, Socket::wrist}, 
                {"true_aim_stalker_bands", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 50.0, 0, 0.0}, Socket::wrist}, 
                {"bands_of_the_celestial_archer", Attributes{0.0, 17.0}, Special_stats{0.7699275362318841, 0.0, 48.0, 0, 0.0}, Socket::wrist}, 
                {"vambraces_of_ending", Attributes{0.0, 24.0}, Special_stats{0.0, 0.0, 52.0, 0, 0.0}, Socket::wrist}, 
                {"veterans_plate_bracers", Attributes{21.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"shard_bound_bracers", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 42.0, 0, 0.0}, Socket::wrist}, 
                
                // Phase 3
                {"wraps_of_precise_flight", Attributes{0.0, 18.0}, Special_stats{0.8605072463768116, 0.0, 58.0, 0, 0.0}, Socket::wrist}, 
                {"insidious_bands", Attributes{0.0, 28.0}, Special_stats{0.0, 0.7609384908053266, 58.0, 0, 0.0}, Socket::wrist}, 
                {"deadly_cuffs", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 0.7609384908053266, 58.0, 0, 0.0}, Socket::wrist}, 
                {"furious_shackles", Attributes{35.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 0.0, 0, 0.0}, Socket::wrist}, 
                {"bracers_of_the_pathfinder", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 48.0, 0, 0.0}, Socket::wrist}, 
                {"bindings_of_lighting_reflexes", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 56.0, 0, 0.017121116043119847}, Socket::wrist}, 
                {"swiftstrike_bracers", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.017121116043119847}, Socket::wrist}, 
                {"swiftsteel_bracers", Attributes{35.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.017121116043119847}, Socket::wrist}, 
                {"vindicators_plate_bracers", Attributes{25.0, 0.0}, Special_stats{0.9510869565, 0.0, 0.0}, Socket::wrist},

                // Phase 4
                {"eternium_rage_shackles", Attributes{32.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 150}, Socket::wrist},
                {"master_assassin_wristwraps", Attributes{0.0, 17.0}, Special_stats{0.82, 0.0, 50}, Socket::wrist},

                // Phase 5
                {"onslaught_bracers", Attributes{33, 0.0}, Special_stats{1.358695652173913, 1.0145846544071022, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, Socket::wrist, Set::onslaught}, 
                {"guardians_plate_bracers", Attributes{28.0, 0.0}, Special_stats{1.085972850678733, 0.0, 0.0}, Socket::wrist},
            };

    std::vector<Armor> hands_t
            {
                {"empty_hands", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::hands},
                
                // Phase 1
                {"gauntlets_of_martial_perfection", Attributes{36.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
                {"liars_tongue_gloves", Attributes{0.0, 0.0}, Special_stats{1.177536231884058, 0.0, 72.0, 0, 0.0}, Socket::hands}, 
                {"fel_leather_gloves", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 1.077996195307546, 36.0, 0, 0.0}, Socket::hands}, 
                {"grips_of_deftness", Attributes{0.0, 29.0}, Special_stats{0.0, 0.0, 60.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 3.804692454026633}, Socket::hands}, 
                {"warbringer_gauntlets", Attributes{34.0, 23.0}, Special_stats{0.0, 1.14140773620799, 0.0, 0, 0.0}, Socket::hands, Set::warbringer}, 
                {"ragesteel_gloves", Attributes{35.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::hands, Set::ragesteel}, 
                {"windstrike_gloves", Attributes{36.0, 18.0}, Special_stats{0.0, 1.077996195307546, 0.0, 0, 0.0}, Socket::hands}, 
                {"felfury_gauntlets", Attributes{0.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 40.0, 0, 0.0}, Socket::hands}, 
                {"gloves_of_dexterous_manipulation", Attributes{0.0, 35.0}, Special_stats{0.0, 0.0, 42.0, 0, 0.0}, Socket::hands}, 
                {"cobrascale_gloves", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 1.2682308180088777, 50.0, 0, 0.0}, Socket::hands}, 
                {"gloves_of_quickening", Attributes{0.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 52.0, 0, 0.0}, Socket::hands}, 
                {"doomplate_gauntlets", Attributes{31.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::hands, Set::doomplate}, 
                {"gauntlets_of_annihilation", Attributes{35.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 0.0, 0, 0.0}, Socket::hands}, 
                {"gauntlets_of_the_dragonslayer", Attributes{0.0, 24.0}, Special_stats{0.0, 0.0, 48.0, 0, 0.0}, Socket::hands}, 
                {"predatory_gloves", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 1.2048192771084338, 48.0, 0, 0.0}, Socket::hands}, 
                {"gladiators_plate_gauntlets", Attributes{28.0, 0.0}, Special_stats{1.177536231884058, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
                {"gloves_of_the_unbound", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.0}, Socket::hands}, 
                {"gauntlets_of_the_skullsplitter", Attributes{31.0, 18.0}, Special_stats{0.0, 0.7609384908053266, 0.0, 0, 0.0}, Socket::hands}, 
                {"handgrips_of_assassination", Attributes{0.0, 25.0}, Special_stats{0.0, 1.077996195307546, 50.0, 0, 0.0}, Socket::hands}, 
                {"deft_handguards", Attributes{0.0, 0.0}, Special_stats{0.5434782608695653, 0.0, 52.0, 0, 0.0}, Socket::hands}, 
                {"flesh_handlers_gauntlets", Attributes{18.0, 17.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.56563094483196}, Socket::hands}, 
                {"gauntlets_of_cruel_intention", Attributes{30.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
                {"gauntlets_of_the_vanquisher", Attributes{24.0, 23.0}, Special_stats{0.0, 0.8877615726062144, 0.0, 0, 0.0}, Socket::hands}, 
                {"gloves_of_enforcement", Attributes{28.0, 20.0}, Special_stats{0.0, 0.6341154090044389, 0.0, 0, 0.0}, Socket::hands}, 
                {"ironblade_gauntlets", Attributes{20.0, 14.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
                {"ogre_defilers_handguards", Attributes{15.0, 0.0}, Special_stats{1.177536231884058, 0.6341154090044389, 0.0, 0, 0.0}, Socket::hands}, 
                {"fists_of_the_unrelenting", Attributes{26.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 0.0, 0, 0.0}, Socket::hands}, 
                {"wastewalker_gloves", Attributes{0.0, 32.0}, Special_stats{0.0, 0.0, 16.0, 0, 0.0}, Socket::hands, Set::wastewalker}, 
                {"high_warlords_plate_gauntlets", Attributes{22.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
                {"edgemasters_handguards", Attributes{0.0, 0.0}, Special_stats{0.0, 1.2048192771084338, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.311984781230184}, Socket::hands}, 

                // Phase 2
                {"gloves_of_the_searing_grip", Attributes{0.0, 33.0}, Special_stats{0.0, 0.0, 66.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.56563094483196}, Socket::hands}, 
                {"destroyer_gauntlets", Attributes{44.0, 0.0}, Special_stats{1.358695652173913, 0.0, 0.0, 0, 0.0}, Socket::hands, Set::destroyer}, 
                {"merciless_gladiators_plate_gauntlets", Attributes{33.0, 0.0}, Special_stats{1.3134057971014494, 0.0, 0.0, 0, 0.0}, Socket::hands}, 

                // Phase 3
                {"pillagers_gauntlets", Attributes{38.0, 0.0}, Special_stats{0.0, 1.14140773620799, 0.0, 0, 0.024096385542168676}, Socket::hands}, 
                {"fist_of_mukoa", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 76.0, 0, 0.023462270133164237}, Socket::hands}, 
                {"grips_of_silent_justice", Attributes{40.0, 0.0}, Special_stats{0.0, 0.9511731135066582, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 175}, Socket::hands}, 
                {"grips_of_damnation", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 76.0, 0, 0.023462270133164237}, Socket::hands}, 
                {"onslaught_gauntlets", Attributes{41.0, 30.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 91}, Socket::hands, Set::onslaught}, 
                {"vengeful_gladiators_plate_gauntlets", Attributes{37.0, 0.0}, Special_stats{1.4492753623188408, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
            
                // Phase 4
                {"furious_deathgrips", Attributes{34.0, 0.0}, Special_stats{0.0, 1.59, 0.0, 0, 0.0}, Socket::hands},
                {"tricksters_stickyfingers", Attributes{0.0, 30.0}, Special_stats{0.0, 0.0, 68, 0, 0.0159}, Socket::hands}, 

                // Phase 5
                {"thalassian_ranger_gauntlets", Attributes{0.0, 40.0}, Special_stats{1.0416666666666667, 0.0, 80.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 182}, Socket::hands}, 
                {"borderland_paingrips", Attributes{47.0, 0.0}, Special_stats{1.7663043478260871, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 161}, Socket::hands}, 
                {"gloves_of_immortal_dusk", Attributes{0.0, 30.0}, Special_stats{1.358695652173913, 0.0, 90.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 154}, Socket::hands}, 
                {"hard_khorium_battlefists", Attributes{45.0, 0.0}, Special_stats{0.0, 1.4584654407102093, 0.0, 0, 0.022828154724159798}, Socket::hands}, 
                {"shadowed_gauntlets_of_paroxysm", Attributes{0.0, 41.0}, Special_stats{0.0, 0.0, 82.0, 0, 0.019023462270133164}, Socket::hands}, 
                {"fletchers_gloves_of_the_phoenix", Attributes{0.0, 30.0}, Special_stats{0.0, 0.0, 76.0, 0, 0.021559923906150923}, Socket::hands}, 
                {"gloves_of_the_forest_drifter", Attributes{34.0, 34.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, Socket::hands}, 
                {"brutal_gladiators_plate_gauntlets", Attributes{42.0, 0.0}, Special_stats{1.67420814479638, 0.0, 0.0, 0, 0.0}, Socket::hands}, 
            };

    std::vector<Armor> belt_t
            {
                {"empty_belt", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::belt},
                
                // Phase 1
                {"girdle_of_the_endless_pit", Attributes{34.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"gronn_stitched_girdle", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 0.0, 72.0, 0, 0.0}, Socket::belt}, 
                {"ebon_netherscale_belt", Attributes{0.0, 0.0}, Special_stats{0.996376811594203, 0.0, 48.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_the_deathdealer", Attributes{0.0, 28.0}, Special_stats{0.0, 1.2682308180088777, 56.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_treachery", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 58.0, 0, 0.0}, Socket::belt}, 
                {"deathforge_girdle", Attributes{22.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"socrethars_girdle", Attributes{0.0, 23.0}, Special_stats{0.9057971014492754, 0.0, 48.0, 0, 0.0}, Socket::belt}, 
                {"terror_pit_girdle", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 44.0, 0, 0.0}, Socket::belt}, 
                {"rune_engraded_belt", Attributes{0.0, 25.0}, Special_stats{0.0, 1.5218769816106532, 50.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_the_mentor", Attributes{21.0, 20.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 0.0, 0, 0.0}, Socket::belt}, 
                {"dunewind_sash", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, Socket::belt}, 
                {"belt_of_the_raven_lord", Attributes{25.0, 17.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_gallantry", Attributes{27.0, 14.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"marshals_plate_belt", Attributes{27.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"marshals_linked_girdle", Attributes{27.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"marshals_scaled_belt", Attributes{36.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"shattraths_champion_belt", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.9511731135066582, 50.0, 0, 0.0}, Socket::belt}, 
                {"primalstrike_belt", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 84.0, 0, 0.0}, Socket::belt}, 
                {"felstalker_belt", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, Socket::belt}, 
                {"onslaught_girdle", Attributes{31.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 0.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_the_prowler", Attributes{0.0, 25.0}, Special_stats{0.0, 1.077996195307546, 48.0, 0, 0.0}, Socket::belt}, 
                {"evas_strap", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.7609384908053266, 60.0, 0, 0.0}, Socket::belt}, 
                {"belt_of_never_ending_agony", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 64.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_ferocity", Attributes{0.0, 28.0}, Special_stats{0.0, 0.8877615726062144, 44.0, 0, 0.0}, Socket::belt}, 
                {"liars_cord", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 34.0, 0, 0.0}, Socket::belt}, 
                {"rubium_war_girdle", Attributes{31.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_siege", Attributes{30.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"muscle_toning_belt", Attributes{15.0, 0.0}, Special_stats{0.7699275362318841, 0.6975269499048827, 0.0, 0, 0.0}, Socket::belt}, 

                // Phase 2
                {"belt_of_one_hundred_deaths", Attributes{0.0, 29.0}, Special_stats{0.0, 0.0, 74.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 6.341154090044388}, Socket::belt}, 
                {"red_belt_of_battle", Attributes{41.0, 0.0}, Special_stats{1.0869565217391306, 0.8877615726062144, 0.0, 0, 0.0}, Socket::belt}, 
                {"belt_of_deep_shadow", Attributes{0.0, 32.0}, Special_stats{0.0, 1.14140773620799, 66.0, 0, 0.0}, Socket::belt}, 
                {"belt_of_the_black_eagle", Attributes{0.0, 17.0}, Special_stats{0.7699275362318841, 0.0, 66.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_the_tidal_call", Attributes{35.0, 0.0}, Special_stats{1.4945652173913044, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"veterans_plate_belt", Attributes{31.0, 0.0}, Special_stats{1.403985507246377, 0.0, 0.0, 0, 0.0}, Socket::belt}, 

                // Phase 3
                {"girdle_of_the_lightbearer", Attributes{49.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.020291693088142045}, Socket::belt}, 
                {"shadow-walkers_cord", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 76.0, 0, 0.023462270133164237}, Socket::belt}, 
                {"boneweave_gridle", Attributes{0.0, 38.0}, Special_stats{1.0869565217391306, 1.077996195307546, 76.0, 0, 0.0}, Socket::belt}, 
                {"don_alejandros_money_belt", Attributes{0.0, 29.0}, Special_stats{0.8605072463768116, 0.0, 76.0, 0, 0.0}, Socket::belt}, 
                {"belt_of_seething_fury", Attributes{48.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.024096385542168676}, Socket::belt}, 
                {"valestalker_girdle", Attributes{0.0, 27.0}, Special_stats{0.0, 0.0, 76.0, 0, 0.022828154724159798}, Socket::belt}, 
                {"vindicators_plate_belt", Attributes{35.0, 0.0}, Special_stats{1.585144927536232, 0.0, 0.0, 0, 0.0}, Socket::belt}, 

                // Phase 4
                {"chain_of_unleashed_rage", Attributes{34.0, 0.0}, Special_stats{0.0, 0.8877615726062144, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 154}, Socket::belt},
                {"bladeangels_money_belt", Attributes{0.0, 25.0}, Special_stats{0.95, 0.0, 58, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 77}, Socket::belt},

                // Phase 5
                {"onslaught_belt", Attributes{43, 0.0}, Special_stats{1.8115942028985508, 0.0, 0.0, 0, 0.016487000634115410, 0, 0, 0, 0, 0, 0, 6.594800253646164}, Socket::belt, Set::onslaught}, 
                {"guardians_plate_belt", Attributes{40.0, 0.0}, Special_stats{1.809954751131222, 0.0, 0.0, 0, 0.0}, Socket::belt}, 
                {"girdle_of_seething_rage", Attributes{40.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.018987341772152}, Socket::belt},          
                {"belt_of_the_silent_path", Attributes{0.0, 34.0}, Special_stats{0.0, 1.458465441, 78.0, 0, 0.0}, Socket::belt},            
            };

    std::vector<Armor> legs_t
            {
                {"empty_legs", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::legs},

                // Phase 1
                {"scaled_greaves_of_the_marksman", Attributes{0.0, 37.0}, Special_stats{0.0, 1.0145846544071022, 76.0, 0, 0.0}, Socket::legs}, 
                {"skulkers_greaves", Attributes{0.0, 32.0}, Special_stats{0.0, 1.7755231452124287, 64.0, 0, 0.0}, Socket::legs}, 
                {"fel_leather_leggings", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 1.585288522511097, 52.0, 0, 0.0}, Socket::legs}, 
                {"midnight_legguards", Attributes{0.0, 0.0}, Special_stats{1.2228260869565217, 1.077996195307546, 64.0, 0, 0.0}, Socket::legs}, 
                {"warbringer_greaves", Attributes{51.0, 0.0}, Special_stats{1.6757246376811596, 0.0, 0.0, 0, 0.0}, Socket::legs, Set::warbringer}, 
                {"clefthoof_hide_leggings", Attributes{30.0, 30.0}, Special_stats{0.0, 1.077996195307546, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.56563094483196}, Socket::legs}, 
                {"vanquishers_legplates", Attributes{36.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"gladiators_plate_legguards", Attributes{40.0, 0.0}, Special_stats{1.6304347826086958, 0.7609384908053266, 0.0, 0, 0.0}, Socket::legs}, 
                {"wyrmscale_greaves", Attributes{0.0, 0.0}, Special_stats{1.177536231884058, 0.0, 64.0, 0, 0.0}, Socket::legs}, 
                {"shattrath_leggings", Attributes{35.0, 25.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 5.580215599239062}, Socket::legs}, 
                {"legguards_of_the_shattered_hand", Attributes{39.0, 32.0}, Special_stats{0.0, 1.5218769816106532, 0.0, 0, 0.0}, Socket::legs}, 
                {"rip_flayer_leggings", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 56.0, 0, 0.0}, Socket::legs}, 
                {"forestwalker_kilt", Attributes{33.0, 26.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"mennus_scaled_leggings", Attributes{0.0, 0.0}, Special_stats{1.4492753623188408, 0.0, 46.0, 0, 0.0}, Socket::legs}, 
                {"bloodlord_legplates", Attributes{38.0, 0.0}, Special_stats{0.4981884057971015, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"doomplate_legguards", Attributes{36.0, 0.0}, Special_stats{1.2228260869565217, 1.077996195307546, 0.0, 0, 0.0}, Socket::legs, Set::doomplate}, 
                {"oilcloth_breeches", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 1.585288522511097, 60.0, 0, 0.0}, Socket::legs}, 
                {"wastewalker_leggings", Attributes{0.0, 31.0}, Special_stats{0.0, 1.2048192771084338, 28.0, 0, 0.0}, Socket::legs, Set::wastewalker}, 
                {"legplates_of_carnage", Attributes{42.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"midrealm_leggings", Attributes{36.0, 0.0}, Special_stats{0.9510869565217392, 0.6341154090044389, 0.0, 0, 0.0}, Socket::legs}, 
                {"leggings_of_apocalypse", Attributes{15.0, 31.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"leggings_of_assassination", Attributes{0.0, 40.0}, Special_stats{0.0, 1.3950538998097655, 44.0, 0, 0.0}, Socket::legs}, 
                {"greaves_of_the_iron_guardian", Attributes{32.0, 0.0}, Special_stats{0.0, 0.4438807863031072, 0.0, 0, 0.0}, Socket::legs}, 
                {"maghari_huntsmans_leggings", Attributes{0.0, 16.0}, Special_stats{0.0, 0.9511731135066582, 86.0, 0, 0.0}, Socket::legs}, 
                {"retainers_leggings", Attributes{0.0, 0.0}, Special_stats{0.0, 1.648700063411541, 92.0, 0, 0.0}, Socket::legs}, 
                {"titanic_leggings", Attributes{30.0, 0.0}, Special_stats{0.9057971014492754, 0.8877615726062144, 0.0, 0, 0.0}, Socket::legs}, 
                {"elekk_hide_leggings", Attributes{38.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"inklings_leggings", Attributes{37.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"high_warlords_plate_legguards", Attributes{28.0, 0.0}, Special_stats{1.2681159420289856, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"maghari_warlords_legplates", Attributes{12.0, 0.0}, Special_stats{0.5434782608695653, 0.9511731135066582, 0.0, 0, 0.0}, Socket::legs}, 

                // Phase 2
                {"leggings_of_murderous_intent", Attributes{0.0, 45.0}, Special_stats{1.6757246376811596, 0.0, 92.0, 0, 0.0}, Socket::legs}, 
                {"destroyer_greaves", Attributes{52.0, 0.0}, Special_stats{1.4492753623188408, 1.3950538998097655, 0.0, 0, 0.0}, Socket::legs, Set::destroyer}, 
                {"greaves_of_the_bloodwarder", Attributes{49.0, 0.0}, Special_stats{1.403985507246377, 0.0, 0.0, 0, 0.0}, Socket::legs}, 
                {"merciless_gladiators_plate_legguards", Attributes{47.0, 0.0}, Special_stats{1.9474637681159421, 0.8877615726062144, 0.0, 0, 0.0}, Socket::legs}, 
                {"void_reaver_greaves", Attributes{0.0, 37.0}, Special_stats{0.0, 0.0, 88.0, 0, 0.0}, Socket::legs}, 

                // Phase 3
                {"leggings_of_divine_retribution", Attributes{51.0, 0.0}, Special_stats{1.585144927536232, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 350}, Socket::legs}, 
                {"bow-stitched_leggings", Attributes{0.0, 42.0}, Special_stats{0.9057971014492754, 0.0, 100.0, 0, 0.0}, Socket::legs}, 
                {"shady_dealers_pantaloons", Attributes{0.0, 50.0}, Special_stats{0.0, 0.0, 102.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 175}, Socket::legs}, 
                {"legguards_of_endless_rage", Attributes{70.0, 0.0}, Special_stats{2.0833333333333335, 1.2048192771084338, 0.0, 0, 0.0}, Socket::legs}, 
                {"onslaught_greaves", Attributes{62.0, 41.0}, Special_stats{0.0, 0.8877615726062144, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 105}, Socket::legs, Set::onslaught}, 
                {"vengeful_gladiators_plate_legguards", Attributes{51.0, 0.0}, Special_stats{2.1286231884057973, 0.7609384908053266, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::legs}, 
                {"legplates_of_undending_fury", Attributes{52.0, 0.0}, Special_stats{0.0, 1.585288523, 0.0, 0, 0.02726696259}, Socket::legs}, 
            
                // Phase 4
                {"bloodthirsters_wargreaves", Attributes{46.0, 0.0}, Special_stats{0.0, 2.41, 0.0, 0, 0.0}, Socket::legs},
                {"shallow-grave_trousers", Attributes{0.0, 45.0}, Special_stats{0.0, 0.0, 92.0, 0, 0.0190}, Socket::legs}, 

                // Phase 5
                {"felfury_legplates", Attributes{61.0, 0.0}, Special_stats{1.5398550724637683, 0.0, 0.0, 0, 0.031705770450221944}, Socket::legs}, 
                {"leggings_of_the_immortal_night", Attributes{0.0, 41.0}, Special_stats{0.0, 2.0291693088142044, 124.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 224}, Socket::legs}, 
                {"leggings_of_the_immortal_beast", Attributes{44.0, 46.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 189}, Socket::legs}, 
                {"starstalker_legguards", Attributes{0.0, 57.0}, Special_stats{0.0, 1.2682308180088777, 120.0, 0, 0.0}, Socket::legs}, 
                {"brutal_gladiators_plate_legguards", Attributes{57.0, 0.0}, Special_stats{2.398190045248869, 1.139240506329114, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 84}, Socket::legs}, 
                {"trousers_of_the_scryers_retainer", Attributes{0.0, 43.0}, Special_stats{0.0, 1.89873417721519, 104.0, 0, 0.0}, Socket::legs}, 
            };

    std::vector<Armor> boots_t
            {
                {"empty_boots", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::boots},
                
                // Phase 1
                {"fel_leather_boots", Attributes{0.0, 0.0}, Special_stats{0.7699275362318841, 1.585288522511097, 36.0, 0, 0.0}, Socket::boots}, 
                {"edgewalker_longboots", Attributes{0.0, 29.0}, Special_stats{0.0, 0.8243500317057705, 44.0, 0, 0.0}, Socket::boots}, 
                {"ironstriders_of_urgency", Attributes{33.0, 20.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"fiend_slayer_boots", Attributes{0.0, 26.0}, Special_stats{0.0, 1.077996195307546, 34.0, 0, 0.0}, Socket::boots},
                {"rapscallion_boots", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 82.0, 0, 0.0}, Socket::boots}, 
                {"feroucious_swift_kickers", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 58.0, 0, 0.0}, Socket::boots}, 
                {"boots_of_the_unjust", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.8243500317057705, 64.0, 0, 0.0}, Socket::boots}, 
                {"shatari_wrought_greaves", Attributes{24.0, 21.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"boots_of_the_shifting_sands", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 40.0, 0, 0.0}, Socket::boots}, 
                {"zierhuts_lost_treads", Attributes{34.0, 28.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"obsidian_clodstompers", Attributes{34.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"generals_plate_greaves", Attributes{27.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"farahlite_studded_boots", Attributes{0.0, 16.0}, Special_stats{0.7246376811594204, 0.0, 56.0, 0, 0.0}, Socket::boots}, 
                {"generals_scaled_greaves", Attributes{36.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"felboar_hide_shoes", Attributes{0.0, 18.0}, Special_stats{0.0, 0.570703868103995, 68.0, 0, 0.0}, Socket::boots}, 
                {"the_masters_treads", Attributes{0.0, 24.0}, Special_stats{0.0, 1.0145846544071022, 48.0, 0, 0.0}, Socket::boots}, 
                {"clocktocks_jumpers", Attributes{15.0, 0.0}, Special_stats{1.2228260869565217, 0.6975269499048827, 0.0, 0, 0.0}, Socket::boots}, 
                {"eaglecrest_warboots", Attributes{29.0, 21.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"boots_of_the_shadow_flame", Attributes{0.0, 44.0}, Special_stats{0.0, 1.2682308180088777, 0.0, 0, 0.0}, Socket::boots}, 
                {"chromatic_boots", Attributes{20.0, 20.0}, Special_stats{0.0, 0.6341154090044389, 0.0, 0, 0.0}, Socket::boots}, 

                // Phase 2
                {"boots_of_utter_darkness", Attributes{0.0, 0.0}, Special_stats{1.4492753623188408, 1.4584654407102093, 66.0, 0, 0.0}, Socket::boots}, 
                {"warboots_of_obliteration", Attributes{44.0, 0.0}, Special_stats{1.403985507246377, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"cobra_lash_boots", Attributes{0.0, 33.0}, Special_stats{0.0, 0.0, 66.0, 0, 0.0}, Socket::boots}, 
                {"star_strider_boots", Attributes{0.0, 33.0}, Special_stats{0.0, 0.0, 66.0, 0, 0.0}, Socket::boots}, 
                {"vortex_walking_boots", Attributes{28.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"veterans_plate_greaves", Attributes{31.0, 0.0}, Special_stats{1.403985507246377, 0.0, 0.0, 0, 0.0}, Socket::boots}, 

                // Phase 3
                {"threads_of_the_den_mother", Attributes{38.0, 32.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::boots}, 
                {"softstep_boots_of_tracking", Attributes{0.0, 27.0}, Special_stats{1.177536231884058, 1.077996195307546, 76.0, 0, 0.0}, Socket::boots}, 
                {"dreadboots_of_the_legion", Attributes{38.0, 0.0}, Special_stats{1.358695652173913, 1.14140773620799, 0.0, 0, 0.0}, Socket::boots}, 
                {"shadowmasters_boots", Attributes{0.0, 30.0}, Special_stats{0.7699275362318841, 0.0, 76.0, 0, 0.0}, Socket::boots}, 
                {"quickstriders_moccasins", Attributes{0.0, 28.0}, Special_stats{0.0, 0.9511731135066582, 58.0, 0, 0.0}, Socket::boots}, 
                {"black_featherlight_boots", Attributes{0.0, 0.0}, Special_stats{0.0, 2.155992390615092, 98.0, 0, 0.0}, Socket::boots}, 
                {"vindicators_plate_greaves", Attributes{35.0, 0.0}, Special_stats{1.585144928, 0.0, 0.0}, Socket::boots},

                // Phase 4
                {"skullshatter_warboots", Attributes{30.0, 0.0}, Special_stats{1.31, 0.0, 0.0, 0, 0.0}, Socket::boots},
                {"shadowhunters_treads", Attributes{0.0, 26.0}, Special_stats{0.0, 0.0, 66.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 105}, Socket::boots},
                {"nynjahs_tabi_boots", Attributes{0.0, 29.0}, Special_stats{0.0, 1.40, 60.0, 0, 0.0}, Socket::boots}, 

                // Phase 5
                {"onslaught_treads", Attributes{44, 0.0}, Special_stats{1.6304347826086958, 1.585288522511097, 0.0, 0, 0.019023462270133164}, Socket::boots, Set::onslaught}, 
                {"sunrage_treads", Attributes{0.0, 28}, Special_stats{0.0, 0.0, 58, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::boots}, 
                {"guardians_plate_greaves", Attributes{40.0, 0.0}, Special_stats{1.809954751131222, 0.0, 0.0}, Socket::boots},
                {"sunstrider_warboots", Attributes{29.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, Socket::boots},
            };

    std::vector<Armor> ring_t
            {
                {"empty_ring", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::ring},
                
                // Phase 1
                {"ring_of_reciprocity", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.9511731135066582, 62.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_a_thousand_marks", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 1.2048192771084338, 44.0, 0, 0.0}, Socket::ring}, 
                {"shaffars_band_of_brutality", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 1.2048192771084338, 40.0, 0, 0.0}, Socket::ring}, 
                {"mithril_band_of_the_unscarred", Attributes{26.0, 0.0}, Special_stats{0.996376811594203, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"garonas_signet_ring", Attributes{0.0, 20.0}, Special_stats{0.0, 1.14140773620799, 40.0, 0, 0.0}, Socket::ring}, 
                {"band_of_unnatural_forces", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.6341154090044389, 52.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_arathi_warlords", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 46.0, 0, 0.0}, Socket::ring}, 
                {"violet_signet_of_the_master_assassin", Attributes{0.0, 0.0}, Special_stats{0.0, 1.585288522511097, 56.0, 0, 0.0}, Socket::ring}, 
                {"overseers_signet", Attributes{25.0, 15.0}, Special_stats{0.0, 0.6341154090044389, 0.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_the_recalcitrant", Attributes{0.0, 24.0}, Special_stats{0.0, 0.0, 54.0, 0, 0.0}, Socket::ring}, 
                {"adals_command", Attributes{29.0, 16.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"reaverss_ring", Attributes{23.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"kaylaans_signet", Attributes{0.0, 15.0}, Special_stats{0.0, 0.6341154090044389, 50.0, 0, 0.0}, Socket::ring}, 
                {"slayers_mark_of_the_redemption", Attributes{0.0, 15.0}, Special_stats{0.0, 0.6341154090044389, 50.0, 0, 0.0}, Socket::ring}, 
                {"shapeshifters_signet", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 5.072923272035511}, Socket::ring},
                {"weathered_band_of_the_swamplord", Attributes{20.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_the_shadow_deeps", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.8243500317057705, 24.0, 0, 0.0}, Socket::ring}, 
                {"band_of_anguish", Attributes{0.0, 13.0}, Special_stats{0.0, 0.6341154090044389, 50.0, 0, 0.0}, Socket::ring}, 
                {"aggressors_mark_of_the_shatar", Attributes{0.0, 0.0}, Special_stats{1.3134057971014494, 0.0, 24.0, 0, 0.0}, Socket::ring}, 
                {"band_of_the_exorcist", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.6975269499048827, 34.0, 0, 0.0}, Socket::ring}, 
                {"acrobats_mark_of_the_shatar", Attributes{29.0, 12.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_umbral_doom", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 40.0, 0, 0.0}, Socket::ring}, 
                {"band_of_reanimation", Attributes{0.0, 22.0}, Special_stats{0.0, 0.0, 46.0, 0, 0.0}, Socket::ring}, 
                {"ravenclaw_band", Attributes{0.0, 20.0}, Special_stats{0.0, 0.8243500317057705, 30.0, 0, 0.0}, Socket::ring}, 
                {"protectorate_assassins_ring", Attributes{16.0, 15.0}, Special_stats{0.0, 0.7609384908053266, 0.0, 0, 0.0}, Socket::ring}, 
                {"quick_strike_ring", Attributes{5.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 30.0, 0, 0.0}, Socket::ring}, 
                {"band_of_ursol", Attributes{23.0, 12.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"master_dragonslayers_ring", Attributes{0.0, 0.0}, Special_stats{0.0, 0.6341154090044389, 48.0, 0, 0.0}, Socket::ring}, 

                // Phase 2
                {"band_of_the_ranger_general", Attributes{0.0, 0.0}, Special_stats{1.2681159420289856, 1.14140773620799, 56.0, 0, 0.0}, Socket::ring}, 
                {"ring_of_lethality", Attributes{0.0, 24.0}, Special_stats{0.0, 1.2048192771084338, 50.0, 0, 0.0}, Socket::ring}, 
                {"ancestral_ring_of_conquest", Attributes{32.0, 21.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ring},
                {"veterans_band_of_triumph", Attributes{0.0, 0.0}, Special_stats{0.9963768116, 0.0, 42.0, 0, 0.0}, Socket::ring},

                // Phase 3
                {"band_of_devastation", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 66.0, 0, 0.019657577679137603}, Socket::ring}, 
                {"ring_of_deceitful_intent", Attributes{0.0, 21.0}, Special_stats{0.0, 1.2048192771084338, 58.0, 0, 0.0}, Socket::ring}, 
                {"unstoppable_aggressors_ring", Attributes{36.0, 0.0}, Special_stats{1.358695652173913, 0.0, 0.0, 0, 0.0}, Socket::ring}, 
                {"stormrage_signet_ring", Attributes{0.0, 0.0}, Special_stats{0.0, 1.9023462270133165, 66.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::ring}, 
                {"vindicators_band_of_triumph", Attributes{0.0, 0.0}, Special_stats{1.177536231884058, 0.0, 44.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 56}, Socket::ring}, 
                {"band_of_the_eternal_champion", Attributes{0.0, 29.0}, Special_stats{0.0, 0.0, 60.0, 0, 0.0}, Socket::ring, Set::none, {{"band_of_the_eternal_champion", Hit_effect::Type::stat_boost, {}, {0, 0, 160}, 0, 10, 60, 0, 0, 1, 0, 0, 1}}}, 

                // Phase 4
                {"signet_of_primal_wrath", Attributes{0.0, 28.0}, Special_stats{0.0, 0.0, 58.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::ring},  

                // Phase 5
                {"band_of_ruinous_delight", Attributes{0.0, 0.0}, Special_stats{1.4492753623188408, 0.0, 66, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 161}, Socket::ring}, 
                {"hard_khorium_band", Attributes{0.0, 30.0}, Special_stats{0.0, 0.0, 58, 0, 0.017755231452124286}, Socket::ring}, 
                {"angelistas_revenge", Attributes{0.0, 29.0}, Special_stats{0.0, 0.0, 58, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::ring}, 
                {"guardians_band_of_triumph", Attributes{0.0, 0.0}, Special_stats{1.357466063348416, 0.0, 52.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 56}, Socket::ring}, 
            };

    std::vector<Armor> trinket_t
            {
                {"empty_trinket", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::trinket},
                
                // Phase 1
                {"dragonspine_trophy", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40.0, 0, 0.0}, Socket::trinket, Set::none, {{"dragonspine_trophy", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .206}, 0, 10, 20, 0, 0, 1, 0, 0, 1}}}, 
                {"bloodlust_brooch", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 72.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"bloodlust_brooch", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 278}, 0, 20, 120, true}}}, 
                {"empty_mug_of_direbrew", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 72.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"empty_mug_of_direbrew", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 278}, 0, 20, 120, true}}}, 
                {"hourglass_of_the_unraveller", Attributes{0.0, 0.0}, Special_stats{1.4492753623188408, 0.0, 0.0, 0, 0.0}, Socket::trinket, Set::none, {{"hourglass_of_the_unraveller", Hit_effect::Type::stat_boost, {}, {0.0, 0.0, 300}, 0, 10, 50, .1, Hit_effect::Proc_type::crits}}},
                {"abacus_of_violent_odds", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 64.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"abacus_of_violent_odds", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 0, 0, .16}, 0, 10, 120, true}}}, 
                {"bladefists_breadth", Attributes{0.0, 0.0}, Special_stats{1.18, 0.0, 0.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"bladefists_breadth", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 200}, 0, 15, 90, true}}}, 
                {"core_of_arkelos", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 52.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"core_of_arkelos", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 200}, 0, 20, 120, true}}}, 
                {"drake_fang_talisman", Attributes{0.0, 0.0}, Special_stats{0.0, 1.2682308180088777, 56.0, 0, 0.0}, Socket::trinket}, 
                {"icon_of_unyielding_courage", Attributes{0.0, 0.0}, Special_stats{0.0, 1.9023462270133165, 0.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"icon_of_unyielding_courage", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 600}, 0, 20, 120, true}}},
                {"ancient_draenei_war_talisman", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 0.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"ancient_draenei_war_talisman", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 200}, 0, 15, 90, true}}}, 
                {"figurine_nightseye_panther", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 54.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"figurine_nightseye_panther", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 320}, 0, 12, 180, true}}},
		{"figurine_shadowsong_panther", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 80.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"figurine_shadowsong_panther", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 320}, 0, 15, 90, true}}},
                {"ogre_maulers_badge", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 48.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"ogre_maulers_badge", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 185}, 0, 15, 120, true}}}, 
                {"figurine_felsteel_boar", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 68.0, 0, 0.0}, Socket::trinket}, 
                {"fetish_of_the_fallen", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 56.0, 0, 0.0}, Socket::trinket}, 
                {"terokkar_tablet_of_precision", Attributes{0.0, 0.0}, Special_stats{0.0, 1.3950538998097655, 0.0, 0, 0.0}, Socket::trinket, Set::none, {}, {{"terokkar_tablet_of_precision", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 140}, 0, 15, 90, true}}},
                {"romulos_poison_vial", Attributes{0.0, 0.0}, Special_stats{0.0, 2.219, 0.0, 0, 0.0}, Socket::trinket, Set::none, {{"romulos_poison_vial", Hit_effect::Type::damage_magic, {}, {}, 277, 0, 0, 0, 0, 1, 0, 0, 1}}},
                {"badge_of_the_swarmguard", Attributes{0, 0}, Special_stats{0, 0, 0}, Socket::trinket, Set::none, {}, {{"badge_of_the_swarmguard", Use_effect::Effect_socket::unique, {}, {}, 0, 30, 180, false, {{"badge_of_the_swarmguard", Hit_effect::Type::reduce_armor, {}, {}, 0, 30, 0, 0, 0, 1, 200, 6, 10}}}}},
                {"kiss_of_the_spider", Attributes{0, 0}, Special_stats{0.63, 0.63, 0}, Socket::trinket, Set::none, {}, {{"kiss_of_the_spider", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 0, 0, .126}, 0, 15, 120, true}}},
                {"mark_of_the_champion", Attributes{0, 0}, Special_stats{0, 0, 150}, Socket::trinket},
                {"slayers_crest", Attributes{0, 0}, Special_stats{0, 0, 64}, Socket::trinket, Set::none, {}, {{"slayers_crest", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 260}, 0, 20, 120, true}}},
                
                // Phase 2
                {"solarians_sapphire", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::trinket},
                {"tsunami_talisman", Attributes{0.0, 0.0}, Special_stats{1.7210144927536233, 0.6341154090044389, 0.0, 0, 0.0}, Socket::trinket, Set::none, {{"tsunami_talisman", Hit_effect::Type::stat_boost, {}, {0.0, 0.0, 340}, 0, 10, 45, .1, Hit_effect::Proc_type::crits}}},
                {"crystalforged_trinket", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0, 0, 0.0, 0, 0, 7}, Socket::trinket, Set::none, {}, {{"crystalforged_trinket", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 216}, 0, 10, 60, true}}},

                // Phase 3
                {"madness_of_the_betrayer", Attributes{0.0, 0.0}, Special_stats{0.0, 1.2682308180088777, 84.0, 0, 0.0}, Socket::trinket, Set::none, {{"madness_of_the_betrayer", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 0, 0, 0, 1, 300, 1, 1}}}, 
                {"ashtongue_talisman_of_valor", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::trinket, Set::none, {{"ashtongue_talisman_of_valor", Hit_effect::Type::ashtongue_talisman_of_valor, {55, 0}, {}, 0, 12, 0, .25}}},
                {"darkmoon_card_crusade", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0}, Socket::trinket, Set::none, {{"darkmoon_card_crusade", Hit_effect::Type::stat_boost, {}, {0, 0, 6}, 0, 10, 0, 1, 0, 1, 0, 20}}},
                {"darkmoon_card_wrath", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::trinket, Set::none, {{"darkmoon_card_wrath", Hit_effect::Type::stat_boost, {}, {0.7699275362, 0, 0}, 0, 10, 0, 1, Hit_effect::Proc_type::non_crits, 0, 0, 130}}}, 

                // Phase 4
                {"berserkers_call", Attributes{0, 0}, Special_stats{0, 0, 90}, Socket::trinket, Set::none, {}, {{"berserkers_call", Use_effect::Effect_socket::shared, {0,  0}, {0, 0, 360}, 0, 20, 120, true}}},

                // Phase 5
                {"blackened_naaru_sliver", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.03424223208623969}, Socket::trinket, Set::none, {{"blackened_naaru_sliver", Hit_effect::Type::blackened_naaru_sliver, {}, {}, 0, 20, 45, .1}}}, 
                {"shard_of_contempt", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 11.160431198478124}, Socket::trinket, Set::none, {{"shard_of_contempt", Hit_effect::Type::stat_boost, {}, {0.0, 0.0, 230}, 0, 20, 45, .1}}}, 
                {"steely_naaru_sliver", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 13.696892834495879}, Socket::trinket}, 

            };

    std::vector<Armor> ranged_t
            {
                {"empty_ranged", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, Socket::ranged},
                
                // Phase 1
                {"barrel_blade_longrifle", Attributes{0.0, 16.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ranged}, 
                {"sunfury_bow_of_the_phoenix", Attributes{0.0, 19.0}, Special_stats{0.0, 0.0, 34.0, 0, 0.0}, Socket::ranged}, 
                {"mamas_insurance", Attributes{0.0, 10.0}, Special_stats{0.27173913043478265, 0.0, 32.0, 0, 0.0}, Socket::ranged}, 
                {"xavian_stiletto", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.7609384908053266, 0.0, 0, 0.0}, Socket::ranged}, 
                {"marksmans_bow", Attributes{0.0, 12.0}, Special_stats{0.0, 0.6975269499048827, 22.0, 0, 0.0}, Socket::ranged}, 
                {"felsteel_whisper_knives", Attributes{0.0, 0.0}, Special_stats{0.4528985507246377, 0.6341154090044389, 20.0, 0, 0.0}, Socket::ranged}, 
                {"wolfslayer_sniper_rifle", Attributes{0.0, 15.0}, Special_stats{0.0, 0.0, 32.0, 0, 0.0}, Socket::ranged}, 
                {"steelhawk_crossbow", Attributes{0.0, 0.0}, Special_stats{0.0, 1.0145846544071022, 30.0, 0, 0.0}, Socket::ranged}, 
                {"nerubian_slavemaker", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 24.0, 0, 0.0}, Socket::ranged}, 
                {"emberhawk_crossbow", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 14.0, 0, 0.0}, Socket::ranged}, 
                {"wrathfire_hand_cannon", Attributes{0.0, 0.0}, Special_stats{0.4528985507246377, 0.0, 30.0, 0, 0.0}, Socket::ranged}, 
                {"wrathtide_longbow", Attributes{0.0, 14.0}, Special_stats{0.0, 0.0, 28.0, 0, 0.0}, Socket::ranged}, 
                {"hemets_elekk_gun", Attributes{0.0, 0.0}, Special_stats{0.5434782608695653, 0.0, 24.0, 0, 0.0}, Socket::ranged}, 
                {"telescropic_sharprifle", Attributes{0.0, 13.0}, Special_stats{0.0, 0.0, 28.0, 0, 0.0}, Socket::ranged}, 
                {"larvae_of_the_great_worm", Attributes{0.0, 0.0}, Special_stats{0.6340579710144928, 0.0, 18.0, 0, 0.0}, Socket::ranged}, 
                {"gladiators_war_edge", Attributes{0.0, 0.0}, Special_stats{0.4981884057971015, 0.0, 24.0, 0, 0.0}, Socket::ranged}, 

                // Phase 2  
                {"serpent_spine_longbow", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 38.0, 0, 0.0}, Socket::ranged}, 
                {"merciless_gladiators_war_edge", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 28.0, 0, 0.0}, Socket::ranged}, 
                {"serpentshrine_shuriken", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.7609384908053266, 0.0, 0, 0.0}, Socket::ranged}, 
                {"arcanite_steam_pistol", Attributes{0.0, 20.0}, Special_stats{0.0, 1.2048192771084338, 0.0, 0, 0.0}, Socket::ranged}, 
                {"crystalline_crossbow", Attributes{0.0, 12.0}, Special_stats{0.0, 0.6975269499, 0.0, 0, 0.0}, Socket::ranged},

                // Phase 3
                {"legionkiller", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, Socket::ranged}, 
                {"twisted_blades_of_zarak", Attributes{0.0, 23.0}, Special_stats{0.7246376811594204, 0.0, 0.0, 0, 0.0}, Socket::ranged}, 
                {"black_bow_of_the_betrayer", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 26.0, 0, 0.0}, Socket::ranged}, 
                {"bristleblitz_striker", Attributes{0.0, 0.0}, Special_stats{1.1322463768115942, 0.0, 0.0, 0, 0.0}, Socket::ranged}, 
                {"vengeful_gladiators_war_edge", Attributes{0.0, 0.0}, Special_stats{0.7699275362318841, 0.0, 30.0, 0, 0.0}, Socket::ranged}, 

                // Phase 4
                {"ancient_amani_longbow", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, Socket::ranged}, 
                {"tuskbreaker", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.01141407736}, Socket::ranged}, 

                // Phase 5
                {"thoridal_the_stars_fury", Attributes{0.0, 17.0}, Special_stats{0.7246376811594204, 0.0, 34, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 112}, Socket::ranged}, 
                {"golden_bow_of_quelthalas", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 34, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 175}, Socket::ranged}, 
                {"blade_of_lifes_inevitability", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 36, 0, 0.010145846544071023}, Socket::ranged}, 
                {"crossbow_of_relentless_strikes", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.8877615726062144, 32, 0, 0.0}, Socket::ranged}, 
                {"nightstrike", Attributes{0.0, 0.0}, Special_stats{0.0, 0.634115409, 0, 0, 0.0, 0, 0, 0, 0, 0, 2.536461636}, Socket::ranged}, 
                {"distracting_blades", Attributes{0.0, 14.0}, Special_stats{0.0, 0.0, 26, 0, 0.00634115409}, Socket::ranged}, 
            };

    std::vector<Armor> none_t {};

    std::vector<Weapon> swords_t
            {
                // Phase 1
                // main_hand / one_hand
                {"gladiators_slicer", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.570703868103995, 28.0, 0, 0.0}, 2.6, 189, 285, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"hope_ender", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 70.0, 0, 0.0}, 2.6, 163, 304, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"spiteblade", Attributes{0.0, 14.0}, Special_stats{0.0, 0.0, 46.0, 0, 0.0}, 2.7, 165, 308, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"blinkstrike", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.6, 147, 275, Weapon_socket::one_hand, Weapon_type::sword, {{"blinkstrike", Hit_effect::Type::extra_hit, {}, {}, 0, 0, 0, 2.6 / 60}}}, 
                {"vindicators_brand", Attributes{0.0, 0.0}, Special_stats{0.0, 1.2048192771084338, 38.0, 0, 0.0}, 2.6, 147, 275, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"felsteel_longblade", Attributes{0.0, 26.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.2, 128, 239, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"high_warlords_slicer", Attributes{0.0, 0.0}, Special_stats{0.588768115942029, 0.5072923272035511, 26.0, 0, 0.0}, 2.6, 149, 224, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"gressil_dawn_of_ruin", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 40.0, 0, 0.0}, 2.7, 138, 257, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"edge_of_the_cosmos", Attributes{0.0, 0.0}, Special_stats{0.7246376811594204, 0.0, 30.0, 0, 0.0}, 2.6, 130, 243, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"phosphorescent_blade", Attributes{0.0, 0.0}, Special_stats{0.4981884057971015, 0.0, 40.0, 0, 0.0}, 2.4, 120, 224, Weapon_socket::one_hand, Weapon_type::sword},
                {"high_warlords_quickblade", Attributes{0.0, 0.0}, Special_stats{0.588768115942029, 0.5072923272035511, 26.0, 0, 0.0}, 1.8, 103, 155, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"the_hungering_cold", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 3.5510462904248574}, 1.5, 76, 143, Weapon_socket::one_hand, Weapon_type::sword},  
                {"the_sun_eater", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 1.6, 93, 174, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"latros_shifting_sword", Attributes{0.0, 15.0}, Special_stats{0.0, 0.0, 26.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 3.5510462904248574}, 1.4, 70, 131, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"kings_defender", Attributes{0.0, 0.0}, Special_stats{0.0, 1.077996195307546, 0.0, 0, 0.0}, 1.6, 98, 182, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"gladiators_quickblade", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.570703868103995, 28.0, 0, 0.0}, 1.5, 95, 178, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"blazeguard", Attributes{0.0, 17.0}, Special_stats{0.0, 1.077996195, 0.0, 0, 0.0}, 1.6, 102, 190, Weapon_socket::one_hand, Weapon_type::sword},
                {"blazefury", Attributes{0.0, 19.0}, Special_stats{0.0, 1.141407736, 0.0, 0, 0.0}, 1.6, 109, 203, Weapon_socket::one_hand, Weapon_type::sword},

                // Phase 2
                // main_hand / one_hand
                {"merciless_gladiators_slicer", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.6341154090044389, 30.0, 0, 0.0}, 2.6, 203, 305, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"talon_of_azshara", Attributes{0.0, 15.0}, Special_stats{0.0, 1.2682308180088777, 40.0, 0, 0.0}, 2.7, 182, 339, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"merciless_gladiators_quickblade", Attributes{0.0, 0.0}, Special_stats{0.8605072463768116, 0.6341154090044389, 30, 0, 0.0}, 1.5, 102, 191, Weapon_socket::one_hand, Weapon_type::sword}, 

                //Kael'thas Legendaries
                {"warp_slicer", Attributes{0.0, 0.0}, Special_stats{1.132246377, 0.0, 70.0, 0, 0.0}, 2.9, 248, 461, Weapon_socket::one_hand, Weapon_type::sword, {{"warp_slicer", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .2}, 0, 10, 0, 0.5}}},

                // Phase 3
                // main_hand / one_hand
                {"blade_of_savagery", Attributes{0.0, 0.0}, Special_stats{0.996376811594203, 0.9511731135066582, 44.0, 0, 0.0}, 1.4, 98, 183, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"warglaive_of_azzinoth_mh", Attributes{0.0, 22.0}, Special_stats{0.0, 1.3316423589093216, 44.0, 0, 0.0}, 2.8, 214, 398, Weapon_socket::main_hand, Weapon_type::sword, {}, Set::the_twin_blades_of_azzinoth},
                {"warglaive_of_azzinoth_mh_non_demon", Attributes{0.0, 22.0}, Special_stats{0.0, 1.3316423589093216, 44.0, 0, 0.0}, 2.8, 214, 398, Weapon_socket::main_hand, Weapon_type::sword, {}, Set::the_twin_blades_of_azzinoth_non_demon},
                {"blade_of_infamy", Attributes{0.0, 28.0}, Special_stats{0.0, 0.0, 56.0, 0, 0.0}, 2.6, 182, 339, Weapon_socket::one_hand, Weapon_type::sword},
                {"vengeful_gladiators_slicer", Attributes{0.0, 0.0}, Special_stats{0.9510869565, 0.5072923272, 34.00, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 214, 322, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"vengeful_gladiators_quickblade", Attributes{0.0, 0.0}, Special_stats{0.9510869565, 0.5072923272, 34.00, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 108, 201, Weapon_socket::one_hand, Weapon_type::sword},
                {"the_unbreakable_will", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0,}, 1.6, 112, 209, Weapon_socket::one_hand, Weapon_type::sword},
                // off_hand
                {"warglaive_of_azzinoth_oh", Attributes{0.0, 21.0}, Special_stats{1.0416666666666667, 0.0, 44, 0, 0.0}, 1.4, 107, 199, Weapon_socket::off_hand, Weapon_type::sword, {}, Set::the_twin_blades_of_azzinoth},
                {"warglaive_of_azzinoth_oh_non_demon", Attributes{0.0, 21.0}, Special_stats{1.0416666666666667, 0.0, 44, 0, 0.0}, 1.4, 107, 199, Weapon_socket::off_hand, Weapon_type::sword, {}, Set::the_twin_blades_of_azzinoth_non_demon},
           
                // Phase 4
                // main_hand / one_hand
                {"heartless", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 42.0, 0, 0.0127}, 2.6, 174, 323, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"akilzons_talonblade", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 52.0, 0, 0.0114}, 1.5, 100, 187, Weapon_socket::one_hand, Weapon_type::sword}, 

                // Phase 5
                // main_hand / one_hand
                {"dragonscale_encrusted_longblade", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.01582278481012658, 0, 0, 0, 0, 0, 0, 6.34147578824544}, 1.5, 113, 211, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"muramasa", Attributes{0.0, 0.0}, Special_stats{0.0, 1.075949367088608, 0.0, 0, 0.02215189873417722}, 2.6, 196, 365, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"brutal_gladiators_quickblade", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 113, 211, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"quickening_blade_of_the_prince", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 38.0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 126}, 1.5, 96, 180, Weapon_socket::one_hand, Weapon_type::sword}, 
                {"brutal_gladiators_slicer", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 224, 337, Weapon_socket::one_hand, Weapon_type::sword},   
           };


    std::vector<Weapon> two_handed_swords_t
            {
                {"empty_weapon_2h", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 1.0, 1, 2, Weapon_socket::two_hand, Weapon_type::unarmed},
                // Phase 1
                {"lionheart_champion", Attributes{49.0, 44.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.6, 341, 513, Weapon_socket::two_hand, Weapon_type::sword, {{"lionheart_champion", Hit_effect::Type::stat_boost, {100, 0}, {}, 0, 10, 0, 3.6 / 60}}},
                {"gladiators_greatsword", Attributes{32.0, 0.0}, Special_stats{1.585144927536232, 1.2682308180088777, 0.0, 0, 0.0}, 3.6, 341, 513, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"khorium_champion", Attributes{50.0, 0.0}, Special_stats{0.0, 2.346227013316424, 0.0, 0, 0.0}, 3.3, 286, 430, Weapon_socket::two_hand, Weapon_type::sword, {{"khorium_champion", Hit_effect::Type::stat_boost, {120, 0}, {}, 0, 30, 0, 0.5 * 3.3 / 60}}},
                {"despair", Attributes{52.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.5, 319, 479, Weapon_socket::two_hand, Weapon_type::sword, {{"despair", Hit_effect::Type::damage_physical, {}, {}, 600, 0, 0, 0.5 * 3.5 / 60}}},
                {"lionheart_blade", Attributes{47.0, 42.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.6, 315, 474, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"quantum_blade", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 0.0, 102.0, 0, 0.0}, 3.5, 304, 456, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"endbringer", Attributes{29.0, 0.0}, Special_stats{2.0380434782608696, 0.0, 0.0, 0, 0.0}, 3.4, 253, 381, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"illidari-bane_claymore", Attributes{0.0, 0.0}, Special_stats{1.6757246376811596, 0.0, 0.0, 0, 0.0}, 3.7, 276, 414, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"illidari-bane_claymore_demons", Attributes{0.0, 0.0}, Special_stats{1.6757246376811596, 0.0, 150.0, 0, 0.0}, 3.7, 276, 414, Weapon_socket::two_hand, Weapon_type::sword},
                {"bonereavers_edge", Attributes{0.0, 0.0}, Special_stats{0.634057971, 0.0, 0.0},3.4, 206.0, 310.0, Weapon_socket::two_hand, Weapon_type::sword, {{"bonereavers_edge", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 0, 2.0*3.4/60.0, 0, 1, 700, 3}}}, 

                // Phase 2
                {"lionheart_executioner", Attributes{52.0, 44.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.6, 365, 549, Weapon_socket::two_hand, Weapon_type::sword, {{"lionheart_executioner", Hit_effect::Type::stat_boost, {100, 0}, {}, 0, 10, 0, 3.6 / 60}}}, 
                {"twinblade_of_the_phoenix", Attributes{0.0, 0.0}, Special_stats{1.6757246376811596, 0.0, 110.0, 0, 0.0}, 3.6, 375, 564, Weapon_socket::two_hand, Weapon_type::sword}, 
                {"merciless_gladiators_greatsword", Attributes{42.0, 0.0}, Special_stats{1.9021739130434785, 1.14140773620799, 0.0, 0, 0.0}, 3.6, 365, 549, Weapon_socket::two_hand, Weapon_type::sword}, 

                // Phase 3
                {"cataclysms_edge", Attributes{75.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 335}, 3.5, 386, 580, Weapon_socket::two_hand, Weapon_type::sword},
                {"vengeful_gladiators_greatsword", Attributes{46.0, 0.0}, Special_stats{2.083333333, 0.9511731135, 0.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 98}, 3.6, 386, 580, Weapon_socket::two_hand, Weapon_type::sword}, 
            
                // Phase 4
                {"jin-rokh_the_great_apocalypse", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 120.0, 0, 0.0285, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 0}, 3.7, 380, 570, Weapon_socket::two_hand, Weapon_type::sword},

                // Phase 5
                {"apolyon_the_soulrender", Attributes{0.0, 0.0}, Special_stats{1.900452488687783, 0.0, 126.0, 0, 0.02025316455696203, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 0}, 3.4, 404, 607, Weapon_socket::two_hand, Weapon_type::sword},
                {"brutal_gladiators_greatsword", Attributes{50.0, 0.0}, Special_stats{2.192982456140351, 1.20253164556962, 0.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 98}, 3.6, 404, 606, Weapon_socket::two_hand, Weapon_type::sword},             
            };

    std::vector<Weapon> two_handed_axes_polearm_t
            {
                // Phase 1
                {"mooncleaver", Attributes{0.0, 0.0}, Special_stats{2.355072463768116, 0.0, 106.0, 0, 0.0}, 3.7, 351, 527, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"ethereum_nexus_reaver", Attributes{50.0, 0.0}, Special_stats{1.358695652173913, 0.0, 0.0, 0, 0.0}, 3.7, 346, 519, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"gorehowl", Attributes{49.0, 43.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.6, 345, 518, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"gladiators_decapitator", Attributes{0.0, 0.0}, Special_stats{1.585144927536232, 1.2682308180088777, 64.0, 0, 0.0}, 3.6, 341, 513, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"axe_of_the_gronn_lords", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 124.0, 0, 0.0}, 3.6, 345, 518, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"glaive_of_the_pit", Attributes{0.0, 0.0}, Special_stats{0.1811594202898551, 0.0, 0.0, 0, 0.0}, 3.7, 354, 532, Weapon_socket::two_hand, Weapon_type::axe, {{"glaive_of_the_pit", Hit_effect::Type::damage_magic, {}, {}, 300, 0, 0, 3.7 / 60}}}, 
                {"legacy", Attributes{0.0, 40.0}, Special_stats{0.0, 0.0, 80.0, 0, 0.0}, 3.5, 319, 479, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"lunar_crescent", Attributes{0.0, 0.0}, Special_stats{2.1286231884057973, 0.0, 96.0, 0, 0.0}, 3.7, 324, 487, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"felsteel_reaper", Attributes{0.0, 0.0}, Special_stats{1.358695652173913, 0.0, 112.0, 0, 0.0}, 3.4, 295, 443, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"trident_of_the_outcast_tribe", Attributes{37.0, 0.0}, Special_stats{0.0, 2.28281547241598, 0.0, 0, 0.0}, 3.2, 270, 406, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"singing_crystal_axe", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 3.5, 295, 444, Weapon_socket::two_hand, Weapon_type::axe, {{"singing_crystal_axe", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .2537}, 0, 10, 0, 3.5 / 60}}},
                {"blackened_spear", Attributes{0.0, 26.0}, Special_stats{0.0, 1.2048192771084338, 92.0, 0, 0.0}, 3.5, 261, 392, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"sonic_spear", Attributes{0.0, 35.0}, Special_stats{0.0, 1.5218769816106532, 62.0, 0, 0.0}, 3.5, 261, 392, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"apexis_cleaver", Attributes{46.0, 0.0}, Special_stats{0.8605072463768116, 0.0, 0.0, 0, 0.0}, 3.6, 268, 403, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"hellscreams_will", Attributes{0.0, 0.0}, Special_stats{0.0, 2.663284717818643, 84.0, 0, 0.0}, 3.5, 261, 392, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"crow_wing_reaper", Attributes{28.0, 17.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.4, 253, 381, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"reaver_of_the_infinites", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, 3.6, 268, 403, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"crystalforged_war_axe", Attributes{0.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 80.0, 0, 0.0}, 3.5, 253, 380, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"terokks_quill", Attributes{0.0, 54.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.3, 246, 370, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"severance", Attributes{43.0, 0.0}, Special_stats{1.268115942, 0.0, 0.0}, 3.6, 235.0, 354.0, Weapon_socket::two_hand, Weapon_type::axe},

                // Phase 2
                {"bloodmoon", Attributes{0.0, 0.0}, Special_stats{2.536231884057971, 0.0, 112.0, 0, 0.0}, 3.7, 375, 564, Weapon_socket::two_hand, Weapon_type::axe},
                {"merciless_gladiators_decapitator", Attributes{0.0, 0.0}, Special_stats{1.9021739130434785, 1.14140773620799, 84.0, 0, 0.0}, 3.6, 365, 549, Weapon_socket::two_hand, Weapon_type::axe},

                // Kael'thas Legendaries
                {"devastation", Attributes{0.0, 0.0}, Special_stats{2.264492754, 0.0, 150.0, 0, 0.0}, 3.9, 496, 744, Weapon_socket::two_hand, Weapon_type::axe, {{"devastation", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .2}, 0, 10, 0, 0.5}}},

                // Phase 3
                {"halberd_of_desolation", Attributes{0.0, 51.0}, Special_stats{0.0, 1.9023462270133165, 100.0, 0, 0.0}, 3.5, 365, 548, Weapon_socket::two_hand, Weapon_type::axe}, 
                {"soul_cleaver", Attributes{65.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 315}, 3.7, 386, 579, Weapon_socket::two_hand, Weapon_type::axe},
                {"vengeful_gladiators_decapitator", Attributes{0.0, 0.0}, Special_stats{2.083333333, 0.9511731135, 92.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 98}, 3.6, 386, 580, Weapon_socket::two_hand, Weapon_type::axe}, 
            
                // Phase 4
                {"trollbane", Attributes{0.0, 39.0}, Special_stats{0.0, 0.0, 94.0, 0, 0.0}, 3.6, 358, 537, Weapon_socket::two_hand, Weapon_type::axe}, 
            
                // Phase 5
                {"shivering_felspine", Attributes{0.0, 52.0}, Special_stats{0.0, 0.0, 120.0, 0, 0.03354430379746835}, 3.5, 393, 590, Weapon_socket::two_hand, Weapon_type::axe},
                {"the_blade_of_harbingers", Attributes{0.0, 52.0}, Special_stats{2.48868778280543, 0.0, 55.0, 0, 0.03354430379746835}, 3.5, 375, 563, Weapon_socket::two_hand, Weapon_type::axe},  
                {"brutal_gladiators_decapitator", Attributes{0.0, 0.0}, Special_stats{2.192982456140351, 1.20253164556962, 100, 0.0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 98}, 3.6, 404, 606, Weapon_socket::two_hand, Weapon_type::axe},
            };

    std::vector<Weapon> two_handed_maces_t
            {
                // Phase 1
                {"deep_thunder", Attributes{37.0, 37.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.8, 360, 541, Weapon_socket::two_hand, Weapon_type::mace}, 
                {"thunder", Attributes{37.0, 37.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.8, 333, 500, Weapon_socket::two_hand, Weapon_type::mace}, 
                {"might_of_menethil", Attributes{20.0, 0.0}, Special_stats{1.268, 0.0, 0.0, 0, 0.0}, 3.8, 289, 435, Weapon_socket::two_hand, Weapon_type::mace}, 

                // Phase 2
                {"stormherald", Attributes{42.0, 42.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.8, 386, 579, Weapon_socket::two_hand, Weapon_type::mace}, 
                {"world_breaker", Attributes{50.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 3.7, 371, 558, Weapon_socket::two_hand, Weapon_type::mace, {{"world_breaker", Hit_effect::Type::stat_boost, {}, {40.76086957, 0, 0}, 0, 4, 0, 3.7/60}}},

                // Phase 3
                {"torch_of_the_damned", Attributes{51.0, 0.0}, Special_stats{1.7210144927536233, 0.0, 0.0, 0, 0.031705770450221944}, 3.8, 396, 595, Weapon_socket::two_hand, Weapon_type::mace}, 
            
                // Phase 5
                {"brutal_gladiators_bonegrinder", Attributes{50.0, 0.0}, Special_stats{2.192982456140351, 1.20253164556962, 0.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 98}, 3.6, 404, 606, Weapon_socket::two_hand, Weapon_type::mace},
            };

    std::vector<Weapon> axes_t
            {
                // Phase 1
                // main_hand / one_hand
                {"black_planar_edge", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 44.0, 0, 0.0}, 2.7, 172, 320, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"gladiators_cleaver", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.570703868103995, 28.0, 0, 0.0}, 2.6, 189, 285, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"gladiators_hacker", Attributes{0.0, 0.0}, Special_stats{0.6793478260869565, 0.570703868103995, 28.0, 0, 0.0}, 1.5, 95, 178, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"the_decapitator", Attributes{0.0, 0.0}, Special_stats{1.2228260869565217, 0.0, 0.0, 0, 0.0}, 2.6, 167, 312, Weapon_socket::one_hand, Weapon_type::axe, {{"the_decapitator", Hit_effect::Type::damage_physical, {}, {}, 540, 0, 180, 1}}}, 
                {"the_planar_edge", Attributes{0.0, 0.0}, Special_stats{0.9057971014492754, 0.0, 42.0, 0, 0.0}, 2.7, 159, 296, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"fel_edged_battleaxe", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.8877615726062144, 0.0, 0, 0.0}, 2.2, 128, 239, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"firebrand_battleaxe", Attributes{17.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 3.5510462904248574}, 2.4, 120, 224, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"the_harvester_of_souls", Attributes{16.0, 0.0}, Special_stats{0.6793478260869565, 0.0, 0.0, 0, 0.0}, 2.6, 130, 243, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"high_warlords_cleaver", Attributes{0.0, 0.0}, Special_stats{0.588768115942029, 0.5072923272035511, 26.0, 0, 0.0}, 2.6, 149, 224, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"warbringer", Attributes{0.0, 0.0}, Special_stats{0.0, 0.4438807863031072, 0.0, 0, 0.0}, 1.8, 102, 190, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"high_warlords_hacker", Attributes{0.0, 0.0}, Special_stats{0.588768115942029, 0.5072923272035511, 26.0, 0, 0.0}, 1.8, 103, 155, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"bogreaver", Attributes{0.0, 0.0}, Special_stats{0.8152173913043479, 0.0, 28.0, 0, 0.0}, 1.7, 85, 159, Weapon_socket::one_hand, Weapon_type::axe},

                // Phase 2
                // main_hand / one_hand
                {"wicked_edge_of_the_planes", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 0.0, 48.0, 0, 0.0}, 2.7, 184, 343, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"netherbane", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 40.0, 0, 0.0}, 2.6, 175, 327, Weapon_socket::one_hand, Weapon_type::axe},
                {"merciless_gladiators_cleaver", Attributes{0.0, 0.0}, Special_stats{0.8605072464, 0.634115409, 30.0, 0, 0.0}, 2.6, 177, 330, Weapon_socket::one_hand, Weapon_type::axe}, 

                // Phase 3
                // main_hand / one_hand
                {"rising_tide", Attributes{0.0, 0.0}, Special_stats{0.0, 1.3316423589093216, 44.0, 0, 0.0}, 2.6, 208, 313, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"the_brutalizer", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0, 5.326569435637286}, 1.6, 128, 193, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"vengeful_gladiators_cleaver", Attributes{0.0, 0.0}, Special_stats{0.9510869565, 0.5072923272, 34.00, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 214, 322, Weapon_socket::one_hand, Weapon_type::axe}, 
                {"vengeful_gladiators_hacker", Attributes{0.0, 0.0}, Special_stats{0.9510869565, 0.5072923272, 34.00, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 108, 201, Weapon_socket::one_hand, Weapon_type::axe}, 
            
                // Phase 4
                // main_hand / one_hand
                {"cleaver_of_the_unforgiving", Attributes{0.0, 0.0}, Special_stats{0.0, 0.76, 0.0, 0, 0.0}, 1.6, 110, 205, Weapon_socket::one_hand, Weapon_type::axe},
            
                // Phase 5
                // one_hand
                {"brutal_gladiators_cleaver", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 196, 365, Weapon_socket::one_hand, Weapon_type::axe},
                // off_hand
                {"brutal_gladiators_chopper", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 196, 365, Weapon_socket::off_hand, Weapon_type::axe},
                {"brutal_gladiators_hacker", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 113, 211, Weapon_socket::off_hand, Weapon_type::axe},     
            };

    std::vector<Weapon> daggers_t
            {
                // Phase 1
                {"malchazeen", Attributes{0.0, 0.0}, Special_stats{0.0, 0.9512213682368160, 50.0, 0, 0.0}, 1.8, 132, 199, Weapon_socket::one_hand, Weapon_type::dagger},
                {"the_night_blade", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0},1.8, 117.0, 176.0, Weapon_socket::one_hand, Weapon_type::dagger, {{"the_night_blade", Hit_effect::Type::reduce_armor, {}, {}, 0, 10, 0, 2.0*1.8/60.0, 0, 1, 435, 3}}},
                {"retainers_blade", Attributes{0.0, 22.0}, Special_stats{0.0, 0.0, 0.0}, 1.5, 97, 146, Weapon_socket::one_hand, Weapon_type::dagger},
                {"guile_of_khoraazi", Attributes{0.0, 19.0}, Special_stats{0.0, 0.0, 38.0}, 1.6, 104, 156, Weapon_socket::one_hand, Weapon_type::dagger},
                {"feltooth_eviscerator", Attributes{0.0, 0.0}, Special_stats{0.9963768116, 0.0, 34.0}, 1.4, 83, 156, Weapon_socket::one_hand, Weapon_type::dagger},
                {"blade_of_the_unrequited", Attributes{0.0, 0.0}, Special_stats{0.4076086957, 0.0, 18.0}, 1.6, 112, 168, Weapon_socket::one_hand, Weapon_type::dagger},
                {"emerald_ripper", Attributes{0.0, 10.0}, Special_stats{0.0, 0.0, 36.0}, 1.8, 126, 189, Weapon_socket::one_hand, Weapon_type::dagger},

                // Phase 2
                {"fang_of_vashj", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 56.0, 0, 0, 0, 0, 0, 0, 0, 0, 5.326569436}, 1.8, 144, 217, Weapon_socket::one_hand, Weapon_type::dagger},
                {"heartrazor", Attributes{0, 20}, {}, 1.8, 121, 226, Weapon_socket::one_hand, Weapon_type::dagger, {{"heartrazor", Hit_effect::Type::stat_boost, {}, {0, 0, 270}, 0, 10, 0, 1.8/60}}},

                // Kael'thas Legendaries
                {"infinity_blade", Attributes{0.0, 0.0}, Special_stats{1.132246377, 0.0, 70.0, 0, 0.0}, 2, 171, 318, Weapon_socket::one_hand, Weapon_type::dagger},

                // Phase 3
                {"messenger_of_fate", Attributes{0.0, 22.0}, Special_stats{0.0, 0.0, 44.0, 0, 0.0}, 1.4, 112, 169, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"shard_of_azzinoth", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 64.0, 0, 0.0}, 1.9, 161, 242, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"trackers_blade", Attributes{0.0, 0.0}, Special_stats{1.0416666666666667, 1.2682308180088777, 44.0, 0, 0.0}, 1.5, 105, 196, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"boundless_agony", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 210}, 1.8, 144, 217, Weapon_socket::one_hand, Weapon_type::dagger}, 
            
                // Phase 4
                {"dagger_of_bad_mojo", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 40, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, 1.8, 137, 207, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"prowlers_strikeblade", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 42, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, 1.5, 100, 187, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"umbral_shiv", Attributes{0.0, 12.0}, Special_stats{0.0, 0.82, 0.0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, 1.8, 137, 207, Weapon_socket::one_hand, Weapon_type::dagger}, 

                // Phase 5
                {"crux_of_the_apocalypse", Attributes{0.0, 18.0}, Special_stats{0.0, 0.0, 56, 0, 0.01708860759493671, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 0}, 1.8, 164, 247, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"shiv_of_exsanguination", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 42, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 196}, 1.8, 136, 253, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"fang_of_kalecgos", Attributes{0.0, 25.0}, Special_stats{0.0, 0.0, 48, 0, 0.01582278481012658, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 0}, 1.5, 113, 211, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"brutal_gladiators_shiv", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.4, 105, 197, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"brutal_gladiators_mutilator", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.8, 136, 253, Weapon_socket::one_hand, Weapon_type::dagger}, 
                {"brutal_gladiators_shanker", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.8, 155, 233, Weapon_socket::one_hand, Weapon_type::dagger},             
            };

    std::vector<Weapon> maces_t
            {
                // Phase 1
                // main_hand / one_hand
                {"dragonmaw", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.7, 172, 320, Weapon_socket::one_hand, Weapon_type::mace, {{"bs_mace_haste", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .134}, 0, 10, 0, 2.7 / 60}}}, 
                {"fools_bane", Attributes{0.0, 0.0}, Special_stats{1.0869565217391306, 0.0, 30.0, 0, 0.0}, 2.6, 159, 296, Weapon_socket::one_hand, Weapon_type::mace},
                {"drakefist_hammer", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.7, 159, 296, Weapon_socket::one_hand, Weapon_type::mace, {{"bs_mace_haste", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .134}, 0, 10, 0, 2.7 / 60}}}, 
                {"runic_hammer", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, 2.4, 140, 261, Weapon_socket::one_hand, Weapon_type::mace}, 
                {"bloodskull_destroyer", Attributes{0.0, 0.0}, Special_stats{0.9510869565217392, 0.0, 22.0, 0, 0.0}, 2.6, 130, 243, Weapon_socket::one_hand, Weapon_type::mace}, 
                {"terokks_nightmace", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.819277108433735}, 2.0, 100, 187, Weapon_socket::one_hand, Weapon_type::mace},
                {"blackout_truncheon", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 1.5, 73, 136, Weapon_socket::one_hand, Weapon_type::mace, {{"blackout_truncheon", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .083}, 0, 10, 0, 1.5 * 0.8 / 60}}},

                // Phase 2
                // main_hand / one_hand
                {"dragonstrike", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.7, 184, 343, Weapon_socket::one_hand, Weapon_type::mace, {{"bs_mace_haste", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .134}, 0, 10, 0, 2.7 / 60}}}, 
                {"rod_of_the_sun_king", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 52.0, 0, 0.0}, 2.7, 189, 352, Weapon_socket::one_hand, Weapon_type::mace, {{"rod_of_the_sun_king", Hit_effect::Type::rage_boost, {}, {}, 5, 0, 0, 2.7 / 60}}},
                {"mallet_of_the_tides", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 3.5510462904248574}, 1.7, 115, 214, Weapon_socket::one_hand, Weapon_type::mace}, 

                // Phase 3
                // main_hand / one_hand
                {"syphon_of_the_nathrezim", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 50.0, 0, 0.0}, 2.8, 196, 365, Weapon_socket::one_hand, Weapon_type::mace}, 
                {"swiftsteel_bludgeon", Attributes{0.0, 0.0}, Special_stats{0.0, 1.2048192771084338, 40.0, 0, 0.017121116043119847}, 1.5, 105, 196, Weapon_socket::one_hand, Weapon_type::mace}, 
            
                // Phase 5
                // main_hand / one_hand
                // one_hand
                {"brutal_gladiators_pummeler", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 196, 365, Weapon_socket::one_hand, Weapon_type::mace},
                // off_hand
                {"brutal_gladiators_bonecracker", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38.0, 0.00, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 113, 211, Weapon_socket::off_hand, Weapon_type::mace},     

            };

    std::vector<Weapon> fists_t
            {
                {"empty_weapon", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0}, 1.0, 1, 2, Weapon_socket::one_hand, Weapon_type::unarmed},
                
                // Phase 1
                // main_hand / one_hand
                {"big_bad_wolfs_paw", Attributes{0.0, 17.0}, Special_stats{0.9057971014492754, 0.0, 0.0, 0, 0.0}, 2.5, 153, 285, Weapon_socket::main_hand, Weapon_type::unarmed}, 
                {"the_bladefist", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 0.0, 0, 0.0}, 2.6, 152, 283, Weapon_socket::main_hand, Weapon_type::unarmed, {{"the_bladefist", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, .114}, 0, 10, 0, 2.6 / 60}}},
                {"claw_of_the_watcher", Attributes{0.0, 0.0}, Special_stats{0.5434782608695653, 0.0, 24.0, 0, 0.0}, 2.5, 125, 233, Weapon_socket::one_hand, Weapon_type::unarmed}, 
                {"demonblood_eviscerator", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 28.0, 0, 0.00, 0, 0, 0, 0, 0, 0, 4.311984781230184}, 2.6, 130, 243, Weapon_socket::one_hand, Weapon_type::unarmed}, 
                {"reflex_blades", Attributes{0.0, 0.0}, Special_stats{0.0, 1.0145846544071022, 32.0, 0, 0.0}, 2.7, 135, 252, Weapon_socket::one_hand, Weapon_type::unarmed}, 

                // Phase 2
                // main_hand / one_hand
                {"talon_of_the_phoenix", Attributes{0.0, 0.0}, Special_stats{0.8605, 0.9512, 52.0, 0, 0.0}, 2.7, 182, 339, Weapon_socket::main_hand, Weapon_type::unarmed},
                // off_hand
                {"claw_of_the_phoenix", Attributes{0.0, 21.0}, Special_stats{0.0, 0.0, 40, 0, 0.0}, 1.5, 101, 189, Weapon_socket::off_hand, Weapon_type::unarmed},

                // Phase 3
                // main_hand / one_hand
                {"claw_of_molten_fury", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 38.0, 0, 0.0}, 2.7, 216, 325, Weapon_socket::main_hand, Weapon_type::unarmed, {}, Set::the_fists_of_fury},
                // off_hand
                {"fist_of_molten_fury", Attributes{0.0, 20.0}, Special_stats{0.0, 0.0, 38, 0, 0.0}, 1.5, 120, 181, Weapon_socket::off_hand, Weapon_type::unarmed, {}, Set::the_fists_of_fury},
            
                // Phase 4
                // main_hand / one_hand
                {"rage", Attributes{0.0, 20.0}, Special_stats{0.59, 0.0, 0, 0, 0.0}, 2.6, 174, 323, Weapon_socket::main_hand, Weapon_type::unarmed},
                // off_hand
                {"fury", Attributes{0.0, 0.0}, Special_stats{0.0, 1.27, 26, 0, 0.0}, 2.6, 174, 323, Weapon_socket::off_hand, Weapon_type::unarmed},

                // Phase 5
                // main_hand
                {"hand_of_the_deceiver", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 54, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 196}, 2.6, 238, 357, Weapon_socket::main_hand, Weapon_type::unarmed}, 
                {"vanirs_right_fist_of_brutality", Attributes{0.0, 23.0}, Special_stats{0.0, 0.0, 44, 0, 0.01329113924050633}, 2.5, 180, 335, Weapon_socket::main_hand, Weapon_type::unarmed},
                {"brutal_gladiators_right_ripper", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 196, 365, Weapon_socket::main_hand, Weapon_type::unarmed}, 
                // off_hand
                {"vanirs_left_fist_of_brutality", Attributes{0.0, 0.0}, Special_stats{0.0, 1.329113924050633, 44, 0, 0.01455696202531646}, 2.5, 180, 335, Weapon_socket::off_hand, Weapon_type::unarmed},
                {"vanirs_left_fist_of_savagery", Attributes{0.0, 0.0}, Special_stats{0.0, 1.329113924050633, 44, 0, 0.01455696202531646}, 1.5, 108, 201, Weapon_socket::off_hand, Weapon_type::unarmed},
                {"brutal_gladiators_left_ripper", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 1.5, 113, 211, Weapon_socket::off_hand, Weapon_type::unarmed}, 
                {"brutal_gladiators_left_render", Attributes{0.0, 0.0}, Special_stats{0.995475113122172, 0.569620253164557, 38, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 49}, 2.6, 196, 365, Weapon_socket::off_hand, Weapon_type::unarmed},
                {"mounting_vengeance", Attributes{0.0, 0.0}, Special_stats{0.0, 0.0, 46, 0, 0.0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 140}, 2.6, 196, 365, Weapon_socket::off_hand, Weapon_type::unarmed}, 
                {"grip_of_mannoroth", Attributes{0.0, 0.0}, Special_stats{0.0, 1.268230818, 46, 0, 0.01965757768}, 1.5, 113, 211, Weapon_socket::off_hand, Weapon_type::unarmed}, 
             };

    std::vector<Set_bonus> set_bonuses{
        {Set::ragesteel, 2, "ragesteel-2pc", {}, {0, 1.2683, 0}},
        {Set::wastewalker, 2, "wastewalker-2pc", {}, {0, 2.2195, 0}},
        {Set::wastewalker, 4, "wastewalker-4pc", {}, {}, {"wastewalker-4pc", Hit_effect::Type::stat_boost, {}, {0, 0, 160}, 0, 15, 0, 0.02}},
        {Set::doomplate, 2, "doomplate-2pc", {}, {0, 2.2195, 0}},
        {Set::doomplate, 4, "doomplate-4pc", {}, {}, {"doomplate-4pc", Hit_effect::Type::stat_boost, {}, {0, 0, 160}, 0, 15, 0, 0.02}},
        {Set::warbringer, 2, "warbringer-2pc (2t4)"},
        {Set::warbringer, 4, "warbringer-4pc (4t4)"},
        {Set::destroyer, 2, "destroyer-2pc (2t5)"},
        {Set::destroyer, 4, "destroyer-4pc (4t5)"},
        {Set::onslaught, 2, "onslaught-2pc (2t6)"},
        {Set::onslaught, 4, "onslaught-4pc (4t6)"},
        {Set::the_fists_of_fury, 2, "the_fists_of_fury", {}, {}, {"the_fists_of_fury", Hit_effect::Type::damage_magic, {}, {}, 125, 0, 0, 0, 0, 0, 0, 0, 2}},
        {Set::the_twin_blades_of_azzinoth, 2, "the_twin_blades_of_azzinoth", {}, {0, 0, 200}, {"the_twin_blades_of_azzinoth", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, 0.2853519341}, 0, 10, 45, 0, 0, 0, 0, 0, 1}},
        {Set::the_twin_blades_of_azzinoth_non_demon, 2, "the_twin_blades_of_azzinoth_non_demon", {}, {}, {"the_twin_blades_of_azzinoth_non_demon", Hit_effect::Type::stat_boost, {}, {0, 0, 0, 0, 0.2853519341}, 0, 10, 45, 0, 0, 0, 0, 0, 1}},
        
    };

    [[nodiscard]] const std::vector<Armor>& get_items_in_socket(Socket socket) const;

    [[nodiscard]] std::vector<Weapon> get_weapon_in_socket(Weapon_socket socket) const;

    [[nodiscard]] std::unordered_map<std::string, const Armor*> build_armor_index() const;
    [[nodiscard]] std::unordered_map<std::string, const Weapon*> build_weapons_index() const;

    [[nodiscard]] Armor find_armor(Socket socket, const std::string& name) const;

    [[nodiscard]] Weapon find_weapon(Weapon_socket weapon_socket, const std::string& name) const;

    [[nodiscard]] static Attributes get_enchant_attributes(Socket socket, Enchant::Type type);

    [[nodiscard]] static Special_stats get_enchant_special_stats(Socket socket, Enchant::Type type);

    [[nodiscard]] static Hit_effect enchant_hit_effect(Weapon &weapon, Enchant::Type type);

    void clean_weapon(Weapon& weapon) const;

    void compute_total_stats(Character& character) const;

    [[nodiscard]] static bool check_if_armor_valid(const std::vector<Armor>& armor);

    [[nodiscard]] static bool check_if_weapons_valid(std::vector<Weapon>& ws);

    static void change_weapon(std::vector<Weapon>& current_weapons, const Weapon& equip_weapon, const Socket& socket);

    static void change_armor(std::vector<Armor> &armor_vec, const Armor &armor, bool first_misc_slot = true);

    static void add_enchants_to_character(Character& character, const std::vector<std::string>& ench_vec);

    void add_gems_to_character(Character& character, const std::vector<std::string>& gem_vec) const;

    void add_buffs_to_character(Character& character, const std::vector<std::string>& buffs_vec) const;

    static void add_talents_to_character(Character& character, const std::vector<std::string>& talent_string,
                                  const std::vector<int>& talent_val);

    Buffs buffs;

    Gems gems;
};

#endif //WOW_SIMULATOR_ARMORY_HPP
