template <typename T>
void Combat_simulator_config::parse_combat_simulator_config(const T& input)
{
    Find_values<double> fv(input.float_options_string, input.float_options_val);

    // n_batches - set from e.g. n_simulations_talent_dd

    // combat_debug - special run mode "debug on"
    // seed - only used in multi, at the moment

    sim_time = fv.find("fight_time_dd"); // TODO(vigo) probably convert to millis as well - but this is kinda infiltrative

    main_target_level = fv.find("opponent_level_dd");
    main_target_initial_armor_ = fv.find("boss_armor_dd");

    n_sunder_armor_stacks = fv.find("sunder_armor_dd");
    exposed_armor = String_helpers::find_string(input.options, "exposed_armor");
    curse_of_recklessness_active = String_helpers::find_string(input.options, "curse_of_recklessness");
    faerie_fire_feral_active = String_helpers::find_string(input.options, "faerie_fire");

    multi_target_mode_ = String_helpers::find_string(input.options, "multi_target_mode");
    number_of_extra_targets = fv.find("number_of_extra_targets_dd");
    extra_target_percentage = fv.find("extra_target_percentage_dd", 100); // renamed from extra_target_duration
    extra_target_initial_armor_ = fv.find("extra_target_armor_dd");
    // extra_target_level isn't currently supported (it would require a second set of hit tables)

    take_periodic_damage_ = String_helpers::find_string(input.options, "periodic_damage");
    periodic_damage_amount_ = fv.find("periodic_damage_amount_dd");
    periodic_damage_interval_ = fv.find("periodic_damage_interval_dd");
    essence_of_the_red_ = String_helpers::find_string(input.options, "essence_of_the_red");

    execute_phase_percentage_ = fv.find("execute_phase_percentage_dd");

    initial_rage = fv.find("initial_rage_dd");
    sunder_armor_globals_ = fv.find("sunder_armor_globals_dd", 0);

    enable_bloodrage = true;
    enable_recklessness = String_helpers::find_string(input.options, "recklessness");
    enable_blood_fury = String_helpers::find_string(input.options, "enable_blood_fury");
    enable_berserking = String_helpers::find_string(input.options, "enable_berserking");
    berserking_haste_ = fv.find("berserking_haste_dd");
    use_death_wish = String_helpers::find_string(input.options, "death_wish");
    use_sweeping_strikes = String_helpers::find_string(input.options, "use_sweeping_strikes");

    enable_unleashed_rage = String_helpers::find_string(input.options, "enable_unleashed_rage");
    unleashed_rage_start_ = fv.find("unleashed_rage_dd");

    deep_wounds = String_helpers::find_string(input.options, "deep_wounds");

    combat.use_bloodthirst = String_helpers::find_string(input.options, "use_bloodthirst");
    combat.use_bt_in_exec_phase = String_helpers::find_string(input.options, "use_bt_in_exec_phase");
    combat.bt_whirlwind_cooldown_thresh = to_millis(fv.find("bt_whirlwind_cooldown_thresh_dd"));

    combat.use_mortal_strike = String_helpers::find_string(input.options, "use_mortal_strike");
    combat.use_ms_in_exec_phase = String_helpers::find_string(input.options, "use_ms_in_exec_phase");
    combat.ms_whirlwind_cooldown_thresh = to_millis(fv.find("ms_whirlwind_cooldown_thresh_dd"));

    combat.use_whirlwind = String_helpers::find_string(input.options, "use_whirlwind");
    combat.use_ww_in_exec_phase = String_helpers::find_string(input.options, "use_ww_in_exec_phase");
    combat.whirlwind_rage_thresh = fv.find("whirlwind_rage_thresh_dd");
    combat.whirlwind_bt_cooldown_thresh = to_millis(fv.find("whirlwind_bt_cooldown_thresh_dd"));

    combat.use_slam = String_helpers::find_string(input.options, "use_slam");
    combat.use_sl_in_exec_phase = String_helpers::find_string(input.options, "use_sl_in_exec_phase");
    combat.slam_rage_thresh = fv.find("slam_rage_thresh_dd", 0); // renamed from slam_rage
    combat.slam_spam_max_time = to_millis(fv.find("slam_spam_max_time_dd"));
    combat.slam_spam_rage = fv.find("slam_spam_rage_dd");
    combat.slam_latency = to_millis(fv.find("slam_latency_dd"));

    combat.use_rampage = String_helpers::find_string(input.options, "use_rampage");
    combat.rampage_use_thresh = to_millis(fv.find("rampage_use_thresh_dd"));

    combat.use_heroic_strike = String_helpers::find_string(input.options, "use_heroic_strike");
    combat.use_hs_in_exec_phase = String_helpers::find_string(input.options, "use_hs_in_exec_phase");
    combat.first_hit_heroic_strike = String_helpers::find_string(input.options, "first_hit_heroic_strike");
    combat.heroic_strike_rage_thresh = fv.find("heroic_strike_rage_thresh_dd");

    combat.cleave_if_adds = String_helpers::find_string(input.options, "cleave_if_adds");
    combat.cleave_rage_thresh = fv.find("cleave_rage_thresh_dd");

    combat.use_overpower = String_helpers::find_string(input.options, "use_overpower");
    combat.overpower_rage_thresh = fv.find("overpower_rage_thresh_dd");
    combat.overpower_bt_cooldown_thresh = to_millis(fv.find("overpower_bt_cooldown_thresh_dd"));
    combat.overpower_ww_cooldown_thresh = to_millis(fv.find("overpower_ww_cooldown_thresh_dd"));

    combat.use_hamstring = String_helpers::find_string(input.options, "use_hamstring");
    combat.hamstring_rage_thresh = fv.find("hamstring_rage_thresh_dd", 70); // renamed from hamstring_thresh
    combat.hamstring_cd_thresh = to_millis(fv.find("hamstring_cd_thresh_dd"));
    combat.dont_use_hm_when_ss = String_helpers::find_string(input.options, "dont_use_hm_when_ss");

    combat.use_sunder_armor = String_helpers::find_string(input.options, "use_sunder_armor");
    combat.sunder_armor_rage_thresh = fv.find("sunder_armor_rage_thresh_dd", 0);
    combat.sunder_armor_cd_thresh = to_millis(fv.find("sunder_armor_cd_thresh_dd"));
}