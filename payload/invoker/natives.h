#pragma once
#include "invoker.h"
#include "../core/offsets.h"
#include <cstdint>

namespace N {
    using ollie::invoker::INVOKE;
    using namespace ollie::offsets;

    // --- player / ped ---
    inline int   PLAYER_ID()                                     { return INVOKE<int>(h_PLAYER_ID); }
    inline int   PLAYER_PED_ID()                                 { return INVOKE<int>(h_PLAYER_PED_ID); }
    inline int   GET_PLAYER_PED(int p)                           { return INVOKE<int>(h_GET_PLAYER_PED, p); }
    inline bool  IS_PLAYER_ACTIVE(int p)                         { return INVOKE<bool>(h_IS_PLAYER_ACTIVE, p); }
    inline const char* GET_PLAYER_NAME(int p)                    { return INVOKE<const char*>(h_GET_PLAYER_NAME, p); }
    inline bool  IS_PLAYER_DEAD(int p)                           { return INVOKE<bool>(h_IS_PLAYER_DEAD, p); }
    inline bool  IS_PED_IN_ANY_VEHICLE(int ped, bool atStop)     { return INVOKE<bool>(h_IS_PED_IN_ANY_VEHICLE, ped, atStop); }
    inline int   GET_VEHICLE_PED_IS_IN(int ped, bool last)       { return INVOKE<int>(h_GET_VEHICLE_PED_IS_IN, ped, last); }
    inline void  SET_PED_CAN_RAGDOLL(int p, bool b)              { INVOKE<void>(h_SET_PED_CAN_RAGDOLL, p, b); }
    inline void  SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE(int p, int s){ INVOKE<void>(h_SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE, p, s); }
    inline void  SET_PED_RESET_FLAG(int p, int f, bool b)        { INVOKE<void>(h_SET_PED_RESET_FLAG, p, f, b); }

    // --- entity ---
    inline void  SET_ENTITY_INVINCIBLE(int e, bool b)            { INVOKE<void>(h_SET_ENTITY_INVINCIBLE, e, b); }
    inline void  SET_ENTITY_COORDS_NO_OFFSET(int e, float x, float y, float z, bool a, bool b, bool c) {
        INVOKE<void>(h_SET_ENTITY_COORDS_NO_OFFSET, e, x, y, z, a, b, c);
    }
    inline void  GET_ENTITY_COORDS(int e, bool alive, float* x, float* y, float* z) {
        INVOKE<void>(h_GET_ENTITY_COORDS, e, alive, x, y, z);
    }
    inline void  SET_ENTITY_HEALTH(int e, int hp)                { INVOKE<void>(h_SET_ENTITY_HEALTH, e, hp); }
    inline int   GET_ENTITY_HEALTH(int e)                        { return INVOKE<int>(h_GET_ENTITY_HEALTH, e); }
    inline void  SET_ENTITY_MAX_HEALTH(int e, int hp)            { INVOKE<void>(h_SET_ENTITY_MAX_HEALTH, e, hp); }
    inline void  SET_ENTITY_VISIBLE(int e, bool v, bool u)       { INVOKE<void>(h_SET_ENTITY_VISIBLE, e, v, u); }
    inline void  FREEZE_ENTITY_POSITION(int e, bool b)           { INVOKE<void>(h_FREEZE_ENTITY_POSITION, e, b); }
    inline void  SET_ENTITY_COLLISION(int e, bool a, bool b)     { INVOKE<void>(h_SET_ENTITY_COLLISION, e, a, b); }
    inline void  SET_ENTITY_VELOCITY(int e, float x, float y, float z) { INVOKE<void>(h_SET_ENTITY_VELOCITY, e, x, y, z); }
    inline void  GET_ENTITY_VELOCITY(int e, float* x, float* y, float* z) { INVOKE<void>(h_GET_ENTITY_VELOCITY, e, x, y, z); }
    inline float GET_ENTITY_HEADING(int e)                       { return INVOKE<float>(h_GET_ENTITY_HEADING, e); }
    inline void  SET_ENTITY_HEADING(int e, float h)              { INVOKE<void>(h_SET_ENTITY_HEADING, e, h); }
    inline bool  IS_ENTITY_ON_SCREEN(int e)                      { return INVOKE<bool>(h_IS_ENTITY_ON_SCREEN, e); }
    inline void  GET_PED_BONE_COORDS(int ped, int bone, float ox, float oy, float oz, float* x, float* y, float* z) {
        INVOKE<void>(h_GET_PED_BONE_COORDS, ped, bone, ox, oy, oz, x, y, z);
    }
    inline void  APPLY_FORCE_TO_ENTITY(int e, int ft, float x, float y, float z, float ox, float oy, float oz,
                                       int bone, bool lr, bool ic, bool rf, bool pb, bool loc) {
        INVOKE<void>(h_APPLY_FORCE_TO_ENTITY, e, ft, x, y, z, ox, oy, oz, bone, lr, ic, rf, pb, loc);
    }
    inline void  DELETE_ENTITY(int* e)                           { INVOKE<void>(h_DELETE_ENTITY, e); }
    inline void  SET_ENTITY_ALPHA(int e, int a, bool s)          { INVOKE<void>(h_SET_ENTITY_ALPHA, e, a, s); }
    inline void  SET_ENTITY_DRAWS_IN_FRONT(int e, bool b)        { INVOKE<void>(h_SET_ENTITY_DRAWS_IN_FRONT, e, b); }

