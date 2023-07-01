#include "application.h"

#include <iostream>
#include <functional>

#include "input.h"


namespace yart
{
    Application::Application()
    {
        m_window = new yart::Window();
    }

    Application::~Application()
    {
        m_window->Close();
        delete m_window;
    }

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


        m_running = true; // m_running is indirectly controlled by Application::Shutdown()                 

        // -- APPLICATION MAINLOOP -- // 
        while (m_running) {
            // Poll and handle incoming events
            glfwPollEvents();

            // Update the input system
            Input::Update();

            // Ray trace the scene on CPU onto the viewport image buffer
            auto viewport = m_window->GetViewport();

            uint32_t viewport_width, viewport_height;
            viewport->GetImageSize(&viewport_width, &viewport_height);
            
            m_renderer.Render(viewport->GetImageData(), viewport_width, viewport_height);
            viewport->Refresh(m_window); // Refresh display 

            // Render and present a frame to a platform window on GPU
            m_window->Render();
        }

        return EXIT_SUCCESS;
    }

    void Application::Shutdown()
    {
        m_running = false;
    }

    bool Application::Setup()
    {
        if (!m_window->Create(YART_WINDOW_TITLE, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        // Register ImGui windows
        m_window->RegisterImGuiWindow("Renderer", std::bind(&Renderer::OnImGui, &m_renderer));      

        return true;
    }
} // namespace yart
