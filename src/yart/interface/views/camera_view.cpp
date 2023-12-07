////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CameraView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camera_view.h"


#include "yart/interface/panels/render_viewport_panel.h"
#include "yart/interface/interface.h"
#include "yart/interface/panel.h"
#include "yart/core/camera.h"
#include "yart/GUI/gui.h"


namespace yart
{
    namespace Interface
    {
        CameraView* CameraView::Get()
        {
            static CameraView s_instance;
            return &s_instance;
        }

        void* CameraView::GetViewTarget() const
        {
            Interface::RootAppPanel* rap = Interface::RootAppPanel::Get();
            Interface::RenderViewportPanel* viewport_panel = rap->GetPanel<Interface::RenderViewportPanel>();

            if (viewport_panel == nullptr)
                return nullptr;

            return static_cast<void*>(viewport_panel->GetCamera());
        }

        bool CameraView::Render(void* target) const
        {
            yart::Camera* camera = static_cast<yart::Camera*>(target);
            bool section_open, made_changes = false;

            section_open = GUI::BeginCollapsableSection("View Transform");
            if (section_open) {
                made_changes |= RenderViewTransformSection(camera);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Camera Properties");
            if (section_open) {
                made_changes |= RenderCameraPropertiesSection(camera);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool CameraView::RenderViewTransformSection(yart::Camera* target)
        {
            bool made_changes = false;

            // Camera position
            GUI::BeginMultiItem(3);
            {
                made_changes |= GUI::SliderFloat("Position X", &target->position.x, "%.3fm", 0.1f);
                made_changes |= GUI::SliderFloat(         "Y", &target->position.y, "%.3fm", 0.1f);
                made_changes |= GUI::SliderFloat(         "Z", &target->position.z, "%.3fm", 0.1f);
            }
            GUI::EndMultiItem();


            return made_changes;
        }

        bool CameraView::RenderCameraPropertiesSection(yart::Camera* target)
        {
            bool made_changes = false;

            float fov = target->GetFOV();
            if (GUI::SliderFloat("FOV", &fov, yart::Camera::FOV_MIN, yart::Camera::FOV_MAX)) {
                target->SetFOV(fov);
                made_changes = true;
            }

            float near = target->GetNearClippingPlane();
            if (GUI::SliderFloat("Near clipping plane", &near, yart::Camera::NEAR_CLIP_MIN, yart::Camera::NEAR_CLIP_MAX)) {
                target->SetNearClippingPlane(near);
                made_changes = true;
            }

            float far = target->GetFarClippingPlane();
            if (GUI::SliderFloat("Far clipping plane", &far, yart::Camera::FAR_CLIP_MIN, yart::Camera::FAR_CLIP_MAX)) {
                target->SetFarClippingPlane(far);
                made_changes = true;
            }

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