    // --- models / spawn ---
    inline void  REQUEST_MODEL(std::uint32_t h)                  { INVOKE<void>(h_REQUEST_MODEL, h); }
    inline bool  HAS_MODEL_LOADED(std::uint32_t h)               { return INVOKE<bool>(h_HAS_MODEL_LOADED, h); }
    inline void  SET_MODEL_AS_NO_LONGER_NEEDED(std::uint32_t h)  { INVOKE<void>(h_SET_MODEL_AS_NO_LONGER_NEEDED, h); }
    inline int   CREATE_VEHICLE(std::uint32_t h, float x, float y, float z, float hd, bool n, bool s, bool t) {
        return INVOKE<int>(h_CREATE_VEHICLE, h, x, y, z, hd, n, s, t);
    }
    inline int   CREATE_PED(int type, std::uint32_t m, float x, float y, float z, float hd, bool n, bool s) {
        return INVOKE<int>(h_CREATE_PED, type, m, x, y, z, hd, n, s);
    }

    // --- vehicle ---
    inline void  SET_VEHICLE_FIXED(int v)                        { INVOKE<void>(h_SET_VEHICLE_FIXED, v); }
    inline void  SET_VEHICLE_ENGINE_HEALTH(int v, float h)       { INVOKE<void>(h_SET_VEHICLE_ENGINE_HEALTH, v, h); }
    inline void  SET_VEHICLE_ENGINE_ON(int v, bool on, bool inst, bool disable) { INVOKE<void>(h_SET_VEHICLE_ENGINE_ON, v, on, inst, disable); }
    inline void  SET_VEHICLE_FORWARD_SPEED(int v, float s)       { INVOKE<void>(h_SET_VEHICLE_FORWARD_SPEED, v, s); }
    inline void  SET_PED_INTO_VEHICLE(int ped, int veh, int seat){ INVOKE<void>(h_SET_PED_INTO_VEHICLE, ped, veh, seat); }
    inline int   GET_PED_IN_VEHICLE_SEAT(int v, int seat)        { return INVOKE<int>(h_GET_PED_IN_VEHICLE_SEAT, v, seat); }
    inline void  SET_VEHICLE_GRAVITY(int v, bool b)              { INVOKE<void>(h_SET_VEHICLE_GRAVITY, v, b); }
    inline void  SET_HELI_BLADES_FULL_SPEED(int v)               { INVOKE<void>(h_SET_HELI_BLADES_FULL_SPEED, v); }
    inline void  SET_VEHICLE_NEON_ENABLED(int v, int idx, bool b){ INVOKE<void>(h_SET_VEHICLE_NEON_ENABLED, v, idx, b); }
    inline void  SET_VEHICLE_NEON_COLOUR(int v, int r, int g, int b) { INVOKE<void>(h_SET_VEHICLE_NEON_COLOUR, v, r, g, b); }
    inline void  MODIFY_VEHICLE_TOP_SPEED(int v, float m)        { INVOKE<void>(h_MODIFY_VEHICLE_TOP_SPEED, v, m); }

