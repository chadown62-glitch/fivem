#include "menu.h"
#include "widgets.h"
#include "imgui.h"
#include "../invoker/natives.h"
#include "../core/logger.h"
#include "../core/config.h"
#include "../core/notify.h"
#include "../core/entities.h"
#include <atomic>
#include <string>
#include <cstdio>
#include <cstring>
#include <unordered_set>

namespace ollie::state {
    // self
    bool godmode = false, infinite_ammo = false, never_wanted = false, super_jump = false;
    bool noclip = false, invisible = false, freecam = false;
    bool no_ragdoll = false, no_knock_off = false, breath_hold = false, super_run = false;
    float noclip_speed = 1.0f;
    char  skin_model_buf[64] = "player_zero";
    // weapons
    bool no_reload = false, explosive_ammo = false, fire_ammo = false, explosive_melee = false;
    // esp
    bool  esp_box = true, esp_name = true, esp_dist = true, esp_health = true, esp_tracers = false;
    bool  esp_skeleton = false, esp_bones = false, esp_crosshair = false, esp_chams = false;
    bool  esp_aim_fov_ring = true, esp_head_dot = false, esp_dist_to_cross = false, esp_box_3d = false;
    int   esp_chams_alpha = 90;
    int   crosshair_style = 0;
    float esp_max_distance = 500.0f;
    // aim / trigger
    bool  aimbot_enabled = false, aimbot_hold_rmb = true, aimbot_visible_check = true, aimbot_ignore_dead = true;
    int   aimbot_bone = 0;
    float aimbot_fov = 8.0f, aimbot_smooth = 0.4f;
    int   aimbot_target_id = -1;
    bool  aimbot_lead = false, aimbot_sticky = true, aimbot_draw_line = false;
    float aimbot_bullet_speed = 1800.f;
    int   aimbot_sticky_ms = 180;
    int   aimbot_priority = 0;
    bool  triggerbot_enabled = false, triggerbot_rmb_only = true, triggerbot_rapid = false;
    int   triggerbot_delay_ms = 90;
    // vehicle
    bool  veh_god = false, veh_speed_boost = false, veh_fly = false, veh_no_gravity = false;
    bool  veh_auto_repair = false, heli_blade_boost = false, veh_rainbow_neon = false, veh_top_speed_mod = false;
    bool  veh_horn_boost = false;
    float veh_boost_mult = 1.5f;
    // world
    float teleport_coords[3] = { 0.f, 0.f, 72.f };
    char  vehicle_model_buf[64] = "adder";
    char  weapon_name_buf[64]   = "WEAPON_COMBATPDW";
    char  ped_model_buf[64]     = "a_m_m_bevhills_02";
    int   world_hour = 12, world_minute = 0;
    bool  world_time_frozen = false;
    int   weather_index = 1;
    bool  weather_locked = false, weather_cycle_on = false;
    int   weather_cycle_sec = 30;
    // players
    int   selected_player = 0;
    char  player_search_buf[64] = "";
    // radar / overlay
    bool  radar_on = true, radar_rotate = true, overlay_on = true, watermark_on = true;
    bool  speedo_on = true, speedo_kmh = true, streamer_mode = false;
    float radar_range = 200.f;
    int   radar_size = 110;
    // friends
    std::unordered_set<int> friends_set;
    bool friends_skip_esp = true, friends_skip_aim = true;
    // fun
    bool rain_explosions_on_me = false, orbital_on_target = false;
    // panic
    bool panic_active = false;
    // accent
    int accent_r = 255, accent_g = 60, accent_b = 170;
    bool accent_rainbow = false;
    // heal
    float heal_radius = 100.f;
    // favorites + zombie
    char  fav_name_buf[48] = "spot";
    int   zombie_count = 10;
    char  zombie_model_buf[48] = "a_m_m_hillbilly_01";
}

namespace ollie::keys {
    struct Bind { const char* label; int vk; bool* target; void(*action)(); };
    int   slot_count();
    Bind& slot(int i);
    void  request_capture(int slot);
    int   capturing();
    const char* name(int vk);
}

namespace ollie::favs {
    void add(const std::string& n, float x, float y, float z);
    void remove(int i);
    int  count();
    bool get(int i, std::string& n, float* x, float* y, float* z);
    void clear();
    void serialize(std::string& out);
    void deserialize(const std::string& s);
}

