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

} // namespace yart
