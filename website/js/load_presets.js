function selectElement(id, valueToSelect) {
    if (id !== "" && id !== "undefined") {
        if (id.slice(id.length - 2, id.length) === "dd" ||
            id.slice(id.length - 4, id.length) === "mult") {
            try {
                document.getElementById(id).value = valueToSelect;
            } catch (err) {
                console.log(err);
            }
        } else if (id.slice(id.length - 6, id.length) === "talent") {
            document.getElementById(id).setAttribute("data-count", valueToSelect)
        } else if (valueToSelect === "true" || valueToSelect === "false") {
            console.log("asd");
            let all_dropdowns = armor_mult.concat(weapons_mult);
            for (let dropdown of all_dropdowns) {
                let select_menu = document.getElementById(dropdown);
                for (let i = 0; i < select_menu.length; i++) {
                    let item = select_menu[i];
                    if (item.value === id) {
                        (valueToSelect === "true") ? item.selected = true : item.selected = false;
                    }
                }
            }
        } else {
            if (id === valueToSelect) {
                try {
                    document.getElementById(id).checked = true;
                } catch (err) {
                    console.log(err);
                }
            }
        }
    }
}

function select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems) {
    for (let i = 0; i < armor.length; i++) {
        selectElement(armor[i], selected_items[i])
    }
    for (let i = 0; i < weapons.length; i++) {
        selectElement(weapons[i], selected_weapons[i])
    }
    for (let i = 0; i < enchants.length; i++) {
        selectElement(enchants[i], selected_enchants[i])
    }
    for (let i = 0; i < gems.length; i++) {
        selectElement(gems[i], selected_gems[i])
    } 
}

function select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems) {
    for (let option_name of armor_mult) {
        console.log(option_name);
        let select_option = document.getElementById(option_name);
        for (let option of select_option.options) {
            for (let item of selected_items) {
                if (option.value === item) {
                    option.selected = true;
                    break;
                } else {
                    option.selected = false;
                }
            }
        }
    }

    for (let i =0; i < weapons_mult.length; i++)
    {
        let select_option = document.getElementById(weapons_mult[i]);
        console.log(select_option.options.length);
        for (let option of select_option.options) {
            for (let item of selected_weapons) {
                if (option.value === item) {
                    option.selected = true;
                    break;
                } else {
                    option.selected = false;
                }
            }
        }
    }
    
    for (let i = 0; i < enchants.length; i++) {
        selectElement(enchants[i], selected_enchants[i])
    }
    for (let i = 0; i < gems.length; i++) {
        selectElement(gems[i], selected_gems[i])
    }
}