    // --- weapons ---
    inline void  GIVE_WEAPON_TO_PED(int ped, std::uint32_t wh, int ammo, bool hidden, bool equip) {
        INVOKE<void>(h_GIVE_WEAPON_TO_PED, ped, wh, ammo, hidden, equip);
    }
    inline void  REMOVE_WEAPON_FROM_PED(int ped, std::uint32_t wh) { INVOKE<void>(h_REMOVE_WEAPON_FROM_PED, ped, wh); }
    inline void  REMOVE_ALL_PED_WEAPONS(int ped, bool b)         { INVOKE<void>(h_REMOVE_ALL_PED_WEAPONS, ped, b); }
    inline void  SET_PED_INFINITE_AMMO_CLIP(int ped, bool b)     { INVOKE<void>(h_SET_PED_INFINITE_AMMO_CLIP, ped, b); }
    inline void  SET_PED_INFINITE_AMMO(int ped, bool b, std::uint32_t wh) { INVOKE<void>(h_SET_PED_INFINITE_AMMO, ped, b, wh); }
    inline bool  GET_CURRENT_PED_WEAPON(int ped, std::uint32_t* wh, bool inv) { return INVOKE<bool>(h_GET_CURRENT_PED_WEAPON, ped, wh, inv); }
    inline void  SET_CURRENT_PED_WEAPON(int ped, std::uint32_t wh, bool force){ INVOKE<void>(h_SET_CURRENT_PED_WEAPON, ped, wh, force); }
    inline void  SET_PED_SHOOTS_AT_COORD(int ped, float x, float y, float z, bool b) {
        INVOKE<void>(h_SET_PED_SHOOTS_AT_COORD, ped, x, y, z, b);
    }
    inline void  SET_AMMO_IN_CLIP(int ped, std::uint32_t wh, int ammo) { INVOKE<void>(h_SET_AMMO_IN_CLIP, ped, wh, ammo); }
    inline void  SET_EXPLOSIVE_AMMO_THIS_FRAME(int p)            { INVOKE<void>(h_SET_EXPLOSIVE_AMMO_THIS_FRAME, p); }
    inline void  SET_FIRE_AMMO_THIS_FRAME(int p)                 { INVOKE<void>(h_SET_FIRE_AMMO_THIS_FRAME, p); }
    inline void  SET_EXPLOSIVE_MELEE_THIS_FRAME(int p)           { INVOKE<void>(h_SET_EXPLOSIVE_MELEE_THIS_FRAME, p); }
    inline void  SET_SUPER_JUMP_THIS_FRAME(int p)                { INVOKE<void>(h_SET_SUPER_JUMP_THIS_FRAME, p); }
    inline void  ADD_EXPLOSION(float x, float y, float z, int type, float dmg, bool aud, bool inv, float cam) {
        INVOKE<void>(h_ADD_EXPLOSION, x, y, z, type, dmg, aud, inv, cam);
    }
    inline void  SHOOT_SINGLE_BULLET_BETWEEN(float sx, float sy, float sz,
                                             float ex, float ey, float ez,
                                             int dmg, bool p1, std::uint32_t wh,
                                             int owner, bool audible, bool invis, float speed) {
        INVOKE<void>(h_SHOOT_SINGLE_BULLET_BETWEEN, sx, sy, sz, ex, ey, ez, dmg, p1, wh, owner, audible, invis, speed);
    }
    inline void  SET_WEAPON_DAMAGE_MOD(std::uint32_t wh, float mult) { INVOKE<void>(h_SET_WEAPON_DAMAGE_MOD, wh, mult); }
    inline void  CLEAR_PED_TASKS_IMMEDIATELY(int ped)            { INVOKE<void>(h_CLEAR_PED_TASKS_IMMEDIATELY, ped); }
    inline int   CREATE_AMBIENT_PICKUP(std::uint32_t pickupH, float x, float y, float z,
                                       int p1, int value, std::uint32_t modelH, bool p2, bool p3) {
        return INVOKE<int>(h_CREATE_AMBIENT_PICKUP, pickupH, x, y, z, p1, value, modelH, p2, p3);
    }
    inline void  ADD_ARMOUR_TO_PED(int ped, int a)               { INVOKE<void>(h_ADD_ARMOUR_TO_PED, ped, a); }
    inline void  SET_PED_ARMOUR(int ped, int a)                  { INVOKE<void>(h_SET_PED_ARMOUR, ped, a); }
    inline void  SET_PLAYER_MODEL(int p, std::uint32_t h)        { INVOKE<void>(h_SET_PLAYER_MODEL, p, h); }
    inline void  SET_PED_DEFAULT_COMPONENT_VAR(int ped)          { INVOKE<void>(h_SET_PED_DEFAULT_COMPONENT_VAR, ped); }
    inline bool  IS_MODEL_IN_CDIMAGE(std::uint32_t h)            { return INVOKE<bool>(h_IS_MODEL_IN_CDIMAGE, h); }
    inline bool  IS_MODEL_VALID(std::uint32_t h)                 { return INVOKE<bool>(h_IS_MODEL_VALID, h); }
    inline void  SET_VEHICLE_DOORS_LOCKED(int v, int state)      { INVOKE<void>(h_SET_VEHICLE_DOORS_LOCKED, v, state); }
    inline void  SET_VEHICLE_DOOR_OPEN(int v, int idx, bool loose, bool shut) { INVOKE<void>(h_SET_VEHICLE_DOOR_OPEN, v, idx, loose, shut); }
    inline void  SET_VEHICLE_DOOR_SHUT(int v, int idx, bool imm) { INVOKE<void>(h_SET_VEHICLE_DOOR_SHUT, v, idx, imm); }
    inline std::uint32_t GET_ENTITY_MODEL(int e)                 { return INVOKE<std::uint32_t>(h_GET_ENTITY_MODEL, e); }
    inline void  GET_MODEL_DIMENSIONS(std::uint32_t m, float* mn, float* mx) {
        INVOKE<void>(h_GET_MODEL_DIMENSIONS, m, mn, mx);
    }
    inline void  SET_PED_RELATIONSHIP_GROUP(int ped, std::uint32_t grp) { INVOKE<void>(h_SET_PED_RELATIONSHIP_GROUP, ped, grp); }
    inline void  SET_PED_AS_ENEMY(int ped, bool b)               { INVOKE<void>(h_SET_PED_AS_ENEMY, ped, b); }
    inline void  SET_PED_COMBAT_ATTRIBUTES(int ped, int idx, bool b){ INVOKE<void>(h_SET_PED_COMBAT_ATTRIBUTES, ped, idx, b); }
    inline void  TASK_COMBAT_PED(int ped, int target, int p1, int p2){ INVOKE<void>(h_TASK_COMBAT_PED, ped, target, p1, p2); }

