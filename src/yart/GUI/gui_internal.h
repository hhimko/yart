////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Renderer class
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
