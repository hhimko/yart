////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ViewportView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "viewport_view.h"


#include "yart/GUI/gui.h"


namespace yart
{
    bool Interface::ViewportView::OnRenderGUI(yart::Viewport* target)
    {
        bool section_open, made_changes = false;

        section_open = GUI::BeginCollapsableSection("Output");
        if (section_open) {
            made_changes |= RenderOutputSection(target);
        }
        GUI::EndCollapsableSection(section_open);

        return made_changes;
    }

    bool Interface::ViewportView::RenderOutputSection(yart::Viewport* target)
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

        static constexpr size_t samplers_count = 2;
        static const char* sampler_names_LUT[samplers_count] =            { "Nearest",                      "Bilinear"                      };
        static constexpr Backend::ImageSampler samplers[samplers_count] = { Backend::ImageSampler::NEAREST, Backend::ImageSampler::BILINEAR };

        int selected_sampler = (target->GetImageSampler() == samplers[0]) ? 0 : 1;
        if (GUI::ComboHeader("Interpolation", sampler_names_LUT, samplers_count, &selected_sampler))
            target->SetImageSampler(samplers[selected_sampler]);

        return made_changes;
    }

} // namespace yart
