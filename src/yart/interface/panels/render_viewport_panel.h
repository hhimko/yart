////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the RenderViewportPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <imgui.h>

#include "yart/interface/panels/panel_settings.h"
#include "yart/interface/panel.h"
#include "yart/core/viewport.h"
#include "yart/core/camera.h"


namespace yart
{
    namespace Interface
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Saveable settings class for render viewport panels
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RenderViewportPanelSettings : public PanelSettings {
        public:
            /// @brief RenderViewportPanelSettings class custom constructor
            /// @param panel Panel instance, for which the settings apply
            RenderViewportPanelSettings(const Panel* const panel)
                : PanelSettings(panel) { }

        public:
            uint8_t viewportScale; ///< Scale of the render viewport
            yart::Backend::ImageSampler viewportImageSampler; ///< Sampler type for the render viewport

        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief UI panel for displaying and handling render viewports 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class RenderViewportPanel : public Panel, SaveablePanel<RenderViewportPanelSettings> {
        public:
            /// @brief RenderViewportPanel class custom constructor
            /// @param name Unique name of the panel, for saving and retrieving settings
            RenderViewportPanel(const char* name);

            /// @brief RenderViewportPanel class custom destructor
            ~RenderViewportPanel() 
            { 
                SavePanelSettings(); 
            }

            /// @brief Get the viewport associated with this panel
            /// @return YART viewport instance 
            yart::Viewport* GetViewport()
            {
                return &m_viewport;
            }

            /// @brief Get the camera associated with this panel
            /// @return YART camera instance 
            yart::Camera* GetCamera()
            {
                return &s_camera;
            }

        private:
            /// @brief Get the saveable state of this panel 
            /// @return New panel settings object for this instance
            RenderViewportPanelSettings GetPanelSettings() const override;

            /// @brief Apply a given settings object to this panel
            /// @param settings Panel settings object from which to recreate panel state
            void ApplyPanelSettings(const RenderViewportPanelSettings* const settings) override;

            /// @brief Handle incoming user inputs
            /// @param should_refresh_viewports Output parameter, used for specifying wether any changes that 
            ///     invalidate viewports have been made
            /// @return Whether the incoming events have been handled by this panel
            bool HandleInputs(bool* should_refresh_viewports) override;

            /// @brief Issue panel GUI render commands
            /// @param active_panel Output parameter for propagating the active panel back through the call stack.
            ///     Should be set to `this` whenever the panel should be activated in the layout
            /// @return Whether any changes were made by the user during this frame
            bool OnRender(Panel** active_panel) override;

            /// @brief Render the camera view axes overlay window over the viewport
            /// @param camera YART camera instance 
            /// @param clicked_axis Output variable set to a base axis clicked by the user
            /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
            bool RenderCameraViewAxesOverlay(const yart::Camera& camera, glm::vec3& clicked_axis);

            /// @brief Camera view axes overlay window rendering helper function
            /// @param draw_list ImGui window draw list
            /// @param win_pos Center window position of the view axes context window
            /// @param axis0 First axis in the drawing order (back)
            /// @param axis1 Second axis in the drawing order (middle)
            /// @param axis2 Third axis in the drawing order (front)
            /// @param order The order in which the X, Y and Z axes will be drawn
            /// @param length Maximum axis length in pixels 
            /// @param active Whether the view axes context window is currently active
            /// @param swap Wether the negative axes should be rendered prior to the positive ones
            /// @param clicked_axis Output variable set to a base axis clicked by the user
            /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
            static bool DrawViewAxesH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis0, const glm::vec3& axis1, const glm::vec3& axis2, 
                                      const int* order, float length, bool active, bool swap, glm::vec3& clicked_axis);

            /// @brief Positive view axis rendering helper function
            /// @param draw_list ImGui window draw list
            /// @param win_pos Center window position of the view axes context window
            /// @param axis Normalized axis direction
            /// @param color Color of the axis
            /// @param length Maximum axis length in pixels 
            /// @param hovered Whether the axis handle is currently hovered
            static void DrawPositiveViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered);

            /// @brief Negative view axis rendering helper function
            /// @param draw_list ImGui window draw list
            /// @param win_pos Center window position of the view axes context window
            /// @param axis Normalized axis direction
            /// @param color Color of the axis
            /// @param length Maximum axis length in pixels 
            /// @param hovered Whether the axis handle is currently hovered
            static void DrawNegativeViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered);

        public:
            /// @brief Type of this panel, used for panel retrieval in a layout
            static constexpr Interface::PanelType TYPE = PanelType::RENDER_VIEWPORT_PANEL;

        private:
            inline static yart::Camera s_camera; ///< YART camera instance, shared between all viewport panels
            inline static float s_cameraMoveSpeed = 2.5f; ///< Speed multiplier for camera translation

            yart::Viewport m_viewport { 1, 1, 2 };

        };

    } // namespace Interface
} // namespace yart
