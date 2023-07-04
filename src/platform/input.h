#pragma once

#include "../application.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Singleton class for handling user input 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Input {
    public:
        /// @brief Get the current frame horizontal velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetHorizontalAxis();

        /// @brief Get the current frame vertical velocity value mapped to arrow keys 
        /// @return Value in `-1` to `1` range
        static float GetVerticalAxis();

    private:
        /// @brief Update the input system
        /// @note This method should be called once every frame
        static void Update();


        // -- FRIEND DECLARATIONS -- // 
        friend class yart::Application;

    };
} // namespace yart
