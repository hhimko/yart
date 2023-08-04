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


            // Draw the window
            ImGui::PushStyleColor(ImGuiCol_WindowBg, { YART_GUI_COLOR_BLACK, window_alpha });
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Window title bottom border on
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 4.0f }); // Window title vertical padding +2 
            
            ImGui::Begin(window.name);

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
            

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

    } // namespace GUI
} // namespace yart
