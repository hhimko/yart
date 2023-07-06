////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the platform specific Input class for GLFW 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <glm/glm.hpp>


namespace yart
{
    // yart::Application forward reference to avoid circular refs
    class Application;


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Singleton class for handling user input 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Input {
    public:
        // -- KEYBOARD CONTROLS -- //

        /// @brief Get the current frame horizontal velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetHorizontalAxis();

        /// @brief Get the current frame vertical velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetVerticalAxis();


        // -- MOUSE CONTROLS -- // 

        /// @brief Set the lock state of the mouse 
        /// @details 
        ///     Locking the mouse hides the cursor and disables it's movement on the screen while still 
        ///     capturing the mouse movement delta, which is helpful for creating 3D camera controls. 
        ///     The cursor is unlocked by default
        /// @param state Whether the mouse should be locked
        static void SetCursorLocked(bool state);

        /// @brief Get the amount of screen pixels the mouse cursor has moved since the previous frame
        /// @return Vector containing the amount of pixels moved
        static const glm::ivec2& GetMouseMoveDelta();

    private:
        /// @brief Update the input system
        /// @note This method should be called once every frame
        static void Update();


        // -- FRIEND DECLARATIONS -- // 
        friend class yart::Application;

    };
} // namespace yart
