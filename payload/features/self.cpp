#include "../invoker/natives.h"

namespace ollie::state {
    extern bool godmode, infinite_ammo, never_wanted, super_jump, invisible;
    extern bool no_ragdoll, no_knock_off, breath_hold, super_run;
}

namespace ollie::features {
    void tick_weapons();

    void tick_self() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            if (ped == 0) return;
            using namespace ollie::state;
            int pid = N::PLAYER_ID();

            if (godmode)       N::SET_ENTITY_INVINCIBLE(ped, true);
            if (infinite_ammo) N::SET_PED_INFINITE_AMMO_CLIP(ped, true);
            if (super_jump)    N::SET_SUPER_JUMP_THIS_FRAME(pid);
            if (never_wanted) {
                N::SET_MAX_WANTED_LEVEL(0);
                N::SET_PLAYER_WANTED_LEVEL(pid, 0, false);
                N::SET_PLAYER_WANTED_LEVEL_NOW(pid, false);
            }
            if (invisible) N::SET_ENTITY_VISIBLE(ped, false, false);
            if (no_ragdoll)   N::SET_PED_CAN_RAGDOLL(ped, false);
            if (no_knock_off) N::SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(ped, 1);
            if (breath_hold)  N::SET_PED_RESET_FLAG(ped, 134, true); // infinite breath
            if (super_run)    N::SET_PED_RESET_FLAG(ped, 127, true); // disable sprint cooldown
        });
        tick_weapons();
    }
}
