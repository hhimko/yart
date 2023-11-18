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
        RootAppPanel* RootAppPanel::Get()
        {
            static RootAppPanel s_instance;
            return &s_instance;
        }

        void RootAppPanel::AttachPanel(Panel* panel)
        {
            if (panel != m_child)
                delete m_child;

            m_child = panel;
            m_child->m_parent = this;
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

            bool made_changes = Render(g->CurrentWindow);

            ImGui::End();
            return made_changes;
        }

        bool RootAppPanel::Render(ImGuiWindow* window)
        {
            if (m_child) {
                // The root panel Dear ImGui window gets reused by the child panel
                return m_child->Render(window);
            }

            return false;
        }

        LayoutPanel::LayoutPanel(yart::GUI::LayoutDirection direction, Panel* ul_child, Panel* lr_child)
            : m_ul_child(ul_child), m_lr_child(lr_child)
        {
            // Attach the child panels
            m_ul_child->m_parent = this;
            m_lr_child->m_parent = this;

            m_layoutContext.direction = direction;
            m_layoutContext.preserveSecondSectionSize = true;
            m_layoutContext.default_size_ratio = 1.0f / 2.0f;
            m_layoutContext.min_size = 50.0f;
        }

        LayoutPanel::~LayoutPanel()
        {
            delete m_ul_child;
            delete m_lr_child;
        }

        bool LayoutPanel::Render(ImGuiWindow* window)
        {
            ImGuiContext* g = ImGui::GetCurrentContext();
            bool made_changes = false;

            GUI::BeginLayout(m_layoutContext);
            {
                made_changes |= m_ul_child->Render(g->CurrentWindow);
            }
            GUI::LayoutSeparator(m_layoutContext);
            {
                made_changes |= m_lr_child->Render(g->CurrentWindow);
            }
            GUI::EndLayout(m_layoutContext);

            return made_changes;
        }

    } // namespace Interface
} // namespace yart
