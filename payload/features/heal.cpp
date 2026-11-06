// heal.cpp -- one-shot heal nearby players
#include "../invoker/natives.h"
#include <cmath>

namespace ollie::features {
    void heal_nearby(float radius) {
        ollie::invoker::run_on_script([radius] {
            int localPed = N::PLAYER_PED_ID();
            float lx = 0, ly = 0, lz = 0;
            N::GET_ENTITY_COORDS(localPed, true, &lx, &ly, &lz);
            for (int i = 0; i < 32; ++i) {
                if (!N::IS_PLAYER_ACTIVE(i)) continue;
                int ped = N::GET_PLAYER_PED(i);
                if (!ped) continue;
                float x = 0, y = 0, z = 0;
                N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
                float dx = x - lx, dy = y - ly, dz = z - lz;
                if (std::sqrt(dx*dx + dy*dy + dz*dz) > radius) continue;
                N::SET_ENTITY_HEALTH(ped, 200);
                N::SET_PED_ARMOUR(ped, 100);
            }
        });
    }
}