namespace ollie::features {
    void tick_self();
    void tick_world();
    void tick_vehicle();
    void tick_players();
    void tick_visuals();
    void tick_freecam();
    void tick_aimbot();
    void tick_fun();
    void tick_radar();
    void tick_overlay();
    void tick_keybinds();
    void tick_speedo();

    void spawn_vehicle(const char* model);
    void spawn_ped(const char* model);
    void give_weapon(const char* w);
    void remove_all_weapons();
    void repair_current_vehicle();
    void teleport_waypoint();
    void teleport_to(float x, float y, float z);
    void clear_nearby_peds();
    void clear_nearby_vehicles();
    void apply_weather_now();

    void player_bring();
    void player_teleport_to();
    void player_teleport_into_vehicle();
    void player_explode();
    void player_slingshot();
    void player_freeze_toggle();
    void player_kill();

    void airstrike_waypoint();
    void orbital_on_target_once();
    void rain_vehicles();
    void spawn_heli_squad();

    void drop_money(int amount);
    void drop_health();
    void drop_armour();
    void give_max_armour();

    void set_local_skin(const char* model);
    void copy_current_coords();
    void tp_undo();
    void teleport_record_to(float x, float y, float z);
    int  history_count();
    bool history_at(int i, float* x, float* y, float* z);

    void heal_nearby(float radius);

    void doors_open_all();
    void doors_shut_all();
    void doors_lock();
    void doors_unlock();

    void fav_add_here(const char* name);
    void fav_tp(int i);

    void spawn_zombie_swarm(int count, const char* model);
}

namespace ollie::menu {
    static std::atomic<bool> g_visible = false;
    static bool g_config_loaded = false;
    static int  g_page = 0;

    void init() { if (!g_config_loaded) { ollie::config::load(); g_config_loaded = true; } }
    void shutdown() { ollie::config::save(); }
    void toggle() { g_visible.store(!g_visible.load()); }
    bool visible() { return g_visible.load(); }

