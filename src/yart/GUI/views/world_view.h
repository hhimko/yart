////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the WorldView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


namespace yart
{
    /// @brief View target class forward declaration
    class World;


    namespace GUI
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief GUI view for the World class
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class WorldView {
        public:
            /// @brief Issue GUI render commands to display the World object context menu
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool OnRenderGUI(yart::World& target);

        private:
            WorldView() = delete;

            /// @brief Issue "Sky" section GUI render commands
            /// @param target View target instance
            /// @returns Whether any changes were made by the user since the last frame
            static bool RenderSkySection(yart::World& target);

        };   

    } // namespace GUI
} // namespace yart
