////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the RendererView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer_view.h"


#include "yart/interface/interface.h"
#include "yart/core/renderer.h"
#include "yart/application.h"
#include "yart/GUI/gui.h"


namespace yart
{
    Interface::RendererView* Interface::RendererView::Get()
    {
        static RendererView s_instance;
        return &s_instance;
    }

    void* Interface::RendererView::GetViewTarget() const
    {
        yart::Renderer* renderer = yart::Application::Get().GetRenderer();

        return static_cast<void*>(renderer);
    }

    bool Interface::RendererView::Render(void* target) const
    {
        yart::Renderer* renderer = static_cast<yart::Renderer*>(target);
        bool section_open, made_changes = false;

        section_open = GUI::BeginCollapsableSection("View Transform");
        if (section_open) {
            made_changes |= RenderViewTransformSection(renderer);
        }
        GUI::EndCollapsableSection(section_open);

        section_open = GUI::BeginCollapsableSection("Camera Properties");
        if (section_open) {
            made_changes |= RenderCameraPropertiesSection(renderer);
        }
        GUI::EndCollapsableSection(section_open);

        section_open = GUI::BeginCollapsableSection("Overlays");
        if (section_open) {
            made_changes |= RenderOverlaysSection(renderer);
        }
        GUI::EndCollapsableSection(section_open);

        return made_changes;
    }

    bool Interface::RendererView::RenderViewTransformSection(yart::Renderer* target)
    {
        bool made_changes = false;

        // // Camera position
        // GUI::BeginMultiItem(3);
        // {
        //     made_changes |= GUI::SliderFloat("Position X", &target->m_camera.position.x, "%.3fm", 0.1f);
        //     made_changes |= GUI::SliderFloat(         "Y", &target->m_camera.position.y, "%.3fm", 0.1f);
        //     made_changes |= GUI::SliderFloat(         "Z", &target->m_camera.position.z, "%.3fm", 0.1f);
        // }
        // GUI::EndMultiItem();


        return made_changes;
    }

    bool Interface::RendererView::RenderCameraPropertiesSection(yart::Renderer* target)
    {
        bool made_changes = false;

        // float fov = target->m_camera.GetFOV();
        // if (GUI::SliderFloat("FOV", &fov, yart::Camera::FOV_MIN, yart::Camera::FOV_MAX)) {
        //     target->m_camera.SetFOV(fov);
        //     made_changes = true;
        // }

        // float near = target->m_camera.GetNearClippingPlane();
        // if (GUI::SliderFloat("Near clipping plane", &near, yart::Camera::NEAR_CLIP_MIN, yart::Camera::NEAR_CLIP_MAX)) {
        //     target->m_camera.SetNearClippingPlane(near);
        //     made_changes = true;
        // }

        // float far = target->m_camera.GetNearClippingPlane();
        // if (GUI::SliderFloat("Far clipping plane", &far, yart::Camera::FAR_CLIP_MIN, yart::Camera::FAR_CLIP_MAX)) {
        //     target->m_camera.SetNearClippingPlane(far);
        //     made_changes = true;
        // }

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
