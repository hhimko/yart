////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART application UI rendering module public interface
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface.h"


#include "yart/interface/interface_internal.h"
#include "yart/GUI/gui.h"


namespace yart
{
    void Interface::Init()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ctx->renderViewport = std::make_unique<yart::Viewport>(1, 1);
    }

    void Interface::RegisterInspectorSection(const char* name, const char* icon, ImU32 color, callback_t callback)
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();

        InspectorSection item;
        item.name = name;
        item.icon = icon;
        item.color = color;
        item.callback = callback;

        ctx->registeredInspectorSections.push_back(item);
        ctx->activeInspectorSection = &ctx->registeredInspectorSections.front();
    }

    void Interface::Update()
    {
        // Refresh and update the context state
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ctx->madeChanges = false;

        // Resize the render viewport
        uint32_t viewport_width = ctx->renderViewportArea.GetWidth();
        uint32_t viewport_height = ctx->renderViewportArea.GetHeight();
        ctx->renderViewport->Resize(viewport_width > 0 ? viewport_width : 1, viewport_height > 0 ? viewport_height : 1);
    }

    bool Interface::Render()
    {
        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();


        float fps = ImGui::GetCurrentContext()->IO.Framerate;
        ImGui::Text("%.1f FPS", fps);

        // Render the YART application UI layout
        float menu_bar_height = Interface::RenderMainMenuBar();
        Interface::RenderMainContentArea(menu_bar_height);

        // Render registered global callbacks
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        for (auto callback : ctx->registeredCallbacks)
            ctx->madeChanges |= callback();

        return ctx->madeChanges;
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


