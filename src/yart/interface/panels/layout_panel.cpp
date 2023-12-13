////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LayoutPanel class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "layout_panel.h"


#include <imgui_internal.h>


namespace yart
{
    namespace Interface
    {
        LayoutPanel::LayoutPanel(const char* name, yart::GUI::LayoutCreateInfo& layout_create_info, Panel* ul_child, Panel* lr_child)
            : ContainerPanel(name), m_ul_child(ul_child), m_lr_child(lr_child), SaveablePanel(this)
        {
            // Attach the child panels
            YART_ASSERT(m_ul_child != nullptr && m_lr_child != nullptr);
            m_ul_child->m_parent = this;
            m_lr_child->m_parent = this;

            m_layout = GUI::CreateLayout(layout_create_info);
            YART_ASSERT(m_layout != nullptr);

            LoadAndApplyPanelSettings();
        }

        LayoutPanel::~LayoutPanel()
        {
            GUI::DestroyLayout(m_layout);

            delete m_ul_child;
            delete m_lr_child;
        }

        LayoutPanelSettings LayoutPanel::GetPanelSettings() const
        {
            const Panel* panel = dynamic_cast<const Panel*>(this);
            LayoutPanelSettings settings(panel);

            return settings;
        }

        void LayoutPanel::ApplyPanelSettings(const LayoutPanelSettings* const settings)
        {
            YART_ASSERT(settings != nullptr);
        }

        Interface::Panel* LayoutPanel::GetPanel(Interface::PanelType type) const
        {
            if (m_ul_child->m_type == type)
                return m_ul_child;

            if (m_lr_child->m_type == type)
                return m_lr_child;

            if (m_ul_child->m_type == Interface::PanelType::CONTAINER_PANEL) {
                Interface::ContainerPanel* container = dynamic_cast<ContainerPanel*>(m_ul_child);

                Interface::Panel* panel = container->GetPanel(type);
                if (panel != nullptr)
                    return panel;
            }

            if (m_lr_child->m_type == Interface::PanelType::CONTAINER_PANEL) {
                Interface::ContainerPanel* container = dynamic_cast<ContainerPanel*>(m_lr_child);
                return container->GetPanel(type);
            }

            return nullptr;
        }

        bool LayoutPanel::OnRender(Panel** active_panel)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            bool made_changes = false;

            GUI::BeginLayout(m_layout);
            {
                made_changes |= m_ul_child->Render(g->CurrentWindow, active_panel);
            }
            GUI::LayoutSeparator(m_layout);
            {
                made_changes |= m_lr_child->Render(g->CurrentWindow, active_panel);
            }
            GUI::EndLayout(m_layout);

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
