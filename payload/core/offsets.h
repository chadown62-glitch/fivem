// payload/core/offsets.h
#pragma once
#include <cstdint>

namespace ollie::offsets {
    constexpr const char* game_module = "GTA5.dll";

    constexpr const char* sig_native_registration_table = "FILL_SIG";
    constexpr const char* sig_world  = "FILL_SIG";
    constexpr const char* sig_globals = "FILL_SIG";

    // --- player / ped core ---
    constexpr std::uint64_t h_GET_PLAYER_PED              = 0x43A66C31C68491C0ULL;
    constexpr std::uint64_t h_PLAYER_ID                   = 0x4F8644AF03D0E0D6ULL;
    constexpr std::uint64_t h_PLAYER_PED_ID               = 0xD80958FC74E988A6ULL;
    constexpr std::uint64_t h_IS_PLAYER_ACTIVE            = 0xB8215953BBF70EF1ULL;
    constexpr std::uint64_t h_GET_PLAYER_NAME             = 0x6D0DE6A7B5DA71F8ULL;
    constexpr std::uint64_t h_IS_PLAYER_DEAD              = 0x424D4687FA1E5652ULL;
    constexpr std::uint64_t h_IS_PED_IN_ANY_VEHICLE       = 0x997ABD671D25CA0BULL;
    constexpr std::uint64_t h_GET_VEHICLE_PED_IS_IN       = 0x9A9112A0FE9A4713ULL;
    constexpr std::uint64_t h_SET_PED_CAN_RAGDOLL         = 0xB128377056A54E2AULL;
    constexpr std::uint64_t h_SET_PED_CAN_BE_KNOCKED_OFF_VEHICLE = 0x7A6535691B477C48ULL;
    constexpr std::uint64_t h_SET_PED_RESET_FLAG          = 0xC1E8A365BF3B29F2ULL;

    // --- entity ---
    constexpr std::uint64_t h_SET_ENTITY_INVINCIBLE       = 0x3882114BDE571AD4ULL;
    constexpr std::uint64_t h_SET_ENTITY_COORDS_NO_OFFSET = 0x239A3351AC1DA385ULL;
    constexpr std::uint64_t h_SET_ENTITY_COORDS           = 0x06843DA7060A026BULL;
    constexpr std::uint64_t h_GET_ENTITY_COORDS           = 0x3FEF770D40960D5AULL;
    constexpr std::uint64_t h_SET_ENTITY_HEALTH           = 0x6B76DC1F3AE6E6A3ULL;
    constexpr std::uint64_t h_GET_ENTITY_HEALTH           = 0xEEF059FAD016D209ULL;
    constexpr std::uint64_t h_SET_ENTITY_MAX_HEALTH       = 0x166E7CF68597D8B5ULL;
    constexpr std::uint64_t h_SET_ENTITY_VISIBLE          = 0xEA1C610A04DB6BBBULL;
    constexpr std::uint64_t h_FREEZE_ENTITY_POSITION      = 0x428CA6DBD1094446ULL;
    constexpr std::uint64_t h_SET_ENTITY_COLLISION        = 0x1A9205C1B9EE827FULL;
    constexpr std::uint64_t h_SET_ENTITY_VELOCITY         = 0x1C99BB7B6E96D16FULL;
    constexpr std::uint64_t h_GET_ENTITY_VELOCITY         = 0x4805D2B1D8CF94A9ULL;
    constexpr std::uint64_t h_SET_ENTITY_ROTATION         = 0x8524A8B0171D5E07ULL;
    constexpr std::uint64_t h_GET_ENTITY_HEADING          = 0xE83D4F9BA2A38914ULL;
    constexpr std::uint64_t h_SET_ENTITY_HEADING          = 0x8E2530AA8ADA980EULL;
    constexpr std::uint64_t h_IS_ENTITY_ON_SCREEN         = 0xE659E47AF827484BULL;
    constexpr std::uint64_t h_GET_PED_BONE_COORDS         = 0x17C07FC640E86B4EULL;
    constexpr std::uint64_t h_APPLY_FORCE_TO_ENTITY       = 0xC5F68BE9613E2D18ULL;
    constexpr std::uint64_t h_SET_ENTITY_ALPHA            = 0x44A0870B7E92D7C0ULL;
    constexpr std::uint64_t h_SET_ENTITY_DRAWS_IN_FRONT   = 0xA6D31CE61EB8C21DULL;

