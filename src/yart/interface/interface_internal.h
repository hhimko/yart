////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal Interface module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the Interface module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>
#include <memory>

#include <imgui_internal.h>
#include <glm/glm.hpp>

#include "interface.h"


namespace yart
{
    namespace Interface
    {
        /// @brief Interface module internal context structure
        struct InterfaceContext {
        public:
            std::vector<callback_t> registeredCallbacks; ///< Custom Dear ImGui render function callbacks registered by the application
            bool shouldRefreshViewports = true; ///< Whether all viewports should be refreshed next frame
        };


        /// @brief Get the current Interface module context
        /// @return The currently active context object
        InterfaceContext* GetInterfaceContext();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// YART application's UI layout rendering functions 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Load and attach the default YART application UI layout
        void ApplyDefaultLayout();

        /// @brief Render the YART application window main menu bar
        /// @return Height of the menu bar, used for determining the content area size
        float RenderMainMenuBar();

        /// @brief Render a YART application style detached window 
        /// @details The window is rendered on top of the viewport stack, detached from the static layout
        /// @param name Name of the window
        /// @param callback Window contents rendering function callback 
        void RenderWindow(const char* name, callback_t callback);

    } // namespace Interface
} // namespace yart
