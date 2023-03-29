#pragma once

#include <functional>
#include <assert.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Core Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#define YART_STRINGIFY(x) #x

#define YART_SUPPRESS_PUSH(code) \
    _Pragma("warning(push)")   \
    _Pragma(YART_STRINGIFY(warning(disable: ## code)))

#define YART_SUPPRESS_POP() \
    _Pragma("warning(pop)") 

#define UNUSED(...) (void)sizeof(__VA_ARGS__)

#define YART_ASSERT(expr) assert(expr)
#define YART_UNREACHABLE() YART_ASSERT(0 && "Reached unreachable section")


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
