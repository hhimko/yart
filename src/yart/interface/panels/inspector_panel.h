////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/panel.h"
#include "yart/core/object.h"
#include "yart/GUI/gui.h"


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
            /// @brief Issue panel UI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel);

            /// @brief Issue panel UI render commands for the scene tab item
            void RenderSceneTab();

            /// @brief 
            /// @param row 
            /// @param object 
            void RenderObjectTreeRow(size_t row, yart::Object* object);

            /// @brief 
            /// @param row 
            void RenderObjectTreeRowEmpty(size_t row);

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::INSPECTOR_PANEL;

        private:
            static constexpr ImU32 OBJECT_TREE_ROW_BG_COL_ODD   = GUI::ColorConvertFloat4ToU32({ 0.022f, 0.022f, 0.022f, 1.0f });
            static constexpr ImU32 OBJECT_TREE_ROW_BG_COL_EVEN  = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, 1.0f });
            static constexpr ImU32 OBJECT_TREE_ROW_BG_COL_HOVER = GUI::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, 1.0f });

        };

    } // namespace Interface
} // namespace yart
