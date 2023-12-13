////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "inspector_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"
#include "yart/GUI/gui_internal.h"
#include "font/IconsCodicons.h"
#include "yart/application.h"


namespace yart
{
    namespace Interface
    {
        bool InspectorPanel::OnRender(Panel** active_panel)
        {
            if (GUI::BeginTabBar("Scene")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                RenderSceneTab(active_panel);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            yart::Scene* scene = yart::Application::Get().GetScene();
            Object* selected_object = scene->GetSelectedObject();
            
            if (selected_object == nullptr)
                ImGui::BeginDisabled();

            if (ImGui::BeginTabItem("Object")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened | ImGuiWindowFlags_AlwaysUseWindowPadding;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                RenderObjectTab(selected_object, active_panel);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            if (selected_object == nullptr)
                ImGui::EndDisabled();

            return false;
        }

        void InspectorPanel::RenderSceneTab(Panel** active_panel)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            // Retrieve scene collections 
            size_t collections_count;
            yart::Scene* scene = yart::Application::Get().GetScene();
            yart::SceneCollection* collections = scene->GetSceneCollections(&collections_count);
            const yart::SceneCollection* selected_collection = scene->GetSelectedCollection();
            const yart::Object* selected_object = scene->GetSelectedObject();

            // Iterate through all scene collections 
            size_t row = 0;
            for (size_t ci = 0; ci < collections_count; ++ci) {
                yart::SceneCollection* collection = collections + ci;
                if (RenderObjectTreeRowCollection(row++, collection, collection == selected_collection))
                    scene->ToggleSelection(collection);

                // Iterate through all collection objects
                uint8_t indent_level = 1;
                for (size_t oi = 0; oi < collection->objects.size(); ++oi) {
                    yart::Object* object = collection->objects[oi];
                    if (RenderObjectTreeRowObject(row++, indent_level, object, object == selected_object))
                        scene->ToggleSelection(object);
                }
            }

            // Fill the remaining space with empty rows
            const float max_y = window->Pos.y + window->Size.y;
            while (window->DC.CursorPos.y < max_y)
                RenderObjectTreeRowEmpty(row++);

            if (ImGui::BeginPopup("Test popup")) {
                ImGui::LabelText("", "New mesh object");
                ImGui::Button("Cube mesh");
                ImGui::Button("Plane mesh");
                ImGui::Button("UV Sphere mesh");
                
                ImGui::EndPopup();
            }

            if (g->HoveredWindow == window && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup("Test popup");
                *active_panel = this;
            }
        }

        void InspectorPanel::RenderObjectTab(Object* selected_object, Panel** active_panel)
        {
            GUI::Label("Object name", selected_object->GetName());
        }

        bool InspectorPanel::RenderObjectTreeRowCollection(size_t row, yart::SceneCollection* collection, bool selected)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const ImRect item_rect = GetObjectTreeRowRect();

            const ImVec2 backup_item_spacing = g->Style.ItemSpacing;
            g->Style.ItemSpacing = { 0.0f, 0.0f };

            ImGui::ItemSize(item_rect);
            const ImGuiID id = ImGui::GetID(collection->name);
            ImGui::ItemAdd(item_rect, id);

            g->Style.ItemSpacing = backup_item_spacing;
            
            bool hovered, held;
            const bool clicked = ImGui::ButtonBehavior(item_rect, id, &hovered, &held);

            const ImU32 bg_col = GetObjectTreeRowColorH(row, hovered, selected);
            RenderObjectTreeRowH(item_rect, row, 0, bg_col, ICON_CI_ARCHIVE, collection->name);

            return clicked;
        }

        bool InspectorPanel::RenderObjectTreeRowObject(size_t row, uint8_t indent, yart::Object* object, bool selected)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const ImRect item_rect = GetObjectTreeRowRect();

