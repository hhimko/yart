////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <memory>

#include "yart/core/renderer.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART main application singleton
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Application {
    public:
        /// @brief Get the static instance, lazily initialized on first call
        /// @return Static Application instance
        static Application& Get()
        {
            static Application instance; // Instantiated on first call and guaranteed to be destroyed on exit 
            return instance;
        }

        /// @brief Query whether the YART application is currently running in debug mode
        /// @return Whether the application is in debug mode 
        /// @note This can also be queried in code using the `YART_DEBUG` macro
        static constexpr bool InDebugMode()
        {
            #ifdef YART_DEBUG
                return true;
            #else
                return false;
            #endif
        }

        /// @brief Run the application mainloop if not already running
        /// @return Application exit status code 
        int Run();

        /// @brief Request application shutdown
        /// @details This method essentially just terminates the application's mainloop
        void Shutdown()
        {
            m_running = false;
        }

    private:
        Application() = default;
        Application(const Application&) = delete;
        Application& operator=(Application const&) = delete;

        /// @brief Initialize members and hook up event handlers
        /// @return Whether the application has been successfully initialized 
        bool Setup();

        /// @brief Initialize GUI rendering and register custom views
        void SetupGUI();

    private:
        yart::Renderer m_renderer;
        bool m_running = false;

    };
} // namespace yart
