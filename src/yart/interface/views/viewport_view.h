////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the ViewportView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "yart/interface/views/view.h"
#include "font/IconsCodicons.h"


namespace yart
{
    /// @brief View target class forward declaration
    class Viewport;


    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief GUI view for the Viewport class
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ViewportView : public View {
        public:
            /// @brief Get the static instance, lazily initialized on first call
            /// @return Static ViewportView instance
            static ViewportView* Get();

            /// @brief Try to retrieve a target instance for the view
            /// @return Target instance pointer for this frame, or `nullptr` if no targets are active
            void* GetViewTarget() const;  

            /// @brief Issue view render commands for a given view target
            /// @param target View target instance, retrieved from View::GetViewTarget()
            /// @return Whether any changes were made by the user since the last frame
            bool Render(void* target) const;

        private:
            /// @brief WorldView class private constructor
            ViewportView()
                : View(NAME, ICON, ICON_COLOR) { }

            ViewportView(const ViewportView&) = delete;
            ViewportView& operator=(ViewportView const&) = delete;

            /// @brief Issue "Output" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderOutputSection(yart::Viewport* target);

        private:
            static constexpr char* NAME = "Viewport";
            static constexpr char* ICON = ICON_CI_DEVICE_DESKTOP;
            static constexpr ImU32 ICON_COLOR = YART_VIEW_ICON_COLOR_GRAY; 

        };   

    } // namespace Interface
} // namespace yart
