////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects and methods definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <glm/glm.hpp>
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


        /// @brief Render a YART GUI window 
        /// @param window Window to be rendered
        void RenderWindow(const GuiWindow& window);

        /// @brief Check whether the mouse cursor lies within a given circle
        /// @param pos Circle position on the screen
        /// @param radius Radius of the circle
        /// @return Whether the mouse cursor is inside circle
        bool IsMouseHoveringCircle(const ImVec2& pos, float radius);

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        void RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis);

    } // namespace GUI
} // namespace yart
