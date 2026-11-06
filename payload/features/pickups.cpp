// pickups.cpp -- spawn ambient money / armour / health pickups at the player
#include "../invoker/natives.h"
#include <Windows.h>
#include <string>

namespace ollie::features {
    static void spawn_pickup(const char* pickup, int value, const char* model) {
        ollie::invoker::run_on_script([ph = std::string(pickup), mh = std::string(model), value] {
            int ped = N::PLAYER_PED_ID();
            float x = 0, y = 0, z = 0;
            N::GET_ENTITY_COORDS(ped, true, &x, &y, &z);
            std::uint32_t mod = N::GET_HASH_KEY(mh.c_str());
            N::REQUEST_MODEL(mod);
            for (int i = 0; i < 40 && !N::HAS_MODEL_LOADED(mod); ++i) Sleep(16);
            N::CREATE_AMBIENT_PICKUP(N::GET_HASH_KEY(ph.c_str()),
                                     x + 1.f, y, z + 0.5f, 0, value, mod, true, true);
            N::SET_MODEL_AS_NO_LONGER_NEEDED(mod);
        });
    }

    void drop_money(int amount) {
        spawn_pickup("PICKUP_MONEY_CASE", amount, "prop_money_bag_01");
    }
    void drop_health() {
        spawn_pickup("PICKUP_HEALTH_STANDARD", 200, "prop_ld_health_pack");
    }
    void drop_armour() {
        spawn_pickup("PICKUP_ARMOUR_STANDARD", 200, "prop_armour_pickup");
    }

    void give_max_armour() {
        ollie::invoker::run_on_script([] {
            int ped = N::PLAYER_PED_ID();
            N::SET_PED_ARMOUR(ped, 200);
        });
    }
}
