// doors.cpp -- open / close / lock current vehicle's doors
#include "../invoker/natives.h"

namespace ollie::features {
    static int my_veh() {
        int ped = N::PLAYER_PED_ID();
        if (!N::IS_PED_IN_ANY_VEHICLE(ped, false)) return 0;
        return N::GET_VEHICLE_PED_IS_IN(ped, false);
    }

    void doors_open_all() {
        ollie::invoker::run_on_script([] {
            int v = my_veh();
            if (!v) return;
            for (int i = 0; i < 6; ++i) N::SET_VEHICLE_DOOR_OPEN(v, i, false, false);
        });
    }

    void doors_shut_all() {
        ollie::invoker::run_on_script([] {
            int v = my_veh();
            if (!v) return;
            for (int i = 0; i < 6; ++i) N::SET_VEHICLE_DOOR_SHUT(v, i, true);
        });
    }

    void doors_lock() {
        ollie::invoker::run_on_script([] {
            int v = my_veh();
            if (!v) return;
            N::SET_VEHICLE_DOORS_LOCKED(v, 2); // 2 = locked
        });
    }

    void doors_unlock() {
        ollie::invoker::run_on_script([] {
            int v = my_veh();
            if (!v) return;
            N::SET_VEHICLE_DOORS_LOCKED(v, 1); // 1 = unlocked
        });
    }
}
