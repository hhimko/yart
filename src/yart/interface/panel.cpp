////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Common definitions for application UI panels 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "panel.h"


#include <imgui.h>

#include "yart/interface/interface_internal.h"


namespace yart
{
    namespace Interface
    {
        Panel::Panel(const char* name, Interface::PanelType type)
            : m_name(name), m_type(type)
        {

        }

        bool Panel::IsPanelHovered() const
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            ImGuiWindow* hovered_window = g->HoveredWindow;

            if (hovered_window != nullptr)
                return ImGui::IsWindowChildOf(hovered_window, m_window, true);

            return false;
        }

        bool Panel::Render(ImGuiWindow* window, Panel** active_panel)
        {
            m_window = window;

            // Test for the panel being activated by user input
            if (m_type != Interface::PanelType::CONTAINER_PANEL) {
                ImGuiContext* g = ImGui::GetCurrentContext();
                ImGuiWindow* hovered_window = g->HoveredWindow;

                const bool lmb_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                if (lmb_clicked && IsPanelHovered())
                    *active_panel = this; // Propagate the active panel back to the caller
            }

            return OnRender(active_panel);
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

            delete m_child;
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

        bool RootAppPanel::HandleInputs()
        {
            bool should_refresh_viewports = false;

            Panel* panel = m_activePanel ? m_activePanel : dynamic_cast<Panel*>(this);
            while (panel != nullptr) {
                if (panel->HandleInputs(&should_refresh_viewports))
                    break;

                panel = panel->m_parent;
            }

            return should_refresh_viewports;
        }

        bool RootAppPanel::Render(const float menu_bar_height)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();

            if (m_activePanel)
                ImGui::Text("active id: %d", m_activePanel->m_type);

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

            Panel* active_panel = nullptr;
            bool made_changes = Panel::Render(g->CurrentWindow, &active_panel);
            if (active_panel != nullptr)
                m_activePanel = active_panel;

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

        bool RootAppPanel::HandleInputs(bool* should_refresh_viewports)
        {
            InterfaceContext* ctx = Interface::GetInterfaceContext();

            // Fullscreen toggle
            if (ImGui::IsKeyPressed(ImGuiKey_F, false)) {
                LayoutType layout = ctx->currentLayoutType != LayoutType::FULLSCREEN ? LayoutType::FULLSCREEN : LayoutType::DEFAULT;
                Interface::ApplyLayout(layout);
                return true;
            }

            return false;
        }

        bool RootAppPanel::OnRender(Panel** active_panel)
        {
            if (m_child) {
                // The root panel Dear ImGui window gets reused by the child panel
                ImGuiWindow* window = GetPanelWindow();
                return m_child->Render(window, active_panel);
            }

            return false;
        }

    } // namespace Interface
} // namespace yart
