////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the ViewportView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


namespace yart
{
    /// @brief View target class forward declaration
    class Viewport;


    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief GUI view for the Viewport class
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class ViewportView {
        public:
            /// @brief Issue GUI render commands to display the Viewport object context menu
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool OnRenderGUI(yart::Viewport* target);

        private:
            ViewportView() = delete;

            /// @brief Issue "Output" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderOutputSection(yart::Viewport* target);

        };   

    } // namespace Interface
} // namespace yart
