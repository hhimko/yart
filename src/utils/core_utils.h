#pragma once

#include <functional>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Core Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#define UNUSED(...) (void)sizeof(__VA_ARGS__)


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Core Utility Functions 
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace yart
{
    namespace utils
    {
        /* Stack-based object lifetime management structure */
        class LTStack {
        public:
            LTStack() = default;
            LTStack(const LTStack&) = delete;
            LTStack& operator=(LTStack const&) = delete;
            ~LTStack() { Release(); }

            template<typename T>
            void Push(T var, std::function<void(T)>&& dtor) {
                Push((void*)var, [dtor](void* var){ dtor( (T)var ); }); 
            }

            bool Pop() {
                if (m_slots.empty()) 
                    return false;

                Slot& slot = m_slots[m_slots.size() - 1];
                slot.dtor(slot.var);
                m_slots.pop_back();

                return true;
            }

            void Release() {
                while(Pop()) {};
            }

        private:
            void Push(void* var, std::function<void(void*)>&& dtor) {
                m_slots.emplace_back( var, dtor );
            };

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
