////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "context_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"


namespace yart
{
    namespace Interface
    {
        bool ContextPanel::Render(ImGuiWindow* window)
        {
            InterfaceContext* ctx = Interface::GetInterfaceContext();
            ImGuiContext* g = ImGui::GetCurrentContext();
            bool made_changes = false;

            RenderContextNavBar();

            InspectorSection* active_section = ctx->activeInspectorSection;

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
            ImGui::SameLine();
            ImGui::PopStyleVar();

            const char* name = "Inspector";
            if (active_section != nullptr)
                ImFormatStringToTempBuffer(&name, nullptr, "%s###InspectorTabItem", active_section->name);

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

                // Render the currently active inspector item
                if (active_section != nullptr)
                    made_changes |= active_section->callback();

                ImGui::EndChild();
                if (open)
                    ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            return made_changes;
        }

        void ContextPanel::RenderContextNavBar()
        {
            InterfaceContext* ctx = Interface::GetInterfaceContext();
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

            // Render menu item icons
            static constexpr float item_spacing = 4.0f; 
            window->DC.CursorPos.y += window_y_offset;

            for (auto&& item : ctx->registeredInspectorSections) {
                window->DC.CursorPos.x += icon_button_outer_padding;
                window->DC.CursorPos.y += item_spacing;

                p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y + 1.0f };
                p_max = { p_min.x + 2.0f * icon_button_inner_padding + icon_button_outer_padding + size, p_min.y + 2.0f * icon_button_inner_padding + size };

                const ImGuiID id = ImGui::GetID(item.name);
                ImGui::ItemAdd({ p_min, p_max }, id);

                bool hovered, held;
                bool clicked = ImGui::ButtonBehavior({ p_min, p_max }, id, &hovered, &held);

                bool active = &item == ctx->activeInspectorSection;
                if (!active && clicked) {
                    ctx->activeInspectorSection = &item;
                    active = true;
                }

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    ImGui::SetTooltip(item.name);

                // Render background
                const ImU32 col = ImGui::GetColorU32(hovered ? ImGuiCol_TabHovered : active ? ImGuiCol_TabActive : ImGuiCol_Tab);
                window->DrawList->AddRectFilled(p_min, p_max, col, child_rounding, ImDrawFlags_RoundCornersLeft);

                // Render the icon
                window->DC.CursorPos.x += icon_button_inner_padding;
                window->DC.CursorPos.y += icon_button_inner_padding;

                ImVec4 backup_text_color = g->Style.Colors[ImGuiCol_Text];
                g->Style.Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(item.color);
                GUI::PushIconsFont();
                ImGui::Text(item.icon);
                ImGui::PopFont();
                g->Style.Colors[ImGuiCol_Text] = backup_text_color;
            }
            
            ImGui::EndChild();
        }

    } // namespace Interface
} // namespace yart