    // --- models / peds / vehicles ---
    constexpr std::uint64_t h_REQUEST_MODEL                 = 0x963D27A58DF860ACULL;
    constexpr std::uint64_t h_HAS_MODEL_LOADED              = 0x98A4EB5D89A0C952ULL;
    constexpr std::uint64_t h_SET_MODEL_AS_NO_LONGER_NEEDED = 0xE532F5D78798DAABULL;
    constexpr std::uint64_t h_CREATE_VEHICLE                = 0xAF35D0D2583051B0ULL;
    constexpr std::uint64_t h_CREATE_PED                    = 0xD49F9B0955C367DEULL;
    constexpr std::uint64_t h_DELETE_ENTITY                 = 0xAE3CBE5BF394C9C9ULL;
    constexpr std::uint64_t h_SET_VEHICLE_FIXED             = 0x115722B1B9C14C1CULL;
    constexpr std::uint64_t h_SET_VEHICLE_ENGINE_HEALTH     = 0x45F6D8EEF34ABEF1ULL;
    constexpr std::uint64_t h_SET_VEHICLE_ENGINE_ON         = 0x2497C4717C8B881EULL;
    constexpr std::uint64_t h_SET_VEHICLE_FORWARD_SPEED     = 0xAB54A438726D25D5ULL;
    constexpr std::uint64_t h_SET_PED_INTO_VEHICLE          = 0xF75B0D629E1C063DULL;
    constexpr std::uint64_t h_GET_PED_IN_VEHICLE_SEAT       = 0xBB40DD2270B65366ULL;
    constexpr std::uint64_t h_SET_VEHICLE_GRAVITY           = 0x89F149B6131E57DAULL;
    constexpr std::uint64_t h_SET_HELI_BLADES_FULL_SPEED    = 0xA178472EBB8AE60DULL;
    constexpr std::uint64_t h_SET_VEHICLE_NEON_ENABLED      = 0x2AA720E4287BF269ULL;
    constexpr std::uint64_t h_SET_VEHICLE_NEON_COLOUR       = 0x8E0A582209A62695ULL;
    constexpr std::uint64_t h_MODIFY_VEHICLE_TOP_SPEED      = 0x93A3996368C94158ULL;

