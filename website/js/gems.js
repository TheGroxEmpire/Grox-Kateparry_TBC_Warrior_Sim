let socket_total = [];
let socket_in_item = [];
let item_with_gems = ["helmet_dd", "neck_dd", "shoulder_dd", "back_dd",
"chest_dd", "wrists_dd", "belt_dd", "legs_dd", "hands_dd",
"boots_dd", "main_hand_dd", "off_hand_dd", "ranged_dd"];
let gems = ["helmet_gem1_dd", "helmet_gem2_dd", "helmet_gem3_dd", "neck_gem1_dd", "neck_gem2_dd", "neck_gem3_dd", "shoulder_gem1_dd", "shoulder_gem2_dd", "shoulder_gem3_dd", "back_gem1_dd", "back_gem2_dd", "back_gem3_dd",
"chest_gem1_dd", "chest_gem2_dd", "chest_gem3_dd", "legs_gem1_dd", "legs_gem2_dd", "legs_gem3_dd", "wrists_gem1_dd", "wrists_gem2_dd", "wrists_gem3_dd", "belt_gem1_dd", "belt_gem2_dd", "belt_gem3_dd", "hands_gem1_dd", "hands_gem2_dd", "hands_gem3_dd", 
"boots_gem1_dd", "boots_gem2_dd", "boots_gem3_dd",  "main_hand_gem1_dd", "main_hand_gem2_dd", "main_hand_gem3_dd", "off_hand_gem1_dd", "off_hand_gem2_dd", "off_hand_gem3_dd", 
"ranged_gem1_dd", "ranged_gem2_dd", "ranged_gem3_dd"];
let gem_option = {meta: ["meta", "+3 dmg", "+12 crit", "agi critDmg", "haste proc",
                        "none"], 
                  red: ["red", "+4 strength", "+6 strength", "+8 strength", "+4 agility", "+6 agility", "+10 agility", "+20 AP", "+24 AP",
                 /*orange*/ "orange", "+3 crit_+3_str", "+4 crit_+4_str", "+4 crit_+5_str", "+5 crit_+5_str",
                /*purple*/ "purple", "+3 strength", "+5 strength", "+3 agility", "+5 agility",
                        "none"], 
                  yellow: ["yellow", "+4 crit", "+6 crit", "+8 crit", "+10 crit", "+12 crit", "+4 hit", "+6 hit", "+8 hit", "+10 hit", "+12 hit",
                /*orange*/ "orange", "+3 crit_+3_str", "+4 crit_+4_str", "+4 crit_+5_str", "+5 crit_+5_str",
                /*green*/ "green", "+3 crit", "+5 crit",
                        "none"], 
                  blue: [
                /*purple*/ "purple", "+3 strength", "+5 strength", "+3 agility", "+5 agility",
                /*green*/ "green", "+3 crit", "+5 crit",
                        "none"],
                  none: ["none"],
                  empty: ["empty"]};
let gem_list = ["red", "+4 strength", "+6 strength", "+8 strength", "+4 agility", "+6 agility", "+10 agility", "+20 AP", "+24 AP",  
                "yellow", "+4 crit", "+6 crit", "+8 crit", "+10 crit", "+12 crit", "+4 hit", "+6 hit", "+8 hit", "+10 hit", "+12 hit",
                "meta", "+3 dmg", "+12 crit", "agi critDmg", "+20 AP", "+24 AP", "haste proc",
                "purple", "+3 strength", "+5 strength", "+3 agility", "+5 agility",
                "orange", "+3 crit_+3_str", "+4 crit_+4_str", "+4 crit_+5_str", "+5 crit_+5_str",
                "green", "+3 crit", "+5 crit",
                "empty"];
let gem_type = [];
let gems_input = [];
let item_gem_socket = {
                        //item socket begin
                        // lionheart_helm: ["meta", "yellow", "blue"],
                        // stormrages_talisman_of_seething: ["red", "yellow", "blue"],
                        // conquerors_spaulders: ["red", "yellow", "blue"],
                        // shroud_of_dominion: ["red", "yellow", "blue"],
                        // plated_abomination_ribcage: ["red", "yellow", "blue"],
                        // hive_defiler_wristguards: ["red", "yellow", "blue"],
                        // gauntlets_of_annihilation: ["red", "yellow", "blue"],
                        // girdle_of_the_mentor: ["red", "yellow", "blue"],
                        // leggings_of_carnage: ["red", "yellow", "blue"],
                        // chromatic_boots: ["red", "yellow", "blue"],
                        // nerubian_slavemaker: ["red", "yellow", "blue"],
                        // the_hungering_cold: ["red", "yellow", "blue"],
                        // iblis_blade_of_the_fallen_seraph: ["red", "yellow", "blue"],
                        //item socket end
                        empty: ["empty", "empty", "empty"]};
                        
let select_copy = [];


function gem_select(){
    socket_total = [];
    temp = [];
    num2 = [];
    num1 = [];
    gem_type = [];
    
    item_with_gems.forEach(iterate_gem_select);
    socket_total = [].concat(...socket_total);
    //restore gems option
    gems.forEach((num1, index) => {
        num2 = gem_list;
        if (index == 0){
            num2.push("none");
        }
        console.log("Num1: ", num1, " Num2: ", num2);
        showOptions(num1, num2);
    });
    //delete unavailable gem option
    gems.forEach((num1, index) => {
        if (index == 0){
            num2 = socket_total;
        }
        temp = num2.splice(0, 1);
        //console.log("Num1: ", num1, " Num2: ", num2);
        //generate_gem_select(num1, num2);
        hideOptions(num1, temp);
    });

}

function iterate_gem_select(selectObject){
    //console.log("Before change selectObject: ", selectObject)
    selectObject = document.getElementById(selectObject).value;
    //console.log("After change selectObject: ", selectObject)
    socket_in_item = [];
    if (item_gem_socket.hasOwnProperty(selectObject)){
        socket_in_item = item_gem_socket[selectObject];
    }
    
    for (let i = 3 - socket_in_item.length; i > 0; i--){
        socket_in_item.push("empty");
    }
    socket_total.push(socket_in_item);
    //console.log("Socket in items: ", socket_in_item);
}

/* function generate_gem_select(selectObject, gem_type){
    let select = document.getElementById(selectObject);
    gem_type = gem_option[gem_type];
    //console.log("Select: ", select)
    //console.log("Gem type: ", gem_type);
    for (let i = 0; i < select.options.length; i++){
        let label = select.options[i].innerHTML.trim();
        //console.log("Label: ", label);
        if(gem_type.indexOf(label) < 0){
            //selectObject[selectObject.options[i]].setAttribute('hidden','true');
            select.removeChild(select.options[i]);
            i--;
        }
    }
}
 */
function hideOptions(selectObject, array) {
    console.log("selectObject: ", selectObject, "Array: ", array);
    array = gem_option[array];
    console.log("Array gem option: ", array);
    array = gem_list.filter( ( el ) => !array.includes( el ) );
    for (var i = 0; i < array.length; i++) {
        $('#'+selectObject+' option[value="' + array[i] + '"]').hide();
    }
}

function showOptions(selectObject, array) {
    for (var i = 0; i < array.length; i++) {
        $('#'+selectObject+' option[value="' + array[i] + '"]').show();
    }
}