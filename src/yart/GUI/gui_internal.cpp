////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects and methods definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui_internal.h"


/// @brief Helper macro useful for opening `break`-able context scopes
#define WITH(handle) for(auto* _h = handle; _h != nullptr; _h = nullptr)

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define HOVER_RECT_PADDING 2.0f
#endif


namespace yart
{
    namespace GUI
    {
        GuiWindow::GuiWindow(const char* name, imgui_callback_t callback)
            : name(name), callback(callback)
        {

        }

        void RenderWindow(const GuiWindow &window)
        {
            // Local alpha multiplier for the whole window
            float window_alpha = YART_GUI_ALPHA_OPAQUE;

            // Try querying the window state prior to ImGui::Begin() to apply custom style
            ImGuiWindow* imgui_window = ImGui::FindWindowByName(window.name);
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
            ImGui::Begin(window.name);

            ImGui::PopStyleVar(2);
            

            // Draw window contents 
            ImGuiStyle* style = &ImGui::GetStyle();

            float restore_alpha = style->Alpha;
            style->Alpha = window_alpha;

            // Call the window's render callback
            window.callback();

            // Restore global alpha value
            style->Alpha = restore_alpha;


            ImGui::End();
        }

        bool IsMouseHoveringCircle(const ImVec2 &pos, float radius)
        {
            ImGuiIO& io = ImGui::GetIO();
            float dx = io.MousePos.x - pos.x;
            float dy = io.MousePos.y - pos.y;
            return dx * dx + dy * dy <= radius * radius;
        }

        void RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis)
        {
            static const ImVec2 window_size = { 75.0f, 75.0f }; // Expected to be a square
            static const ImVec2 window_margin = { 20.0f, 10.0f };

            static const ImGuiWindowFlags window_flags = (
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav
                | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground
            );


            // Set a constant window size and position
            ImVec2 viewport_pos = ImGui::GetMainViewport()->WorkPos;
            ImVec2 viewport_size = ImGui::GetMainViewport()->WorkSize;
            ImVec2 window_center = {
                viewport_pos.x + viewport_size.x - window_size.x / 2 - window_margin.x, 
                viewport_pos.y + window_size.y / 2 + window_margin.y 
            };

            ImGui::SetNextWindowPos(window_center, ImGuiCond_None, { 0.5f, 0.5f });
            ImGui::SetNextWindowSize(window_size);


            // Open the window
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::Begin("View Axes Context", nullptr, window_flags);
            ImGui::PopStyleVar();


            // Draw window contents
            auto* imgui_window = ImGui::GetCurrentWindow();
            ImDrawList* draw_list = imgui_window->DrawList;
            float circle_radius = window_size.x / 2;
            
            bool hovered = false;
            if (ImGui::IsWindowHovered())
                hovered = IsMouseHoveringCircle(window_center, circle_radius);

            static const ImU32 background_color = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_LIGHT_GRAY, YART_GUI_ALPHA_LOW });
            static const ImU32 background_color_hovered = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_LIGHT_GRAY, YART_GUI_ALPHA_MEDIUM });

            if (hovered)
                draw_list->AddNgonFilled(window_center, circle_radius, background_color_hovered, 16);
            else
                draw_list->AddNgonFilled(window_center, circle_radius, background_color, 16);

            ImGui::End();
        }

    } // namespace GUI
} // namespace yart
