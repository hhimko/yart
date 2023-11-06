////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief LTStack memory management class definition for object allocation unwinding 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <functional>
#include <vector>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART memory management helper definitions
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace memory
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Stack-based object lifetime management structure
        /// @details Used for "unwinding" object allocations in reverse order to their allocation, 
        ///     for objects that might be dependant of each other
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class LTStack {
        public:
            LTStack() = default;
            LTStack(const LTStack&) = delete;
            LTStack& operator=(LTStack const&) = delete;
            ~LTStack() { Release(); }

            /// @brief Push a new managed object into the stack with a given destructor 
            /// @tparam T Type of the managed object. Must be a pointer type 
            /// @param var Pointer to the managed object
            /// @param dtor Custom destructor for `var`
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

    } // namespace memory
} // namespace yart