    // --- weapons ---
    constexpr std::uint64_t h_GIVE_WEAPON_TO_PED            = 0xBF0FD6E56C964FCBULL;
    constexpr std::uint64_t h_REMOVE_WEAPON_FROM_PED        = 0x4899CB088EDF59B8ULL;
    constexpr std::uint64_t h_REMOVE_ALL_PED_WEAPONS        = 0xF25DF915FA38C5F3ULL;
    constexpr std::uint64_t h_SET_PED_INFINITE_AMMO_CLIP    = 0x183DADC6AA953186ULL;
    constexpr std::uint64_t h_SET_PED_INFINITE_AMMO         = 0x3EDCB0505123623BULL;
    constexpr std::uint64_t h_GET_CURRENT_PED_WEAPON        = 0x3A87E44BB9A01D54ULL;
    constexpr std::uint64_t h_SET_CURRENT_PED_WEAPON        = 0xADF692B254977C0CULL;
    constexpr std::uint64_t h_SET_PED_SHOOTS_AT_COORD       = 0x96A05E4FB321B1BAULL;
    constexpr std::uint64_t h_SET_AMMO_IN_CLIP              = 0xDCD2A934D65CB497ULL;
    constexpr std::uint64_t h_ADD_AMMO_TO_PED               = 0x78F0424C34306220ULL;
    constexpr std::uint64_t h_SET_EXPLOSIVE_AMMO_THIS_FRAME = 0xA66C71C98D5F2CFBULL;
    constexpr std::uint64_t h_SET_FIRE_AMMO_THIS_FRAME      = 0x11879CDD803D30F4ULL;
    constexpr std::uint64_t h_SET_EXPLOSIVE_MELEE_THIS_FRAME= 0xFF1BED81BFDC0FE0ULL;
    constexpr std::uint64_t h_SET_SUPER_JUMP_THIS_FRAME     = 0x57FFF03E423A4C0BULL;
    constexpr std::uint64_t h_ADD_EXPLOSION                 = 0xE3AD2BDBAEE269ACULL;
    constexpr std::uint64_t h_SHOOT_SINGLE_BULLET_BETWEEN   = 0x867654CBC7606F2CULL;
    constexpr std::uint64_t h_SET_WEAPON_DAMAGE_MOD         = 0x4757F00BC6323CFEULL;
    constexpr std::uint64_t h_CLEAR_PED_TASKS_IMMEDIATELY   = 0xAAA34F8A7CB32098ULL;
    constexpr std::uint64_t h_CREATE_AMBIENT_PICKUP         = 0x673966A0C0FD7171ULL;
    constexpr std::uint64_t h_ADD_ARMOUR_TO_PED             = 0x5BA652A0CD14DF2FULL;
    constexpr std::uint64_t h_SET_PED_ARMOUR                = 0xCEA04D83135264CCULL;
    constexpr std::uint64_t h_SET_PLAYER_MODEL              = 0x00A1CADD00108836ULL;
    constexpr std::uint64_t h_SET_PED_DEFAULT_COMPONENT_VAR = 0x45EEE61580806D63ULL;
    constexpr std::uint64_t h_IS_MODEL_IN_CDIMAGE           = 0x35B9E0803292B641ULL;
    constexpr std::uint64_t h_IS_MODEL_VALID                = 0xC0296A2EDF545E92ULL;
    constexpr std::uint64_t h_SET_VEHICLE_DOORS_LOCKED      = 0xB664292EAECF7FA6ULL;
    constexpr std::uint64_t h_SET_VEHICLE_DOOR_OPEN         = 0x7C65DAC73C35C862ULL;
    constexpr std::uint64_t h_SET_VEHICLE_DOOR_SHUT         = 0x93D9BD300D7789E5ULL;
    constexpr std::uint64_t h_GET_ENTITY_MODEL              = 0x9F47B058362C84B5ULL;
    constexpr std::uint64_t h_GET_MODEL_DIMENSIONS          = 0x03E8D3D5F549087AULL;
    constexpr std::uint64_t h_GET_ENTITY_MATRIX             = 0xECB2FC7235A7D137ULL;
    constexpr std::uint64_t h_SET_PED_RELATIONSHIP_GROUP    = 0xC80A74AC829DDD92ULL;
    constexpr std::uint64_t h_SET_PED_AS_ENEMY              = 0x02A0C9720B854BFAULL;
    constexpr std::uint64_t h_SET_PED_COMBAT_ATTRIBUTES     = 0x9F7794730795E019ULL;
    constexpr std::uint64_t h_TASK_COMBAT_PED               = 0xF166E48407BAC484ULL;
    constexpr std::uint64_t h_GET_RAYCAST_FROM_SCREEN       = 0x3D87450E15D98694ULL; // placeholder; see note

