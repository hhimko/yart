////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal Interface module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the Interface module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface_internal.h"


#include "yart/interface/panels/render_viewport_panel.h"
#include "yart/interface/panels/inspector_panel.h"
#include "yart/interface/panels/context_panel.h"
#include "yart/interface/panels/layout_panel.h"
#include "yart/interface/panel.h"
#include "yart/application.h"
#include "yart/GUI/gui.h"


/// @brief Helper macro useful for opening `break`-able context scopes
#define WITH(handle) for(auto* _h = handle; _h != nullptr; _h = nullptr)

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define HOVER_RECT_PADDING 2.0f
#endif // ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    Interface::InterfaceContext* Interface::GetInterfaceContext()
    {
        static yart::Interface::InterfaceContext context = { };  
        return &context;
    }

    void Interface::ApplyDefaultLayout()
    {
        RootAppPanel* root_panel = RootAppPanel::Get();

        // Sidebar (inspector + context) menu layout
        LayoutPanel* sidebar_layout_panel = nullptr; 
        {
            GUI::LayoutCreateInfo layout_ci(GUI::LayoutDirection::VERTICAL);
            layout_ci.scaling_mode = GUI::LayoutScalingMode::FIRST_SECTION_FIXED;
            layout_ci.default_size_ratio = 0.3f;

            InspectorPanel* inspector_panel = new InspectorPanel("Inspector");
            ContextPanel* context_panel = new ContextPanel("Context");
            sidebar_layout_panel = new LayoutPanel("Sidebar Layout", layout_ci, inspector_panel, context_panel);
        }

        // Master layout (sidebar + render viewport)
        RenderViewportPanel* render_viewport_panel = nullptr;
        LayoutPanel* master_layout_panel = nullptr;
        {
            GUI::LayoutCreateInfo layout_ci(GUI::LayoutDirection::HORIZONTAL);
            layout_ci.scaling_mode = GUI::LayoutScalingMode::PRESERVE_RATIO;
            layout_ci.default_size_ratio = 0.7f;

            render_viewport_panel = new RenderViewportPanel("Primary Viewport");
            master_layout_panel = new LayoutPanel("Master Layout", layout_ci, render_viewport_panel, sidebar_layout_panel);
        }
        
        root_panel->AttachPanel(master_layout_panel);
        root_panel->SetActivePanel(render_viewport_panel);
    }

    void Interface::ApplyFullscreenLayout()
    {
        RootAppPanel* root_panel = RootAppPanel::Get();

        RenderViewportPanel* render_viewport_panel = new RenderViewportPanel("Primary Viewport");

        root_panel->AttachPanel(render_viewport_panel);
        root_panel->SetActivePanel(render_viewport_panel);
    }

    bool Interface::RenderMainMenuBar(float* menu_bar_height)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        bool made_changes = false;

        ImVec4 backup_color = g->Style.Colors[ImGuiCol_MenuBarBg];
        g->Style.Colors[ImGuiCol_MenuBarBg] = { YART_GUI_COLOR_BLACK, YART_GUI_ALPHA_OPAQUE };
        ImGui::BeginMainMenuBar();
        g->Style.Colors[ImGuiCol_MenuBarBg] = backup_color;

        // Render menu items
        if (ImGui::BeginMenu("File")) {
            yart::Scene* scene = yart::Application::Get().GetScene();
            if (ImGui::MenuItem("New")) {
                scene->Clear();
                made_changes = true;
            }

            if (ImGui::BeginMenu("Load Scene")) {
                if (ImGui::MenuItem("Default")) {
                    scene->Clear();
                    scene->LoadDefault();
                    made_changes = true;
                }
                if (ImGui::MenuItem("Spheres")) {
                    scene->Clear();
                    scene->LoadSpheres();
                    made_changes = true;
                }
                if (ImGui::MenuItem("UV Spheres")) {
                    scene->Clear();
                    scene->LoadUvSpheres();
                    made_changes = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        RenderViewMenu();

        // Store and return the menu bar height, used for determining content area sizes
        *menu_bar_height = g->CurrentWindow->Size.y;

        ImGui::EndMainMenuBar();
        return made_changes;
    }

    void Interface::RenderViewMenu()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        if (!ImGui::BeginMenu("View")) return;
        
        // Appearance sub menu
        if (ImGui::BeginMenu("Appearance")) {
            static constexpr size_t layout_count = 2;
            static constexpr char* layout_names[layout_count] = { "Default",           "Full Screen"          };
            static constexpr LayoutType layouts[layout_count] = { LayoutType::DEFAULT, LayoutType::FULLSCREEN };

            for (size_t i = 0; i < layout_count; ++i) {
                const bool selected = ctx->currentLayoutType == layouts[i];
                if (ImGui::MenuItem(layout_names[i], nullptr, selected))
                    ApplyLayout(layouts[i]);
            }
            
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    void Interface::RenderWindow(const char* name, callback_t callback)
    {
        // Local alpha multiplier for the whole window
        float window_alpha = YART_GUI_ALPHA_OPAQUE;
        
        // Try querying the window state prior to ImGui::Begin() to apply custom style
        ImGuiWindow* imgui_window = ImGui::FindWindowByName(name);
        WITH(imgui_window) {
            auto* active_window = ImGui::GetCurrentContext()->NavWindow;
            if (imgui_window == active_window)
                break;

            auto size = imgui_window->Size;
            auto pos = imgui_window->Pos;

            if (ImGui::IsMouseHoveringRect(
                {pos.x - HOVER_RECT_PADDING, pos.y - HOVER_RECT_PADDING}, 
                {pos.x + size.x + HOVER_RECT_PADDING, pos.y + size.y + HOVER_RECT_PADDING}, false)
            ) {
                window_alpha = YART_GUI_ALPHA_HIGH;
                break;
            }

            window_alpha = YART_GUI_ALPHA_MEDIUM;
        }


        // Open the window
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Window title bottom border on
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 4.0f }); // Window title vertical padding +2 
        
        ImGui::SetNextWindowBgAlpha(window_alpha);
        ImGui::Begin(name);

        ImGui::PopStyleVar(2);

        // Draw window contents 
        ImGuiStyle* style = &ImGui::GetStyle();
        float restore_alpha = style->Alpha;
        style->Alpha = window_alpha;

        // Call the window's render callback
        callback();

        // Restore global alpha value
        style->Alpha = restore_alpha;

        ImGui::End();
    }
} // namespace yart
