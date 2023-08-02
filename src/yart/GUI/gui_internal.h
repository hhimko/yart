////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include "gui.h"


namespace yart
{
    namespace GUI
    {
        struct GUIContext {
            struct ImGuiWindow {
                const char* name;
                imgui_callback_t callback;
            };

            std::vector<ImGuiWindow> registeredImGuiWindows;
        };

    } // namespace GUI
} // namespace yart
