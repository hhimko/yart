////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the CameraView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "yart/interface/views/view.h"
#include "font/IconsCodicons.h"


namespace yart
{
    /// @brief View target class forward declaration
    class Camera;


    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Context view for the yart::Camera target
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class CameraView : public View {
        public:
            /// @brief Get the static instance, lazily initialized on first call
            /// @return Static CameraView instance
            static CameraView* Get();

            /// @brief Try to retrieve a target instance for the view
            /// @return Target instance pointer for this frame, or `nullptr` if no targets are active
            void* GetViewTarget() const;  

            /// @brief Issue view render commands for a given view target
            /// @param target View target instance, retrieved from View::GetViewTarget()
            /// @return Whether any changes were made by the user since the last frame
            bool Render(void* target) const;

        private:
            /// @brief CameraView class private constructor
            CameraView()
                : View(NAME, ICON, ICON_COLOR) { }

            CameraView(const CameraView&) = delete;
            CameraView& operator=(CameraView const&) = delete;

            /// @brief Issue "View Transform" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderViewTransformSection(yart::Camera* target);

            /// @brief Issue "Camera Properties" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderCameraPropertiesSection(yart::Camera* target);

        private:
            static constexpr char* NAME = "Camera";
            static constexpr char* ICON = ICON_CI_DEVICE_CAMERA_VIDEO;
            static constexpr ImU32 ICON_COLOR = YART_VIEW_ICON_COLOR_GRAY; 

        };   

    } // namespace Interface
} // namespace yart
