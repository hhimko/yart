////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"


#include <iostream>
#include <functional>

#include "yart/platform/input.h"
#include "yart/GUI/views/renderer_view.h"
#include "yart/GUI/gui.h"


#ifdef YART_DEBUG
    /// @brief YART's application window name
    #define YART_WINDOW_TITLE "Yet Another Ray Tracer (Debug)" 
#else
    /// @brief YART's application window name
    #define YART_WINDOW_TITLE "Yet Another Ray Tracer" 
#endif

/// @brief YART's application default window width in pixels
#define YART_WINDOW_WIDTH 1280 

/// @brief YART's application default window height in pixels
#define YART_WINDOW_HEIGHT 720 


namespace yart
{
    Application &Application::Get()
    {
        static Application instance; // Instantiated on first call and guaranteed to be destroyed on exit 
        return instance;
    }

    int Application::Run()
    {
        YART_ASSERT(!m_running);
        if (!Setup())
            return EXIT_FAILURE;


        yart::Window& window = yart::Window::Get();
        m_running = true; // m_running is indirectly controlled by Application::Shutdown()

        // -- APPLICATION MAINLOOP -- // 
        while (m_running) {
            // Poll and handle incoming events
            glfwPollEvents();

            // Handle user input
            yart::Input::Update();
            yart::GUI::RendererView::HandleInputs(m_renderer);

            // Ray trace the scene on CPU onto the viewport image buffer
            auto viewport = window.GetViewport();
            viewport->Resize(GUI::GetRenderViewportAreaSize());
            viewport->SetPosition(GUI::GetRenderViewportAreaPosition());

            uint32_t image_width, image_height;
            viewport->GetImageSize(&image_width, &image_height);
            
            bool viewport_dirty = m_renderer.Render(viewport->GetImageData(), image_width, image_height);
            if (viewport_dirty)
                viewport->EnsureRefresh(); // Make sure the viewport image gets refreshed  

            // Render and present a frame to a platform window on GPU
            window.Render();
        }

        return EXIT_SUCCESS;
    }

    void Application::Shutdown()
    {
        m_running = false;
    }

    bool Application::Setup()
    {
        yart::Window& window = yart::Window::Get();
        window.SetFontLoadCallback(yart::GUI::LoadFonts);
        if (!window.Init(YART_WINDOW_TITLE, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        // Setup GUI rendering
        window.SetDearImGuiCallback(yart::GUI::Render);  
        yart::GUI::ApplyCustomStyle();

        // Register GUI callbacks
        yart::GUI::RegisterCallback(std::bind(&yart::GUI::RendererView::OnRenderViewAxesWindow, std::ref(m_renderer)));

        const ImU32 color_gray = 0xFF6F767D;
        yart::GUI::RegisterInspectorWindow("Renderer", ICON_CI_EDIT, color_gray, 
            std::bind(&yart::GUI::RendererView::OnRenderGUI, std::ref(m_renderer))
        );

        yart::GUI::RegisterInspectorWindow("Window", ICON_CI_DEVICE_DESKTOP, color_gray, 
            std::bind(&yart::Window::OnImGui, &window)
        );

        return true;
    }
} // namespace yart
