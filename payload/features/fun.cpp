// fun.cpp -- airstrike at waypoint, rain explosions over player, rain vehicles,
// orbital beam at target player, instant fill the sky with helis.
#include "../invoker/natives.h"
#include <Windows.h>
#include <random>
#include <string>

namespace ollie::state {
    extern int selected_player;
    extern bool rain_explosions_on_me;
    extern bool orbital_on_target;
}

static std::mt19937& rng() {
    static std::mt19937 r{ std::random_device{}() };
    return r;
}

namespace ollie::features {

    void airstrike_waypoint() {
        ollie::invoker::run_on_script([] {
            if (!N::IS_WAYPOINT_ACTIVE()) return;
            int blip = N::GET_FIRST_BLIP_INFO_ID(8);
            float x = 0, y = 0, z = 0;
            N::GET_BLIP_INFO_ID_COORD(blip, &x, &y, &z);
            std::uniform_real_distribution<float> jitter(-18.f, 18.f);
            for (int i = 0; i < 24; ++i) {
                float jx = jitter(rng()), jy = jitter(rng());
                N::ADD_EXPLOSION(x + jx, y + jy, z + 2.f, 7, 120.f, true, false, 1.f);
            }
        });
    }

    void orbital_on_target_once() {
        ollie::invoker::run_on_script([] {
            int pid = ollie::state::selected_player;
            if (pid < 0 || pid >= 32) return;
            if (!N::IS_PLAYER_ACTIVE(pid)) return;
            int ped = N::GET_PLAYER_PED(pid);
            if (!ped) return;
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            // single huge rocket from above + ground follow-up
            N::ADD_EXPLOSION(x, y, z + 1.f, 29, 300.f, true, false, 1.f);
            N::ADD_EXPLOSION(x, y, z,        7, 200.f, true, false, 1.f);
        });
    }

    void rain_vehicles() {
        ollie::invoker::run_on_script([] {
            auto h = N::GET_HASH_KEY("adder");
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 30 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            std::uniform_real_distribution<float> off(-25.f, 25.f);
            for (int i = 0; i < 12; ++i) {
                N::CREATE_VEHICLE(h, x + off(rng()), y + off(rng()), z + 80.f + i * 4.f, 0.f, true, true, false);
            }
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
        });
    }

    void spawn_heli_squad() {
        ollie::invoker::run_on_script([] {
            auto h = N::GET_HASH_KEY("buzzard");
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 30 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            for (int i = 0; i < 6; ++i) {
                int v = N::CREATE_VEHICLE(h, x + 6.f + i * 4.f, y + 20.f, z + 50.f + i * 5.f, 90.f, true, true, false);
                if (v) N::SET_HELI_BLADES_FULL_SPEED(v);
            }
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
        });
    }

    void tick_fun() {
        ollie::invoker::run_on_script([] {
            using namespace ollie::state;
            static int last_rain = 0;
            int now = N::GET_GAME_TIMER();

            if (rain_explosions_on_me && now - last_rain > 500) {
                last_rain = now;
                int ped = N::PLAYER_PED_ID();
                float x = 0, y = 0, z = 0;
                N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
                std::uniform_real_distribution<float> jitter(-14.f, 14.f);
                N::ADD_EXPLOSION(x + jitter(rng()), y + jitter(rng()), z + 2.f, 7, 80.f, true, false, 0.6f);
            }
            if (orbital_on_target) {
                static int last_orb = 0;
                if (now - last_orb > 1200) {
                    last_orb = now;
                    int pid = ollie::state::selected_player;
                    if (pid >= 0 && pid < 32 && N::IS_PLAYER_ACTIVE(pid)) {
                        int ped = N::GET_PLAYER_PED(pid);
                        if (ped) {
                            float x = 0, y = 0, z = 0;
                            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
                            N::SHOOT_SINGLE_BULLET_BETWEEN(x, y, z + 300.f, x, y, z, 1000, true,
                                                           N::GET_HASH_KEY("WEAPON_RAILGUN"),
                                                           N::PLAYER_PED_ID(), true, false, 10000.f);
                        }
                    }
                }
            }
        });
    }
}
