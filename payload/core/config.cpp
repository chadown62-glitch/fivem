#include "config.h"
#include "logger.h"
#include <Windows.h>
#include <Shlobj.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#pragma comment(lib, "shell32.lib")
namespace fs = std::filesystem;

namespace ollie::state {
    extern bool godmode, infinite_ammo, never_wanted, super_jump, noclip, invisible;
    extern bool no_ragdoll, no_knock_off, breath_hold, super_run;
    extern bool no_reload, explosive_ammo, fire_ammo, explosive_melee;
    extern bool esp_box, esp_name, esp_dist, esp_health, esp_tracers;
    extern bool esp_skeleton, esp_bones, esp_crosshair, esp_chams, esp_aim_fov_ring;
    extern bool esp_head_dot, esp_dist_to_cross;
    extern float esp_max_distance;
    extern bool  aimbot_enabled, aimbot_hold_rmb, aimbot_visible_check, aimbot_ignore_dead;
    extern int   aimbot_bone;
    extern float aimbot_fov, aimbot_smooth;
    extern bool  aimbot_lead, aimbot_sticky, aimbot_draw_line;
    extern float aimbot_bullet_speed;
    extern int   aimbot_sticky_ms;
    extern bool  triggerbot_enabled, triggerbot_rmb_only;
    extern int   triggerbot_delay_ms;
    extern bool  veh_god, veh_speed_boost, veh_fly, veh_no_gravity, veh_auto_repair;
    extern bool  heli_blade_boost, veh_rainbow_neon, veh_top_speed_mod;
    extern float veh_boost_mult;
    extern int   world_hour, world_minute, weather_index;
    extern bool  world_time_frozen, weather_locked;
    extern float noclip_speed;
    extern bool  radar_on, radar_rotate, overlay_on, watermark_on;
    extern float radar_range;
    extern int   radar_size;
    extern bool  friends_skip_esp, friends_skip_aim;
    extern std::unordered_set<int> friends_set;
    extern int   accent_r, accent_g, accent_b;
    extern bool  rain_explosions_on_me, orbital_on_target;
    extern bool  accent_rainbow;
    extern bool  speedo_on, speedo_kmh;
    extern bool  weather_cycle_on;
    extern int   weather_cycle_sec;
    extern int   aimbot_priority;
    extern bool  triggerbot_rapid;
    extern float heal_radius;
    extern bool  esp_box_3d, streamer_mode;
    extern int   esp_chams_alpha, crosshair_style;
    extern bool  veh_horn_boost;
}

namespace ollie::favs {
    void serialize(std::string& out);
    void deserialize(const std::string& s);
}

namespace ollie::keys {
    struct Bind { const char* label; int vk; bool* target; void(*action)(); };
    int   slot_count();
    Bind& slot(int i);
}

static fs::path cfg_path() {
    PWSTR p = nullptr;
    fs::path base;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &p))) {
        base = p; CoTaskMemFree(p);
    } else {
        wchar_t temp[MAX_PATH]{}; GetTempPathW(MAX_PATH, temp);
        base = temp;
    }
    base /= L"ollie";
    std::error_code ec; fs::create_directories(base, ec);
    return base / L"config.ini";
}

namespace ollie::config {

    void save() {
        std::ofstream f(cfg_path());
        if (!f) return;
        using namespace ollie::state;
    #define W(name) f << #name " = " << (name) << "\n"
        W(godmode); W(infinite_ammo); W(never_wanted); W(super_jump); W(noclip); W(invisible);
        W(no_ragdoll); W(no_knock_off); W(breath_hold); W(super_run);
        W(no_reload); W(explosive_ammo); W(fire_ammo); W(explosive_melee);
        W(esp_box); W(esp_name); W(esp_dist); W(esp_health); W(esp_tracers);
        W(esp_skeleton); W(esp_bones); W(esp_crosshair); W(esp_chams); W(esp_aim_fov_ring);
        W(esp_head_dot); W(esp_dist_to_cross); W(esp_max_distance);
        W(aimbot_enabled); W(aimbot_hold_rmb); W(aimbot_visible_check); W(aimbot_ignore_dead);
        W(aimbot_bone); W(aimbot_fov); W(aimbot_smooth);
        W(aimbot_lead); W(aimbot_sticky); W(aimbot_draw_line);
        W(aimbot_bullet_speed); W(aimbot_sticky_ms);
        W(triggerbot_enabled); W(triggerbot_rmb_only); W(triggerbot_delay_ms);
        W(veh_god); W(veh_speed_boost); W(veh_fly); W(veh_no_gravity); W(veh_auto_repair);
        W(heli_blade_boost); W(veh_rainbow_neon); W(veh_top_speed_mod); W(veh_boost_mult);
        W(world_hour); W(world_minute); W(weather_index); W(world_time_frozen); W(weather_locked);
        W(noclip_speed);
        W(radar_on); W(radar_rotate); W(overlay_on); W(watermark_on); W(radar_range); W(radar_size);
        W(friends_skip_esp); W(friends_skip_aim);
        W(accent_r); W(accent_g); W(accent_b);
        W(rain_explosions_on_me); W(orbital_on_target);
        W(accent_rainbow); W(speedo_on); W(speedo_kmh);
        W(weather_cycle_on); W(weather_cycle_sec);
        W(aimbot_priority); W(triggerbot_rapid); W(heal_radius);
        W(esp_box_3d); W(streamer_mode); W(esp_chams_alpha); W(crosshair_style);
        W(veh_horn_boost);
    #undef W
        {
            std::string s; ollie::favs::serialize(s);
            f << "favs = " << s << "\n";
        }
        f << "friends = ";
        bool first = true;
        for (int id : friends_set) { if (!first) f << ","; f << id; first = false; }
        f << "\n";
        for (int i = 0; i < ollie::keys::slot_count(); ++i) {
            f << "kb_" << i << " = " << ollie::keys::slot(i).vk << "\n";
        }
        ollie::log::line("config: saved");
    }

