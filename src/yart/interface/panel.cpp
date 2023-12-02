////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Common definitions for application UI panels 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "panel.h"


#include <imgui_internal.h>


namespace yart
{
    namespace Interface
    {
        bool Panel::IsPanelHovered() const
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            if (g->HoveredWindow != nullptr)
                return g->HoveredWindow->ID == m_window->ID;

            return false;
        }

        bool Panel::Render(ImGuiWindow* window)
        {
            m_window = window;

            return OnRender();
        }

        RootAppPanel* RootAppPanel::Get()
        {
            static RootAppPanel s_instance;
            return &s_instance;
        }

        void RootAppPanel::AttachPanel(Panel* panel)
        {
            if (panel == nullptr)
                return DetachLayout();

            m_child = panel;
            m_child->m_parent = this;
        }

        void RootAppPanel::DetachLayout()
        {
            m_activePanel = nullptr;

            // Deleting the root's child should destroy the whole layout in a dominos effect
            delete m_child;
            m_child = nullptr;
        }

        bool RootAppPanel::Render(const float menu_bar_height)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();

            // Root panel window area is essentially the display area minus the main menu bar size 
            ImVec2 display_size = g->IO.DisplaySize;
            ImGui::SetNextWindowPos({ 0.0f, menu_bar_height });
            ImGui::SetNextWindowSize({ display_size.x, display_size.y - menu_bar_height });

            static constexpr ImGuiWindowFlags flags = (
                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | 
                ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground
            );
            
            ImVec2 backup_padding = g->Style.WindowPadding;
            g->Style.WindowPadding = { 0.0f, 0.0f };
            ImGui::Begin("YART_Panel_Root_Window", nullptr, flags);
            g->Style.WindowPadding = backup_padding;

            bool made_changes = Panel::Render(g->CurrentWindow);

            ImGui::End();
            return made_changes;
        }

        Interface::Panel* RootAppPanel::GetPanel(Interface::PanelType type) const
        {
            if (m_child == nullptr || m_child->m_type == type)
                return m_child;

            if (m_child->m_type == Interface::PanelType::CONTAINER_PANEL) {
                Interface::ContainerPanel* container = dynamic_cast<ContainerPanel*>(m_child);
                return container->GetPanel(type);
            }

            return nullptr;
        }

        bool RootAppPanel::OnRender()
        {
            if (m_child) {
                // The root panel Dear ImGui window gets reused by the child panel
                ImGuiWindow* window = GetPanelWindow();
                return m_child->Render(window);
            }

            return false;
        }

        LayoutPanel::LayoutPanel(yart::GUI::LayoutCreateInfo& layout_create_info, Panel* ul_child, Panel* lr_child)
            : m_ul_child(ul_child), m_lr_child(lr_child)
        {
            // Attach the child panels
            YART_ASSERT(m_ul_child != nullptr && m_lr_child != nullptr);
            m_ul_child->m_parent = this;
            m_lr_child->m_parent = this;

            m_layout = GUI::CreateLayout(layout_create_info);
            YART_ASSERT(m_layout != nullptr);
        }

        LayoutPanel::~LayoutPanel()
        {
            GUI::DestroyLayout(m_layout);

            delete m_ul_child;
            delete m_lr_child;
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

        bool LayoutPanel::OnRender()
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            bool made_changes = false;

            GUI::BeginLayout(m_layout);
            {
                made_changes |= m_ul_child->Render(g->CurrentWindow);
            }
            GUI::LayoutSeparator(m_layout);
            {
                made_changes |= m_lr_child->Render(g->CurrentWindow);
            }
            GUI::EndLayout(m_layout);

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
