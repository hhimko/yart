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


        m_running = true; // m_running is indirectly controlled by Application::Shutdown()

        // -- APPLICATION MAINLOOP -- // 
        while (m_running) {
            // Poll and handle incoming events
            yart::Backend::PollEvents();

            // Handle user input
            yart::GUI::Input::Update();
            m_shouldRefresh |= yart::GUI::RendererView::HandleInputs(m_renderer);

            // Ray trace the scene on CPU onto the viewport image buffer
            // auto viewport = window.GetViewport();
            // viewport->Resize(GUI::GetRenderViewportAreaSize());
            // viewport->SetPosition(GUI::GetRenderViewportAreaPosition());

            // uint32_t image_width, image_height;
            // viewport->GetImageSize(&image_width, &image_height);
            
            // bool viewport_dirty = m_renderer.Render(viewport->GetImageData(), image_width, image_height);
            // if (viewport_dirty || m_shouldRefresh)
            //     viewport->EnsureRefresh(); // Make sure the viewport image gets refreshed  

            // Render and present a frame to a platform window on GPU
            yart::Backend::Render();
        }

        yart::Backend::Close();
        return EXIT_SUCCESS;
    }

    bool Application::Setup()
    {
        // Set backend event callbacks and initialize the platform window
        yart::Backend::SetDearImGuiSetupCallback(std::bind(&yart::Application::SetupGUI, this));
        yart::Backend::SetRenderCallback(std::bind(&yart::Application::OnRender, this));
        yart::Backend::SetWindowCloseCallback(std::bind(&yart::Application::Shutdown, this));

        if (!yart::Backend::Init(InDebugMode() ? YART_WINDOW_TITLE_DEBUG : YART_WINDOW_TITLE, YART_WINDOW_WIDTH, YART_WINDOW_HEIGHT))
            return false;

        return true;
    }

    void Application::SetupGUI()
    {
        // Enable keyboard navigation
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 

        // Set Dear ImGui style
        yart::GUI::ApplyCustomStyle();
        yart::GUI::LoadFonts();

        // Register GUI callbacks
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

    void Application::OnRender()
    {
        // This is called each frame by the platform window
        m_shouldRefresh = false;

        bool made_changes = yart::GUI::Render();
        m_shouldRefresh |= made_changes;
    }
} // namespace yart