    void load() {
        std::ifstream f(cfg_path());
        if (!f) return;
        std::unordered_map<std::string, std::string> kv;
        std::string line;
        auto trim = [](std::string& s) {
            while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
            while (!s.empty() && isspace((unsigned char)s.back()))  s.pop_back();
        };
        while (std::getline(f, line)) {
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string k = line.substr(0, eq);
            std::string v = line.substr(eq + 1);
            trim(k); trim(v);
            kv[k] = v;
        }
        using namespace ollie::state;
        auto geti = [&](const char* k, int def)   { auto it = kv.find(k); return it == kv.end() ? def : std::stoi(it->second); };
        auto getf = [&](const char* k, float def) { auto it = kv.find(k); return it == kv.end() ? def : std::stof(it->second); };
        auto getb = [&](const char* k, bool def)  { auto it = kv.find(k); return it == kv.end() ? def : (it->second == "1"); };
    #define R(name, def) name = get##def(#name, name)
        R(godmode, b); R(infinite_ammo, b); R(never_wanted, b); R(super_jump, b); R(noclip, b); R(invisible, b);
        R(no_ragdoll, b); R(no_knock_off, b); R(breath_hold, b); R(super_run, b);
        R(no_reload, b); R(explosive_ammo, b); R(fire_ammo, b); R(explosive_melee, b);
        R(esp_box, b); R(esp_name, b); R(esp_dist, b); R(esp_health, b); R(esp_tracers, b);
        R(esp_skeleton, b); R(esp_bones, b); R(esp_crosshair, b); R(esp_chams, b); R(esp_aim_fov_ring, b);
        R(esp_head_dot, b); R(esp_dist_to_cross, b); R(esp_max_distance, f);
        R(aimbot_enabled, b); R(aimbot_hold_rmb, b); R(aimbot_visible_check, b); R(aimbot_ignore_dead, b);
        R(aimbot_bone, i); R(aimbot_fov, f); R(aimbot_smooth, f);
        R(aimbot_lead, b); R(aimbot_sticky, b); R(aimbot_draw_line, b);
        R(aimbot_bullet_speed, f); R(aimbot_sticky_ms, i);
        R(triggerbot_enabled, b); R(triggerbot_rmb_only, b); R(triggerbot_delay_ms, i);
        R(veh_god, b); R(veh_speed_boost, b); R(veh_fly, b); R(veh_no_gravity, b); R(veh_auto_repair, b);
        R(heli_blade_boost, b); R(veh_rainbow_neon, b); R(veh_top_speed_mod, b); R(veh_boost_mult, f);
        R(world_hour, i); R(world_minute, i); R(weather_index, i); R(world_time_frozen, b); R(weather_locked, b);
        R(noclip_speed, f);
        R(radar_on, b); R(radar_rotate, b); R(overlay_on, b); R(watermark_on, b);
        R(radar_range, f); R(radar_size, i);
        R(friends_skip_esp, b); R(friends_skip_aim, b);
        R(accent_r, i); R(accent_g, i); R(accent_b, i);
        R(rain_explosions_on_me, b); R(orbital_on_target, b);
        R(accent_rainbow, b); R(speedo_on, b); R(speedo_kmh, b);
        R(weather_cycle_on, b); R(weather_cycle_sec, i);
        R(aimbot_priority, i); R(triggerbot_rapid, b); R(heal_radius, f);
        R(esp_box_3d, b); R(streamer_mode, b); R(esp_chams_alpha, i); R(crosshair_style, i);
        R(veh_horn_boost, b);
    #undef R
        {
            auto it = kv.find("favs");
            if (it != kv.end()) ollie::favs::deserialize(it->second);
        }

        friends_set.clear();
        auto it = kv.find("friends");
        if (it != kv.end()) {
            std::stringstream ss(it->second);
            std::string tok;
            while (std::getline(ss, tok, ',')) {
                try { friends_set.insert(std::stoi(tok)); } catch (...) {}
            }
        }
        for (int i = 0; i < ollie::keys::slot_count(); ++i) {
            char key[16]; _snprintf_s(key, sizeof(key), "kb_%d", i);
            auto jt = kv.find(key);
            if (jt != kv.end()) {
                try { ollie::keys::slot(i).vk = std::stoi(jt->second); } catch (...) {}
            }
        }
        ollie::log::line("config: loaded (%zu friends)", friends_set.size());
    }
}
