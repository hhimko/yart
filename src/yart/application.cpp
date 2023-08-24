////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"


#include <iostream>
#include <functional>

#include "GUI/gui.h"
#include "yart/platform/input.h"


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
        if (!window.Init(YART_WINDOW_TITLE, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        // Setup GUI rendering
        window.SetDearImGuiCallback(yart::GUI::Render);  
        yart::GUI::ApplyCustomStyle();

        // Register GUI callbacks
        yart::GUI::RegisterCallback(std::bind(&yart::Renderer::OnGuiViewAxes, &m_renderer));
        yart::GUI::RegisterWindow("Renderer", std::bind(&yart::Renderer::OnGuiWindow, &m_renderer));

        return true;
    }
} // namespace yart
