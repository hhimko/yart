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

    #define AXIS_POSITIVE_X 0
    #define AXIS_POSITIVE_Y 1
    #define AXIS_POSITIVE_Z 2
    #define AXIS_NEGATIVE_X 3
    #define AXIS_NEGATIVE_Y 4
    #define AXIS_NEGATIVE_Z 5
#endif // #ifndef DOXYGEN_EXCLUDE 


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

        /// @brief Positive view axis rendering helper function
        /// @param draw_list ImGui window draw list
        /// @param win_pos Center window position of the view axes context window
        /// @param axis Normalized axis direction
        /// @param color Color of the axis
        /// @param length Maximum axis length in pixels 
        /// @param hovered Whether the axis handle is currently hovered
        void DrawPositiveViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
        {
            static constexpr float axis_thickness = 2.5f;
            static constexpr float handle_radius = 6.5f;

            float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
            ImU32 col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });

            glm::vec3 handle_pos = win_pos + axis * length;
            draw_list->AddLine({ win_pos.x, win_pos.y }, { handle_pos.x, handle_pos.y }, col, axis_thickness);
            draw_list->AddCircleFilled({ handle_pos.x, handle_pos.y }, handle_radius, col);
        }

        /// @brief Negative view axis rendering helper function
        /// @param draw_list ImGui window draw list
        /// @param win_pos Center window position of the view axes context window
        /// @param axis Normalized axis direction
        /// @param color Color of the axis
        /// @param length Maximum axis length in pixels 
        /// @param hovered Whether the axis handle is currently hovered
        void DrawNegativeViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
        {
            static constexpr float handle_thickness = 2.0f;
            static constexpr float handle_radius = 7.0f;

            float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
            ImU32 outer_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });
            ImU32 inner_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul * 0.2f, color.y * col_mul * 0.2f, color.z * col_mul * 0.2f, 1.0f });

            glm::vec3 handle_pos = win_pos + axis * length;
            draw_list->AddCircleFilled({handle_pos.x, handle_pos.y}, handle_radius - 0.5f, inner_col, 0);
            draw_list->AddCircle({handle_pos.x, handle_pos.y}, handle_radius, outer_col, 0, handle_thickness);
        }

        /// @brief View axes rendering helper function
        /// @param draw_list ImGui window draw list
        /// @param win_pos Center window position of the view axes context window
        /// @param axis0 First axis in the drawing order (back)
        /// @param axis1 Second axis in the drawing order (middle)
        /// @param axis2 Third axis in the drawing order (front)
        /// @param order The order in which the X, Y and Z axes will be drawn
        /// @param length Maximum axis length in pixels 
        /// @param active Whether the view axes context window is currently active
        /// @param swap Wether the negative axes should be rendered prior to the positive ones
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool DrawViewAxesH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis0, const glm::vec3& axis1, const glm::vec3& axis2, const int* order, float length, bool active, bool swap, glm::vec3& clicked_axis)
        {
            static constexpr glm::vec3 axes_colors_LUT[] = {
                { 244.0f / 255.0f, 36.0f  / 255.0f, 84.0f  / 255.0f }, // + X-axis 
                { 84.0f  / 255.0f, 244.0f / 255.0f, 36.0f  / 255.0f }, // + Y-axis 
                { 36.0f  / 255.0f, 84.0f  / 255.0f, 244.0f / 255.0f }, // + Z-axis 
                { 247.0f / 255.0f, 99.0f  / 255.0f, 133.0f / 255.0f }, // - X-axis 
                { 133.0f / 255.0f, 247.0f / 255.0f, 99.0f  / 255.0f }, // - Y-axis 
                { 99.0f  / 255.0f, 133.0f / 255.0f, 247.0f / 255.0f }, // - Z-axis 
            };


            // No need to render negative view axes and check for input if the window is not active
            if (!active) {
                DrawPositiveViewAxisH(draw_list, win_pos, axis0, axes_colors_LUT[order[0]], length, false);
                DrawPositiveViewAxisH(draw_list, win_pos, axis1, axes_colors_LUT[order[1]], length, false);
                DrawPositiveViewAxisH(draw_list, win_pos, axis2, axes_colors_LUT[order[2]], length, false);

                return false;
            }


            glm::vec3* axes;
            if (!swap){
                glm::vec3 p_axes[6] = { axis0, axis1, axis2, -axis2, -axis1, -axis0 };
                axes = p_axes;
            } else {
                glm::vec3 n_axes[6] = { -axis2, -axis1, -axis0, axis0, axis1, axis2 };
                axes = n_axes;
            }

            // Mouse hover tests have to be done first, in reverse order to rendering
            int hovered_axis_index = -1;
            for (int i=5; i >= 0; --i) {
                if (IsMouseHoveringCircle({ win_pos.x + axes[i].x * length, win_pos.y + axes[i].y * length }, 6.5f)) {
                    hovered_axis_index = i;
                    break;
                }
            }

            // Render the individual axes
            for (int i=0; i < 6; ++i) {
                if (swap && i < 3 || !swap && i >= 3)
                    DrawNegativeViewAxisH(draw_list, win_pos, axes[i], axes_colors_LUT[order[(5 - i) % 3] + 3], length, i == hovered_axis_index);
                else
                    DrawPositiveViewAxisH(draw_list, win_pos, axes[i], axes_colors_LUT[order[i % 3]], length, i == hovered_axis_index);
            }


            // Return the clicked axis
            if (hovered_axis_index >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                int hovered_axis;
                if (swap && hovered_axis_index < 3 || !swap && hovered_axis_index >= 3)
                    hovered_axis = order[(5 - hovered_axis_index) % 3] + 3;
                else 
                    hovered_axis = order[hovered_axis_index % 3];

                switch (hovered_axis) {
                    case AXIS_POSITIVE_X:
                        clicked_axis = {1, 0, 0};
                        return true;
                    case AXIS_POSITIVE_Y:
                        clicked_axis = {0, 1, 0};
                        return true;
                    case AXIS_POSITIVE_Z:
                        clicked_axis = {0, 0, 1};
                        return true;
                    case AXIS_NEGATIVE_X:
                        clicked_axis = {-1, 0, 0};
                        return true;
                    case AXIS_NEGATIVE_Y:
                        clicked_axis = {0, -1, 0};
                        return true;
                    case AXIS_NEGATIVE_Z:
                        clicked_axis = {0, 0, -1};
                        return true;
                }
            }

            return false;
        }

        bool RenderViewAxesWindowEx(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis)
        {
            static constexpr ImVec2 window_size = { 75.0f, 75.0f }; // Expected to be a square
            static constexpr ImVec2 window_margin = { 20.0f, 10.0f };

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
            static constexpr float circle_radius = window_size.x / 2;
            
            bool hovered = false;
            if (ImGui::IsWindowHovered())
                hovered = IsMouseHoveringCircle(window_center, circle_radius);

            // Background
            static const ImU32 background_color = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_GRAY, YART_GUI_ALPHA_MEDIUM });

            if (hovered)
                draw_list->AddCircleFilled(window_center, circle_radius, background_color);

            // Axes
            glm::vec3 center(window_center.x, window_center.y, 0);
            static constexpr float axis_length = circle_radius - 10.0f;

            // Z-sort the axes in draw order
            bool clicked;
            bool swap = (y_axis.z > -0.8f && x_axis.z + y_axis.z + z_axis.z >= -0.5f); // Wether the negative axes should be rendered first
            if (x_axis.z > y_axis.z) {
                if (y_axis.z > z_axis.z) {
                    static constexpr int order[3] = { AXIS_POSITIVE_Z, AXIS_POSITIVE_Y, AXIS_POSITIVE_X };
                    clicked = DrawViewAxesH(draw_list, center, z_axis, y_axis, x_axis, order, axis_length, hovered, swap, clicked_axis);
                } else {
                    if (x_axis.z > z_axis.z) {
                        static constexpr int order[3] = { AXIS_POSITIVE_Y, AXIS_POSITIVE_Z, AXIS_POSITIVE_X };
                        clicked = DrawViewAxesH(draw_list, center, y_axis, z_axis, x_axis, order, axis_length, hovered, swap, clicked_axis);
                    } else {
                        static constexpr int order[3] = { AXIS_POSITIVE_Y, AXIS_POSITIVE_X, AXIS_POSITIVE_Z };
                        clicked = DrawViewAxesH(draw_list, center, y_axis, x_axis, z_axis, order, axis_length, hovered, swap, clicked_axis);
                    }
                }
            } else {
                if (x_axis.z > z_axis.z) {
                    static constexpr int order[3] = { AXIS_POSITIVE_Z, AXIS_POSITIVE_X, AXIS_POSITIVE_Y };
                    clicked = DrawViewAxesH(draw_list, center, z_axis, x_axis, y_axis, order, axis_length, hovered, swap, clicked_axis);
                } else {
                    if (y_axis.z > z_axis.z) {
                        static constexpr int order[3] = { AXIS_POSITIVE_X, AXIS_POSITIVE_Z, AXIS_POSITIVE_Y };
                        clicked = DrawViewAxesH(draw_list, center, x_axis, z_axis, y_axis, order, axis_length, hovered, swap, clicked_axis);
                    } else {
                        static constexpr int order[3] = { AXIS_POSITIVE_X, AXIS_POSITIVE_Y, AXIS_POSITIVE_Z };
                        clicked = DrawViewAxesH(draw_list, center, x_axis, y_axis, z_axis, order, axis_length, hovered, swap, clicked_axis);
                    }
                }
            }


            ImGui::End();
            return clicked;
        }

    } // namespace GUI
} // namespace yart
