////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART application UI rendering module public interface
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface.h"


#include "yart/interface/panels/panel_settings.h"
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
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ctx->currentLayoutType = LayoutType::DEFAULT;

        // Load and attach the default UI layout
        Interface::ApplyLayout(ctx->currentLayoutType);
    }

    void Interface::ApplyLayout(LayoutType type)
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ctx->currentLayoutType = type;

        // Detach the layout early, for current panel states to save
        RootAppPanel::Get()->DetachLayout();

        switch (type) {
        case LayoutType::DEFAULT:
            return ApplyDefaultLayout();
        case LayoutType::FULLSCREEN:
            return ApplyFullscreenLayout();
        default:
            YART_UNREACHABLE();
        }
    }

    bool Interface::HandleInputs()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();

        // Update application inputs state
        yart::GUI::Input::Update();

        RootAppPanel* rap = RootAppPanel::Get();
        bool made_changes = rap->HandleInputs();

        ctx->shouldRefreshViewports |= made_changes;
        return made_changes;
    }

    bool Interface::Render()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();

        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();

        float fps = ImGui::GetCurrentContext()->IO.Framerate;
        ImGui::Text("%.1f FPS", fps);

        // Render the YART application UI layout
        float menu_bar_height = Interface::RenderMainMenuBar();

        RootAppPanel* rap = RootAppPanel::Get();
        bool made_changes = rap->Render(menu_bar_height);

        // Render registered global callbacks
        for (auto callback : ctx->registeredCallbacks)
            made_changes |= callback();

        ctx->shouldRefreshViewports = made_changes;
        return made_changes;
    }

    void Interface::Shutdown()
    {
        // Detach and destroy the current layout 
        RootAppPanel::Get()->DetachLayout();

        // Clear all saved panel state
        PanelSettings::ClearPanelSettings();
    }

} // namespace yart
