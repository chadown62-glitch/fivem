#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <functional>
#include <type_traits>

namespace ollie::invoker {
    bool init();
    void shutdown();

    using NativeHandler = void(*)(void* ctx);

    NativeHandler get(std::uint64_t hash);
    std::uint64_t* invoke_via_shv(std::uint64_t hash, const std::uint64_t* args, std::uint32_t argc);

    struct Context {
        void*        returns;
        std::uint32_t arg_count;
        void*        args;
        std::uint32_t data_count;
        void*        data;
        std::uint64_t pad[14];
    };

    class Call {
    public:
        Call() { std::memset(m_args, 0, sizeof(m_args)); std::memset(m_ret, 0, sizeof(m_ret)); }

        template<class T> Call& a(T v) {
            static_assert(sizeof(T) <= sizeof(std::uint64_t));
            m_args[m_argc] = 0;
            std::memcpy(&m_args[m_argc], &v, sizeof(T));
            ++m_argc;
            return *this;
        }

        template<class R = std::uint64_t> R call(std::uint64_t hash) {
            auto h = get(hash);
            if (!h) { m_argc = 0; return R{}; }
            // scripthookv path — the sentinel handler routes to invoke_via_shv
            std::uint64_t* r = invoke_via_shv(hash, m_args, m_argc);
            m_argc = 0;
            if constexpr (std::is_same_v<R, void>) {
                return;
            } else {
                R out{};
                if (r) std::memcpy(&out, r, (std::min)(sizeof(R), sizeof(std::uint64_t) * 3));
                return out;
            }
        }
    private:
        std::uint64_t m_args[32];
        std::uint64_t m_ret[3];
        std::uint32_t m_argc = 0;
    };

    template<class R, class... A>
    inline R INVOKE(std::uint64_t h, A... a) {
        Call c;
        (c.a(a), ...);
        return c.template call<R>(h);
    }

    void run_on_script(std::function<void()> f);
    void pump();
}
