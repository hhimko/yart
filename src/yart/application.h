////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <memory>

#include "yart/platform/window.h"
#include "yart/core/renderer.h"


#define YART_WINDOW_TITLE "Yet Another Ray Tracer"
#define YART_WINDOW_WIDTH 1280
#define YART_WINDOW_HEIGHT 720


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART main application singleton
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Application {
    public:
        Application(const Application&) = delete;
        Application& operator=(Application const&) = delete;

        /// @brief Get the static singleton instance, lazily initialized on first call
        /// @return Static Application instance
        static Application& Get();

        /// @brief Run the application mainloop if not already running
        /// @return Application success code 
        int Run();

        /// @brief Request application shutdown and cleanup
        void Shutdown();

    private:
        Application() = default;

        /// @brief Initialize members and hook up event handlers
        /// @return Boolean value indicating whether the application has been successfully initialized 
        bool Setup();

    private:
        bool m_running = false;

        yart::Renderer m_renderer;

    };
} // namespace yart
