// skin.cpp -- swap local player ped model
#include "../invoker/natives.h"
#include "../core/notify.h"
#include <Windows.h>
#include <string>

namespace ollie::features {
    void set_local_skin(const char* model) {
        ollie::invoker::run_on_script([mm = std::string(model)] {
            auto h = N::GET_HASH_KEY(mm.c_str());
            if (!N::IS_MODEL_VALID(h) || !N::IS_MODEL_IN_CDIMAGE(h)) {
                ollie::notify::push(std::string("skin invalid: ") + mm);
                return;
            }
            N::REQUEST_MODEL(h);
            for (int i = 0; i < 60 && !N::HAS_MODEL_LOADED(h); ++i) Sleep(16);
            if (!N::HAS_MODEL_LOADED(h)) { ollie::notify::push("skin load timeout"); return; }
            N::SET_PLAYER_MODEL(N::PLAYER_ID(), h);
            N::SET_PED_DEFAULT_COMPONENT_VAR(N::PLAYER_PED_ID());
            N::SET_MODEL_AS_NO_LONGER_NEEDED(h);
            ollie::notify::push(std::string("skin -> ") + mm);
        });
    }
}
