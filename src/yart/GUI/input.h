////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the platform specific Input class for GLFW 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Singleton class for handling user input 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Input {
    public:
        /// @brief Update the input system
        /// @note This method should be called once every frame
        static void Update();

        // -- KEYBOARD CONTROLS -- //

        /// @brief Get the current frame horizontal velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetHorizontalAxis();

        /// @brief Get the current frame vertical velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetVerticalAxis();


        // -- MOUSE CONTROLS -- // 

        /// @brief Lock the mouse cursor for a single frame
        /// @details 
        ///     Locking the mouse hides the cursor and disables it's movement on the screen while still 
        ///     capturing the mouse movement delta, which is helpful for creating 3D camera controls. 
        ///     The cursor is unlocked by default
        /// @param force Whether to force the platform to lock the cursor position each frame
        static void SetCursorLocked(bool force = false);

        /// @brief Get the amount of screen pixels the mouse cursor has moved since the previous frame
        /// @return Vector containing the amount of pixels moved
        static ImVec2 GetMouseMoveDelta();

    };
} // namespace yart
