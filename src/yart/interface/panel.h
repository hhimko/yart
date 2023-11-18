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

        private:
            /// @brief Issue panel GUI render commands
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            virtual bool Render(ImGuiWindow* window) = 0;

        private:
            /// @brief Parent panel in hierarchy
            Panel* m_parent = nullptr;

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
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

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
            /// @param direction Direction of the layout
            /// @param ul_child Either the upper or left child panel, depending on the chosen layout direction
            /// @param lr_child Either the lower or right child panel, depending on the chosen layout direction
            LayoutPanel(yart::GUI::LayoutDirection direction, Panel* ul_child, Panel* lr_child);

            /// @brief Custom destructor
            ~LayoutPanel();

        private:
            /// @brief Issue panel GUI render commands
            /// @param window Dear ImGui window representing the panel
            /// @return Whether any changes were made by the user during this frame
            bool Render(ImGuiWindow* window);

        private:
            yart::GUI::LayoutContext m_layoutContext;   

            Panel* m_ul_child = nullptr;
            Panel* m_lr_child = nullptr;

        };

    } // namespace Interface
} // namespace yart
