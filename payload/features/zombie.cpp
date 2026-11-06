// zombie.cpp -- spawn hostile peds around local, they fight the local player
#include "../invoker/natives.h"
#include "../core/entities.h"
#include "../core/notify.h"
#include <Windows.h>
#include <random>
#include <string>

static std::mt19937& rng() {
    static std::mt19937 r{ std::random_device{}() };
    return r;
}

namespace ollie::features {
    void spawn_zombie_swarm(int count, const char* model) {
        ollie::invoker::run_on_script([count, mm = std::string(model)] {
            auto h = N::GET_HASH_KEY(mm.c_str());
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 60 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            if (!N::HAS_MODEL_LOADED(h)) {
                ollie::notify::push("zombie: model not loaded");
                return;
            }
            int localPed = N::PLAYER_PED_ID();
            float lx = 0, ly = 0, lz = 0;
            N::GET_ENTITY_COORDS(localPed, true, &lx, &ly, &lz);
            std::uniform_real_distribution<float> off(-18.f, 18.f);
            for (int i = 0; i < count; ++i) {
                int p = N::CREATE_PED(4, h, lx + off(rng()), ly + off(rng()), lz, 0.f, true, true);
                if (!p) continue;
                ollie::entities::track(p);
                // 46 = always combat attack
                N::SET_PED_COMBAT_ATTRIBUTES(p, 46, true);
                N::SET_PED_AS_ENEMY(p, true);
                N::GIVE_WEAPON_TO_PED(p, N::GET_HASH_KEY("WEAPON_KNIFE"), 1, false, true);
                N::TASK_COMBAT_PED(p, localPed, 0, 16);
            }
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
            ollie::notify::push("zombie swarm spawned");
        });
    }
}
