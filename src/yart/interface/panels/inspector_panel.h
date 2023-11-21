////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief UI panel for displaying and handling the inspector view
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class InspectorPanel : public Panel {
        private:
            /// @brief Issue panel GUI render commands
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

        };

    } // namespace Interface
} // namespace yart
