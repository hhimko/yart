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
#include "yart/core/viewport.h"


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
    #define YART_GUI_COLOR_DARK_PRIMARY  0.066f, 0.179f, 0.344f
    #define YART_GUI_COLOR_PRIMARY       0.054f, 0.206f, 0.427f
    #define YART_GUI_COLOR_LIGHT_PRIMARY 0.072f, 0.243f, 0.535f

    // Shades of secondary 
    #define YART_GUI_COLOR_DARK_SECONDARY  0.073f, 0.329f, 0.382f
    #define YART_GUI_COLOR_SECONDARY       0.057f, 0.421f, 0.497f
    #define YART_GUI_COLOR_LIGHT_SECONDARY 0.045f, 0.493f, 0.586f

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


/// @brief GUI item flags type for the GuiItemFlags_ enum
using GuiItemFlags = uint16_t; 

/// @brief Common flags enum for specifying GUI items appearance/behavior
enum GuiItemFlags_ : GuiItemFlags {
    GuiItemFlags_None =               (0),      /// @brief No flags
    GuiItemFlags_HideLabel =          (1 << 0), /// @brief Don't display a text label next to the item's frame 
    GuiItemFlags_FullWidth =          (1 << 1), /// @brief Display the item over the full content width. Hides item's label in result
    GuiItemFlags_CornersRoundTop =    (1 << 2), /// @brief Only round the top corners of the item's frame
    GuiItemFlags_CornersRoundBottom = (1 << 3), /// @brief Only round the bottom corners of the item's frame
    GuiItemFlags_NoCornerRounding =   (1 << 4), /// @brief Don't use any rounding on the item's frame
    GuiItemFlags_FrameBorder =        (1 << 5), /// @brief Display the item's frame border 
    GuiItemFlags_FrameStyleDark =     (1 << 6), /// @brief Use a dark style for rendering the item's frame
};


namespace yart
{
    namespace GUI
    {
        /// @brief GUI layout direction enum
        enum class LayoutDirection : uint8_t {
            HORIZONTAL,
            VERTICAL
        };

        /// @brief Layout specification object used to store state of layout widgets
        struct LayoutContext {
        public:
            /// @brief Direction of the layout (vertical / horizontal)
            LayoutDirection direction;
            /// @brief Whether the size should be updated based on the second layout section when resizing the OS window
            bool preserveSecondSectionSize;
            /// @brief The default layout segment sizing ratio. Should be in the [0..1] range 
            float default_size_ratio = 0.5f;
            /// @brief Separator handle position state. Negative value represents an uninitialized context
            float size = -1.0f;
            /// @brief The minimum possible size 
            float min_size = 100.0f;
        };

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


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GUI module setup functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Apply the default YART application GUI style and color palette
        void ApplyCustomStyle();

        /// @brief Load the YART application fonts into Dear ImGui
        /// @note Should only ever be called before uploading font textures to the GPU
        void LoadFonts();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GUI module public interface 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Set custom flags for the next GUI item 
        /// @param flags Flags to be set
        void SetNextItemFlags(GuiItemFlags flags);

        /// @brief Get the Dear ImGui font for GUI icons 
        /// @brief The font should be loaded with GUI::LoadFonts()
        /// @return Icons font pointer
        const ImFont* GetIconsFont();

        /// @brief Push the icons font to the Dear ImGui font stack 
        /// @note Call `ImGui::PopFont()` to return to the previous font in the stack
        void PushIconsFont();

        /// @brief Check whether the mouse cursor lies within a given circle
        /// @param pos Circle position on the screen
        /// @param radius Radius of the circle
        /// @return Whether the mouse cursor is inside circle
        bool IsMouseHoveringCircle(const ImVec2& pos, float radius);

        /// @brief Get the amount of pixels the OS window size has changed since last frame
        /// @return Display size delta amount in pixels
        ImVec2 GetDisplaySizeDelta();


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
        /// Custom GUI widgets rendering public interface
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Render a YART GUI style label widget
        /// @param name Name of the widget, displayed next to the label 
        /// @param text Formatted label text
        void Label(const char* name, const char* text, ...);

