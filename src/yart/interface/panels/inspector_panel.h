////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the InspectorPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>
#include <imgui_internal.h>

#include "yart/interface/panel.h"
#include "yart/core/object.h"
#include "yart/core/scene.h"
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
            /// @param name Unique name of the panel, for saving and retrieving settings
            InspectorPanel(const char* name)
                : Panel(name, InspectorPanel::TYPE) { }

        private:
            /// @brief Issue panel UI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel);

            /// @brief Issue panel UI render commands for the scene tab item
            void RenderSceneTab();

            /// @brief Issue object tree UI render commands for collection rows 
            /// @param row Row number
            /// @param collection Collection instance for which to render the row
            void RenderObjectTreeRowCollection(size_t row, yart::SceneCollection* collection);

            /// @brief Issue object tree UI render commands for object rows 
            /// @param row Row number
            /// @param indent Item indent level in the hierarchy
            /// @param object Object instance for which to render the row
            void RenderObjectTreeRowObject(size_t row, uint8_t indent, yart::Object* object);

            /// @brief Issue object tree UI render commands for empty rows 
            /// @param row Row number
            void RenderObjectTreeRowEmpty(size_t row);

            /// @brief Calculate the object tree row bounding box 
            /// @return Next object tree item bounding rect
            ImRect GetObjectTreeRowRect();

            /// @brief Helper function for calculating object tree rows background color
            /// @param row Row number
            /// @param hovered whether the item is currently hovered
            /// @param selected whether the item is currently selected
            /// @return Row background color
            ImU32 GetObjectTreeRowColorH(size_t row, bool hovered, bool selected);

            /// @brief Helper function for rendering common parts of object tree items
            /// @param rect Item bounding box
            /// @param row Row number
            /// @param indent Item indent level in the hierarchy
            /// @param color Background color of the item 
            /// @param icon Icon displayed on the tree item 
            /// @param text Label text displayed on the tree item 
            void RenderObjectTreeRowH(const ImRect& rect, size_t row, uint8_t indent, ImU32 color, const char* icon, const char* text);

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::INSPECTOR_PANEL;

        };

    } // namespace Interface
} // namespace yart
