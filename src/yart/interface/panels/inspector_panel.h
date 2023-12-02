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
        public:
            /// @brief InspectorPanel class constructor
            InspectorPanel()
                : Panel(InspectorPanel::TYPE) { }

        private:
            /// @brief Issue panel GUI render commands
            /// @return Whether any changes were made by the user during this frame
            bool OnRender();

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::INSPECTOR_PANEL;

        };

    } // namespace Interface
} // namespace yart
