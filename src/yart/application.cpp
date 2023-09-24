////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"


#include <iostream>
#include <functional>

#include "yart/GUI/views/renderer_view.h"
#include "yart/backend/backend.h"
#include "yart/GUI/input.h"
#include "yart/GUI/gui.h"


/// @brief YART's application window name
#define YART_WINDOW_TITLE "Yet Another Ray Tracer" 
/// @brief YART's application window name for the debug build
#define YART_WINDOW_TITLE_DEBUG "Yet Another Ray Tracer (Debug)" 
/// @brief YART's application default window width in pixels
#define YART_WINDOW_WIDTH 1280 
/// @brief YART's application default window height in pixels
#define YART_WINDOW_HEIGHT 720 


namespace yart
{
    int Application::Run()
    {
        YART_ASSERT(!m_running);
        if (!Setup())
            return EXIT_FAILURE;


        bool viewport_dirty = true; // Whether the viewport should be refreshed this frame
        m_running = true; // m_running is indirectly controlled by Application::Shutdown()

        // -- APPLICATION MAINLOOP -- // 
        while (m_running) {
            // Poll and handle incoming events
            yart::Backend::PollEvents();
            viewport_dirty |= yart::GUI::RendererView::HandleInputs(m_renderer);

            // Begin recording a new frame
            yart::Backend::NewFrame();

            // Update application state
            yart::GUI::Update();
            yart::GUI::Input::Update();

            // Ray trace the scene onto the main render viewport image on CPU
            yart::Viewport* viewport = GUI::GetRenderViewport();
            viewport_dirty |= m_renderer.Render(viewport);
            if (viewport_dirty) {
                viewport->EnsureRefresh(); // Make sure the viewport image gets refreshed this frame
                viewport_dirty = false;
            }

            // Render application GUI
            viewport_dirty |= yart::GUI::Render();

            // Render and present a new frame to the OS window on GPU
            yart::Backend::Render();
        }

        yart::Backend::Close();
        return EXIT_SUCCESS;
    }

    bool Application::Setup()
    {
        // Set backend event callbacks and initialize the platform window
        yart::Backend::SetDearImGuiSetupCallback(std::bind(&yart::Application::SetupGUI, this));
        yart::Backend::SetWindowCloseCallback(std::bind(&yart::Application::Shutdown, this));

        if (!yart::Backend::Init(InDebugMode() ? YART_WINDOW_TITLE_DEBUG : YART_WINDOW_TITLE, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        return true;
    }

    void Application::SetupGUI()
    {
        // Initialize the YART GUI module
        yart::GUI::Init();

        // Enable keyboard navigation
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

        // Set Dear ImGui style
        yart::GUI::ApplyCustomStyle();
        yart::GUI::LoadFonts();

        // Register YART GUI callbacks
        yart::GUI::RegisterCallback(std::bind(&yart::GUI::RendererView::OnRenderViewAxesWindow, std::ref(m_renderer)));

        const ImU32 color_gray = 0xFF6F767D;
        yart::GUI::RegisterInspectorWindow("Renderer", ICON_CI_EDIT, color_gray, 
            std::bind(&yart::GUI::RendererView::OnRenderGUI, std::ref(m_renderer))
        );

        yart::GUI::RegisterInspectorWindow("World", ICON_CI_GLOBE, color_gray, 
            std::bind(&yart::GUI::WorldView::OnRenderGUI, std::ref(m_renderer.GetWorld()))
        );

        // yart::GUI::RegisterInspectorWindow("Window", ICON_CI_DEVICE_DESKTOP, color_gray, 
        //     std::bind(&yart::Window::OnImGui, &window)
        // );
    }
} // namespace yart
