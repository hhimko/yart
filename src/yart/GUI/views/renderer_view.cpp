////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer_view.h"


#include "yart/core/renderer.h"
#include "yart/GUI/input.h"
#include "yart/GUI/gui.h"


/// @brief The speed multiplier for camera's movements
static float s_cameraMoveSpeed = 0.01f;


namespace yart
{
    namespace GUI
    {
        bool RendererView::OnRenderGUI(yart::Renderer* target)
        {
            bool section_open, made_changes = false;

            section_open = GUI::BeginCollapsableSection("View Transform");
            if (section_open) {
                made_changes |= RenderViewTransformSection(target);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Camera Properties");
            if (section_open) {
                made_changes |= RenderCameraPropertiesSection(target);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool RendererView::OnRenderViewAxesWindow(yart::Renderer* target)
        {
            const glm::vec3 x_axis = { glm::sin(target->m_cameraYaw), glm::sin(target->m_cameraPitch) * glm::cos(target->m_cameraYaw), -glm::cos(target->m_cameraYaw) };
            const glm::vec3 y_axis = { 0, -glm::cos(target->m_cameraPitch), -glm::sin(target->m_cameraPitch) };
            const glm::vec3 z_axis = glm::normalize(glm::cross(x_axis, y_axis));

            glm::vec3 clicked_axis = {0, 0, 0};
            if (yart::GUI::RenderViewAxesWindow(x_axis, y_axis, z_axis, clicked_axis)) {
                // Base axis to pitch, yaw rotation transformation magic
                target->m_cameraPitch = clicked_axis.y * CAMERA_PITCH_MAX;
                target->m_cameraYaw = (clicked_axis.y + clicked_axis.z) * 90.0f * yart::utils::DEG_TO_RAD + (clicked_axis.x == -1.0f) * 180.0f * yart::utils::DEG_TO_RAD;
                target->m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(target->m_cameraYaw, target->m_cameraPitch); // Can't use `clicked_axis` directly here, because of rotation clamping

                target->RecalculateCameraTransformationMatrix();
                return true;
            }

            return false;
        }

        bool RendererView::HandleInputs(yart::Renderer* target)
        {
            bool made_changes = false;

            // -- TRANSLATION -- //
            // Forward/backward movement
            float vertical_speed = yart::GUI::Input::GetVerticalAxis();
            if (vertical_speed != 0) {
                target->m_cameraPosition += target->m_cameraLookDirection * vertical_speed * s_cameraMoveSpeed;
                made_changes = true;
            }

            // Side-to-side movement
            float horizontal_speed = yart::GUI::Input::GetHorizontalAxis();
            if (horizontal_speed != 0) {
                const glm::vec3 u = -glm::normalize(glm::cross(target->m_cameraLookDirection, target->UP_DIRECTION)); // Camera view horizontal (right) direction vector
                target->m_cameraPosition += u * horizontal_speed * s_cameraMoveSpeed;
                made_changes = true;
            }

            // Ascend/descend movement
            float elevation_speed = static_cast<float>(ImGui::IsKeyDown(ImGuiKey_Space));
            elevation_speed -= static_cast<float>(ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
            if (elevation_speed != 0) {
                target->m_cameraPosition += target->UP_DIRECTION * elevation_speed * s_cameraMoveSpeed;
                made_changes = true;
            }


            // -- ROTATION -- //
            if (GUI::IsMouseOverRenderViewport() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                yart::GUI::Input::SetCursorLocked(true); // Lock and hide the cursor
                ImVec2 mouse_delta = yart::GUI::Input::GetMouseMoveDelta();

                if (mouse_delta.x != 0 || mouse_delta.y != 0) {
                    target->m_cameraYaw -= mouse_delta.x * 0.01f;
                    target->m_cameraPitch -= mouse_delta.y * 0.01f;
                    target->m_cameraPitch = glm::clamp(target->m_cameraPitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);
                    target->m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(target->m_cameraYaw, target->m_cameraPitch);

                    target->RecalculateCameraTransformationMatrix();
                    made_changes = true;
                }
            }

            return made_changes;
        }

        bool RendererView::RenderViewTransformSection(yart::Renderer* target)
        {
            bool made_changes = false;

            // Camera position
            GUI::BeginMultiItem(3);
            {
                made_changes |= GUI::SliderFloat("Position X", &target->m_cameraPosition.x, "%.3fm", 0.1f);
                made_changes |= GUI::SliderFloat(         "Y", &target->m_cameraPosition.y, "%.3fm", 0.1f);
                made_changes |= GUI::SliderFloat(         "Z", &target->m_cameraPosition.z, "%.3fm", 0.1f);
            }
            GUI::EndMultiItem();


            return made_changes;
        }

        bool RendererView::RenderCameraPropertiesSection(yart::Renderer* target)
        {
            bool made_changes = false;

            if (GUI::SliderFloat("FOV", &target->m_fieldOfView, FOV_MIN, FOV_MAX)) {
                target->RecalculateCameraTransformationMatrix();
                made_changes = true;
            }

            if (GUI::SliderFloat("Near clipping plane", &target->m_nearClippingPlane, NEAR_CLIP_MIN, NEAR_CLIP_MAX)) {
                target->RecalculateCameraTransformationMatrix();
                made_changes = true;
            }

            if (GUI::SliderFloat("Far clipping plane", &target->m_farClippingPlane, FAR_CLIP_MIN, FAR_CLIP_MAX)) {
                made_changes = true;
            }

            return made_changes;
        }

    } // namespace GUI
} // namespace yart
