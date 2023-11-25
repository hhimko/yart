////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal Interface module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the Interface module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface_internal.h"


#include "yart/interface/panels/render_viewport_panel.h"
#include "yart/interface/panels/inspector_panel.h"
#include "yart/interface/panels/context_panel.h"
#include "yart/interface/panel.h"
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
            GUI::LayoutCreateInfo sidebar_layout_ci(GUI::LayoutDirection::VERTICAL);
            sidebar_layout_ci.scaling_mode = GUI::LayoutScalingMode::FIRST_SECTION_FIXED;
            sidebar_layout_ci.default_size_ratio = 0.4f;

            InspectorPanel* inspector_panel = new InspectorPanel();
            ContextPanel* context_panel = new ContextPanel();
            sidebar_layout_panel = new LayoutPanel(sidebar_layout_ci, inspector_panel, context_panel);
        }

        // Master layout (sidebar + render viewport)
        LayoutPanel* master_layout_panel = nullptr;
        {
            GUI::LayoutCreateInfo master_layout_ci(GUI::LayoutDirection::HORIZONTAL);
            master_layout_ci.scaling_mode = GUI::LayoutScalingMode::PRESERVE_RATIO;
            master_layout_ci.default_size_ratio = 0.7f;

            RenderViewportPanel* render_viewport_panel = new RenderViewportPanel();
            master_layout_panel = new LayoutPanel(master_layout_ci, render_viewport_panel, sidebar_layout_panel);
        }
        
        root_panel->AttachPanel(master_layout_panel);
    }

    float Interface::RenderMainMenuBar()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        ImVec4 backup_color = g->Style.Colors[ImGuiCol_MenuBarBg];
        g->Style.Colors[ImGuiCol_MenuBarBg] = { YART_GUI_COLOR_BLACK, YART_GUI_ALPHA_OPAQUE };
        ImGui::BeginMainMenuBar();
        g->Style.Colors[ImGuiCol_MenuBarBg] = backup_color;

        // Render menu items
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Create");
            ImGui::EndMenu();
        }

        // Store and return the menu bar height, used for determining content area sizes
        float menu_bar_height = g->CurrentWindow->Size.y;

        ImGui::EndMainMenuBar();
        return menu_bar_height;
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
