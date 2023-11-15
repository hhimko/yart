////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of YART's application singleton 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "application.h"


#include <iostream>
#include <functional>

#include "yart/interface/views/viewport_view.h"
#include "yart/interface/interface.h"
#include "yart/backend/backend.h"
#include "yart/core/viewport.h"
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
            viewport_dirty |= yart::Interface::HandleInputs();

            // Begin recording a new frame
            yart::Backend::NewFrame();

            // Update application state
            yart::Interface::Update();

            // Ray trace the scene onto the main render viewport image on CPU
            yart::Viewport* viewport = yart::Interface::GetRenderViewport();
            viewport_dirty |= m_renderer->Render(viewport);
            if (viewport_dirty) {
                viewport->EnsureRefresh(); // Make sure the viewport image gets refreshed this frame
                viewport_dirty = false;
            }

            // Render the application UI
            viewport_dirty |= yart::Interface::Render();

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

        const char* window_title = InDebugMode() ? YART_WINDOW_TITLE_DEBUG : YART_WINDOW_TITLE;
        if (!yart::Backend::Init(window_title, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        // Load the default scene
        m_renderer->SetScene(m_scene);
        m_scene->LoadDefault();

        return true;
    }

    void Application::SetupGUI()
    {
        yart::Interface::Init();

        // Enable keyboard navigation
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

        // Set Dear ImGui style
        yart::GUI::ApplyCustomStyle();
        yart::GUI::LoadFonts();

        // Register YART GUI callbacks
        // yart::Interface::RegisterCallback(std::bind(&yart::GUI::RendererView::OnRenderViewAxesWindow, m_renderer.get()));

        const ImU32 color_gray = 0xFF6F767D;
        yart::Interface::RegisterInspectorSection("Renderer", ICON_CI_EDIT, color_gray, 
            std::bind(&yart::Interface::RendererView::OnRenderGUI, m_renderer.get())
        );

        yart::Interface::RegisterInspectorSection("World", ICON_CI_GLOBE, color_gray, 
            std::bind(&yart::Interface::WorldView::OnRenderGUI, m_renderer->GetWorld())
        );

        yart::Interface::RegisterInspectorSection("Viewport", ICON_CI_DEVICE_DESKTOP, color_gray, 
            std::bind(&yart::Interface::ViewportView::OnRenderGUI, yart::Interface::GetRenderViewport())
        );
    }
} // namespace yart