    // --- cams ---
    constexpr std::uint64_t h_SET_CAM_ACTIVE                = 0x026FB97D0AA7A26FULL;
    constexpr std::uint64_t h_RENDER_SCRIPT_CAMS            = 0x08BBBCE8B16F7C3EULL;
    constexpr std::uint64_t h_CREATE_CAM                    = 0xC3981DCE61D9E13FULL;
    constexpr std::uint64_t h_SET_CAM_COORD                 = 0x4D41783FB745E42EULL;
    constexpr std::uint64_t h_SET_CAM_ROT                   = 0x85973643155D0B07ULL;
    constexpr std::uint64_t h_DESTROY_CAM                   = 0x865908C81A2C22E9ULL;
    constexpr std::uint64_t h_GET_GAMEPLAY_CAM_ROT          = 0x837765A25378F0BBULL;
    constexpr std::uint64_t h_GET_GAMEPLAY_CAM_COORD        = 0x14D6F5678D8F1B37ULL;
    constexpr std::uint64_t h_GET_GAMEPLAY_CAM_FOV          = 0x80EC114669DAEFF4ULL;

    // --- hud / map ---
    constexpr std::uint64_t h_IS_WAYPOINT_ACTIVE            = 0x1DD1F58F493F1DA5ULL;
    constexpr std::uint64_t h_GET_FIRST_BLIP_INFO_ID        = 0x1BEDE233E6CD2A1FULL;
    constexpr std::uint64_t h_GET_BLIP_INFO_ID_COORD        = 0xFA7C7F0AADF25D09ULL;
    constexpr std::uint64_t h_GET_SCREEN_COORD_FROM_WORLD   = 0xF9904D11F1ACBEC3ULL;
    constexpr std::uint64_t h_NOTIFY_PREP                   = 0x202709F4C58A0424ULL;
    constexpr std::uint64_t h_ADD_TEXT_COMPONENT_STRING     = 0x6C188BE134E074AAULL;
    constexpr std::uint64_t h_DRAW_NOTIFICATION             = 0x2ED7843F8F801023ULL;

    // --- wanted ---
    constexpr std::uint64_t h_SET_MAX_WANTED_LEVEL          = 0xAA5F02DB48D704B9ULL;
    constexpr std::uint64_t h_SET_PLAYER_WANTED_LEVEL       = 0xB7A0914B67CD9170ULL;
    constexpr std::uint64_t h_SET_PLAYER_WANTED_LEVEL_NOW   = 0xE0AF41401ADF87E3ULL;

    // --- world / weather / time ---
    constexpr std::uint64_t h_SET_CLOCK_TIME                = 0x47C3B5848C3E45D8ULL;
    constexpr std::uint64_t h_NETWORK_OVERRIDE_CLOCK_TIME   = 0xE679E3E06E363892ULL;
    constexpr std::uint64_t h_SET_WEATHER_TYPE_NOW          = 0x29B487C359E19889ULL;
    constexpr std::uint64_t h_SET_WEATHER_TYPE_PERSIST      = 0x704983DF373B198FULL;
    constexpr std::uint64_t h_CLEAR_AREA_OF_PEDS            = 0xBB8E69F04F00C5B8ULL;
    constexpr std::uint64_t h_CLEAR_AREA_OF_VEHICLES        = 0x01C7B9B38428AEB6ULL;

    // --- utility ---
    constexpr std::uint64_t h_GET_HASH_KEY                  = 0xD24D37CC275948CCULL;
    constexpr std::uint64_t h_GET_GAME_TIMER                = 0x9CD27B0045628463ULL;

    enum Bone : int {
        BONE_HEAD   = 31086,
        BONE_NECK   = 39317,
        BONE_SPINE3 = 24818,
        BONE_PELVIS = 11816,
        BONE_L_HAND = 60309,
        BONE_R_HAND = 57005,
        BONE_L_FOOT = 14201,
        BONE_R_FOOT = 52301,
        BONE_L_ELBOW= 45509,
        BONE_R_ELBOW= 28252,
        BONE_L_KNEE = 63931,
        BONE_R_KNEE = 36864,
    };
}
