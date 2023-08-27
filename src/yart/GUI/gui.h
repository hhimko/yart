////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 


#include <functional>

#include <imgui.h>
#include <glm/glm.hpp>

#include "font/IconsCodicons.h"


// -- YART APPLICATION COLOR PALETTE -- //
#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    // Shades of gray
    #define YART_GUI_COLOR_WHITE         1.000f, 1.000f, 1.000f
    #define YART_GUI_COLOR_LIGHTEST_GRAY 0.320f, 0.320f, 0.320f
    #define YART_GUI_COLOR_LIGHTER_GRAY  0.240f, 0.240f, 0.240f
    #define YART_GUI_COLOR_LIGHT_GRAY    0.160f, 0.160f, 0.160f
    #define YART_GUI_COLOR_GRAY          0.120f, 0.120f, 0.120f
    #define YART_GUI_COLOR_DARK_GRAY     0.100f, 0.100f, 0.100f
    #define YART_GUI_COLOR_DARKER_GRAY   0.031f, 0.031f, 0.031f
    #define YART_GUI_COLOR_DARKEST_GRAY  0.010f, 0.010f, 0.010f
    #define YART_GUI_COLOR_BLACK         0.000f, 0.000f, 0.000f

    // Shades of primary 
    #define YART_GUI_COLOR_DARK_PRIMARY  0.06f, 0.00f, 0.45f
    #define YART_GUI_COLOR_PRIMARY       0.12f, 0.01f, 0.82f
    #define YART_GUI_COLOR_LIGHT_PRIMARY 0.16f, 0.04f, 0.90f

    // Shades of secondary 
    #define YART_GUI_COLOR_DARK_SECONDARY  0.34f, 0.01f, 0.44f
    #define YART_GUI_COLOR_SECONDARY       0.34f, 0.01f, 0.44f
    #define YART_GUI_COLOR_LIGHT_SECONDARY 0.54f, 0.02f, 0.62f

    // Shades of tertiary 
    #define YART_GUI_COLOR_DARK_TERTIARY  0.02f, 0.72f, 0.55f
    #define YART_GUI_COLOR_TERTIARY       0.00f, 1.00f, 0.72f
    #define YART_GUI_COLOR_LIGHT_TERTIARY 0.00f, 1.00f, 0.72f

    // Levels of transparency
    #define YART_GUI_ALPHA_OPAQUE      1.00f
    #define YART_GUI_ALPHA_HIGH        0.75f
    #define YART_GUI_ALPHA_MEDIUM      0.50f
    #define YART_GUI_ALPHA_LOW         0.25f
    #define YART_GUI_ALPHA_TRANSPARENT 0.00f
#endif // ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    namespace GUI
    {
        /// @brief Callback function for rendering custom Dear ImGui windows  
        typedef std::function<void()> imgui_callback_t;

        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GuiContext; // Opaque type without including `gui_internal.h`


        /// @brief Get the active GUI context
        /// @return The currently active context object
        GuiContext* GetCurrentContext();

        /// @brief Apply the default YART application GUI style and color palette
        void ApplyCustomStyle();

        /// @brief Load the YART application fonts into Dear ImGui
        /// @note Should only ever be called before uploading font textures to the GPU
        void LoadFonts();

        /// @brief Push the icons font to the Dear ImGui font stack 
        /// @note Call `ImGui::PopFont()` to return to the previous font in the stack
        void PushIconsFont();

        /// @brief Get the current position of the render viewport area
        /// @return Position in screen pixel coordinates
        ImVec2 GetMainViewportAreaPosition();

        /// @brief Get the current size of the render viewport area
        /// @return Size in screen pixels
        ImVec2 GetMainViewportAreaSize();

        /// @brief Test whether the mouse cursor is currently directly over the render viewport
        /// @return Whether mouse is over viewport
        bool IsMouseOverRenderViewport();

        /// @brief Register a global Dear ImGui render function
        /// @param callback Callback function pointer
        void RegisterCallback(imgui_callback_t callback);

        /// @brief Register a nav bar menu item, visible in the main inspector window
        /// @param name Name of the nav bar item
        /// @param icon Nav bar icon code point from the application's icons font 
        /// @param color Nav bar icon color
        /// @param callback Callback function pointer to the nav bar item contents
        void RegisterInspectorWindow(const char* name, const char* icon, ImU32 color, imgui_callback_t callback);

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis);

        /// @brief Issue Dear ImGui render commands for the current GUI context
        /// @note This method should only be called after calling `ImGui::NewFrame()`
        void Render();
        
    } // namespace GUI
} // namespace yart
