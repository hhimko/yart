////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects definitions to hide implementation detail
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
        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GUIContext {
            /// @brief Structure containing data required to render a YART GUI window
            struct ImGuiWindow {
                /// @brief Window title
                const char* name; 

                /// @brief Window contents immediate rendering callback  
                imgui_callback_t callback;
            
            };


            /// @brief GUI windows registered by the application  
            std::vector<ImGuiWindow> registeredImGuiWindows;
            
        };

    } // namespace GUI
} // namespace yart
