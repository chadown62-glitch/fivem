#include "../invoker/natives.h"
#include <Windows.h>

namespace ollie::state {
    extern int selected_player;
}

namespace ollie::features {

    static bool valid_target(int pid) {
        if (pid < 0 || pid >= 32) return false;
        if (pid == N::PLAYER_ID()) return false;
        if (!N::IS_PLAYER_ACTIVE(pid)) return false;
        return true;
    }

    void player_bring() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(N::PLAYER_PED_ID(), true, &x, &y, &z);
            N::SET_ENTITY_COORDS_NO_OFFSET(targetPed, x + 1.5f, y, z, false, false, false);
        });
    }

    void player_teleport_to() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(targetPed, true, &x, &y, &z);
            N::SET_ENTITY_COORDS_NO_OFFSET(N::PLAYER_PED_ID(), x + 1.5f, y, z, false, false, false);
        });
    }

    void player_teleport_into_vehicle() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            if (!N::IS_PED_IN_ANY_VEHICLE(targetPed, false)) return;
            int v = N::GET_VEHICLE_PED_IS_IN(targetPed, false);
            if (!v) return;
            // jam into any free seat starting from passenger
            for (int seat = 0; seat < 6; ++seat) {
                int occ = N::GET_PED_IN_VEHICLE_SEAT(v, seat);
                if (!occ) { N::SET_PED_INTO_VEHICLE(N::PLAYER_PED_ID(), v, seat); return; }
            }
            N::SET_PED_INTO_VEHICLE(N::PLAYER_PED_ID(), v, 0);
        });
    }

    void player_explode() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(targetPed, true, &x, &y, &z);
            N::ADD_EXPLOSION(x, y, z, 7, 100.f, true, false, 1.f);
        });
    }

    void player_slingshot() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            N::APPLY_FORCE_TO_ENTITY(targetPed, 1, 0, 0, 2500.f, 0, 0, 0, 0, false, true, true, false, true);
        });
    }

    void player_freeze_toggle() {
        static bool state = false;
        state = !state;
        ollie::invoker::run_on_script([s = state] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            N::FREEZE_ENTITY_POSITION(targetPed, s);
        });
    }

    void player_kill() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (!valid_target(pid)) return;
            int targetPed = N::GET_PLAYER_PED(pid);
            if (!targetPed) return;
            N::SET_ENTITY_HEALTH(targetPed, 0);
        });
    }

    void tick_players() {}
}
