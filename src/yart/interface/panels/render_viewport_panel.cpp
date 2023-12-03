////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the RenderViewportPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "render_viewport_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"
#include "yart/core/renderer.h"
#include "yart/application.h"


#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define HOVER_RECT_PADDING 2.0f

    #define AXIS_POSITIVE_X 0
    #define AXIS_POSITIVE_Y 1
    #define AXIS_POSITIVE_Z 2
    #define AXIS_NEGATIVE_X 3
    #define AXIS_NEGATIVE_Y 4
    #define AXIS_NEGATIVE_Z 5
#endif // ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    namespace Interface
    {
        bool RenderViewportPanel::OnRender(Panel** active_panel)
        {
            ImGuiWindow* window = GetPanelWindow();

            // Resize the underlying viewport to match the panel size
            const ImRect win_rect = window->Rect();
            m_viewport.Resize(win_rect.GetWidth(), win_rect.GetHeight());

            // Ray trace the scene onto the main render viewport image on CPU
            yart::Renderer* renderer = yart::Application::Get().GetRenderer();
            yart::Camera& camera = RenderViewportPanel::m_camera;

            bool viewport_dirty = renderer->Render(camera, m_viewport);
            if (viewport_dirty) {
                m_viewport.EnsureRefresh(); // Make sure the viewport image gets refreshed this frame
                viewport_dirty = false;
            }

            // Render the viewport image
            ImTextureID viewport_texture = m_viewport.GetImTextureID();
            ImGui::GetBackgroundDrawList()->AddImage(viewport_texture, win_rect.Min, win_rect.Max);

            // Render the camera view axes overlay window
            glm::vec3 clicked_axis = {0, 0, 0};
            if (RenderCameraViewAxesOverlay(camera, clicked_axis)) {
                // Base axis to pitch, yaw rotation transformation magic
                const float pitch = clicked_axis.y * yart::Camera::PITCH_MAX;
                const float yaw = (clicked_axis.y + clicked_axis.z) * 90.0f * yart::utils::DEG_TO_RAD + (clicked_axis.x == -1.0f) * 180.0f * yart::utils::DEG_TO_RAD;

                camera.SetRotation(pitch, yaw);
                return true;
            }

            return false;
        }

        bool RenderViewportPanel::RenderCameraViewAxesOverlay(const yart::Camera& camera, glm::vec3& clicked_axis)
        {
            static constexpr ImVec2 window_size = { 75.0f, 75.0f }; // Expected to be a square
            static constexpr ImVec2 window_margin = { 25.0f, 15.0f };

            static const ImGuiWindowFlags window_flags = (
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav
                | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground
            );

            // Set a constant window size and position
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* window = GetPanelWindow();
            ImRect viewport_area = window->Rect();

            ImVec2 window_center = {
                viewport_area.Min.x + viewport_area.GetWidth() - window_size.x / 2 - window_margin.x, 
                viewport_area.Min.y + window_size.y / 2 + window_margin.y 
            };

            ImGui::SetNextWindowPos(window_center, ImGuiCond_None, { 0.5f, 0.5f });
            ImGui::SetNextWindowSize(window_size);

            // Open the window
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::Begin("View Axes Context", nullptr, window_flags);
            ImGui::PopStyleVar();

            // Draw window contents
            ImDrawList* draw_list = window->DrawList;
            static constexpr float circle_radius = window_size.x / 2;
            
            bool hovered = false;
            if (IsPanelHovered())
                hovered = GUI::IsMouseHoveringCircle(window_center, circle_radius);

            // Background
            static const ImU32 background_color = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_LIGHTER_GRAY, YART_GUI_ALPHA_MEDIUM });

            if (hovered)
                draw_list->AddCircleFilled(window_center, circle_radius, background_color);

            // Axes
            float yaw, pitch;
            camera.GetRotation(&yaw, &pitch);

            const float sin_pitch = glm::sin(pitch);
            const float cos_yaw = glm::cos(yaw);

            const glm::vec3 x_axis = { glm::sin(yaw), sin_pitch * cos_yaw, -cos_yaw };
            const glm::vec3 y_axis = { 0, -glm::cos(pitch), -sin_pitch };
            const glm::vec3 z_axis = glm::normalize(glm::cross(x_axis, y_axis));

            glm::vec3 center = { window_center.x, window_center.y, 0 };
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

        bool RenderViewportPanel::DrawViewAxesH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis0, const glm::vec3& axis1, const glm::vec3& axis2, 
                                                const int* order, float length, bool active, bool swap, glm::vec3& clicked_axis)
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
                if (GUI::IsMouseHoveringCircle({ win_pos.x + axes[i].x * length, win_pos.y + axes[i].y * length }, 6.5f)) {
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

        void RenderViewportPanel::DrawPositiveViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
        {
            static constexpr float axis_thickness = 2.5f;
            static constexpr float handle_radius = 6.5f;

            float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
            ImU32 col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });

            glm::vec3 handle_pos = win_pos + axis * length;
            draw_list->AddLine({ win_pos.x, win_pos.y }, { handle_pos.x, handle_pos.y }, col, axis_thickness);
            draw_list->AddCircleFilled({ handle_pos.x, handle_pos.y }, handle_radius, col);
        }

        void RenderViewportPanel::DrawNegativeViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
        {
            static constexpr float handle_thickness = 2.0f;
            static constexpr float handle_radius = 7.0f;

            float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
            ImU32 outer_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });
            ImU32 inner_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul * 0.2f, color.y * col_mul * 0.2f, color.z * col_mul * 0.2f, 1.0f });

            glm::vec3 handle_pos = win_pos + axis * length;
            draw_list->AddCircleFilled({ handle_pos.x, handle_pos.y }, handle_radius - 0.5f, inner_col, 0);
            draw_list->AddCircle({ handle_pos.x, handle_pos.y }, handle_radius, outer_col, 0, handle_thickness);
        }

    } // namespace Interface
} // namespace yart
