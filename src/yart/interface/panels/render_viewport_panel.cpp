////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the RenderViewportPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "render_viewport_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"


namespace yart
{
    namespace Interface
    {
        bool RenderViewportPanel::Render(ImGuiWindow* window)
        {
            InterfaceContext* ctx = Interface::GetInterfaceContext();

            // Store the viewport window ID and bounding box
            ctx->renderViewportWindowID = window->ID;
            ctx->renderViewportArea = window->Rect();

            // Render the viewport image
            ImTextureID viewport_texture = ctx->renderViewport->GetImTextureID();
            ImGui::GetBackgroundDrawList()->AddImage(viewport_texture, ctx->renderViewportArea.Min, ctx->renderViewportArea.Max);

            return false;
        }

    } // namespace Interface
} // namespace yart
