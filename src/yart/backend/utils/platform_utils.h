////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART platform helper utility classes and macros
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <functional>
#include <assert.h>
#include <vector>
#include <cstdio>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define YART_STRINGIFY(x) #x

    #define YART_SUPPRESS_PUSH(code) \
        _Pragma("warning(push)")     \
        _Pragma(YART_STRINGIFY(warning(disable: ## code)))

    #define YART_SUPPRESS_POP() \
        _Pragma("warning(pop)") 

    #define YART_ARRAYSIZE(arr) (sizeof(arr) / sizeof(*(arr)))
    #define YART_UNUSED(...) (void)sizeof(__VA_ARGS__)

    #define YART_ASSERT(expr) assert(expr) 
    #define YART_ABORT(msg) assert(0 && msg)
    #define YART_UNREACHABLE() YART_ABORT("Reached unreachable section")

    #define YART_LOG_ERR(format, ...) fprintf(stderr, format, __VA_ARGS__)
#endif // #ifndef DOXYGEN_EXCLUDE


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART helper utility functions 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace utils
    {
        /// @brief Stack-based object lifetime management structure
        class LTStack {
        public:
            LTStack() = default;
            LTStack(const LTStack&) = delete;
            LTStack& operator=(LTStack const&) = delete;
            ~LTStack() { Release(); }

            /// @brief Push a new managed object into the stack with a given destructor 
            /// @tparam T Type of the managed object. Must be a pointer type 
            /// @param var Object to be managed
            /// @param dtor Custom destructor for var
            template<typename T>
            void Push(T var, std::function<void(T)>&& dtor) {
                static_assert(std::is_pointer<T>::value, "Expected a pointer");
                PushImpl((void*)var, [dtor](void* var){ dtor( (T)var ); }); 
            }

            /// @brief Pop the last pushed object onto the stack, effectively freeing the object. 
            ///     It's safe to call this function when the stack in empty
            /// @return Whether the stack was not empty and the item was successfully popped
            bool Pop() {
                if (m_slots.empty()) 
                    return false;

                Slot& slot = m_slots[m_slots.size() - 1];
                slot.dtor(slot.var);
                m_slots.pop_back();

                return true;
            }

            /// @brief Free all objects managed by the stack, effectively emptying the stack
            void Release() {
                while(Pop());
            }

        private:
            void PushImpl(void* var, std::function<void(void*)>&& dtor) {
                m_slots.emplace_back(var, dtor);
            }

        private:
            struct Slot {
                void* var;
                std::function<void(void*)> dtor;

                Slot(void* var, std::function<void(void*)>& dtor)
                    : var(var), dtor(dtor) {};
            };

            std::vector<Slot> m_slots;
            
        };

    } // namespace utils
} // namespace yart