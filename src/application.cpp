#include "application.h"

#include <iostream>


namespace yart
{
    Application::Application()
    {
        m_window = std::make_unique<yart::Window>();
    }

    Application& Application::Get()
    {
        static Application instance; // Instantiated on first call and guaranteed to be destroyed on exit 
        return instance;
    }

    int Application::Run()
    {
        YART_ASSERT(!m_running);

        if (!Setup())
            return EXIT_FAILURE;

        // m_running is indirectly controlled by Application::Shutdown()
        m_running = true; 

        auto image_buffer = std::make_unique<double[]>(100*100*3);

        // -- APPLICATION MAINLOOP -- // 
        while (m_running) {
            // Poll and handle incoming events
            glfwPollEvents();

            // Ray trace the scene on CPU onto a image buffer
            m_renderer.Render(image_buffer.get(), 100, 100);
            // Render and present a frame to a platform window on GPU
            m_window->Render();
        }

        // Cleanup resources after shutting down the mainloop 
        Cleanup();

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

        return true;
    }

    void Application::Cleanup()
    {
        m_window->Close();
    }
} // namespace yart
