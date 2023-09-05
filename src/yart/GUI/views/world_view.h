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
            static void OnRenderGUI(yart::World& target);

        private:
            WorldView() = delete;

            /// @brief Issue "Sky" section GUI render commands
            /// @param target View target instance
            static void RenderSkySection(yart::World& target);

        };   

    } // namespace GUI
} // namespace yart
