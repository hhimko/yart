////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "inspector_panel.h"


#include <iostream>

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
            bool made_changes = false;

            if (GUI::BeginTabBar("Scene")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                made_changes |= RenderSceneTab(active_panel);

                
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            yart::Scene* scene = yart::Application::Get().GetScene();
            Object* selected_object = scene->GetSelectedObject();
            
            if (selected_object == nullptr) {
                ImGui::BeginDisabled();

                // Avoid rendering the Object tab when the selected object is null
                ImGuiTabBar* tab_bar = ImGui::GetCurrentTabBar();
                ImGui::TabBarQueueFocus(tab_bar, &tab_bar->Tabs[0]);
            }

            if (ImGui::BeginTabItem("Object")) {
                static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NavFlattened | ImGuiWindowFlags_AlwaysUseWindowPadding;
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

                if (selected_object != nullptr)
                    made_changes |= RenderObjectTab(selected_object, active_panel);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            if (selected_object == nullptr)
                ImGui::EndDisabled();

            return made_changes;
        }

        bool InspectorPanel::RenderSceneTab(Panel** active_panel)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = g->CurrentWindow;
            bool made_changes = false;

            // Retrieve scene collections 
            size_t collections_count;
            yart::Scene* scene = yart::Application::Get().GetScene();
            yart::SceneCollection* collections = scene->GetSceneCollections(&collections_count);
            const yart::SceneCollection* selected_collection = scene->GetSelectedCollection();
            const yart::Object* selected_object = scene->GetSelectedObject();

            // Iterate through all scene collections 
            size_t row = 0;
            yart::Object* hovered_object = nullptr;
            for (size_t ci = 0; ci < collections_count; ++ci) {
                yart::SceneCollection* collection = collections + ci;
                if (RenderObjectTreeRowCollection(row++, collection, collection == selected_collection))
                    scene->ToggleSelection(collection);

                // Iterate through all collection objects
                uint8_t indent_level = 1;
                for (yart::Object* object : collection->objects) {
                    bool hovered = false;
                    if (RenderObjectTreeRowObject(row++, indent_level, object, object == selected_object, &hovered))
                        scene->ToggleSelection(object);
                    if (hovered)
                        hovered_object = object;
                }
            }

            // Fill the remaining space with empty rows
            const float max_y = window->Pos.y + window->Size.y;
            while (window->DC.CursorPos.y < max_y)
                RenderObjectTreeRowEmpty(row++);


            // Define and render popup menus
            static Object* settings_object_context = nullptr;
            if (g->HoveredWindow == window && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                if (hovered_object == nullptr) {
                    ImGui::OpenPopup("Popup_NewObject");
                } else {
                    settings_object_context = hovered_object;
                    ImGui::OpenPopup("Popup_ObjectSettings");
                }
                *active_panel = this;
            }

            if (ImGui::BeginPopup("Popup_NewObject")) {
                ImGui::LabelText("", "Add mesh object");

                if (ImGui::Button("Cube mesh")) {
                    Mesh* mesh = MeshFactory::CubeMesh({ 0, 0, 0 });
                    scene->AddMeshObject("Cube", mesh);

                    MeshFactory::DestroyMesh(mesh);

                    ImGui::CloseCurrentPopup();
                    made_changes = true;
                }

                if (ImGui::Button("Plane mesh")) {
                    Mesh* mesh = MeshFactory::PlaneMesh({ 0, 0, 0 }, 5.0f);
                    scene->AddMeshObject("Plane", mesh);

                    MeshFactory::DestroyMesh(mesh);

                    ImGui::CloseCurrentPopup();
                    made_changes = true;
                }

                if (ImGui::Button("UV Sphere mesh")) {
                    Mesh* mesh = MeshFactory::UvSphereMesh({ 0, 0, 0 }, 16, 8);
                    scene->AddMeshObject("UV Sphere", mesh);

                    MeshFactory::DestroyMesh(mesh);

                    ImGui::CloseCurrentPopup();
                    made_changes = true;
                }

                if (ImGui::Button("SDF Sphere")) {
                    scene->AddSdfObject("Sphere", 0.5f);

                    ImGui::CloseCurrentPopup();
                    made_changes = true;
                }
                
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Popup_ObjectSettings")) {
                if (ImGui::Button("Remove Object")) {
                    scene->RemoveObject(settings_object_context);

                    ImGui::CloseCurrentPopup();
                    made_changes = true;
                }

                ImGui::EndPopup();
            }

            return made_changes;
        }

        bool InspectorPanel::RenderObjectTab(Object* selected_object, Panel** active_panel)
        {
            bool section_open, made_changes = false;

            GUI::Label("Object name", selected_object->GetName());

            section_open = GUI::BeginCollapsableSection("Position");
            if (section_open) {
                static const char* names[3] = { "Position X", "Position Y", "Position Z" };
                made_changes |= GUI::SliderVec3(names, &selected_object->position);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Scale");
            if (section_open) {
                static const char* names[3] = { "Scale X", "Scale Y", "Scale Z" };
                made_changes |= GUI::SliderVec3(names, &selected_object->scale);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Material");
            if (section_open) {
                static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
                made_changes |= GUI::ColorEdit("Diffuse color", reinterpret_cast<float*>(&selected_object->materialColor));

                float diffuse_percent = selected_object->materialDiffuse * 100.0f;
                if (GUI::SliderFloat("Diffuse", &diffuse_percent, 0.0f, 100.0f, "%.1f%%")) {
                    selected_object->materialDiffuse = diffuse_percent / 100.0f;
                    made_changes = true;
                }

                float specular_percent = selected_object->materialSpecular * 100.0f;
                if (GUI::SliderFloat("Specular", &specular_percent, 0.0f, 100.0f, "%.1f%%")) {
                    selected_object->materialSpecular = specular_percent / 100.0f;
                    made_changes = true;
                }

                float* spec_off = &selected_object->materialSpecularFalloff;
                made_changes |= GUI::SliderFloat("Specular falloff", spec_off, 1.0f, 512.0f, "%.0f");

                float reflection_percent = selected_object->materialReflection * 100.0f;
                if (GUI::SliderFloat("Reflection strength", &reflection_percent, 0.0f, 100.0f, "%.1f%%")) {
                    selected_object->materialReflection = reflection_percent / 100.0f;
                    made_changes = true;
                }
            }
            GUI::EndCollapsableSection(section_open);

            if (made_changes)
                selected_object->TransformationChanged();

            return made_changes;
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

        bool InspectorPanel::RenderObjectTreeRowObject(size_t row, uint8_t indent, yart::Object* object, bool selected, bool* hovered)
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

            bool hov, held;
            const bool clicked = ImGui::ButtonBehavior(item_rect, id, &hov, &held);
            *hovered = hov;
            
            const ImU32 bg_col = GetObjectTreeRowColorH(row, hov, selected);
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