    // --- pages ---
    static void page_self() {
        ui::hero("self", "survival + movement + skin");
        ImGui::Columns(2, nullptr, false);

        if (ui::begin_card("self_surv", 0, 320)) {
            ui::section("survival");
            ui::toggle("godmode",      &state::godmode);
            ui::toggle("invisible",    &state::invisible);
            ui::toggle("never wanted", &state::never_wanted);
            ui::toggle("no ragdoll",   &state::no_ragdoll);
            ui::toggle("no knock off vehicle", &state::no_knock_off);
            ui::toggle("infinite breath", &state::breath_hold);
            ui::toggle("super run",    &state::super_run);
            if (ui::pill("max armour", ImVec2(140, 0))) ollie::features::give_max_armour();
            ImGui::SameLine();
            ImGui::SliderFloat("heal radius", &state::heal_radius, 20.f, 500.f);
            if (ui::pill("heal nearby", ImVec2(160, 0))) ollie::features::heal_nearby(state::heal_radius);
        }
        ui::end_card();

        ImGui::NextColumn();

        if (ui::begin_card("self_move", 0, 320)) {
            ui::section("movement");
            ui::toggle("super jump", &state::super_jump);
            ui::toggle("freecam",    &state::freecam);
            ImGui::PushItemWidth(-1);
            ImGui::SliderFloat("##nspd", &state::noclip_speed, 0.1f, 20.f, "noclip speed  %.1f");
            ImGui::PopItemWidth();
            ImGui::Dummy(ImVec2(0, 6));
            ui::section("skin");
            ImGui::PushItemWidth(-120);
            ImGui::InputTextWithHint("##skin", "player_zero", state::skin_model_buf, sizeof(state::skin_model_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("set", ImVec2(100, 0))) ollie::features::set_local_skin(state::skin_model_buf);
            const char* presets[] = { "player_zero", "player_one", "player_two", "mp_m_freemode_01", "mp_f_freemode_01" };
            for (int i = 0; i < 5; ++i) { if (i) ImGui::SameLine(); if (ui::pill(presets[i], ImVec2(0, 26))) ollie::features::set_local_skin(presets[i]); }
        }
        ui::end_card();

        ImGui::Columns(1);

        if (ui::begin_card("self_drops", 0, 90)) {
            ui::section("drops");
            if (ui::pill("drop money $1k",   ImVec2(180, 0))) ollie::features::drop_money(1000);  ImGui::SameLine();
            if (ui::pill("drop money $10k",  ImVec2(200, 0))) ollie::features::drop_money(10000); ImGui::SameLine();
            if (ui::pill("health pickup",    ImVec2(160, 0))) ollie::features::drop_health();     ImGui::SameLine();
            if (ui::pill("armour pickup",    ImVec2(160, 0))) ollie::features::drop_armour();
        }
        ui::end_card();
    }

    static void page_weapons() {
        ui::hero("weapons", "ammo, fire, mods");
        if (ui::begin_card("wpn_toggles", 0, 170)) {
            ui::section("ammo + fire");
            ui::toggle("infinite ammo",   &state::infinite_ammo);
            ui::toggle("no reload",       &state::no_reload);
            ui::toggle("explosive ammo",  &state::explosive_ammo);
            ui::toggle("fire ammo",       &state::fire_ammo);
            ui::toggle("explosive melee", &state::explosive_melee);
        }
        ui::end_card();

        if (ui::begin_card("wpn_give", 0, 190)) {
            ui::section("give");
            ImGui::PushItemWidth(-140);
            ImGui::InputTextWithHint("##wname", "WEAPON_COMBATPDW", state::weapon_name_buf, sizeof(state::weapon_name_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("give", ImVec2(70, 0))) ollie::features::give_weapon(state::weapon_name_buf);
            ImGui::SameLine();
            if (ui::pill("remove all", ImVec2(0, 0))) ollie::features::remove_all_weapons();
            ImGui::Dummy(ImVec2(0, 6));
            if (ui::pill("full arsenal", ImVec2(-1, 34))) {
                const char* arsenal[] = {
                    "WEAPON_COMBATPISTOL","WEAPON_CARBINERIFLE","WEAPON_PUMPSHOTGUN",
                    "WEAPON_SNIPERRIFLE","WEAPON_RPG","WEAPON_MINIGUN","WEAPON_GRENADE",
                    "WEAPON_STICKYBOMB","WEAPON_KNIFE","WEAPON_PARACHUTE","WEAPON_HEAVYSNIPER",
                    "WEAPON_SMG","WEAPON_ASSAULTSHOTGUN","WEAPON_PISTOL50","WEAPON_HOMINGLAUNCHER",
                    "WEAPON_RAILGUN","WEAPON_COMPACTLAUNCHER"
                };
                for (auto* w : arsenal) ollie::features::give_weapon(w);
            }
        }
        ui::end_card();
    }

    static void page_aim() {
        ui::hero("aim", "aimbot + triggerbot");
        if (ui::begin_card("aim_bot", 0, 370)) {
            ui::section("aimbot");
            ui::toggle("enabled",         &state::aimbot_enabled);
            ui::toggle("hold RMB",        &state::aimbot_hold_rmb);
            ui::toggle("visible only",    &state::aimbot_visible_check);
            ui::toggle("ignore dead",     &state::aimbot_ignore_dead);
            ui::toggle("lead target",     &state::aimbot_lead);
            ui::toggle("sticky target",   &state::aimbot_sticky);
            ui::toggle("draw line",       &state::aimbot_draw_line);
            const char* bones[] = { "head", "neck", "chest", "pelvis", "L hand", "R hand", "L foot", "R foot" };
            ImGui::PushItemWidth(180);
            ImGui::Combo("bone", &state::aimbot_bone, bones, IM_ARRAYSIZE(bones));
            const char* pri[] = { "closest to cross", "closest to me", "lowest health" };
            ImGui::Combo("priority", &state::aimbot_priority, pri, IM_ARRAYSIZE(pri));
            ImGui::PopItemWidth();
            ImGui::SliderFloat("fov (deg)", &state::aimbot_fov,    1.f, 45.f, "%.1f");
            ImGui::SliderFloat("smooth",    &state::aimbot_smooth, 0.f, 0.95f, "%.2f");
            ImGui::SliderFloat("bullet speed (m/s)", &state::aimbot_bullet_speed, 500.f, 4000.f, "%.0f");
            ImGui::SliderInt("sticky window (ms)",   &state::aimbot_sticky_ms, 50, 800);
            char line[64]; std::snprintf(line, sizeof(line), "target  %d", state::aimbot_target_id);
            ui::chip(line, IM_COL32(50, 50, 70, 255));
        }
        ui::end_card();

        if (ui::begin_card("aim_trig", 0, 180)) {
            ui::section("triggerbot");
            ui::toggle("enabled",  &state::triggerbot_enabled);
            ui::toggle("RMB only", &state::triggerbot_rmb_only);
            ui::toggle("rapid fire", &state::triggerbot_rapid);
            ImGui::SliderInt("delay (ms)", &state::triggerbot_delay_ms, 20, 500);
            ui::toggle("friends skip aim", &state::friends_skip_aim);
        }
        ui::end_card();
    }

    static void page_vehicle() {
        ui::hero("vehicle", "god, fly, boost, doors, spawner");
        if (ui::begin_card("veh_toggles", 0, 340)) {
            ui::section("in-vehicle");
            ui::toggle("vehicle god",        &state::veh_god);
            ui::toggle("auto-repair",        &state::veh_auto_repair);
            ui::toggle("no gravity",         &state::veh_no_gravity);
            ui::toggle("speed boost (SHIFT)",&state::veh_speed_boost);
            ui::toggle("horn boost (RCTRL)", &state::veh_horn_boost);
            ImGui::SliderFloat("boost mult", &state::veh_boost_mult, 1.1f, 5.0f);
            ui::toggle("vehicle fly",        &state::veh_fly);
            ui::toggle("heli blade boost",   &state::heli_blade_boost);
            ui::toggle("top speed +100%",    &state::veh_top_speed_mod);
            ui::toggle("rainbow neon",       &state::veh_rainbow_neon);
            if (ui::pill("repair now", ImVec2(130, 0))) ollie::features::repair_current_vehicle();
        }
        ui::end_card();

        if (ui::begin_card("veh_doors", 0, 120)) {
            ui::section("doors / lock");
            if (ui::pill("open all",  ImVec2(120, 32))) ollie::features::doors_open_all(); ImGui::SameLine();
            if (ui::pill("shut all",  ImVec2(120, 32))) ollie::features::doors_shut_all(); ImGui::SameLine();
            if (ui::pill("lock",      ImVec2(100, 32))) ollie::features::doors_lock();     ImGui::SameLine();
            if (ui::pill("unlock",    ImVec2(100, 32))) ollie::features::doors_unlock();
        }
        ui::end_card();

        if (ui::begin_card("veh_spawn", 0, 230)) {
            ui::section("spawner");
            ImGui::PushItemWidth(-130);
            ImGui::InputTextWithHint("##vmodel", "adder", state::vehicle_model_buf, sizeof(state::vehicle_model_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("spawn", ImVec2(110, 0))) ollie::features::spawn_vehicle(state::vehicle_model_buf);
            ImGui::Dummy(ImVec2(0, 4));
            const char* grid[] = { "adder","buzzard","insurgent","oppressor2","deluxo","hydra","lazer","kuruma2" };
            for (int i = 0; i < 8; ++i) {
                if (i && (i % 4)) ImGui::SameLine();
                if (ui::pill(grid[i], ImVec2(96, 28))) ollie::features::spawn_vehicle(grid[i]);
            }
            ImGui::Dummy(ImVec2(0, 4));
            char ec[48]; std::snprintf(ec, sizeof(ec), "spawned entities: %d", ollie::entities::count());
            ImGui::TextUnformatted(ec);
            ImGui::SameLine();
            if (ui::pill("delete all spawned", ImVec2(200, 0))) ollie::entities::delete_all();
        }
        ui::end_card();
    }

    static void page_world() {
        ui::hero("world", "teleport, time, weather, favorites");

        if (ui::begin_card("w_tp", 0, 240)) {
            ui::section("teleport");
            ImGui::PushItemWidth(-220);
            ImGui::InputFloat3("##coords", state::teleport_coords);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("tp", ImVec2(70, 0)))
                ollie::features::teleport_record_to(state::teleport_coords[0], state::teleport_coords[1], state::teleport_coords[2]);
            ImGui::SameLine();
            if (ui::pill("copy", ImVec2(70, 0))) ollie::features::copy_current_coords();
            ImGui::SameLine();
            if (ui::pill("undo", ImVec2(70, 0))) ollie::features::tp_undo();

            struct P { const char* name; float x, y, z; };
            P ps[] = {
                {"airport",  -1037.f, -2737.f,  21.f},
                {"casino",     925.f,    46.f,  81.f},
                {"chiliad",    450.f,  5566.f, 806.f},
                {"zancudo",  -2047.f,  2995.f,  32.f},
                {"vinewood",   711.f,  1200.f, 325.f},
                {"sandy",     1960.f,  3740.f,  32.f},
            };
            for (int i = 0; i < 6; ++i) {
                if (i && (i % 4) == 0) {} else if (i) ImGui::SameLine();
                if (ui::pill(ps[i].name, ImVec2(96, 28))) ollie::features::teleport_record_to(ps[i].x, ps[i].y, ps[i].z);
            }
            if (ui::pill("waypoint", ImVec2(130, 28))) ollie::features::teleport_waypoint();

            ImGui::Dummy(ImVec2(0, 4));
            ImGui::TextUnformatted("history:");
            for (int i = 0; i < ollie::features::history_count(); ++i) {
                float x, y, z;
                ollie::features::history_at(i, &x, &y, &z);
                char row[96]; std::snprintf(row, sizeof(row), "%d  %.0f, %.0f, %.0f", i, x, y, z);
                if (ImGui::Selectable(row)) ollie::features::teleport_record_to(x, y, z);
            }
        }
        ui::end_card();

        if (ui::begin_card("w_fav", 0, 220)) {
            ui::section("favorites");
            ImGui::PushItemWidth(-220);
            ImGui::InputTextWithHint("##favname", "name this spot", state::fav_name_buf, sizeof(state::fav_name_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("add here", ImVec2(130, 0))) ollie::features::fav_add_here(state::fav_name_buf);
            ImGui::Separator();
            for (int i = 0; i < ollie::favs::count(); ++i) {
                std::string nm; float x, y, z;
                if (!ollie::favs::get(i, nm, &x, &y, &z)) continue;
                char row[128]; std::snprintf(row, sizeof(row), "%s   (%.0f, %.0f, %.0f)", nm.c_str(), x, y, z);
                ImGui::PushID(i);
                if (ImGui::Selectable(row)) ollie::features::fav_tp(i);
                ImGui::SameLine();
                if (ui::pill("x", ImVec2(20, 20))) { ollie::favs::remove(i); ImGui::PopID(); break; }
                ImGui::PopID();
            }
        }
        ui::end_card();

        if (ui::begin_card("w_time", 0, 140)) {
            ui::section("time");
            ui::toggle("freeze time", &state::world_time_frozen);
            ImGui::SliderInt("hour",   &state::world_hour,   0, 23);
            ImGui::SliderInt("minute", &state::world_minute, 0, 59);
        }
        ui::end_card();

        if (ui::begin_card("w_wx", 0, 190)) {
            ui::section("weather");
            const char* ws[] = {
                "EXTRASUNNY","CLEAR","CLOUDS","SMOG","FOGGY","OVERCAST",
                "RAIN","THUNDER","CLEARING","NEUTRAL","SNOW","BLIZZARD",
                "SNOWLIGHT","XMAS","HALLOWEEN"
            };
            ImGui::PushItemWidth(240);
            ImGui::Combo("##wx", &state::weather_index, ws, IM_ARRAYSIZE(ws));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ui::toggle("persist", &state::weather_locked);
            if (ui::pill("apply now", ImVec2(130, 0))) ollie::features::apply_weather_now();
            ui::toggle("auto cycle",   &state::weather_cycle_on);
            ImGui::SliderInt("cycle every (s)", &state::weather_cycle_sec, 5, 300);
        }
        ui::end_card();

        if (ui::begin_card("w_area", 0, 130)) {
            ui::section("area");
            if (ui::pill("clear peds", ImVec2(160, 0)))     ollie::features::clear_nearby_peds();
            ImGui::SameLine();
            if (ui::pill("clear vehicles", ImVec2(180, 0))) ollie::features::clear_nearby_vehicles();
            ImGui::Dummy(ImVec2(0, 4));
            ImGui::PushItemWidth(-130);
            ImGui::InputTextWithHint("##ped", "a_m_m_bevhills_02", state::ped_model_buf, sizeof(state::ped_model_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ui::pill("spawn ped", ImVec2(110, 0))) ollie::features::spawn_ped(state::ped_model_buf);
        }
        ui::end_card();
    }

    static void page_players() {
        ui::hero("players", "search, pick, act");
        if (ui::begin_card("p_pick", 0, 260)) {
            ui::section("online");
            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##psearch", "search by name...", state::player_search_buf, sizeof(state::player_search_buf));
            ImGui::PopItemWidth();
            ImGui::BeginChild("plist", ImVec2(0, 160), true);
            int localPid = N::PLAYER_ID();
            for (int i = 0; i < 32; ++i) {
                if (!N::IS_PLAYER_ACTIVE(i)) continue;
                const char* name = N::GET_PLAYER_NAME(i);
                if (state::player_search_buf[0]) {
                    if (!name || !strstr(name, state::player_search_buf)) continue;
                }
                char row[96];
                bool is_friend = state::friends_set.count(i) > 0;
                std::snprintf(row, sizeof(row), "%s%s [%d] %s",
                              is_friend ? "F " : "  ",
                              (i == localPid) ? "YOU" : "   ",
                              i, name ? name : "player");
                if (ImGui::Selectable(row, state::selected_player == i)) state::selected_player = i;
                if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    if (is_friend) state::friends_set.erase(i);
                    else state::friends_set.insert(i);
                }
            }
            ImGui::EndChild();
            ImGui::Text("local %d   target %d   (right-click row = friend)", localPid, state::selected_player);
        }
        ui::end_card();

        if (ui::begin_card("p_acts", 0, 190)) {
            ui::section("actions");
            if (ui::pill("bring",         ImVec2(120, 34))) ollie::features::player_bring();          ImGui::SameLine();
            if (ui::pill("tp to them",    ImVec2(130, 34))) ollie::features::player_teleport_to();    ImGui::SameLine();
            if (ui::pill("tp into car",   ImVec2(130, 34))) ollie::features::player_teleport_into_vehicle();
            if (ui::pill("spectate",      ImVec2(120, 34))) state::freecam = true;                    ImGui::SameLine();
            if (ui::pill("explode",       ImVec2(120, 34))) ollie::features::player_explode();        ImGui::SameLine();
            if (ui::pill("slingshot",     ImVec2(130, 34))) ollie::features::player_slingshot();
            if (ui::pill("freeze",        ImVec2(120, 34))) ollie::features::player_freeze_toggle();  ImGui::SameLine();
            if (ui::pill("kill",          ImVec2(80,  34))) ollie::features::player_kill();
        }
        ui::end_card();
    }

    static void page_visuals() {
        ui::hero("visuals", "esp, chams, radar, overlay, speedo");
        if (ui::begin_card("v_esp", 0, 360)) {
            ui::section("esp");
            ui::toggle("box 2d",        &state::esp_box);
            ui::toggle("box 3d",        &state::esp_box_3d);
            ui::toggle("name",          &state::esp_name);
            ui::toggle("distance",      &state::esp_dist);
            ui::toggle("health",        &state::esp_health);
            ui::toggle("tracers",       &state::esp_tracers);
            ui::toggle("skeleton",      &state::esp_skeleton);
            ui::toggle("bone dots",     &state::esp_bones);
            ui::toggle("head dot",      &state::esp_head_dot);
            ui::toggle("chams tint",    &state::esp_chams);
            ImGui::SliderInt("chams alpha", &state::esp_chams_alpha, 0, 255);
            ui::toggle("dist to cross", &state::esp_dist_to_cross);
            ImGui::SliderFloat("max distance", &state::esp_max_distance, 50.f, 2000.f);
            ui::toggle("friends skip esp", &state::friends_skip_esp);
        }
        ui::end_card();

        if (ui::begin_card("v_hud", 0, 210)) {
            ui::section("hud");
            ui::toggle("crosshair",      &state::esp_crosshair);
            const char* xs[] = { "cross", "dot", "circle", "plus + dot", "T" };
            ImGui::PushItemWidth(180);
            ImGui::Combo("style", &state::crosshair_style, xs, IM_ARRAYSIZE(xs));
            ImGui::PopItemWidth();
            ui::toggle("fov ring",       &state::esp_aim_fov_ring);
            ui::toggle("overlay pill",   &state::overlay_on);
            ui::toggle("watermark",      &state::watermark_on);
            ui::toggle("speedometer",    &state::speedo_on);
            ui::toggle("speedo km/h",    &state::speedo_kmh);
            ui::toggle("streamer mode (hide branding)", &state::streamer_mode);
        }
        ui::end_card();

        if (ui::begin_card("v_radar", 0, 150)) {
            ui::section("radar");
            ui::toggle("enabled",   &state::radar_on);
            ui::toggle("rotate with camera", &state::radar_rotate);
            ImGui::SliderFloat("range (m)", &state::radar_range, 50.f, 1000.f);
            ImGui::SliderInt("size (px)", &state::radar_size, 60, 200);
        }
        ui::end_card();
    }

    static void page_fun() {
        ui::hero("fun", "airstrike, orbital, rain, zombies");
        if (ui::begin_card("f_strike", 0, 170)) {
            ui::section("one-shots");
            if (ui::pill("airstrike waypoint", ImVec2(220, 34))) ollie::features::airstrike_waypoint();
            ImGui::SameLine();
            if (ui::pill("orbital on target",  ImVec2(220, 34))) ollie::features::orbital_on_target_once();
            if (ui::pill("rain vehicles",      ImVec2(220, 34))) ollie::features::rain_vehicles();
            ImGui::SameLine();
            if (ui::pill("heli squad",         ImVec2(220, 34))) ollie::features::spawn_heli_squad();
        }
        ui::end_card();
        if (ui::begin_card("f_auto", 0, 120)) {
            ui::section("auto");
            ui::toggle("rain explosions on me", &state::rain_explosions_on_me);
            ui::toggle("orbital loop on target", &state::orbital_on_target);
        }
        ui::end_card();
        if (ui::begin_card("f_zomb", 0, 150)) {
            ui::section("zombie swarm");
            ImGui::PushItemWidth(-180);
            ImGui::InputTextWithHint("##zmodel", "a_m_m_hillbilly_01", state::zombie_model_buf, sizeof(state::zombie_model_buf));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::SliderInt("count", &state::zombie_count, 1, 40);
            if (ui::pill("spawn swarm (hostile)", ImVec2(240, 34))) ollie::features::spawn_zombie_swarm(state::zombie_count, state::zombie_model_buf);
        }
        ui::end_card();
    }

    static void page_keybinds() {
        ui::hero("keybinds", "click a key cell, press any key");
        if (ui::begin_card("kb", 0, 0)) {
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnWidth(0, 160);
            ImGui::SetColumnWidth(1, 120);
            int cap = ollie::keys::capturing();
            for (int i = 0; i < ollie::keys::slot_count(); ++i) {
                auto& b = ollie::keys::slot(i);
                ImGui::TextUnformatted(b.label);           ImGui::NextColumn();
                char label[32];
                _snprintf_s(label, sizeof(label), "[ %s ]", ollie::keys::name(b.vk));
                bool here = (cap == i);
                if (here) ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 60, 170, 255));
                if (ImGui::Button(here ? "press any key..." : label, ImVec2(110, 0))) {
                    ollie::keys::request_capture(i);
                }
                if (here) ImGui::PopStyleColor();
                ImGui::NextColumn();
                if (b.target) ImGui::TextUnformatted(*b.target ? "ON " : "off");
                else          ImGui::TextUnformatted("action");
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
        }
        ui::end_card();
    }

    static void page_log() {
        ui::hero("log", "recent events");
        if (ui::begin_card("l_log", 0, 0)) {
            if (ui::pill("clear", ImVec2(100, 28))) ollie::notify::clear_log();
            ImGui::SameLine();
            if (ui::pill("test entry", ImVec2(140, 28))) ollie::notify::push("test entry");
            ImGui::Separator();
            ImGui::BeginChild("logscroll", ImVec2(0, 0), true);
            int n = ollie::notify::log_count();
            for (int i = 0; i < n; ++i) ImGui::TextUnformatted(ollie::notify::log_at(i));
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 24.f)
                ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();
        }
        ui::end_card();
    }

    static void page_config() {
        ui::hero("config", "save, load, accent");
        if (ui::begin_card("c_io", 0, 90)) {
            if (ui::pill("save config", ImVec2(160, 34))) ollie::config::save();
            ImGui::SameLine();
            if (ui::pill("load config", ImVec2(160, 34))) ollie::config::load();
        }
        ui::end_card();

        if (ui::begin_card("c_accent", 0, 150)) {
            ui::section("accent");
            float col[3] = {
                state::accent_r / 255.f, state::accent_g / 255.f, state::accent_b / 255.f
            };
            if (ImGui::ColorEdit3("##accent", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueBar)) {
                state::accent_r = (int)(col[0] * 255);
                state::accent_g = (int)(col[1] * 255);
                state::accent_b = (int)(col[2] * 255);
            }
            ui::toggle("rainbow cycle", &state::accent_rainbow);
            ImGui::TextUnformatted("affects ESP primary, FOV ring, overlay, watermark, aim line");
        }
        ui::end_card();
    }

    static void page_about() {
        ui::hero("ollie", "built by olivia");
        if (ui::begin_card("a_about", 0, 170)) {
            ImGui::TextColored(ImVec4(1.f, 0.35f, 0.75f, 1.f), "absolute ollie");
            ImGui::TextUnformatted("discord.gg/ojailbreaks");
            ImGui::Separator();
            ImGui::BulletText("single-file injector (ollie.exe)");
            ImGui::BulletText("payload welded in as RT_RCDATA");
            ImGui::BulletText("PEB unlink + PE header wipe");
            ImGui::BulletText("INSERT = menu, END = unload");
            ImGui::BulletText("F12 = PANIC kill-switch, G = throw grenade");
        }
        ui::end_card();
    }

    // --- shell ---
    static void run_features_each_frame() {
        ollie::features::tick_keybinds();
        ollie::features::tick_self();
        ollie::features::tick_world();
        ollie::features::tick_vehicle();
        ollie::features::tick_players();
        ollie::features::tick_visuals();
        ollie::features::tick_freecam();
        ollie::features::tick_aimbot();
        ollie::features::tick_fun();
        ollie::features::tick_radar();
        ollie::features::tick_overlay();
        ollie::features::tick_speedo();
    }

    void render() {
        run_features_each_frame();
        ollie::notify::render();
        if (!g_visible.load()) return;

        ImGui::SetNextWindowSize(ImVec2(980, 660), ImGuiCond_Once);
        ImGui::SetNextWindowSizeConstraints(ImVec2(840, 540), ImVec2(1800, 1200));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("##ollie", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar)) {
            ui::window_shadow();

            auto dl = ImGui::GetWindowDrawList();
            ImVec2 wp = ImGui::GetWindowPos();
            float ww = ImGui::GetWindowSize().x;
            dl->AddRectFilled(wp, ImVec2(wp.x + ww, wp.y + 40.f), IM_COL32(20, 20, 28, 255),
                              14.f, ImDrawFlags_RoundCornersTop);
            dl->AddRectFilled(ImVec2(wp.x, wp.y + 38.f), ImVec2(wp.x + ww, wp.y + 40.f),
                              ui::accent_breath());
            dl->AddText(ImVec2(wp.x + 18, wp.y + 11), IM_COL32(255, 255, 255, 255),
                        state::streamer_mode ? "menu" : "ollie  //  built by olivia");
            if (!state::streamer_mode) {
                const char* r = "discord.gg/ojailbreaks";
                ImVec2 ts = ImGui::CalcTextSize(r);
                dl->AddText(ImVec2(wp.x + ww - ts.x - 18, wp.y + 11),
                            IM_COL32(230, 180, 220, 255), r);
            }
            ImGui::Dummy(ImVec2(0, 40));

            ImGui::BeginChild("body", ImVec2(0, 0), false);
            {
                const char* labels[] = {
                    "self", "weapons", "aim", "vehicle", "world", "players",
                    "visuals", "fun", "keybinds", "log", "config", "about"
                };
                const char* icons[] = {
                    "O", "W", "@", "V", "#", "P", "V", "F", "K", "L", "C", "?"
                };
                int n = IM_ARRAYSIZE(labels);

                g_page = ui::sidebar_nav(170.f, labels, icons, n, g_page);
                ImGui::SameLine();

                ImGui::BeginChild("content", ImVec2(0, 0), false,
                                  ImGuiWindowFlags_AlwaysUseWindowPadding);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14, 14));
                switch (g_page) {
                    case 0:  page_self();     break;
                    case 1:  page_weapons();  break;
                    case 2:  page_aim();      break;
                    case 3:  page_vehicle();  break;
                    case 4:  page_world();    break;
                    case 5:  page_players();  break;
                    case 6:  page_visuals();  break;
                    case 7:  page_fun();      break;
                    case 8:  page_keybinds(); break;
                    case 9:  page_log();      break;
                    case 10: page_config();   break;
                    case 11: page_about();    break;
                }
                ImGui::PopStyleVar();
                ImGui::EndChild();
            }
            ImGui::EndChild();
        }
        ImGui::End();
        ImGui::PopStyleVar(1);
    }
}

bool g_menu_visible_bool() { return ollie::menu::visible(); }
