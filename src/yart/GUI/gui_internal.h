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
        /// @brief Flags for GUI::CalculateItemSizes()
        typedef uint32_t ItemSizesFlags; 

        /// @brief Internal flags enum for GUI::CalculateItemSizes()
        enum ItemSizesFlags_: ItemSizesFlags {
            ItemSizesFlags_None        = 0,      /// @brief No flags
            ItemSizesFlags_NoLabel     = 1 << 0, /// @brief Don't display the widget's label. Sets the width of `text_bb` to 0
            ItemSizesFlags_SquareFrame = 1 << 1, /// @brief Make the widget's frame bounding box a square 
            
        };

        /// @brief Layout direction enum
        enum class LayoutDir : uint8_t {
            HORIZONTAL,
            VERTICAL

        };

        /// @brief Structure containing data required to render a inspector nav bar window 
        struct InspectorWindow {
        public:
            /// @brief Nav bar icon code point from the application's icons font
            const char* icon;
            /// @brief Nav bar icon color
            ImU32 color;
            /// @brief Nav bar item name
            const char* name; 
            /// @brief Nav bar item contents immediate rendering callback  
            imgui_callback_t callback;

        };

        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GuiContext {
        public:
            /// @brief Custom Dear ImGui render function callbacks registered by the application
            std::vector<imgui_callback_t> registeredCallbacks;
            /// @brief Inspector nav bar items registered by the application 
            std::vector<InspectorWindow> inspectorWindows;
            /// @brief Currently open inspector nav bar window
            InspectorWindow* activeInspectorWindow = nullptr;

            /// @brief Whether any changes were made by the user within the last frame
            bool madeChanges;
            /// @brief Amount of pixels the OS window size has changed since last frame
            ImVec2 displaySizeDelta;
            /// @brief Current render viewport area position on screen in pixel coordinates
            ImVec2 renderViewportAreaPos;
            /// @brief Current width of the render viewport area in pixels
            float renderViewportAreaWidth;
            /// @brief Current height of the render viewport area in pixels
            float renderViewportAreaHeight;
            /// @brief Dear ImGui window ID of the render viewport window
            ImGuiID renderViewportWindowID;
            /// @brief Cached size of the main context window vertical separator
            float mainContextSeparatorHeight;
            /// @brief Current visible height of the main menu bar body 
            float mainMenuBarHeight;

            /// @brief Pointer to a Dear ImGui icon Font object 
            ImFont* iconsFont = nullptr; 

        };

        /// @brief Layout specification object used to store state of layout widgets
        struct LayoutContext {
        public:
            /// @brief Direction of the layout (vertical / horizontal)
            LayoutDir direction;
            /// @brief Dear ImGui window flags for child windows inside layout
            ImGuiWindowFlags window_flags;
            /// @brief Whether the size should be updated based on the second layout section when resizing the OS window
            bool preserveSecondSectionSize;
            /// @brief The default layout segment sizing ratio. Should be in the [0..1] range 
            float default_size_ratio = 0.5f;
            /// @brief Separator handle position state
            float size;
            /// @brief The minimum possible size 
            float min_size = 100.0f;
            
        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// YART application's static layout rendering functions 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Issue main menu bar render commands 
        void RenderMainMenuBar();

        /// @brief Issue the application's main content render commands 
        void RenderMainContentFrame();

        /// @brief Issue the application's context window render commands
        void RenderContextWindow();

        /// @brief Issue the inspector window's side nav bar render commands
        void RenderInspectorNavBar();

        /// @brief Render a YART GUI style detached window 
        /// @details The window is rendered on top of the viewport stack, detached from the static layout
        /// @param name Name of the window
        /// @param callback Dear ImGui callback 
        void RenderWindow(const char* name, imgui_callback_t callback);

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool RenderViewAxesWindowEx(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Layout group widgets rendering functions
        ///  - Layouts are trying to simulate and simplify the docking branch of Dear ImGui, without
        ///    the additional viewports and just the functionality required by YART
        ///  - The current layout code is limited to rendering at most two segments within one layout,
        ///    due to the `LayoutContext` structure currently holding just one `size` value
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Begin a new GUI layout
        /// @param layout Layout state object
        /// @return Whether the layout section is currently visible on screen
        bool BeginLayout(LayoutContext& layout);

        /// @brief End the previous layout segment and start the next segment
        /// @param layout Layout state object
        /// @return Whether the layout section is currently visible on screen
        bool LayoutSeparator(LayoutContext& layout);

        /// @brief Finalize rendering a layout
        /// @param layout Layout state object
        void EndLayout(LayoutContext& layout);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Custom GUI widgets rendering internal functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Begin a YART GUI style tab bar with an initial tab item
        /// @details Subsequent tab items can be issued calling `ImGui::BeginTabItem()`
        /// @param item_name First tab item name
        /// @return Whether the first tab item is currently opened
        /// @note `ImGui::EndTabBar()` should always be called after calling this method
        bool BeginTabBar(const char* item_name);

        /// @brief Finalize rendering a YART GUI style tab bar
        void EndTabBar();

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

        /// @brief Add a width aware text to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list on which to draw the text
        /// @param p_min Text rectangle upper-left corner in screen space coordinates
        /// @param p_max Text rectangle lower-right corner in screen space coordinates
        /// @param align Y-axis text alignment. Expected to be in the [0..1] range
        /// @param text String to render
        /// @return Whether the text had been clipped. Useful for showing tooltips
        bool DrawText(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float align, const char* text);

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
        void DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 min_col, ImU32 max_col, float rounding = 0.0f, ImDrawFlags flags = (ImDrawFlags)0);

        /// @brief Add a YART GUI style highlight rect to a given Dear ImGui draw list
        /// @param draw_list Dear ImGui's draw list on which to render
        /// @param p_min Highlight rectangle upper-left corner 
        /// @param p_max Highlight rectangle lower-right corner
        /// @param t Max gradient color interpolation value. Expected to be in the [0..1] range
        /// @param hovered Whether the highlight is currently hovered
        /// @param active Whether the highlight is currently active 
        /// @param rounding Optional edge rounding value 
        /// @param flags Additional Dear ImGui draw flags 
        void DrawHighlightRect(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float t, bool hovered, bool active, float rounding = 0.0f, ImDrawFlags flags = (ImDrawFlags)0);

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

        /// @brief Helper function for getting a Dear ImGui ID from a formatted string
        /// @param fmt Format of the string
        /// @return Dear ImGui ID
        ImGuiID GetIDFormatted(const char* fmt, ...);

        /// @brief Compute standard YART GUI bounding boxes for the next widget
        /// @param text_bb Bounding box of the label area
        /// @param frame_bb Bounding box of the widget frame area
        /// @param flags Optional flags from the `ItemSizesFlags_` enum
        /// @return The total bounding box 
        ImRect CalculateItemSizes(ImRect& text_bb, ImRect& frame_bb, ItemSizesFlags flags = ItemSizesFlags_None);

        /// @brief Check whether the mouse cursor lies within a given circle
        /// @param pos Circle position on the screen
        /// @param radius Radius of the circle
        /// @return Whether the mouse cursor is inside circle
        bool IsMouseHoveringCircle(const ImVec2& pos, float radius);

    } // namespace GUI
} // namespace yart
