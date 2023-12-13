////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the LayoutPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "yart/interface/panels/panel_settings.h"
#include "yart/interface/panel.h"
#include "yart/GUI/gui.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saveable settings class for layout panels
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class LayoutPanelSettings : public PanelSettings {
        public:
            /// @brief LayoutPanelSettings class custom constructor
            /// @param panel Panel instance, for which the settings apply
            LayoutPanelSettings(const Panel* const panel)
                : PanelSettings(panel) { }

        public:

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Panel for building UI layouts with two segments (child panels)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class LayoutPanel : public ContainerPanel, SaveablePanel<LayoutPanelSettings> {
        public:
            /// @brief Construct a new layout panel
            /// @param name Unique name of the panel, for saving and retrieving settings
            /// @param layout_create_info Layout creation specification object
            /// @param ul_child Either the upper or left child panel, depending on the chosen layout direction
            /// @param lr_child Either the lower or right child panel, depending on the chosen layout direction
            LayoutPanel(const char* name, yart::GUI::LayoutCreateInfo& layout_create_info, Panel* ul_child, Panel* lr_child);

            /// @brief Custom destructor
            ~LayoutPanel();

        private:
            /// @brief Get the saveable state of this panel 
            /// @return New panel settings object for this instance
            LayoutPanelSettings GetPanelSettings() const override;

            /// @brief Apply a given settings object to this panel
            /// @param settings Panel settings object from which to recreate panel state
            void ApplyPanelSettings(const LayoutPanelSettings* const settings) override;

            /// @brief Try retrieving a panel of given type from the container
            /// @param type Type of the required panel
            /// @return Panel of required type, or `nullptr` if panel is not a part of the container
            Interface::Panel* GetPanel(Interface::PanelType type) const;

            /// @brief Issue panel GUI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel);

        private:
            yart::GUI::GuiLayout* m_layout = nullptr;

            Panel* m_ul_child = nullptr;
            Panel* m_lr_child = nullptr;

        };

    } // namespace Interface
} // namespace yart
