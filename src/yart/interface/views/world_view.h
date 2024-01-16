////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the WorldView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "yart/interface/views/view.h"
#include "font/IconsCodicons.h"


namespace yart
{
    /// @brief View target class forward declaration
    class World;


    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Context view for the yart::World target
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class WorldView : public View {
        public:
            /// @brief Get the static instance, lazily initialized on first call
            /// @return Static WorldView instance
            static WorldView* Get();

            /// @brief Try to retrieve a target instance for the view
            /// @return Target instance pointer for this frame, or `nullptr` if no targets are active
            void* GetViewTarget() const;  

            /// @brief Issue view render commands for a given view target
            /// @param target View target instance, retrieved from View::GetViewTarget()
            /// @return Whether any changes were made by the user since the last frame
            bool Render(void* target) const;

        private:
            /// @brief WorldView class private constructor
            WorldView()
                : View(NAME, ICON, ICON_COLOR) { }

            WorldView(const WorldView&) = delete;
            WorldView& operator=(WorldView const&) = delete;

            /// @brief Issue "Sky" section UI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderSkySection(yart::World* target);

            /// @brief Issue "Ambient" section UI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderAmbientSection(yart::World* target);

        private:
            static constexpr char* NAME = "World";
            static constexpr char* ICON = ICON_CI_GLOBE;
            static constexpr ImU32 ICON_COLOR = YART_VIEW_ICON_COLOR_GRAY; 

        };   

    } // namespace Interface
} // namespace yart
