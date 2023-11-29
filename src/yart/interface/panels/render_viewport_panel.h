////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the RenderViewportPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"
#include "yart/core/viewport.h"
#include "yart/core/camera.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief UI panel for displaying and handling render viewports 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RenderViewportPanel : public Panel {
        private:
            /// @brief Issue panel GUI render commands
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

            /// @brief Render the camera view axes overlay window over the viewport
            /// @param window Dear ImGui window representing the panel
            /// @param camera YART camera instance 
            /// @param clicked_axis Output variable set to a base axis clicked by the user
            /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
            static bool RenderCameraViewAxesOverlay(ImGuiWindow* window, const yart::Camera& camera, glm::vec3& clicked_axis);

            /// @brief Camera view axes overlay window rendering helper function
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
            static bool DrawViewAxesH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis0, const glm::vec3& axis1, const glm::vec3& axis2, 
                                      const int* order, float length, bool active, bool swap, glm::vec3& clicked_axis);

            /// @brief Positive view axis rendering helper function
            /// @param draw_list ImGui window draw list
            /// @param win_pos Center window position of the view axes context window
            /// @param axis Normalized axis direction
            /// @param color Color of the axis
            /// @param length Maximum axis length in pixels 
            /// @param hovered Whether the axis handle is currently hovered
            static void DrawPositiveViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered);

            /// @brief Negative view axis rendering helper function
            /// @param draw_list ImGui window draw list
            /// @param win_pos Center window position of the view axes context window
            /// @param axis Normalized axis direction
            /// @param color Color of the axis
            /// @param length Maximum axis length in pixels 
            /// @param hovered Whether the axis handle is currently hovered
            static void DrawNegativeViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered);

        private:
            inline static yart::Camera camera; ///< YART camera instance, shared between all viewport panels

            yart::Viewport m_viewport { 1, 1, 2 };

        };

    } // namespace Interface
} // namespace yart
