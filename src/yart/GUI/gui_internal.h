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

            /// @brief GuiWindow constructor
            /// @param name Window title
            /// @param callback Window contents immediate rendering callback  
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
            /// @brief Custom Dear ImGui render function callbacks registered by the application
            std::vector<imgui_callback_t> registeredCallbacks;

            /// @brief GUI windows registered by the application  
            std::vector<GuiWindow> registeredWindows;
            
        };


        bool BeginHorizontalLayout(float& width);

        bool BeginVerticalLayout(float& height);

        /// @brief Goes to the next segment
        bool HorizontalLayoutSeparator(float& width);

        /// @brief Goes to the next segment
        bool VerticalLayoutSeparator(float& height);

        void EndLayout();

        /// @brief Check whether the mouse cursor lies within a given circle
        /// @param pos Circle position on the screen
        /// @param radius Radius of the circle
        /// @return Whether the mouse cursor is inside circle
        bool IsMouseHoveringCircle(const ImVec2& pos, float radius);

        /// @brief Render a YART GUI window 
        /// @param window Window to be rendered
        void RenderWindow(const GuiWindow& window);

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool RenderViewAxesWindowEx(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis);

    } // namespace GUI
} // namespace yart
