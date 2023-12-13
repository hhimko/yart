////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART application UI rendering module public interface
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <functional>

#include <imgui.h>

#include "yart/core/viewport.h"


namespace yart
{
    namespace Interface
    {
        /// @brief Callback function type for rendering custom UI windows  
        using callback_t = std::function<bool()>;

        /// @brief Type of UI layouts that can be applied for the application GUI
        enum class LayoutType {
            DEFAULT = 0, ///< Default layout, loaded on the application start
            FULLSCREEN   ///< Viewport-only layout
        };


        /// @brief Initialize the Interface module
        void Init();

        /// @brief Load and attach a YART application UI layout
        /// @param type Requested type of the layout 
        void ApplyLayout(LayoutType type);

        /// @brief Handle all incoming UI events
        /// @return 
        bool HandleInputs();

        /// @brief Issue YART application's UI render commands
        /// @note This method should only be called after calling `ImGui::NewFrame()`
        /// @returns Whether any changes were made by the user within this frame
        bool Render();

        /// @brief Close the Interface module
        void Shutdown();

    } // namespace Interface
} // namespace yart
