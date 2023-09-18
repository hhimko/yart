////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Backend module public API for GPU communication and platform dependent operations
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <functional>

#include <imgui.h>


namespace yart
{
    namespace Backend
    {
        /// @brief Backend event handler callback type 
        using event_callback_t = std::function<void()>;


        /// @brief Initialize and open a backend window
        /// @param window_title Initial window title
        /// @param window_width Initial width of the window in pixels
        /// @param window_height Initial height of the window in pixels
        /// @return Whether the window has been successfully created 
        bool Init(const char* window_title, uint32_t window_width, uint32_t window_height);

        /// @brief Set a custom callback function to be called when initializing Dear ImGui for custom setup
        /// @param callback A callback function, or `nullptr` to remove the current callback 
        void SetDearImGuiSetupCallback(event_callback_t callback);

        /// @brief Set a custom callback function for rendering a new frame using Dear ImGui commands 
        /// @param callback A callback function, or `nullptr` to remove the current callback 
        void SetRenderCallback(event_callback_t callback);

        /// @brief Set a custom callback function to be called when the platform window is closed by the user
        /// @param callback A callback function, or `nullptr` to remove the current callback 
        void SetWindowCloseCallback(event_callback_t callback);

        /// @brief Get the mouse cursor position as returned by the platform backend
        /// @return Mouse position in screen-space coordinates
        ImVec2 GetMousePos();

        /// @brief Set the mouse cursor position directly to the platform backend
        /// @param pos New mouse position in screen-space coordinates
        void SetMousePos(const ImVec2& pos);

        /// @brief Process all pending backend events
        /// @note This function should be called at the beginning of each frame 
        void PollEvents();

        /// @brief Submit and present the next frame to the backend window
        void Render();

        /// @brief Terminate the backend window and perform a resource cleanup
        /// @note This method is safe to be called even if the window has not been initialized successfully
        void Close();

    } // namespace Backend
} // namespace yart
