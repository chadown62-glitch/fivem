// panic.cpp -- kill-switch. Flips every "dangerous" toggle off instantly.
// Bound to F12 by default via keybinds.cpp.
#include "../core/notify.h"
#include <chrono>

namespace ollie::state {
    extern bool godmode, invisible, never_wanted, super_jump;
    extern bool no_reload, explosive_ammo, fire_ammo, explosive_melee, infinite_ammo;
    extern bool aimbot_enabled, triggerbot_enabled;
    extern bool esp_box, esp_name, esp_dist, esp_health, esp_tracers, esp_skeleton, esp_bones, esp_chams;
    extern bool esp_crosshair, esp_aim_fov_ring;
    extern bool veh_god, veh_speed_boost, veh_fly, veh_no_gravity, veh_auto_repair;
    extern bool veh_rainbow_neon, veh_top_speed_mod, heli_blade_boost;
    extern bool freecam, noclip;
    extern bool rain_explosions_on_me, orbital_on_target;
    extern bool overlay_on, radar_on;
    extern bool panic_active;
}

namespace ollie::keys {
    void panic_fire() {
        using namespace ollie::state;
        godmode = invisible = never_wanted = super_jump = false;
        no_reload = explosive_ammo = fire_ammo = explosive_melee = infinite_ammo = false;
        aimbot_enabled = triggerbot_enabled = false;
        esp_box = esp_name = esp_dist = esp_health = esp_tracers = false;
        esp_skeleton = esp_bones = esp_chams = esp_crosshair = esp_aim_fov_ring = false;
        veh_god = veh_speed_boost = veh_fly = veh_no_gravity = veh_auto_repair = false;
        veh_rainbow_neon = veh_top_speed_mod = heli_blade_boost = false;
        freecam = noclip = false;
        rain_explosions_on_me = orbital_on_target = false;
        panic_active = true;
        ollie::notify::push("PANIC: all toggles off");
    }
}
