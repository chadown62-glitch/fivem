#include "../invoker/natives.h"
#include "../core/entities.h"
#include <Windows.h>
#include <string>
#include <cmath>

namespace ollie::state {
    extern bool  veh_god;
    extern bool  veh_speed_boost;
    extern bool  veh_fly;
    extern bool  veh_no_gravity;
    extern bool  veh_auto_repair;
    extern bool  veh_rainbow_neon;
    extern bool  veh_top_speed_mod;
    extern bool  veh_horn_boost;
    extern float veh_boost_mult;
    extern bool  heli_blade_boost;
}

namespace ollie::features {

    void spawn_vehicle(const char* model) {
        ollie::invoker::run_on_script([mm = std::string(model)] {
            auto h = N::GET_HASH_KEY(mm.c_str());
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 90 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            int veh = N::CREATE_VEHICLE(h, x + 3.f, y, z, 0.f, true, true, false);
            ollie::entities::track(veh);
            N::SET_PED_INTO_VEHICLE(ped, veh, -1);
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
        });
    }

    void repair_current_vehicle() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            if (!N::IS_PED_IN_ANY_VEHICLE(ped, false)) return;
            int v = N::GET_VEHICLE_PED_IS_IN(ped, false);
            if (!v) return;
            N::SET_VEHICLE_FIXED(v);
            N::SET_VEHICLE_ENGINE_HEALTH(v, 1000.f);
            N::SET_VEHICLE_ENGINE_ON(v, true, true, false);
        });
    }

    void tick_vehicle() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            if (!N::IS_PED_IN_ANY_VEHICLE(ped, false)) return;
            int v = N::GET_VEHICLE_PED_IS_IN(ped, false);
            if (!v) return;
            using namespace ollie::state;

            if (veh_god) {
                N::SET_ENTITY_INVINCIBLE(v, true);
                N::SET_VEHICLE_ENGINE_HEALTH(v, 1000.f);
            }
            if (veh_auto_repair) {
                N::SET_VEHICLE_FIXED(v);
                N::SET_VEHICLE_ENGINE_ON(v, true, true, false);
            }
            if (veh_no_gravity) N::SET_VEHICLE_GRAVITY(v, false);
            if (veh_top_speed_mod) N::MODIFY_VEHICLE_TOP_SPEED(v, 2.0f);

            bool shift_held = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool rctrl_held = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
            if (veh_speed_boost && shift_held) {
                float vx = 0, vy = 0, vz = 0;
                N::GET_ENTITY_VELOCITY(v, &vx, &vy, &vz);
                float mult = (veh_boost_mult > 1.f ? veh_boost_mult : 1.5f);
                N::SET_ENTITY_VELOCITY(v, vx * mult, vy * mult, vz * mult);
            }
            if (veh_horn_boost && rctrl_held) {
                float vx = 0, vy = 0, vz = 0;
                N::GET_ENTITY_VELOCITY(v, &vx, &vy, &vz);
                N::SET_ENTITY_VELOCITY(v, vx * 2.5f, vy * 2.5f, vz * 2.5f);
            }
            if (veh_fly) {
                float cx = 0, cy = 0, cz = 0;
                N::GET_GAMEPLAY_CAM_ROT(2, &cx, &cy, &cz);
                float yaw   = cz * 3.14159265f / 180.f;
                float pitch = cx * 3.14159265f / 180.f;
                float fx = -std::sin(yaw) * std::cos(pitch);
                float fy =  std::cos(yaw) * std::cos(pitch);
                float fz =  std::sin(pitch);
                float rx =  std::cos(yaw), ry = std::sin(yaw);
                float fwd  = ((GetAsyncKeyState('W') & 0x8000) ? 1.f : 0.f) - ((GetAsyncKeyState('S') & 0x8000) ? 1.f : 0.f);
                float strf = ((GetAsyncKeyState('D') & 0x8000) ? 1.f : 0.f) - ((GetAsyncKeyState('A') & 0x8000) ? 1.f : 0.f);
                float up   = ((GetAsyncKeyState(VK_SPACE)    & 0x8000) ? 1.f : 0.f)
                           - ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? 1.f : 0.f);
                float speed = shift_held ? 80.f : 25.f;
                float vx = (fx * fwd + rx * strf) * speed;
                float vy = (fy * fwd + ry * strf) * speed;
                float vz = (fz * fwd) * speed + up * speed;
                N::SET_ENTITY_VELOCITY(v, vx, vy, vz);
            }
            if (heli_blade_boost) N::SET_HELI_BLADES_FULL_SPEED(v);

            if (veh_rainbow_neon) {
                int t = N::GET_GAME_TIMER();
                float hue = (t % 3600) / 3600.f * 6.f;
                int r = 0, g = 0, b = 0;
                float f = hue - std::floor(hue);
                switch ((int)hue) {
                    case 0: r = 255; g = (int)(f*255); b = 0;     break;
                    case 1: r = (int)((1-f)*255); g = 255; b = 0; break;
                    case 2: r = 0; g = 255; b = (int)(f*255);     break;
                    case 3: r = 0; g = (int)((1-f)*255); b = 255; break;
                    case 4: r = (int)(f*255); g = 0; b = 255;     break;
                    default: r = 255; g = 0; b = (int)((1-f)*255);break;
                }
                for (int i = 0; i < 4; ++i) N::SET_VEHICLE_NEON_ENABLED(v, i, true);
                N::SET_VEHICLE_NEON_COLOUR(v, r, g, b);
            }
        });
    }
}
