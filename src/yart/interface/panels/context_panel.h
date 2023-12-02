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
        public:
            /// @brief ContextPanel class constructor
            ContextPanel()
                : Panel(ContextPanel::TYPE) { }

        private:
            /// @brief Issue panel GUI render commands
            /// @return Whether any changes were made by the user during this frame
            bool OnRender();

            /// @brief Render the context's side nav bar
            static void RenderContextNavBar();

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::CONTEXT_PANEL;

        };

    } // namespace Interface
} // namespace yart
