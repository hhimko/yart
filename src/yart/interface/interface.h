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


        /// @brief Initialize the Interface module
        void Init();

        /// @brief Handle all incoming UI events
        /// @return 
        bool HandleInputs();

        /// @brief Update the Interface module state
        /// @details Should be called once every frame
        void Update();

        /// @brief Issue YART application's UI render commands
        /// @note This method should only be called after calling `ImGui::NewFrame()`
        /// @returns Whether any changes were made by the user within this frame
        bool Render();

        /// @brief Get the YART application's main render viewport
        /// @return YART application render viewport
        yart::Viewport* GetRenderViewport();

        /// @brief Test whether the mouse cursor is currently directly over the YART application's main render viewport
        /// @return Whether mouse is over viewport
        bool IsMouseOverRenderViewport();

    } // namespace Interface
} // namespace yart
