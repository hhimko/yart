////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Common definitions for application UI panels 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/GUI/gui.h"


/// @brief Opaque `ImGuiWindow` type to avoid importing `imgui_internal.h` in the header
struct ImGuiWindow; 


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Abstract base class for application UI panels
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class Panel {
        public:
            /// @brief Virtual destructor
            virtual ~Panel() = default;

        protected:
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
            /// @brief Render the panel into a given Dear ImGui window
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

            /// @brief Issue panel GUI render commands. Internal method intended to be overriden by child classes
            /// @return Whether any changes were made by the user during this frame
            virtual bool OnRender() = 0;

        private:
            /// @brief Parent panel in hierarchy
            Panel* m_parent = nullptr;

            /// @brief Dear ImGui window representing the panel at a given frame
            ImGuiWindow* m_window;

            // -- FRIEND DECLARATIONS -- //
            friend class RootAppPanel;
            friend class LayoutPanel;

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief YART application root (main) panel singleton used for attaching custom layouts to the UI
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RootAppPanel : public Panel {
        public:
            /// @brief Get the static singleton instance of the RootAppPanel class
            /// @return Static singleton instance
            static RootAppPanel* Get();

            /// @brief Attach a child panel to the root
            /// @param panel Panel to be attached, or `nullptr` to detach the current layout
            void AttachPanel(Panel* panel);

            /// @brief Render the currently attached UI layout
            /// @param menu_bar_height Height of the main menu bar, used for determining the content area size
            /// @return Whether any changes were made by the user during this frame
            bool Render(const float menu_bar_height);

        private:
            /// @brief Private destructor
            ~RootAppPanel()
            {
                // Deleting the root's child should destroy the whole layout in a dominos effect
                delete m_child;
            }

            /// @brief Issue panel GUI render commands
            /// @return Whether any changes were made by the user during this frame
            bool OnRender();

        private:
            RootAppPanel() = default;

        private:
            Panel* m_child = nullptr;

        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Panel for building UI layouts with two segments (child panels)
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class LayoutPanel : public Panel {
        public:
            /// @brief Construct a new layout panel
            /// @param layout_create_info Layout creation specification object
            /// @param ul_child Either the upper or left child panel, depending on the chosen layout direction
            /// @param lr_child Either the lower or right child panel, depending on the chosen layout direction
            LayoutPanel(yart::GUI::LayoutCreateInfo& layout_create_info, Panel* ul_child, Panel* lr_child);

            /// @brief Custom destructor
            ~LayoutPanel();

        private:
            /// @brief Issue panel GUI render commands
            /// @return Whether any changes were made by the user during this frame
            bool OnRender();

        private:
            yart::GUI::GuiLayout* m_layout = nullptr;   

            Panel* m_ul_child = nullptr;
            Panel* m_lr_child = nullptr;

        };

    } // namespace Interface
} // namespace yart