function load_naxxgear() {
    let selected_items = ["lionheart_helm", "stormrages_talisman_of_seething", "conquerors_spaulders",
        "shroud_of_dominion", "plated_abomination_ribcage", "hive_defiler_wristguards",
        "gauntlets_of_annihilation", "girdle_of_the_mentor", "legplates_of_carnage",
        "chromatic_boots", "quick_strike_ring", "band_of_unnatural_forces",
        "kiss_of_the_spider", "badge_of_the_swarmguard", "nerubian_slavemaker"];
    let selected_weapons = ["gressil_dawn_of_ruin", "the_hungering_cold", "might_of_menethil"];
    let selected_enchants = ["+8 strength", "naxxramas", "+3 agility", "+4 stats", "+9 strength",
        "+15 agility", "+8 strength", "+7 agility", "crusader", "crusader", "ring_1", "ring_2"];
    let selected_gems = ["helmet_gem1_dd", "helmet_gem2_dd", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "shoulder_gem1_dd", "shoulder_gem2_dd", "shoulder_gem3_dd", "back_gem1_dd", "back_gem2_dd", "back_gem3_dd",
    "chest_gem1_dd", "chest_gem2_dd", "chest_gem3_dd", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "wrists_gem1_dd", "wrists_gem2_dd", "wrists_gem3_dd", "belt_gem1_dd", "belt_gem2_dd", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
}

function load_preraidbis() {
    let selected_items = ["mask_of_the_deceiver", "choker_of_vile_intent", "ragesteel_shoulders",
        "vengeance_wrap", "gladiators_plate_chestpiece", "black_felsteel_bracers",
        "fel_leather_gloves", "deathforge_girdle", "midnight_legguards",
        "fel_leather_boots", "band_of_unnatural_forces", "ring_of_arathi_warlords",
        "bloodlust_brooch", "hourglass_of_the_unraveller", "mamas_insurance"];
    let selected_weapons = ["dragonmaw", "gladiators_slicer", "lionheart_champion"];
    let selected_enchants = ["ferocity", "greater_vengeance", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+8 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "chest_gem1_dd", "chest_gem2_dd", "chest_gem3_dd", "+4 crit_+4_str", "+4 crit_+4_str", "+4 crit", "wrists_gem1_dd", "wrists_gem2_dd", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
}

function load_p1armsbis() {
    let selected_items = ["warbringer_battle-helm", "adamantite_chain_of_the_unbroken", "warbringer_shoulderplates",
        "vengeance_wrap", "warbringer_breastplate", "bladespire_warbands",
        "gauntlets_of_martial_perfection", "girdle_of_the_endless_pit", "skulkers_greaves",
        "ironstriders_of_urgency", "ring_of_a_thousand_marks", "mithril_band_of_the_unscarred",
        "bloodlust_brooch", "hourglass_of_the_unraveller", "mamas_insurance"];
    let selected_weapons = ["none", "none", "lionheart_champion"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+4 crit", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
    unique_checkboxes('two_hand_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['two_hand_div'],['dual_wield_div']);
}

function load_p1furybis() {
    let selected_items = ["warbringer_battle-helm", "choker_of_vile_intent", "warbringer_shoulderplates",
        "vengeance_wrap", "warbringer_breastplate", "bladespire_warbands",
        "gauntlets_of_martial_perfection", "girdle_of_the_endless_pit", "skulkers_greaves",
        "ironstriders_of_urgency", "ring_of_a_thousand_marks", "shapeshifters_signet",
        "bloodlust_brooch", "dragonspine_trophy", "mamas_insurance"];
    let selected_weapons = ["dragonmaw", "spiteblade"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+4 crit", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
    unique_checkboxes('dual_wield_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['dual_wield_div'],['two_hand_div']);
}

function load_p2armsbisplate() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "destroyer_shoulderblades",
        "vengeance_wrap", "bulwark_of_ancient_kings_no_cd", "bladespire_warbands",
        "destroyer_gauntlets", "belt_of_one_hundred_deaths", "destroyer_greaves",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["none", "none", "twinblade_of_the_phoenix"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit_+4_str", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+8 crit", "+4 crit_+4_str", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "+8 crit", "+8 crit", "+8 crit", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
    unique_checkboxes('two_hand_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['two_hand_div'],['dual_wield_div']);
}

function load_p2armsbisleather() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "shoulderpads_of_the_stranger",
        "vengeance_wrap", "bulwark_of_ancient_kings_no_cd", "bladespire_warbands",
        "gloves_of_the_searing_grip", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["none", "none", "twinblade_of_the_phoenix"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "shoulder_gem2_dd", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+8 crit", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "+8 crit", "+8 crit", "+8 crit", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
    unique_checkboxes('two_hand_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['two_hand_div'],['dual_wield_div']);
}

function load_p2furybisplate() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "destroyer_shoulderblades",
        "vengeance_wrap", "destroyer_breastplate", "bladespire_warbands",
        "destroyer_gauntlets", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_a_thousand_marks",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["dragonstrike", "talon_of_azshara"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit_+4_str", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+4 crit_+4_str", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
    unique_checkboxes('dual_wield_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['dual_wield_div'],['two_hand_div']);
}

function load_p2furybisleather() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "shoulderpads_of_the_stranger",
        "vengeance_wrap", "destroyer_breastplate", "bracers_of_eradication",
        "gloves_of_the_searing_grip", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["dragonstrike", "merciless_gladiators_slicer"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+4 crit_+4_str", "shoulder_gem2_dd","shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "wrists_gem2_dd", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_single(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
    unique_checkboxes('dual_wield_mode', ['dual_wield_mode', 'two_hand_mode']);
                        unique_div(['dual_wield_div'],['two_hand_div']);
}

function load_naxxgearmult() {
    let selected_items = ["lionheart_helm", "stormrages_talisman_of_seething", "conquerors_spaulders",
    "shroud_of_dominion", "plated_abomination_ribcage", "hive_defiler_wristguards",
    "gauntlets_of_annihilation", "girdle_of_the_mentor", "legplates_of_carnage",
    "chromatic_boots", "quick_strike_ring", "band_of_unnatural_forces",
    "kiss_of_the_spider", "badge_of_the_swarmguard", "nerubian_slavemaker"];
    let selected_weapons = ["gressil_dawn_of_ruin", "the_hungering_cold"];
    let selected_enchants = ["+8 strength", "naxxramas", "+3 agility", "+4 stats", "+9 strength",
    "+15 agility", "+8 strength", "+7 agility", "crusader", "crusader", "ring_1", "ring_2"];
    let selected_gems = ["helmet_gem1_dd", "helmet_gem2_dd", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "shoulder_gem1_dd", "shoulder_gem2_dd", "shoulder_gem3_dd", "back_gem1_dd", "back_gem2_dd", "back_gem3_dd",
    "chest_gem1_dd", "chest_gem2_dd", "chest_gem3_dd", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "wrists_gem1_dd", "wrists_gem2_dd", "wrists_gem3_dd", "belt_gem1_dd", "belt_gem2_dd", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
}

function load_preraidbismult() {
    let selected_items = ["mask_of_the_deceiver", "choker_of_vile_intent", "ragesteel_shoulders",
        "vengeance_wrap", "gladiators_plate_chestpiece", "black_felsteel_bracers",
        "fel_leather_gloves", "deathforge_girdle", "midnight_legguards",
        "fel_leather_boots", "band_of_unnatural_forces", "ring_of_arathi_warlords",
        "bloodlust_brooch", "hourglass_of_the_unraveller", "mamas_insurance"];
    let selected_weapons = ["dragonmaw", "gladiators_slicer", "lionheart_champion"];
    let selected_enchants = ["ferocity", "greater_vengeance", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+8 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "chest_gem1_dd", "chest_gem2_dd", "chest_gem3_dd", "+4 crit_+4_str", "+4 crit_+4_str", "+4 crit", "wrists_gem1_dd", "wrists_gem2_dd", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
}

function load_p1armsbismult() {
    let selected_items = ["warbringer_battle-helm", "adamantite_chain_of_the_unbroken", "warbringer_shoulderplates",
        "vengeance_wrap", "warbringer_breastplate", "bladespire_warbands",
        "gauntlets_of_martial_perfection", "girdle_of_the_endless_pit", "skulkers_greaves",
        "ironstriders_of_urgency", "ring_of_a_thousand_marks", "mithril_band_of_the_unscarred",
        "bloodlust_brooch", "hourglass_of_the_unraveller", "mamas_insurance"];
    let selected_weapons = ["none", "none", "lionheart_champion"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+4 crit", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
}

function load_p1furybismult() {
    let selected_items = ["warbringer_battle-helm", "choker_of_vile_intent", "warbringer_shoulderplates",
        "vengeance_wrap", "warbringer_breastplate", "bladespire_warbands",
        "gauntlets_of_martial_perfection", "girdle_of_the_endless_pit", "skulkers_greaves",
        "ironstriders_of_urgency", "ring_of_a_thousand_marks", "shapeshifters_signet",
        "bloodlust_brooch", "dragonspine_trophy", "mamas_insurance"];
    let selected_weapons = ["dragonmaw", "spiteblade"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+8 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+8 crit", "+4 crit", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+4 crit_+4_str", "+4 crit", "belt_gem3_dd", "+8 crit", "+4 crit_+4_str", "hands_gem3_dd", 
    "+8 crit", "+4 crit_+4_str", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
}

function load_p2armsbisplatemult() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "destroyer_shoulderblades",
        "vengeance_wrap", "bulwark_of_ancient_kings_no_cd", "bladespire_warbands",
        "destroyer_gauntlets", "belt_of_one_hundred_deaths", "destroyer_greaves",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["none", "none", "twinblade_of_the_phoenix"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit_+4_str", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+8 crit", "+4 crit_+4_str", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "+8 crit", "+8 crit", "+8 crit", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
}

function load_p2armsbisleathermult() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "shoulderpads_of_the_stranger",
        "vengeance_wrap", "bulwark_of_ancient_kings_no_cd", "bladespire_warbands",
        "gloves_of_the_searing_grip", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["none", "none", "twinblade_of_the_phoenix"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "off_hand", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "shoulder_gem2_dd", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+8 crit", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "+8 crit", "+8 crit", "+8 crit", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_standard_arms();
}

function load_p2furybisplatemult() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "destroyer_shoulderblades",
        "vengeance_wrap", "destroyer_breastplate", "bladespire_warbands",
        "destroyer_gauntlets", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_a_thousand_marks",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["dragonstrike", "talon_of_azshara"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+8 crit", "+4 crit_+4_str", "shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+4 crit", "+8 crit", "+4 crit_+4_str", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "+4 crit_+4_str", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
}

function load_p2furybisleathermult() {
    let selected_items = ["destroyer_battlehelm", "pendant_of_the_perilous", "shoulderpads_of_the_stranger",
        "vengeance_wrap", "destroyer_breastplate", "bracers_of_eradication",
        "gloves_of_the_searing_grip", "belt_of_one_hundred_deaths", "leggings_of_murderous_intent",
        "warboots_of_obliteration", "band_of_the_ranger_general", "ring_of_reciprocity",
        "dragonspine_trophy", "tsunami_talisman", "serpent_spine_longbow"];
    let selected_weapons = ["dragonstrike", "merciless_gladiators_slicer"];
    let selected_enchants = ["ferocity", "naxxramas", "+12 agility", "+6 stats", "+12 strength",
        "+15 strength", "nethercobra", "cats_swiftness", "mongoose", "mongoose", "ring_1", "ring_2"];
    let selected_gems = ["+4 crit", "agi critDmg", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "+4 crit_+4_str", "shoulder_gem2_dd","shoulder_gem3_dd", "+4 crit_+4_str", "back_gem2_dd", "back_gem3_dd",
    "+8 crit", "+8 crit", "+8 crit", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "+4 crit", "wrists_gem2_dd", "wrists_gem3_dd", "+8 crit", "+8 crit", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
    "boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
    "ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
    select_loadout_mult(selected_items, selected_weapons, selected_enchants, selected_gems);
    load_talent_dual_wield_fury();
}
