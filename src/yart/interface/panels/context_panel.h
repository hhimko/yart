////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the ContextPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <imgui.h>

#include "yart/interface/views/view.h"
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
            /// @param name Unique name of the panel, for saving and retrieving settings
            ContextPanel(const char* name)
                : Panel(name, ContextPanel::TYPE) { }

        private:
            /// @brief Issue panel GUI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel);

            /// @brief Render the context's side nav bar
            /// @param target Output parameter, populated with the returned active view's target instance. 
            ///     Guaranteed to never be `nullptr`
            /// @return Active view instance. Guaranteed to never be `nullptr`
            static const Interface::View* RenderContextNavBar(void** target);

            /// @brief Render a specified context view within the context window
            /// @param view UI view to be rendered
            /// @param target Target instance for the view
            /// @return Whether any changes were made by the user during this frame
            static bool RenderContextView(const Interface::View* view, void* target);

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::CONTEXT_PANEL;

        };

    } // namespace Interface
} // namespace yart
