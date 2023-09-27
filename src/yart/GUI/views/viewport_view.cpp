////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ViewportView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "viewport_view.h"


#include "yart/GUI/gui.h"


namespace yart
{
    namespace GUI
    {
        bool ViewportView::OnRenderGUI(yart::Viewport* target)
        {
            bool section_open, made_changes = false;

            section_open = GUI::BeginCollapsableSection("Output");
            if (section_open) {
                made_changes |= RenderOutputSection(target);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool ViewportView::RenderOutputSection(yart::Viewport* target)
        {
            bool made_changes = false;

            GUI::BeginMultiItem(2);
            {
                ImVec2 image_size = target->GetImageSize();
                GUI::Label("Resolution X", "%dpx", static_cast<uint32_t>(image_size.x));
                GUI::Label("Y",            "%dpx", static_cast<uint32_t>(image_size.y));
            }
            GUI::EndMultiItem();

            int scale = target->GetImageScale();
            if (GUI::SliderInt("Scale", &scale, 1, 10)) {
                target->SetImageScale(static_cast<uint8_t>(scale));
                made_changes = true;
            }

            return made_changes;
        }

    } // namespace GUI
} // namespace yart
