////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the WorldView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "world_view.h"


#include "yart/core/RES/resources/cubemap.h"
#include "yart/common/utils/yart_utils.h"
#include "yart/application.h"
#include "yart/core/world.h"
#include "yart/GUI/gui.h"


namespace yart
{
    namespace Interface
    {
        WorldView* WorldView::Get()
        {
            static WorldView s_instance;
            return &s_instance;
        }

        void* WorldView::GetViewTarget() const
        {
            yart::Renderer* renderer = yart::Application::Get().GetRenderer();
            yart::World* world = renderer->GetWorld();

            return static_cast<void*>(world);
        }

        bool WorldView::Render(void* target) const
        {
            yart::World* world = static_cast<yart::World*>(target);
            bool section_open, made_changes = false;

            section_open = GUI::BeginCollapsableSection("Sky");
            if (section_open) {
                made_changes |= RenderSkySection(world);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Ambient");
            if (section_open) {
                made_changes |= RenderAmbientSection(world);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool WorldView::RenderSkySection(yart::World* target)
        {
            using SkyType = yart::World::SkyType;
            bool made_changes = false;

            static const size_t items_size = 3;
            static_assert(items_size == static_cast<uint8_t>(SkyType::COUNT));
            static const char* items[items_size] = { "Solid color",        "Gradient",        "Cubemap"        };
            static SkyType items_LUT[items_size] = { SkyType::SOLID_COLOR, SkyType::GRADIENT, SkyType::CUBEMAP };

            static int selected_item = static_cast<uint8_t>(target->m_skyType);
            GUI::SetNextItemFlags(GuiItemFlags_FullWidth);
            if (GUI::ComboHeader("Sky type", items, items_size, &selected_item)) 
                made_changes = true;


            switch (items_LUT[selected_item]) {
            case SkyType::SOLID_COLOR: {
                static_assert(sizeof(glm::vec3) == sizeof(float) * 3);
                if (GUI::ColorEdit("Sky color", (float*)&target->m_skySolidColor))
                    made_changes = true;
                
                break;
            }
            case SkyType::GRADIENT: {
                static GUI::GradientEditorContext ge_ctx { 
                    target->m_skyGradientValues, 
                    target->m_skyGradientLocations 
                };

                if (GUI::GradientEditor(ge_ctx))
                    made_changes = true;

                break;
            }
            case SkyType::CUBEMAP: {
                RES::CubeMap* cubemap = RES::GetResourceByID<RES::CubeMap>(target->m_skyCubeMap);
                RES::InterpolationType current_interpolator = cubemap->GetInterpolationType();
                bool interpolate = (current_interpolator != RES::InterpolationType::NEAREST);
                static RES::InterpolationType prev_interpolator = interpolate ? current_interpolator : RES::InterpolationType::BILINEAR;

                if (GUI::CheckBox("Interpolate", &interpolate)) {
                    cubemap->SetInterpolationType(interpolate ? prev_interpolator : RES::InterpolationType::NEAREST);
                    made_changes = true;
                }

                if (!interpolate)
                    ImGui::BeginDisabled();

                static constexpr size_t interpolators_count = 2;
                static const char* interpolators[interpolators_count] =                          { "Bilinear",                       "Bicubic"                       };
                static constexpr RES::InterpolationType interpolators_LUT[interpolators_count] = { RES::InterpolationType::BILINEAR, RES::InterpolationType::BICUBIC };

                int selected_interpolator = static_cast<int>(prev_interpolator) - static_cast<int>(RES::InterpolationType::BILINEAR);
                if (GUI::ComboHeader("Interpolation type", interpolators, interpolators_count, &selected_interpolator)) {
                    cubemap->SetInterpolationType(interpolators_LUT[selected_interpolator]);
                    prev_interpolator = interpolators_LUT[selected_interpolator];
                    made_changes = true;
                }

                if (!interpolate)
                    ImGui::EndDisabled();

                break;
            }
            case SkyType::COUNT:
                YART_UNREACHABLE();
            }

            target->m_skyType = items_LUT[selected_item];
            return made_changes;
        }

        bool WorldView::RenderAmbientSection(yart::World* target)
        {
            bool made_changes = false;

            static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
            made_changes |= GUI::ColorEdit("Ambient color", reinterpret_cast<float*>(&target->ambientColor));

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
