#ifndef __VELO_FUNCTION
#define __VELO_FUNCTION

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace VELO {

    template<class Signature>
    class __function_value;

    template<typename RP, typename ...ArgTypes>
    class __function_value<RP(ArgTypes...)> {
        struct __concept {
            virtual ~__concept() = default;
            virtual RP invoke(ArgTypes&& ...args) = 0;
            virtual std::unique_ptr<__concept> clone() const = 0;
        };

        template<class Callable>
        struct __model final : __concept {
            Callable callable;

            template<class Fp>
            explicit __model(Fp&& f) : callable(std::forward<Fp>(f)) {}

            RP invoke(ArgTypes&& ...args) override {
                if constexpr (std::is_void_v<RP>) {
                    callable(std::forward<ArgTypes>(args)...);
                } else {
                    return callable(std::forward<ArgTypes>(args)...);
                }
            }

            std::unique_ptr<__concept> clone() const override {
                return std::make_unique<__model>(callable);
            }
        };

        std::unique_ptr<__concept> target;

        std::unique_ptr<__concept> __clone_target() const {
            return target ? target->clone() : nullptr;
        }

    public:
        __function_value() = default;

        template<class Fp>
        explicit __function_value(Fp&& f)
            : target(std::make_unique<__model<std::decay_t<Fp>>>(std::forward<Fp>(f))) {}

        __function_value(const __function_value& other)
            : target(other.__clone_target()) {}

        __function_value(__function_value&& other) noexcept = default;

        __function_value& operator=(const __function_value& other) {
            if (this != &other) {
                target = other.__clone_target();
            }
            return *this;
        }

        __function_value& operator=(__function_value&& other) noexcept = default;

        RP operator()(ArgTypes... args) {
            if (!target) {
                throw std::bad_function_call();
            }

            if constexpr (std::is_void_v<RP>) {
                target->invoke(std::forward<ArgTypes>(args)...);
            } else {
                return target->invoke(std::forward<ArgTypes>(args)...);
            }
        }

        explicit operator bool() const noexcept {
            return static_cast<bool>(target);
        }
    };

    template<class FP>
    class __function;

    template<class RP, typename ...ArgTypes>
    class __function<RP(ArgTypes...)>  {
        __function_value<RP(ArgTypes...)> func;

        public:
        __function() = default;

        template<class Fp>
        explicit __function(Fp&& fc) : func(std::forward<Fp>(fc)) {}
        __function(const __function& other) = default;
        __function(__function&& other) noexcept = default;
        __function& operator=(const __function& other) = default;
        __function& operator=(__function&& other) noexcept = default;

        inline RP operator()(ArgTypes... args) {
            return func(std::forward<ArgTypes>(args)...);
        }

        inline explicit operator bool() const noexcept {
            return static_cast<bool>(func);
        }
    };

    template<class Fp>
    using Function = __function<Fp>;
}

#endif
