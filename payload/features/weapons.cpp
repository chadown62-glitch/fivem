// weapons.cpp -- no reload, no spread, no recoil (best-effort via infinite clip),
// explosive ammo / fire ammo / explosive melee this-frame toggles.
#include "../invoker/natives.h"
#include <string>

namespace ollie::state {
    extern bool no_reload;
    extern bool explosive_ammo;
    extern bool fire_ammo;
    extern bool explosive_melee;
    extern bool infinite_ammo;
}

namespace ollie::features {
    void give_weapon(const char* w) {
        ollie::invoker::run_on_script([wp = std::string(w)] {
            int ped = N::PLAYER_PED_ID();
            auto h = N::GET_HASH_KEY(wp.c_str());
            N::GIVE_WEAPON_TO_PED(ped, h, 9999, false, true);
        });
    }

    void remove_all_weapons() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            N::REMOVE_ALL_PED_WEAPONS(ped, true);
        });
    }

    void tick_weapons() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            if (ped == 0) return;
            int pid = N::PLAYER_ID();

            if (ollie::state::no_reload) {
                std::uint32_t cur = 0;
                if (N::GET_CURRENT_PED_WEAPON(ped, &cur, true) && cur) {
                    N::SET_AMMO_IN_CLIP(ped, cur, 250);
                }
            }
            if (ollie::state::infinite_ammo) {
                std::uint32_t cur = 0;
                if (N::GET_CURRENT_PED_WEAPON(ped, &cur, true) && cur) {
                    N::SET_PED_INFINITE_AMMO(ped, true, cur);
                }
                N::SET_PED_INFINITE_AMMO_CLIP(ped, true);
            }
            if (ollie::state::explosive_ammo)  N::SET_EXPLOSIVE_AMMO_THIS_FRAME(pid);
            if (ollie::state::fire_ammo)       N::SET_FIRE_AMMO_THIS_FRAME(pid);
            if (ollie::state::explosive_melee) N::SET_EXPLOSIVE_MELEE_THIS_FRAME(pid);
        });
    }
}
