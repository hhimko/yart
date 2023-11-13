////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <imgui_internal.h>

#include "yart/core/viewport.h"
#include "gui.h"


#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define YART_GUI_TEXT_ALIGN_LEFT    0.0f
    #define YART_GUI_TEXT_ALIGN_CENTER  0.5f
    #define YART_GUI_TEXT_ALIGN_RIGHT   1.0f
    #define YART_GUI_DEFAULT_TEXT_ALIGN YART_GUI_TEXT_ALIGN_RIGHT
#endif


namespace yart
{
    namespace GUI
    {
        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GuiContext {
        public:
            /// @brief Flags for the current GUI item
            GuiItemFlags currentItemFlags;
            /// @brief Flags for the next GUI item
            GuiItemFlags nextItemFlags;
            /// @brief Whether the next GUI item begins a multi-item group
            bool startMultiItems;
            /// @brief Count of remaining items inside a multi-item group
            uint8_t multiItemsCount;

            /// @brief Pointer to a Dear ImGui icon Font object 
            ImFont* iconsFont = nullptr; 
        };


        /// @brief Get the current GUI context
        /// @return The currently active context object
        GuiContext* GetGuiContext();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Custom GUI widgets rendering internal functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Draw the layout separator handle and handle its inputs
        /// @param pos Start position of the handle's bounding box
        /// @param size Size of the handle's bounding box
        /// @param cursor Cursor to be used while the handle is hovered over
        /// @return Mouse drag amount since last frame 
        ImVec2 LayoutSeparatorHandleEx(ImVec2 pos, ImVec2 size, ImGuiMouseCursor_ cursor);

        /// @brief Render a YART GUI style label widget
        /// @param name Name of the widget, displayed next to the label 
        /// @param text Label text
        void LabelEx(const char* name, const char* text);

        /// @brief Render a YART GUI style slider widget
        /// @param name Label text displayed next to the slider
        /// @param data_type Type of the slider variable
        /// @param p_val Pointer to the controlled value
        /// @param p_min Pointer to the minimal value. Can be `NULL`
        /// @param p_max Pointer to the maximal value. Can be `NULL`
        /// @param format Format in which to display the value
        /// @param p_arrow_step Pointer to the step of change in value when using the frame arrows. Should never be `NULL`
        /// @return Whether the input value has changed  
        bool SliderEx(const char* name, ImGuiDataType data_type, void* p_val, const void* p_min, const void* p_max, const char* format, const void* p_arrow_step);

        /// @brief Render a YART GUI style linear gradient editor widget
        /// @param ctx Object holding the widget's state
        /// @return Whether the gradient has changed since the last frame
        bool GradientEditorEx(GradientEditorContext& ctx);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Various primitive drawing helper functions 
        ///  - Designed to extend the Dear ImGui's ImDrawList object  
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Add a YART GUI style item frame to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the frame
        /// @param p_min Frame rectangle upper-left corner in screen space coordinates
        /// @param p_max Frame rectangle lower-right corner in screen space coordinates
        /// @param color Frame background color. Usually the return value of GUI::GetFrameColor()
        /// @param draw_flags Additional Dear ImGui draw flags
        void DrawItemFrame(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 color, ImDrawFlags draw_flags = ImDrawFlags_None);

        /// @brief Add a width aware text to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the text
        /// @param p_min Text rectangle upper-left corner in screen space coordinates
        /// @param p_max Text rectangle lower-right corner in screen space coordinates
        /// @param text String to render
        /// @param align Y-axis text alignment. Expected to be in the [0..1] range
        /// @param frame_padding Whether to use horizontal frame padding for offset from the bounding box edges
        /// @return Whether the text had been clipped. Useful for showing tooltips
        bool DrawText(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const char* text, float align = YART_GUI_DEFAULT_TEXT_ALIGN, bool frame_padding = false);

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

        /// @brief Add a rounded rectangle filled with a gradient to a Dear ImGui draw list
        /// @param draw_list Dear ImGui's draw list on which to render
        /// @param p_min Draw rect upper-left corner 
        /// @param p_max Draw rect lower-right corner
        /// @param min_col Start color of the gradient
        /// @param max_col End color of the gradient
        /// @param rounding Optional edge rounding value 
        /// @param flags Additional Dear ImGui draw flags 
        void DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 min_col, ImU32 max_col, float rounding = 0.0f, ImDrawFlags flags = ImDrawFlags_None);

        /// @brief Add a YART GUI style highlight rect to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui's draw list on which to render
        /// @param p_min Highlight rectangle upper-left corner 
        /// @param p_max Highlight rectangle lower-right corner
        /// @param t Max gradient color interpolation value. Expected to be in the [0..1] range
        /// @param hovered Whether the highlight is currently hovered
        /// @param active Whether the highlight is currently active 
        /// @param flags Additional Dear ImGui draw flags 
        void DrawFrameHighlight(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float t, bool hovered, bool active, ImDrawFlags flags = ImDrawFlags_RoundCornersNone);

        /// @brief Add a left facing arrow to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the arrow
        /// @param p_min Arrow bounding box top-left corner
        /// @param p_max Arrow bounding box bottom-right corner
        /// @param padding X and Y padding values for the bounding box 
        /// @param col Arrow fill color
        void DrawLeftArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col);

        /// @brief Add a right facing arrow to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the arrow
        /// @param p_min Arrow bounding box top-left corner
        /// @param p_max Arrow bounding box bottom-right corner
        /// @param padding X and Y padding values for the bounding box 
        /// @param col Arrow fill color
        void DrawRightArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Various helper & utility functions for the GUI module
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Validate, fix and return the current GUI item flags
        /// @details Additionally sets the `currentItemFlags` GuiContext variable and clears `nextItemFlags`
        /// @note Should be called at the very top of every widget rendering function 
        /// @return Flags for the current GUI item
        GuiItemFlags GetCurrentItemFlags();

        /// @brief Helper function for getting a Dear ImGui ID from a formatted string
        /// @param fmt Format of the string
        /// @return Dear ImGui ID
        ImGuiID GetIDFormatted(const char* fmt, ...);

        /// @brief Compute standard YART GUI bounding boxes for the next widget
        /// @param text_bb Bounding box of the label area
        /// @param frame_bb Bounding box of the widget frame area
        /// @param square_frame Whether the item frame should be a square. Used for combo box rendering
        /// @return The total bounding box 
        ImRect CalculateItemSizes(ImRect& text_bb, ImRect& frame_bb, bool square_frame = false);

        /// @brief Helper function for retrieving the frame background color for the current item
        /// @param hovered Whether the current item is currently hovered
        /// @param active Whether the current item is currently active
        /// @return Item frame background color
        ImU32 GetFrameColor(bool hovered, bool active);

    } // namespace GUI
} // namespace yart
