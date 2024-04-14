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
    namespace Interface
    {
        RendererView* RendererView::Get()
        {
            static RendererView s_instance;
            return &s_instance;
        }

        void* RendererView::GetViewTarget() const
        {
            yart::Renderer* renderer = yart::Application::Get().GetRenderer();

            return static_cast<void*>(renderer);
        }

        bool RendererView::Render(void* target) const
        {
            yart::Renderer* renderer = static_cast<yart::Renderer*>(target);
            bool section_open, made_changes = false;

            section_open = GUI::BeginCollapsableSection("Materials");
            if (section_open) {
                made_changes |= RenderMaterialsSection(renderer);
            }
            GUI::EndCollapsableSection(section_open);

            section_open = GUI::BeginCollapsableSection("Overlays");
            if (section_open) {
                made_changes |= RenderOverlaysSection(renderer);
            }
            GUI::EndCollapsableSection(section_open);

            return made_changes;
        }

        bool RendererView::RenderMaterialsSection(yart::Renderer* target)
        {
            bool made_changes = false;

            made_changes |= GUI::CheckBox("Debug materials", &target->m_debugShading);

            if (!target->m_debugShading) 
                ImGui::BeginDisabled();

            static constexpr size_t materials_count = 2;
            static const char* materials[materials_count] = { "Normals", "UVs" };
            int selection = static_cast<int>(target->m_materialUvs);
            if (GUI::ComboHeader("Render material", materials, materials_count, &selection)) {
                target->m_materialUvs ^= 0x1;
                made_changes = true;
            }

            if (!target->m_debugShading) 
                ImGui::EndDisabled();

            made_changes |= GUI::CheckBox("Cast shadows", &target->m_shadows);

            return made_changes;
        }

        bool RendererView::RenderOverlaysSection(yart::Renderer* target)
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
        
    } // namespace Interface
} // namespace yart
