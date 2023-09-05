////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the WorldView class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "world_view.h"


#include "yart/core/world.h"
#include "yart/GUI/gui.h"


namespace yart
{
    namespace GUI
    {
        bool WorldView::OnRenderGUI(yart::World& target)
        {
            bool section_open, made_changes;
            section_open = GUI::BeginCollapsableSection("Sky");
            if (section_open) {
                made_changes |= RenderSkySection(target);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool WorldView::RenderSkySection(yart::World& target)
        {
            bool made_changes;

            static const char* items[3] = { "Solid color", "Gradient", "Cubemap" }; 
            static int selected_item = 0;
            GUI::ComboHeader("Sky type", items, 3, &selected_item);


            switch (selected_item) {
            case 0: {
                break;
            }
            case 1: {
                static GradientEditorContext ge_ctx { 
                    target.m_skyGradientValues, 
                    target.m_skyGradientLocations 
                };

                if (GUI::GradientEditor(ge_ctx))
                    made_changes = true;

                break;
            }
            case 2: {
                break;
            }
            }

            return made_changes;
        }

    } // namespace GUI
} // namespace yart
