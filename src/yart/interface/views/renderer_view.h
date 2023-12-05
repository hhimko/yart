////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>


namespace yart
{
    /// @brief View target class forward declaration
    class Renderer;


    namespace Interface
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

            /// @brief Issue "Overlays" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderOverlaysSection(yart::Renderer* target);

        };   

    } // namespace Interface
} // namespace yart
