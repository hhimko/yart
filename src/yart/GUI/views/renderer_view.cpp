////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ViewRenderer class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer_view.h"


#include "yart/utils/glm_utils.h"
#include "yart/platform/input.h"
#include "yart/core/renderer.h"
#include "yart/GUI/gui.h"


/// @brief The speed multiplier for camera's movements
static float s_cameraMoveSpeed = 0.01f;


namespace yart
{
    namespace GUI
    {
        void RendererView::OnRenderGUI(yart::Renderer& target)
        {
            bool section_open;
            section_open = GUI::BeginCollapsableSection("Camera");
            if (section_open) {
                RenderCameraSection(target);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("World");
            if (section_open) {
                RenderWorldSection(target);
            }
            GUI::EndCollapsableSection(section_open);

        }

        void RendererView::OnRenderViewAxesWindow(yart::Renderer& target)
        {
            const glm::vec3 x_axis = { glm::sin(target.m_cameraYaw), glm::sin(target.m_cameraPitch) * glm::cos(target.m_cameraYaw), -glm::cos(target.m_cameraYaw) };
            const glm::vec3 y_axis = { 0, -glm::cos(target.m_cameraPitch), -glm::sin(target.m_cameraPitch) };
            const glm::vec3 z_axis = glm::normalize(glm::cross(x_axis, y_axis));

            glm::vec3 clicked_axis = {0, 0, 0};
            if (yart::GUI::RenderViewAxesWindow(x_axis, y_axis, z_axis, clicked_axis)) {
                // Base axis to pitch, yaw rotation transformation magic
                target.m_cameraPitch = clicked_axis.y * CAMERA_PITCH_MAX;
                target.m_cameraYaw = (clicked_axis.y + clicked_axis.z) * 90.0f * yart::utils::DEG_TO_RAD + (clicked_axis.x == -1.0f) * 180.0f * yart::utils::DEG_TO_RAD;
                target.m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(target.m_cameraYaw, target.m_cameraPitch); // Can't use `clicked_axis` directly here, because of rotation clamping

                target.RecalculateCameraTransformationMatrix();
                target.m_dirty = true;
            }
        }

        void RendererView::HandleInputs(yart::Renderer& target)
        {
            // -- TRANSLATION -- //
            // Forward/backward movement
            float vertical_speed = yart::Input::GetVerticalAxis();
            if (vertical_speed != 0) {
                target.m_cameraPosition += target.m_cameraLookDirection * vertical_speed * s_cameraMoveSpeed;
                target.m_dirty = true;
            }

            // Side-to-side movement
            float horizontal_speed = yart::Input::GetHorizontalAxis();
            if (horizontal_speed != 0) {
                const glm::vec3 u = -glm::normalize(glm::cross(target.m_cameraLookDirection, target.UP_DIRECTION)); // Camera view horizontal (right) direction vector
                target.m_cameraPosition += u * horizontal_speed * s_cameraMoveSpeed;
                target.m_dirty = true;
            }

            // Ascend/descend movement
            float elevation_speed = static_cast<float>(ImGui::IsKeyDown(ImGuiKey_Space));
            elevation_speed -= static_cast<float>(ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
            if (elevation_speed != 0) {
                target.m_cameraPosition += target.UP_DIRECTION * elevation_speed * s_cameraMoveSpeed;
                target.m_dirty = true;
            }


            // -- ROTATION -- //
            static bool currently_rotating = false;
            if ((currently_rotating || GUI::IsMouseOverRenderViewport()) && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                currently_rotating = true;
                yart::Input::SetCursorLocked(true); // Lock and hide the cursor
                const glm::ivec2& mouse = yart::Input::GetMouseMoveDelta();

                if (mouse.x != 0 || mouse.y != 0) {
                    target.m_cameraYaw += static_cast<float>(mouse.x) * 0.01f;
                    target.m_cameraPitch += static_cast<float>(mouse.y) * 0.01f;
                    target.m_cameraPitch = glm::clamp(target.m_cameraPitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
                    target.m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(target.m_cameraYaw, target.m_cameraPitch);

                    target.RecalculateCameraTransformationMatrix();
                    target.m_dirty = true;
                }
            } else {
                yart::Input::SetCursorLocked(false); // Unlock the cursor 
                currently_rotating = false;
            }
        }

        void RendererView::RenderCameraSection(yart::Renderer &target)
        {
            if (ImGui::SliderFloat("FOV", &target.m_fieldOfView, FOV_MIN, FOV_MAX)) {
                target.RecalculateCameraTransformationMatrix();
            }

            if (ImGui::SliderFloat("Near clipping plane", &target.m_nearClippingPlane, NEAR_CLIP_MIN, NEAR_CLIP_MAX)) {
                target.RecalculateCameraTransformationMatrix();
            }

            if (ImGui::SliderFloat("Far clipping plane", &target.m_farClippingPlane, FAR_CLIP_MIN, FAR_CLIP_MAX)) {
                target.m_dirty = true;
            }
        }

        void RendererView::RenderWorldSection(yart::Renderer &target)
        {
            static const char* items[3] = { "Solid color", "Gradient", "Cubemap texture" }; 
            static size_t selected_item = 0;
            if (ImGui::BeginCombo("combo", items[selected_item])) {
                for (size_t i = 0; i < 3; ++i) {
                    bool selected = (i == selected_item);
                    if (ImGui::Selectable(items[i], &selected)) {
                        selected_item = i;
                    }
                }
                
                ImGui::EndCombo();
            }

            static GradientEditorContext ge_ctx { 
                target.m_worldSkyGradientValues, 
                target.m_worldSkyGradientLocations 
            };

            if (GUI::GradientEditor(ge_ctx))
                target.m_dirty = true;
        }

    } // namespace GUI
} // namespace yart

