////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the WorldView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "world_view.h"


#include "yart/utils/yart_utils.h"
#include "yart/core/world.h"
#include "yart/GUI/gui.h"


namespace yart
{
    namespace GUI
    {
        bool WorldView::OnRenderGUI(yart::World& target)
        {
            bool section_open, made_changes = false;
            section_open = GUI::BeginCollapsableSection("Sky");
            if (section_open) {
                made_changes |= RenderSkySection(target);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool WorldView::RenderSkySection(yart::World& target)
        {
            using SkyType = yart::World::SkyType;
            bool made_changes = false;

            static const size_t items_size = 3;
            static_assert(items_size == static_cast<uint8_t>(SkyType::COUNT));
            static const char* items[items_size] = { "Solid color",        "Gradient",        "Skybox" };
            static SkyType items_LUT[items_size] = { SkyType::SOLID_COLOR, SkyType::GRADIENT, SkyType::SKYBOX };

            static int selected_item = static_cast<uint8_t>(target.m_skyType);
            if (GUI::ComboHeader("Sky type", items, items_size, &selected_item)) 
                made_changes = true;


            switch (items_LUT[selected_item]) {
            case SkyType::SOLID_COLOR: {
                static_assert(sizeof(glm::vec3) == sizeof(float) * 3);
                if (GUI::ColorEdit("Sky color", (float*)&target.m_skySolidColor))
                    made_changes = true;
                
                break;
            }
            case SkyType::GRADIENT: {
                static GradientEditorContext ge_ctx { 
                    target.m_skyGradientValues, 
                    target.m_skyGradientLocations 
                };

                if (GUI::GradientEditor(ge_ctx))
                    made_changes = true;

                break;
            }
            case SkyType::SKYBOX: {
                break;
            }
            case SkyType::COUNT:
                YART_UNREACHABLE();
            }

            target.m_skyType = items_LUT[selected_item];
            return made_changes;
        }

    } // namespace GUI
} // namespace yart
