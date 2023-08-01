////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 


#include <functional>

#include <imgui.h>


namespace yart
{
    namespace GUI
    {
        /// @brief Callback function for rendering custom Dear ImGui windows  
        typedef std::function<void()> imgui_callback_t;

        /// @brief GUI context, holding all state required to render a specific UI layout
        struct GUIContext; // Opaque type without including `gui_internal.h`


        /// @brief Get the active GUI context
        /// @return The currently active context object
        GUIContext& GetCurrentContext();

        /// @brief Register a custom ImGui window to be rendered every frame 
        /// @param window_name Name of the window
        /// @param callback Callback function to window contents definition 
        void RegisterImGuiWindow(const char* window_name, imgui_callback_t callback);

        /// @brief Issue Dear ImGui render commands for the current GUI context
        /// @note This method should only be called after calling `ImGui::NewFrame()`
        void Render();
        
    } // namespace GUI
} // namespace yart
