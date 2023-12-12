////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Common definitions for application UI panels 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <type_traits>
#include <iostream>

#include <imgui.h>

#include "yart/GUI/gui.h"


/// @brief Opaque `ImGuiWindow` type to avoid importing `imgui_internal.h` in the header
struct ImGuiWindow; 


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief UI panel types enum, for identifying panels in a layout 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class PanelType {
            CONTAINER_PANEL = 0, ///< Special type for root and layout panels
            CONTEXT_PANEL, ///< Type for the Interface::ContextPanel class
            INSPECTOR_PANEL, ///< Type for the Interface::InspectorPanel class
            RENDER_VIEWPORT_PANEL ///< Type for the Interface::RenderViewportPanel class
        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Base class for saveable UI panel settings
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class PanelSettings {
        public:
            /// @brief PanelSettings custom constructor
            /// @param panel_type Type of the panel, for which the settings apply
            /// @param panel_name Name of the panel, for which the settings apply
            PanelSettings(PanelType panel_type, const char* panel_name)
                : panelType(panel_type), panelName(panel_name) { }

            /// @brief PanelSettings class virtual destructor
            virtual ~PanelSettings() = default;

        public:
            const PanelType panelType; ///< Type of the panel, for which the settings apply
            const char* const panelName; ///< Name of the panel, for which the settings apply

        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Abstract base class for application UI panels
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class Panel {
        public:
            /// @brief Panel class virtual destructor
            virtual ~Panel()
            {
                const PanelSettings* settings = GetPanelSettings();
                if (settings != nullptr) {

                }
            }

        protected:
            /// @brief Panel class custom constructor
            /// @param name Unique name of the panel, for saving and retrieving settings
            /// @param type Type of the panel from the Interface::PanelType enum
            Panel(const char* name, Interface::PanelType type);

            /// @brief Get the Dear ImGui window associated with this panel
            /// @return Dear ImGui window instance
            ImGuiWindow* GetPanelWindow() const
            {
                return m_window;
            }

            /// @brief Perform a check whether the panel is currently hovered
            /// @return Whether the panel is hovered by the cursor
            bool IsPanelHovered() const;

        private:
            /// @brief Reconstruct a panel state from saved panel settings
            /// @param settings Pointer to object containing settings
            virtual void ApplyPanelSettings(const PanelSettings* const settings)
            {
                // Defaults to no setting loading
                return; 
            }

            /// @brief Create a PanelSettings object for the current state of this panel
            /// @return 
            virtual const PanelSettings* GetPanelSettings() const
            {
                // Defaults to no setting creation
                return nullptr;
            }  

            /// @brief Handle incoming user inputs
            /// @param should_refresh_viewports Output parameter, used for specifying wether any changes that 
            ///     invalidate viewports have been made
            /// @return Whether the incoming events have been handled by this panel
            virtual bool HandleInputs(bool* should_refresh_viewports)
            {
                // Defaults to no input handling
                return false;
            }

            /// @brief Render the panel into a given Dear ImGui window
            /// @param window Dear ImGui window representing the panel
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window, Panel** active_panel);

            /// @brief Issue panel GUI render commands. Internal method intended to be overriden by child classes
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            virtual bool OnRender(Panel** active_panel) = 0;

        private:
            const char* m_name; ///< Unique name of the panel, used for saving and retrieving settings
            const Interface::PanelType m_type; ///< Type of this panel, used for panel retrieval in a layout
            Interface::Panel* m_parent = nullptr; ///< Parent panel in hierarchy

            /// @brief Dear ImGui window representing the panel at a given frame
            ImGuiWindow* m_window;

            // -- FRIEND DECLARATIONS -- //
            friend class RootAppPanel;
            friend class LayoutPanel;

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Abstract base class for UI panels of container types like RootAppPanel and LayoutPanel
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ContainerPanel : public Panel {
        protected:
            /// @brief ContainerPanel class custom constructor
            /// @param name Unique name of the panel, for saving and retrieving settings
            ContainerPanel(const char* name)
                : Panel(name, ContainerPanel::TYPE) { }

            /// @brief Try retrieving a panel of given type from the container
            /// @param type Type of the required panel
            /// @return Panel of required type, or `nullptr` if panel is not a part of the container
            virtual Interface::Panel* GetPanel(Interface::PanelType type) const = 0;

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::CONTAINER_PANEL;

            // -- FRIEND DECLARATIONS -- //
            friend class RootAppPanel;
            friend class LayoutPanel;

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief YART application root (main) panel singleton used for attaching custom layouts to the UI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RootAppPanel : public ContainerPanel {
        public:
            /// @brief Get the static singleton instance of the RootAppPanel class
            /// @return Static singleton instance
            static RootAppPanel* Get();

            /// @brief Attach a child panel to the root
            /// @param panel Panel to be attached, or `nullptr` to detach the current layout
            void AttachPanel(Panel* panel);

            /// @brief Retrieve a panel of given type from the current layout
            /// @tparam P Type of the panel. Must inherit from Interface::Panel
            /// @return Panel of required type, or `nullptr` if panel is not a part of the current layout
            template <typename P>
            P* GetPanel() const
            {
                static_assert(std::is_convertible<P*, Interface::Panel*>::value, "P must inherit Panel as public");
                static_assert(P::TYPE != Interface::PanelType::CONTAINER_PANEL, "P must not be a container type panel");

                // Look up the panel top-down, starting from the active panel
                Panel* panel = m_activePanel;
                while (panel != nullptr) {
                    if (panel->m_type == P::TYPE)
                        return dynamic_cast<P*>(panel);

                    panel = panel->m_parent;
                }

                // Fall back to searching for the panel through the whole tree
                panel = GetPanel(P::TYPE);
                if (panel != nullptr)
                    return dynamic_cast<P*>(panel);

                return nullptr;
            }

            /// @brief Detach the currently attached layout
            void DetachLayout();

            /// @brief Make a specified panel active in the layout 
            /// @param panel Panel to activate, or `nullptr` to deactivate the currently active panel. 
            ///     `panel` must be a part of the current layout
            void SetActivePanel(Panel* panel)
            {
                m_activePanel = panel;
            }

            /// @brief Handle incoming user inputs
            /// @returns Whether any changes were made this frame. Used for conditional viewport refreshing
            bool HandleInputs();

            /// @brief Render the currently attached UI layout
            /// @param menu_bar_height Height of the main menu bar, used for determining the content area size
            /// @return Whether any changes were made this frame. Used for conditional viewport refreshing
            bool Render(const float menu_bar_height);

        private:
            /// @brief RootAppPanel class private constructor
            RootAppPanel()
                : ContainerPanel("RootAppPanel") { }

            /// @brief RootAppPanel class private destructor
            ~RootAppPanel() { DetachLayout(); }

            /// @brief Try retrieving a panel of given type from the container
            /// @param type Type of the required panel
            /// @return Panel of required type, or `nullptr` if panel is not a part of the container
            Interface::Panel* GetPanel(Interface::PanelType type) const;

            /// @brief Handle incoming user inputs
            /// @param should_refresh_viewports Output parameter, used for specifying wether any changes that 
            ///     invalidate viewports have been made
            /// @return Whether the incoming events have been handled by this panel
            bool HandleInputs(bool* should_refresh_viewports);

            /// @brief Issue panel GUI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel);

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::CONTAINER_PANEL;

        private:
            Panel* m_child = nullptr;
            Panel* m_activePanel = nullptr; ///< Currently active panel in the layout

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Panel for building UI layouts with two segments (child panels)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class LayoutPanel : public ContainerPanel {
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