        /// @brief Render a YART GUI style slider widget for an int variable 
        /// @param name Label text displayed next to the widget
        /// @param p_val Pointer to the controlled value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderInt(const char* name, int* p_val, const char* format = "%d", int arrow_step = 1);

        /// @brief Render a YART GUI style slider widget for an int variable with value clamping
        /// @param name Label text displayed next to the widget
        /// @param p_val Pointer to the controlled value
        /// @param min The minimal (inclusive) valid value
        /// @param max The maximal (inclusive) valid value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderInt(const char* name, int* p_val, int min, int max, const char* format = "%d", int arrow_step = 1);

        /// @brief Render a YART GUI style slider widget for a float variable 
        /// @param name Label text displayed next to the widget
        /// @param p_val Pointer to the controlled value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderFloat(const char* name, float* p_val, const char* format = "%.2f", float arrow_step = 1.0f);

        /// @brief Render a YART GUI style slider widget for a float variable with value clamping
        /// @param name Label text displayed next to the widget
        /// @param p_val Pointer to the controlled value
        /// @param min The minimal (inclusive) valid value
        /// @param max The maximal (inclusive) valid value
        /// @param format Format in which to display the value
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderFloat(const char* name, float* p_val, float min, float max, const char* format = "%.2f", float arrow_step = 1.0f);

        /// @brief Render a YART GUI style slider widget for a `glm::vec3` variable
        /// @param names Array of individual component labels. Expected to be of size 3
        /// @param p_vals Pointer to the controlled value
        /// @param format Format in which to display all the vector values
        /// @param arrow_step The step of change in value when using the frame arrows
        /// @return Whether the input value has changed
        bool SliderVec3(const char* names[3], glm::vec3* p_vals, const char* format = "%.2f", float arrow_step = 1.0f);

        /// @brief Render a YART GUI style checkbox widget
        /// @param name Label text displayed next to the checkbox
        /// @param val Pointer to the controlled boolean value
        /// @return Whether the input value has changed
        bool CheckBox(const char* name, bool* val);

        /// @brief Render a YART GUI style combo header widget
        /// @param name Label text displayed next to the widget
        /// @param items Array of combo items
        /// @param items_size Size of the `items` array
        /// @param selected_item The currently selected combo item index
        /// @returns Whether the selected item has changed since last frame
        bool ComboHeader(const char* name, const char* items[], size_t items_size, int* selected_item); 

        /// @brief Render a YART GUI style color edit widget
        /// @param name Label text displayed next to the widget
        /// @param color 
        /// @return 
        bool ColorEdit(const char* name, float color[3]);

        /// @brief Render a linear gradient editor widget
        /// @param ctx Object holding the widget's state
        /// @return Whether the gradient has changed since the last frame
        bool GradientEditor(GradientEditorContext& ctx);

        /// @brief Render a horizontal separator line over the full window width
        /// @param thickness Separator line thickness
        void FullWidthSeparator(float thickness);

        /// @brief Begin a YART GUI style collapsable header
        /// @param name Name of the section
        /// @return Whether the section is currently opened and the contents should be rendered
        bool BeginCollapsableSection(const char* name);

        /// @brief Finish recording a YART GUI style collapsable header
        /// @param was_open The value returned by GUI::BeginCollapsableSection()
        void EndCollapsableSection(bool was_open);

        /// @brief Begin a YART GUI style tab bar with an initial tab item
        /// @details Subsequent tab items can be issued calling `ImGui::BeginTabItem()`
        /// @param item_name First tab item name
        /// @return Whether the first tab item is currently opened
        /// @note `ImGui::EndTabBar()` should always be called after calling this method
        bool BeginTabBar(const char* item_name);

        /// @brief Finish recording a YART GUI style tab bar
        void EndTabBar();

        /// @brief Begin a new named frame window  
        /// @param name Name of the frame, displayed as the header
        /// @param rows Number of rows to be displayed inside the frame. Used for determining the frame height
        void BeginFrame(const char* name, uint32_t rows);

        /// @brief Finish recording a frame window
        void EndFrame();

        /// @brief Record the next `count` of elements in a single tightly-packed group
        /// @param count Number of GUI widgets inside the group
        void BeginMultiItem(uint8_t count);

        /// @brief Finish recording a multi item group 
        void EndMultiItem();
        
    } // namespace GUI
} // namespace yart
