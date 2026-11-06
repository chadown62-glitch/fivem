#include "../invoker/natives.h"
#include "../core/entities.h"
#include <Windows.h>
#include <string>

namespace ollie::state {
    extern int   world_hour;
    extern int   world_minute;
    extern bool  world_time_frozen;
    extern int   weather_index;
    extern bool  weather_locked;
    extern bool  weather_cycle_on;
    extern int   weather_cycle_sec;
}

static const char* weather_name(int idx) {
    static const char* n[] = {
        "EXTRASUNNY","CLEAR","CLOUDS","SMOG","FOGGY","OVERCAST",
        "RAIN","THUNDER","CLEARING","NEUTRAL","SNOW","BLIZZARD",
        "SNOWLIGHT","XMAS","HALLOWEEN"
    };
    if (idx < 0 || idx >= (int)(sizeof(n)/sizeof(n[0]))) return "CLEAR";
    return n[idx];
}

namespace ollie::features {
    void teleport_to(float x, float y, float z) {
        ollie::invoker::run_on_script([x, y, z] {
            int ped = N::PLAYER_PED_ID();
            N::SET_ENTITY_COORDS_NO_OFFSET(ped, x, y, z, false, false, false);
        });
    }

    void teleport_waypoint() {
        ollie::invoker::run_on_script([] {
            if (!N::IS_WAYPOINT_ACTIVE()) return;
            int blip = N::GET_FIRST_BLIP_INFO_ID(8);
            float x = 0, y = 0, z = 0;
            N::GET_BLIP_INFO_ID_COORD(blip, &x, &y, &z);
            int ped = N::PLAYER_PED_ID();
            N::SET_ENTITY_COORDS_NO_OFFSET(ped, x, y, z + 2.f, false, false, false);
        });
    }

    void spawn_ped(const char* model) {
        ollie::invoker::run_on_script([mm = std::string(model)] {
            auto h = N::GET_HASH_KEY(mm.c_str());
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 60 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            int p = N::CREATE_PED(4, h, x + 2.f, y, z, 0.f, true, true);
            ollie::entities::track(p);
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
        });
    }

    void clear_nearby_peds() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            N::CLEAR_AREA_OF_PEDS(x, y, z, 100.f, 0);
        });
    }

    void clear_nearby_vehicles() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            N::CLEAR_AREA_OF_VEHICLES(x, y, z, 100.f, false, false, false, false, false);
        });
    }

    void apply_weather_now() {
        ollie::invoker::run_on_script([] {
            const char* w = weather_name(ollie::state::weather_index);
            if (ollie::state::weather_locked) N::SET_WEATHER_TYPE_PERSIST(w);
            N::SET_WEATHER_TYPE_NOW(w);
        });
    }

    void tick_world() {
        ollie::invoker::run_on_script([] {
            if (ollie::state::world_time_frozen) {
                N::NETWORK_OVERRIDE_CLOCK_TIME(ollie::state::world_hour, ollie::state::world_minute, 0);
            }
            if (ollie::state::weather_cycle_on) {
                static int last = 0;
                int now = N::GET_GAME_TIMER();
                int step = (ollie::state::weather_cycle_sec > 0 ? ollie::state::weather_cycle_sec : 30) * 1000;
                if (now - last > step) {
                    last = now;
                    ollie::state::weather_index = (ollie::state::weather_index + 1) % 15;
                    N::SET_WEATHER_TYPE_NOW(weather_name(ollie::state::weather_index));
                }
            }
        });
    }
}
