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
        enum class LayoutDir : uint8_t {
            HORIZONTAL,
            VERTICAL
        };

        /// @brief Structure containing data required to render a YART GUI window
        struct GuiWindow {
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
            ImFont* iconsFont; 

        };

        /// @brief Layout specification object used to store state of layout widgets
        struct LayoutState {
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
            
        };


        /// @brief Begin a new GUI layout
        /// @param layout Layout state
        /// @return Whether the layout section is currently visible on screen
        bool BeginLayout(LayoutState& layout);

        /// @brief End the previous layout segment and start the next segment
        /// @param layout Layout state
        /// @return Whether the layout section is currently visible on screen
        bool LayoutSeparator(LayoutState& layout);

        /// @brief Finalize rendering a layout
        /// @param layout Layout state
        void EndLayout(LayoutState& layout);

        /// @brief Begin a custom YART tab bar with a single tab item
        /// @param item_name First tab item name
        /// @return Whether the first tab item is currently opened
        /// @note `ImGui::EndCustomTabBar()` should always be called after calling this method
        bool BeginCustomTabBar(const char* item_name);

        /// @brief Finalize rendering a custom YART tab bar
        void EndCustomTabBar();

        /// @brief Check whether the mouse cursor lies within a given circle
        /// @param pos Circle position on the screen
        /// @param radius Radius of the circle
        /// @return Whether the mouse cursor is inside circle
        bool IsMouseHoveringCircle(const ImVec2& pos, float radius);

        /// @brief Issue main menu bar render commands 
        void RenderMainMenuBar();

        /// @brief Issue the application's main content render commands 
        void RenderMainContentFrame();

        /// @brief Issue the application's context window render commands
        void RenderContextWindow();

        /// @brief Issue the side nav inspector window's render commands
        void RenderInspectorNavBar();

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