    // --- cams ---
    inline void  SET_CAM_ACTIVE(int c, bool b)                   { INVOKE<void>(h_SET_CAM_ACTIVE, c, b); }
    inline void  RENDER_SCRIPT_CAMS(bool a, bool b, int t, bool c, bool d) {
        INVOKE<void>(h_RENDER_SCRIPT_CAMS, a, b, t, c, d);
    }
    inline int   CREATE_CAM(const char* n, bool a)               { return INVOKE<int>(h_CREATE_CAM, n, a); }
    inline void  SET_CAM_COORD(int c, float x, float y, float z) { INVOKE<void>(h_SET_CAM_COORD, c, x, y, z); }
    inline void  SET_CAM_ROT(int c, float x, float y, float z, int ord) {
        INVOKE<void>(h_SET_CAM_ROT, c, x, y, z, ord);
    }
    inline void  DESTROY_CAM(int c, bool b)                      { INVOKE<void>(h_DESTROY_CAM, c, b); }
    inline void  GET_GAMEPLAY_CAM_ROT(int order, float* x, float* y, float* z) {
        INVOKE<void>(h_GET_GAMEPLAY_CAM_ROT, order, x, y, z);
    }
    inline void  GET_GAMEPLAY_CAM_COORD(float* x, float* y, float* z) {
        INVOKE<void>(h_GET_GAMEPLAY_CAM_COORD, x, y, z);
    }
    inline float GET_GAMEPLAY_CAM_FOV()                          { return INVOKE<float>(h_GET_GAMEPLAY_CAM_FOV); }

