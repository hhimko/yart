////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the ContextPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief UI panel for displaying and handling the context view 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ContextPanel : public Panel {
        private:
            /// @brief Issue panel GUI render commands
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

            /// @brief Render the context's side nav bar
            static void RenderContextNavBar();

        };

    } // namespace Interface
} // namespace yart
