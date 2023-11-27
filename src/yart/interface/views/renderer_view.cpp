////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer_view.h"


#include "yart/interface/interface.h"
#include "yart/core/renderer.h"
#include "yart/GUI/input.h"
#include "yart/GUI/gui.h"


/// @brief The speed multiplier for camera's movements
static float s_cameraMoveSpeed = 0.01f;


namespace yart
{
    bool Interface::RendererView::OnRenderGUI(yart::Renderer* target)
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

        section_open = GUI::BeginCollapsableSection("Overlays");
        if (section_open) {
            made_changes |= RenderOverlaysSection(target);
        }
        GUI::EndCollapsableSection(section_open);

        return made_changes;
    }

    bool Interface::RendererView::OnRenderViewAxesWindow(yart::Renderer* target)
    {
        // const glm::vec3 x_axis = { glm::sin(target->m_cameraYaw), glm::sin(target->m_cameraPitch) * glm::cos(target->m_cameraYaw), -glm::cos(target->m_cameraYaw) };
        // const glm::vec3 y_axis = { 0, -glm::cos(target->m_cameraPitch), -glm::sin(target->m_cameraPitch) };
        // const glm::vec3 z_axis = glm::normalize(glm::cross(x_axis, y_axis));

        // glm::vec3 clicked_axis = {0, 0, 0};
        // if (yart::GUI::RenderViewAxesWindow(x_axis, y_axis, z_axis, clicked_axis)) {
        //     // Base axis to pitch, yaw rotation transformation magic
        //     target->m_cameraPitch = clicked_axis.y * CAMERA_PITCH_MAX;
        //     target->m_cameraYaw = (clicked_axis.y + clicked_axis.z) * 90.0f * yart::utils::DEG_TO_RAD + (clicked_axis.x == -1.0f) * 180.0f * yart::utils::DEG_TO_RAD;
        //     target->m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(target->m_cameraYaw, target->m_cameraPitch); // Can't use `clicked_axis` directly here, because of rotation clamping

        //     target->RecalculateRayDirections();
        //     return true;
        // }

        return false;
    }

    bool Interface::RendererView::HandleInputs(yart::Renderer* target)
    {
        bool made_changes = false;

        // -- TRANSLATION -- //
        // Forward/backward movement
        float vertical_speed = yart::GUI::Input::GetVerticalAxis();
        if (vertical_speed != 0) {
            target->m_camera.position += target->m_camera.GetLookDirection() * vertical_speed * s_cameraMoveSpeed;
            made_changes = true;
        }

        // Side-to-side movement
        float horizontal_speed = yart::GUI::Input::GetHorizontalAxis();
        if (horizontal_speed != 0) {
            const glm::vec3 u = -glm::normalize(glm::cross(target->m_camera.GetLookDirection(), yart::Camera::UP_DIRECTION)); // Camera view horizontal (right) direction vector
            target->m_camera.position += u * horizontal_speed * s_cameraMoveSpeed;
            made_changes = true;
        }

        // Ascend/descend movement
        float elevation_speed = static_cast<float>(ImGui::IsKeyDown(ImGuiKey_Space));
        elevation_speed -= static_cast<float>(ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
        if (elevation_speed != 0) {
            target->m_camera.position += yart::Camera::UP_DIRECTION * elevation_speed * s_cameraMoveSpeed;
            made_changes = true;
        }


        // -- ROTATION -- //
        if (Interface::IsMouseOverRenderViewport() && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            yart::GUI::Input::SetCursorLocked(true); // Lock and hide the cursor
            ImVec2 mouse_delta = yart::GUI::Input::GetMouseMoveDelta();

            if (mouse_delta.x != 0 || mouse_delta.y != 0) {
                target->m_camera.RotateByMouseDelta(mouse_delta.x, mouse_delta.y);
                made_changes = true;
            }
        }

        return made_changes;
    }

    bool Interface::RendererView::RenderViewTransformSection(yart::Renderer* target)
    {
        bool made_changes = false;

        // Camera position
        GUI::BeginMultiItem(3);
        {
            made_changes |= GUI::SliderFloat("Position X", &target->m_camera.position.x, "%.3fm", 0.1f);
            made_changes |= GUI::SliderFloat(         "Y", &target->m_camera.position.y, "%.3fm", 0.1f);
            made_changes |= GUI::SliderFloat(         "Z", &target->m_camera.position.z, "%.3fm", 0.1f);
        }
        GUI::EndMultiItem();


        return made_changes;
    }

    bool Interface::RendererView::RenderCameraPropertiesSection(yart::Renderer* target)
    {
        bool made_changes = false;

        float fov = target->m_camera.GetFOV();
        if (GUI::SliderFloat("FOV", &fov, yart::Camera::FOV_MIN, yart::Camera::FOV_MAX)) {
            target->m_camera.SetFOV(fov);
            made_changes = true;
        }

        float near = target->m_camera.GetNearClippingPlane();
        if (GUI::SliderFloat("Near clipping plane", &near, yart::Camera::NEAR_CLIP_MIN, yart::Camera::NEAR_CLIP_MAX)) {
            target->m_camera.SetNearClippingPlane(near);
            made_changes = true;
        }

        float far = target->m_camera.GetNearClippingPlane();
        if (GUI::SliderFloat("Far clipping plane", &far, yart::Camera::FAR_CLIP_MIN, yart::Camera::FAR_CLIP_MAX)) {
            target->m_camera.SetNearClippingPlane(far);
            made_changes = true;
        }

        return made_changes;
    }

    bool Interface::RendererView::RenderOverlaysSection(yart::Renderer* target)
    {
        bool made_changes = GUI::CheckBox("Grid", &target->m_showOverlays);

        if (!target->m_showOverlays)
            ImGui::BeginDisabled();

        static constexpr size_t outlines_count = 2;
        static const char* outlines[outlines_count] = { "Normal", "Thick" };
        int selection = static_cast<int>(target->m_useThickerGrid);
        if (GUI::ComboHeader("Grid outline", outlines, outlines_count, &selection)) {
            target->m_useThickerGrid ^= 0x1;
            made_changes = true;
        }

        if (!target->m_showOverlays)
            ImGui::EndDisabled();

        return made_changes;
    }

} // namespace yart
