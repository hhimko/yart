////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "context_panel.h"


#include <imgui_internal.h>

#include "yart/interface/views/renderer_view.h"
#include "yart/interface/interface_internal.h"
#include "yart/common/utils/yart_utils.h"
#include "yart/interface/views/view.h"


namespace yart
{
    namespace Interface
    {
        bool ContextPanel::OnRender(Panel** active_panel)
        {
            InterfaceContext* ctx = Interface::GetInterfaceContext();
            ImGuiContext* g = ImGui::GetCurrentContext();

            void* active_view_target = nullptr;
            const View* active_view = RenderContextNavBar(&active_view_target);

            bool made_changes = RenderContextView(active_view, active_view_target);

            return made_changes;
        }

        const Interface::View* ContextPanel::RenderContextNavBar(void** target)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();

            const float window_y_offset = ImGui::GetFrameHeight() - 1.0f;
            static constexpr float icon_button_outer_padding = 3.0f;
            static constexpr float icon_button_inner_padding = 4.0f;
            const float size = GUI::GetIconsFont()->FontSize;
        
            const float window_width = size + 2.0f * icon_button_outer_padding + 2.0f * icon_button_inner_padding;
            static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
            ImGui::BeginChild("##SideNavBar", { window_width, 0.0f }, false, flags);

            // Draw background without right edge rounding
            ImGuiWindow* window = g->CurrentWindow;
            const float child_rounding = g->Style.ChildRounding;
            ImVec2 p_min = { window->Pos.x, window->Pos.y + window_y_offset };
            ImVec2 p_max = { p_min.x + window->Size.x + child_rounding, p_min.y + window->Size.y };

            static const ImU32 bg_col = ImGui::GetColorU32({ YART_GUI_COLOR_DARKEST_GRAY, YART_GUI_ALPHA_OPAQUE });
            window->DrawList->AddRectFilled(p_min, p_max, bg_col, child_rounding);

            // Render menu item icons from active views
            static constexpr float item_spacing = 4.0f; 
            window->DC.CursorPos.y += window_y_offset;

            size_t views_count;
            const Interface::View** views = Interface::View::GetAllViews(&views_count);

            void* active_view_target = nullptr;
            static const View* active_view = nullptr;
            for (size_t i = 0; i < views_count; ++i) {
                const Interface::View* view = views[i];

                // Try to retrieve the view target instance for this frame
                void* target = view->GetViewTarget();
                if (target == nullptr) {
                    // If the view was active last frame, update the active_view variable
                    if (active_view == view)
                        active_view = nullptr;

                    continue;
                }

                window->DC.CursorPos.x += icon_button_outer_padding;
                window->DC.CursorPos.y += item_spacing;

                p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y + 1.0f };
                p_max = { 
                    p_min.x + 2.0f * icon_button_inner_padding + icon_button_outer_padding + size,
                    p_min.y + 2.0f * icon_button_inner_padding + size 
                };

                const ImGuiID id = ImGui::GetID(view->GetName());
                ImGui::ItemAdd({ p_min, p_max }, id);

                bool hovered, held;
                const bool clicked = ImGui::ButtonBehavior({ p_min, p_max }, id, &hovered, &held);

                const bool active = (clicked || view == active_view);
                if (active) {
                    active_view = view;
                    active_view_target = target;
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    ImGui::SetTooltip(view->GetName());

                // Render background
                const ImU32 col = ImGui::GetColorU32(hovered ? ImGuiCol_TabHovered : active ? ImGuiCol_TabActive : ImGuiCol_Tab);
                window->DrawList->AddRectFilled(p_min, p_max, col, child_rounding, ImDrawFlags_RoundCornersLeft);

                // Render the icon
                window->DC.CursorPos.x += icon_button_inner_padding;
                window->DC.CursorPos.y += icon_button_inner_padding;

                ImVec4 backup_text_color = g->Style.Colors[ImGuiCol_Text];
                g->Style.Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(view->GetIconColor());
                GUI::PushIconsFont();

                ImGui::Text(view->GetIcon());

                ImGui::PopFont();
                g->Style.Colors[ImGuiCol_Text] = backup_text_color;
            }

            ImGui::EndChild();

            // Default to the renderer view being active, as it's target is always available 
            if (active_view == nullptr) {
                active_view = dynamic_cast<View*>(Interface::RendererView::Get());
                active_view_target = active_view->GetViewTarget();
            }

            // Sanity checks
            YART_ASSERT(active_view != nullptr);
            YART_ASSERT(active_view_target != nullptr);
            
            *target = active_view_target;
            return active_view;
        }

        bool ContextPanel::RenderContextView(const Interface::View* view, void* target)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            bool made_changes = false;

            if (target == nullptr)
                target = view->GetViewTarget();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
            ImGui::SameLine();
            ImGui::PopStyleVar();

            const char* name;
            ImFormatStringToTempBuffer(&name, nullptr, "%s###ContextTabItem", view->GetName());

            bool open = GUI::BeginTabBar(name);
            {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                // Draw background without left edge rounding
                ImGuiWindow* window = g->CurrentWindow;
                float child_rounding = g->Style.ChildRounding;
                ImVec2 p_min = { window->Pos.x, window->Pos.y };
                ImVec2 p_max = { p_min.x + child_rounding, p_min.y + window->Size.y };

                const ImVec4 backup_clip_rect = g->CurrentWindow->DrawList->_ClipRectStack.back();
                g->CurrentWindow->DrawList->PopClipRect();
                static ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_ChildBg);
                g->CurrentWindow->DrawList->AddRectFilled(p_min, p_max, bg_col);
                g->CurrentWindow->DrawList->PushClipRect({ backup_clip_rect.x, backup_clip_rect.y }, { backup_clip_rect.z, backup_clip_rect.w });

                // Render the currently active context view
                made_changes |= view->Render(target);

                ImGui::EndChild();
                if (open) ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
