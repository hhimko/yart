////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal Interface module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the Interface module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>
#include <memory>

#include <imgui_internal.h>
#include <glm/glm.hpp>

#include "interface.h"


namespace yart
{
    namespace Interface
    {
        /// @brief Structure containing data required to render a inspector menu item
        struct InspectorSection {
        public:
            /// @brief Inspector menu item icon code point from the application's icons font
            const char* icon;
            /// @brief Section icon color in `ARGB` format
            ImU32 color;
            /// @brief Section name
            const char* name; 
            /// @brief Section contents rendering function callback 
            callback_t callback;
        };

        /// @brief Interface module internal context structure
        struct InterfaceContext {
        public:
            /// @brief Custom Dear ImGui render function callbacks registered by the application
            std::vector<callback_t> registeredCallbacks;
            /// @brief Inspector sections registered by the application 
            std::vector<InspectorSection> registeredInspectorSections;
            /// @brief The main render viewport used for rendering the scene
            std::unique_ptr<yart::Viewport> renderViewport = nullptr;
            /// @brief Currently open inspector nav bar window
            InspectorSection* activeInspectorSection = nullptr;

            /// @brief Whether any changes were made by the user within the last frame
            bool madeChanges;
            /// @brief Amount of pixels the OS window size has changed since last frame
            /// @details Used for updating layout sizes 
            ImVec2 displaySizeDelta;
            /// @brief Current render viewport area bounding box
            ImRect renderViewportArea;
            /// @brief Dear ImGui window ID of the render viewport window
            ImGuiID renderViewportWindowID;
            /// @brief Cached size of the main context window vertical separator
            float mainContextSeparatorHeight;
        };


        /// @brief Get the current Interface module context
        /// @return The currently active context object
        InterfaceContext* GetInterfaceContext();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// YART application's UI layout rendering functions 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Render the YART application window main menu bar
        /// @return Height of the menu bar, used for determining the content area size
        float RenderMainMenuBar();

        /// @brief Issue the application's main content area render commands 
        /// @param menu_bar_height Height of the menu bar, returned by Interface::RenderMainMenuBar()
        void RenderMainContentArea(const float menu_bar_height);

        /// @brief Issue the application's context window render commands
        void RenderContextWindow();

        /// @brief Issue the inspector window's side nav bar render commands
        void RenderInspectorNavBar();

        /// @brief Render the view axes context window
        /// @param x_axis View x-axis
        /// @param y_axis View y-axis
        /// @param z_axis View z-axis
        /// @param clicked_axis Output variable set to a base axis clicked by the user
        /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
        bool RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis);

        /// @brief Render a YART application style detached window 
        /// @details The window is rendered on top of the viewport stack, detached from the static layout
        /// @param name Name of the window
        /// @param callback Window contents rendering function callback 
        void RenderWindow(const char* name, callback_t callback);

    } // namespace Interface
} // namespace yart