    // --- hud / map ---
    inline bool  IS_WAYPOINT_ACTIVE()                            { return INVOKE<bool>(h_IS_WAYPOINT_ACTIVE); }
    inline int   GET_FIRST_BLIP_INFO_ID(int k)                   { return INVOKE<int>(h_GET_FIRST_BLIP_INFO_ID, k); }
    inline void  GET_BLIP_INFO_ID_COORD(int b, float* x, float* y, float* z) {
        INVOKE<void>(h_GET_BLIP_INFO_ID_COORD, b, x, y, z);
    }
    inline bool  GET_SCREEN_COORD_FROM_WORLD(float wx, float wy, float wz, float* sx, float* sy) {
        return INVOKE<bool>(h_GET_SCREEN_COORD_FROM_WORLD, wx, wy, wz, sx, sy);
    }

    // --- wanted ---
    inline void  SET_MAX_WANTED_LEVEL(int l)                     { INVOKE<void>(h_SET_MAX_WANTED_LEVEL, l); }
    inline void  SET_PLAYER_WANTED_LEVEL(int p, int l, bool b)   { INVOKE<void>(h_SET_PLAYER_WANTED_LEVEL, p, l, b); }
    inline void  SET_PLAYER_WANTED_LEVEL_NOW(int p, bool b)      { INVOKE<void>(h_SET_PLAYER_WANTED_LEVEL_NOW, p, b); }

    // --- world / weather / time ---
    inline void  NETWORK_OVERRIDE_CLOCK_TIME(int h, int m, int s){ INVOKE<void>(h_NETWORK_OVERRIDE_CLOCK_TIME, h, m, s); }
    inline void  SET_WEATHER_TYPE_NOW(const char* w)             { INVOKE<void>(h_SET_WEATHER_TYPE_NOW, w); }
    inline void  SET_WEATHER_TYPE_PERSIST(const char* w)         { INVOKE<void>(h_SET_WEATHER_TYPE_PERSIST, w); }
    inline void  CLEAR_AREA_OF_PEDS(float x, float y, float z, float r, int f) { INVOKE<void>(h_CLEAR_AREA_OF_PEDS, x, y, z, r, f); }
    inline void  CLEAR_AREA_OF_VEHICLES(float x, float y, float z, float r, bool a, bool b, bool c, bool d, bool e) {
        INVOKE<void>(h_CLEAR_AREA_OF_VEHICLES, x, y, z, r, a, b, c, d, e);
    }

    // --- utility ---
    inline std::uint32_t GET_HASH_KEY(const char* s)             { return INVOKE<std::uint32_t>(h_GET_HASH_KEY, s); }
    inline int   GET_GAME_TIMER()                                { return INVOKE<int>(h_GET_GAME_TIMER); }
    inline void  NOTIFY(const char* s) {
        INVOKE<void>(h_NOTIFY_PREP, "STRING");
        INVOKE<void>(h_ADD_TEXT_COMPONENT_STRING, s);
        INVOKE<int>(h_DRAW_NOTIFICATION, false, true);
    }
}