            const ImVec2 backup_item_spacing = g->Style.ItemSpacing;
            g->Style.ItemSpacing = { 0.0f, 0.0f };

            ImGui::ItemSize(item_rect);
            const ImGuiID id = ImGui::GetID(object->GetName());
            ImGui::ItemAdd(item_rect, id);

            g->Style.ItemSpacing = backup_item_spacing;

            bool hovered, held;
            const bool clicked = ImGui::ButtonBehavior(item_rect, id, &hovered, &held);
            
            const ImU32 bg_col = GetObjectTreeRowColorH(row, hovered, selected);
            RenderObjectTreeRowH(item_rect, row, indent, bg_col, ICON_CI_CIRCLE_OUTLINE, object->GetName());

            return clicked;
        }

        void InspectorPanel::RenderObjectTreeRowEmpty(size_t row)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const ImRect item_rect = GetObjectTreeRowRect();

            // Skipping ImGui::ItemSize allows to hide the scrollbar on empty items

            const ImU32 bg_col = GetObjectTreeRowColorH(row, false, false);
            RenderObjectTreeRowH(item_rect, row, 0, bg_col, nullptr, nullptr);

            window->DC.CursorPos.y += item_rect.GetHeight();
        }

        ImRect InspectorPanel::GetObjectTreeRowRect()
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const float row_height = g->Font->FontSize + 2.0f * g->Style.FramePadding.y + 2.0f;
            const ImRect item_rect = { 
                window->DC.CursorPos, 
                { window->Pos.x + window->Size.x, window->DC.CursorPos.y + row_height } 
            };

            return item_rect;
        }

        ImU32 InspectorPanel::GetObjectTreeRowColorH(size_t row, bool hovered, bool selected)
        {
            if (selected) {
                static constexpr ImU32 col_selected = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_PRIMARY, 1.0f });
                static constexpr ImU32 col_selected_hover = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_PRIMARY, 1.0f });

                return hovered ? col_selected_hover : col_selected;
            }

            static constexpr ImU32 col_odd   = GUI::ColorConvertFloat4ToU32({ 0.022f, 0.022f, 0.022f, 1.0f });
            static constexpr ImU32 col_even  = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, 1.0f });
            static constexpr ImU32 col_hover = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, 1.0f });

            const bool is_even = row % 2 == 0;
            return hovered ? col_hover : is_even ? col_even : col_odd;
        }

        void InspectorPanel::RenderObjectTreeRowH(const ImRect& rect, size_t row, uint8_t indent, ImU32 color, const char* icon, const char* text)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;

            const float bg_rounding = row == 0 ? g->Style.FrameRounding : 0.0f;
            const float bg_flags = row == 0 ? ImDrawFlags_RoundCornersTop : ImDrawFlags_RoundCornersNone;
            window->DrawList->AddRectFilled(rect.Min, rect.Max, color, bg_rounding, bg_flags);

            static const float indent_width = 20.0f;
            float left_indent = g->Style.FramePadding.x + indent * indent_width;

            if (icon != nullptr) {
                GUI::PushIconsFont();
                const float icon_width = ImGui::CalcTextSize(icon).x;

                const ImVec2 pos = {
                    rect.Min.x + left_indent, 
                    rect.Min.y + 1.0f
                };

                window->DrawList->AddText(pos, 0xFFFFFFFF, icon);
                left_indent += icon_width + g->Style.ItemSpacing.x;

                ImGui::PopFont();
            }

            if (text != nullptr) {
                const ImRect label_rect = {
                    rect.Min.x + left_indent, rect.Min.y,
                    rect.Max.x, rect.Max.y
                };

                const bool hovered = ImGui::IsItemHovered();
                if (GUI::DrawText(window->DrawList, label_rect.Min, label_rect.Max, text, YART_GUI_TEXT_ALIGN_LEFT) && hovered) 
                    ImGui::SetTooltip(text);
            }
        }

    } // namespace Interface
} // namespace yart
