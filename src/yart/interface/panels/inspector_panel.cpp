////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "inspector_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"


namespace yart
{
    namespace Interface
    {
        bool InspectorPanel::OnRender(Panel** active_panel)
        {
            if (GUI::BeginTabBar("Scene")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                RenderSceneTab();

                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Object")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened | ImGuiWindowFlags_AlwaysUseWindowPadding;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                ImGui::Text("Hello from the Object tab!");

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            return false;
        }

        void InspectorPanel::RenderSceneTab()
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const size_t num_obj = 10;
            const float max_y = window->Pos.y + window->Size.y;
            for (size_t row = 0; row < num_obj || window->DC.CursorPos.y < max_y; ++row) {
                if (row < num_obj) {
                    RenderObjectTreeRow(row, nullptr);
                } else {
                    RenderObjectTreeRowEmpty(row);
                }
            }
        }

        void InspectorPanel::RenderObjectTreeRow(size_t row, yart::Object* object)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const float row_height = g->Font->FontSize + 2.0f * g->Style.FramePadding.y;
            const ImRect item_rect = { 
                window->DC.CursorPos, 
                { window->Pos.x + window->Size.x, window->DC.CursorPos.y + row_height } 
            };

            const ImVec2 backup_item_spacing = g->Style.ItemSpacing;
            g->Style.ItemSpacing = { 0.0f, 0.0f };

            ImGui::ItemSize(item_rect);

            const ImGuiID id = ImGui::GetID("???");
            ImGui::ItemAdd(item_rect, id);

            const bool hovered = ImGui::IsItemHovered();
            
            g->Style.ItemSpacing = backup_item_spacing;

            const ImU32 col = hovered ? OBJECT_TREE_ROW_BG_COL_HOVER : row % 2 ? OBJECT_TREE_ROW_BG_COL_ODD : OBJECT_TREE_ROW_BG_COL_EVEN;
            window->DrawList->AddRectFilled(item_rect.Min, item_rect.Max, col);
        }

        void InspectorPanel::RenderObjectTreeRowEmpty(size_t row)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const float row_height = g->Font->FontSize + 2.0f * g->Style.FramePadding.y;
            const ImRect item_rect = { 
                window->DC.CursorPos, 
                { window->Pos.x + window->Size.x, window->DC.CursorPos.y + row_height } 
            };

            // Skipping ImGui::ItemSize allows to hide the scrollbar on empty items

            const ImU32 col = row % 2 ? OBJECT_TREE_ROW_BG_COL_ODD : OBJECT_TREE_ROW_BG_COL_EVEN;
            window->DrawList->AddRectFilled(item_rect.Min, item_rect.Max, col);
            
            window->DC.CursorPos.y += row_height;
        }

    } // namespace Interface
} // namespace yart
