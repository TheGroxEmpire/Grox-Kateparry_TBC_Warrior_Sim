#ifndef WOW_SIMULATOR_ITEM_OPTIMIZER_HPP
#define WOW_SIMULATOR_ITEM_OPTIMIZER_HPP

#include "Armory.hpp"

class Item_optimizer
{
public:
    static bool no_weapons(const Weapon&) { return true; }
    static bool no_armors(const Armor&) { return true; }

    static std::vector<Weapon> remove_weaker_weapons(Weapon_socket weapon_socket, const std::vector<Weapon>& weapon_vec,
                                              const Special_stats& special_stats, std::string& debug_message,
                                              int keep_n_stronger_items, const std::function<bool(const Weapon&)>& filter = no_weapons);

    static std::vector<Armor> remove_weaker_items(const std::vector<Armor>& armors, const Special_stats& special_stats,
                                           std::string& debug_message, int keep_n_stronger_items, const std::function<bool(const Armor&)>& filter = no_armors);
};

#endif // WOW_SIMULATOR_ITEM_OPTIMIZER_HPP
