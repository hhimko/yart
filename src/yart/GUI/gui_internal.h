////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects and methods definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <imgui_internal.h>

#include "gui.h"


namespace yart
{
    namespace GUI
    {
        /// @brief Structure containing data required to render a YART GUI window
        struct GuiWindow {
        public:
            GuiWindow() = delete;
            GuiWindow(const char* name, imgui_callback_t callback);

        public:
            /// @brief Window title
            const char* name; 

            /// @brief Window contents immediate rendering callback  
            imgui_callback_t callback;

        };


        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GuiContext {
        public:
            /// @brief GUI windows registered by the application  
            std::vector<GuiWindow> registeredWindows;
            
        };


        void RenderWindow(const GuiWindow& window);

    } // namespace GUI
} // namespace yart
