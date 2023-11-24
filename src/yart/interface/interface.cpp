////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART application UI rendering module public interface
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface.h"


#include "yart/interface/views/renderer_view.h"
#include "yart/interface/interface_internal.h"
#include "yart/interface/panel.h"
#include "yart/application.h"
#include "yart/GUI/input.h"
#include "yart/GUI/gui.h"


namespace yart
{   
    void Interface::Init()
    {
        Interface::ApplyDefaultLayout();

        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ctx->renderViewport = std::make_unique<yart::Viewport>(1, 1, 2);
    }

    bool Interface::HandleInputs()
    {
        // Update application inputs state
        yart::GUI::Input::Update();

        yart::Renderer* renderer = yart::Application::Get().GetRenderer();
        bool made_changes = yart::Interface::RendererView::HandleInputs(renderer);

        return made_changes;
    }

    void Interface::Update()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();

        // Resize the render viewport
        uint32_t viewport_width = ctx->renderViewportArea.GetWidth();
        uint32_t viewport_height = ctx->renderViewportArea.GetHeight();
        ctx->renderViewport->Resize(viewport_width, viewport_height);
    }

    bool Interface::Render()
    {
        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();


        float fps = ImGui::GetCurrentContext()->IO.Framerate;
        ImGui::Text("%.1f FPS", fps);

        // Render the YART application UI layout
        float menu_bar_height = Interface::RenderMainMenuBar();
        bool made_changes = RootAppPanel::Get()->Render(menu_bar_height);

        // Render registered global callbacks
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        for (auto callback : ctx->registeredCallbacks)
            made_changes |= callback();

        return made_changes;
    }

    yart::Viewport* Interface::GetRenderViewport()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        return ctx->renderViewport.get();
    }

    bool Interface::IsMouseOverRenderViewport()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (g->HoveredWindow != nullptr) {
            InterfaceContext* ctx = Interface::GetInterfaceContext();
            return g->HoveredWindow->ID == ctx->renderViewportWindowID;
        }

        return false;
    }

} // namespace yart


