////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/views/view.h"
#include "font/IconsCodicons.h"


namespace yart
{
    /// @brief View target class forward declaration
    class Renderer;


    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief GUI view for the Renderer class
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RendererView : public View {
        public:
            /// @brief Get the static instance, lazily initialized on first call
            /// @return Static RendererView instance
            static RendererView* Get();

            /// @brief Try to retrieve a target instance for the view
            /// @return Target instance pointer for this frame, or `nullptr` if no targets are active
            void* GetViewTarget() const;  

            /// @brief Issue view render commands for a given view target
            /// @param target View target instance, retrieved from View::GetViewTarget()
            /// @return Whether any changes were made by the user since the last frame
            bool Render(void* target) const;

        private:
            /// @brief RendererView class private constructor
            RendererView()
                : View(NAME, ICON, ICON_COLOR) { }

            RendererView(const RendererView&) = delete;
            RendererView& operator=(RendererView const&) = delete;

            /// @brief Issue "Materials" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderMaterialsSection(yart::Renderer* target);

            /// @brief Issue "Overlays" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderOverlaysSection(yart::Renderer* target);

        private:
            static constexpr char* NAME = "Renderer";
            static constexpr char* ICON = ICON_CI_EDIT;
            static constexpr ImU32 ICON_COLOR = YART_VIEW_ICON_COLOR_GRAY; 

        };   

    } // namespace Interface
} // namespace yart
