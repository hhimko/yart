////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/core/utils/glm_utils.h"


namespace yart
{
    /// @brief View target class forward declaration
    class Renderer;


    namespace GUI
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief GUI view for the Renderer class
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RendererView {
        public:
            /// @brief Issue GUI render commands to display the Renderer object context menu
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool OnRenderGUI(yart::Renderer* target);

            /// @brief Issue GUI render commands to display the view axes context window
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool OnRenderViewAxesWindow(yart::Renderer* target);

            /// @brief Update the renderer's camera state based on user input
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool HandleInputs(yart::Renderer* target);

        private:
            RendererView() = delete;

            /// @brief Issue "View Transform" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderViewTransformSection(yart::Renderer* target);

            /// @brief Issue "Camera Properties" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderCameraPropertiesSection(yart::Renderer* target);

        private:
            /// @brief Smallest valid value for camera's FOV in degrees
            static constexpr float FOV_MIN = 45.0f;
            /// @brief Largest valid value for camera's FOV in degrees
            static constexpr float FOV_MAX = 180.0f;
            /// @brief Smallest valid value for camera's near clipping plane distance
            static constexpr float NEAR_CLIP_MIN = 0.001f;
            /// @brief Largest valid value for camera's near clipping plane distance
            static constexpr float NEAR_CLIP_MAX = 10.0f;
            /// @brief Smallest valid value for camera's far clipping plane distance
            static constexpr float FAR_CLIP_MIN = 100.0f;
            /// @brief Largest valid value for camera's far clipping plane distance
            static constexpr float FAR_CLIP_MAX = 1000.0f;
            /// @brief Smallest valid value for camera's pitch rotation
            static constexpr float CAMERA_PITCH_MIN = -90.0f * yart::utils::DEG_TO_RAD + yart::utils::EPSILON;
            /// @brief Largest valid value for camera's pitch rotation
            static constexpr float CAMERA_PITCH_MAX =  90.0f * yart::utils::DEG_TO_RAD - yart::utils::EPSILON;

        };   

    } // namespace GUI
} // namespace yart
