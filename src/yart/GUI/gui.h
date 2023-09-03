////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 


#include <functional>
#include <vector>
#include <memory>

#include <imgui.h>
#include <glm/glm.hpp>

#include "font/IconsCodicons.h"


// -- YART APPLICATION COLOR PALETTE -- //
#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    // Shades of gray
    #define YART_GUI_COLOR_WHITE         1.000f, 1.000f, 1.000f
    #define YART_GUI_COLOR_LIGHTEST_GRAY 0.380f, 0.380f, 0.380f
    #define YART_GUI_COLOR_LIGHTER_GRAY  0.260f, 0.260f, 0.260f
    #define YART_GUI_COLOR_LIGHT_GRAY    0.140f, 0.140f, 0.140f
    #define YART_GUI_COLOR_GRAY          0.100f, 0.100f, 0.100f
    #define YART_GUI_COLOR_DARK_GRAY     0.060f, 0.060f, 0.060f
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

    // Text colors
    #define YART_GUI_TEXT_COLOR_LIGHT 0.800f, 0.800f, 0.800f
    #define YART_GUI_TEXT_COLOR_DARK  0.500f, 0.500f, 0.500f
#endif // ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    namespace GUI
    {
        /// @brief Callback function for rendering custom Dear ImGui windows  
        typedef std::function<void()> imgui_callback_t;

        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GuiContext; // Opaque type without including `gui_internal.h`

        /// @brief Structure used for storing state of the GradientEditor widgets
        struct GradientEditorContext {
        public:
            /// @brief Type of the gradient values collection
            typedef std::vector<glm::vec3> values_t;
            /// @brief Type of the gradient locations collection
            typedef std::vector<float> locations_t;

            /// @brief GradientEditorContext Constructor
            /// @param values Vector of the gradient values
            /// @param locations  Vector of the gradient value locations
            GradientEditorContext(values_t& values, locations_t& locations)
                : values(values), locations(locations) {}

        public:
            /// @param values Vector of the gradient values
            values_t& values;
            /// @param locations Vector of the gradient value locations
            locations_t& locations;
            /// @brief Internal member, indicating the index of the currently active color picker handle
            uint8_t selectedItemIndex = std::numeric_limits<uint8_t>::max();
            /// @brief Internal member, storing identifying IDs of color picker handles
            std::unique_ptr<ImGuiID[]> ids = nullptr;

        };


        /// @brief Get the active GUI context
        /// @return The currently active context object
        GuiContext* GetCurrentContext();

        /// @brief Issue Dear ImGui render commands for the current GUI context
        /// @note This method should only be called after calling `ImGui::NewFrame()`
        void Render();

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis);

        /// @brief Apply the default YART application GUI style and color palette
        void ApplyCustomStyle();

        /// @brief Load the YART application fonts into Dear ImGui
        /// @note Should only ever be called before uploading font textures to the GPU
        void LoadFonts();

        /// @brief Push the icons font to the Dear ImGui font stack 
        /// @note Call `ImGui::PopFont()` to return to the previous font in the stack
        void PushIconsFont();

        /// @brief Helper function for getting a Dear ImGui ID from a formatted string
        /// @param fmt Format of the string
        /// @return Dear ImGui ID
        ImGuiID GetIDFormatted(const char* fmt, ...);

        /// @brief Get the current position of the render viewport area
        /// @return Position in screen pixel coordinates
        ImVec2 GetRenderViewportAreaPosition();

        /// @brief Get the current size of the render viewport area
        /// @return Size in screen pixels
        ImVec2 GetRenderViewportAreaSize();

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

        /// @brief Begin a YART GUI style collapsable header
        /// @param name Name of the section
        /// @return Whether the section is currently opened and the contents should be rendered
        bool BeginCollapsableSection(const char* name);

        /// @brief Finish recording a YART GUI style collapsable header
        /// @param was_open The value returned by GUI::BeginCollapsableSection()
        void EndCollapsableSection(bool was_open);

        /// @brief Begin a new named frame window  
        /// @param name Name of the frame, displayed as the header
        /// @param rows Number of rows to be displayed inside the frame. Used for determining the frame height
        void BeginFrame(const char* name, uint32_t rows);

        /// @brief Finish recording a frame window
        void EndFrame();

        /// @brief Draw width aware text to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the text
        /// @param p_min Text rectangle upper-left corner in screen space coordinates
        /// @param p_max Text rectangle lower-right corner in screen space coordinates
        /// @param align Y-axis text alignment. Expected to be in the [0..1] range
        /// @param text String to render
        /// @return Whether the text had been clipped. Useful for showing tooltips
        bool DrawText(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float align, const char* text);

        /// @brief Draw a left facing arrow to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the arrow
        /// @param p_min Arrow bounding box top-left corner
        /// @param p_max Arrow bounding box bottom-right corner
        /// @param padding X and Y padding values for the bounding box 
        /// @param col Arrow fill color
        void DrawLeftArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col);

        /// @brief  Draw a right facing arrow to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the arrow
        /// @param p_min Arrow bounding box top-left corner
        /// @param p_max Arrow bounding box bottom-right corner
        /// @param padding X and Y padding values for the bounding box 
        /// @param col Arrow fill color
        void DrawRightArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col);

        /// @brief Add a rectangle filled with a linear gradient to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the rectangle
        /// @param p_min Rectangle upper-left corner in screen space coordinates
        /// @param p_max Rectangle lower-right corner in screen space coordinates
        /// @param values Array of gradient sampling point color values
        /// @param locations Array of gradient sampling point locations 
        ///     Size of the array is expected to be equal to the `values` array size, and each value should be in the [0..1] range
        /// @param size Size of the `values` and `locations` arrays
        /// @param border Whether a 1px border should be drawn over the rect
        void DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, glm::vec3 const* values, float const* locations, size_t size, bool border = false);

        /// @brief Render a YART GUI style slider widget for a float variable 
        /// @param name Label text displayed next to the slider
        /// @param p_val Pointer to the controlled value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderFloat(const char* name, float* p_val, const char* format = "%.2f", float arrow_step = 1.0f);

        /// @brief Render a YART GUI style slider widget for a float variable with value clamping
        /// @param name Label text displayed next to the slider
        /// @param p_val Pointer to the controlled value
        /// @param min The minimal (inclusive) valid value
        /// @param max The maximal (inclusive) valid value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderFloat(const char* name, float* p_val, float min, float max, const char* format = "%.2f", float arrow_step = 1.0f);

        /// @brief Render a YART GUI style combo header widget
        /// @param name Identifying name of the widget
        /// @param items Array of combo items
        /// @param items_size Size of the `items` array
        /// @param selected_item The currently selected combo item index
        void ComboHeader(const char* name, const char* items[], size_t items_size, int* selected_item); 

        /// @brief Render a linear gradient editor widget
        /// @param ctx Object holding the widget's state
        /// @return Whether the gradient has changed since the last frame
        bool GradientEditor(GradientEditorContext& ctx);

        /// @brief Render a horizontal separator line over the full window width
        /// @param thickness Separator line thickness
        void FullWidthSeparator(float thickness);
        
    } // namespace GUI
} // namespace yart
