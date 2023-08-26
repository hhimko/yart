////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"


#include <iostream>
#include <functional>

#include "yart/platform/input.h"
#include "GUI/gui.h"


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
            m_renderer.UpdateCamera();

            // Ray trace the scene on CPU onto the viewport image buffer
            auto viewport = window.GetViewport();
            viewport->Resize(GUI::GetMainViewportAreaSize());
            viewport->SetPosition(GUI::GetMainViewportAreaPosition());

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
        const ImU32 gray_col = 0xFF6F767D;
        yart::GUI::RegisterCallback(std::bind(&yart::Renderer::OnGuiViewAxes, &m_renderer));
        yart::GUI::RegisterInspectorWindow(ICON_CI_EDIT, gray_col, "Renderer", std::bind(&yart::Renderer::OnImGui, &m_renderer));
        yart::GUI::RegisterInspectorWindow(ICON_CI_DEVICE_DESKTOP, gray_col, "Window", std::bind(&yart::Window::OnImGui, &window));

        return true;
    }
} // namespace yart
