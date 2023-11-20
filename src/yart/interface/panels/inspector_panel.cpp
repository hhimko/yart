#include "inspector_panel.h"


#include <imgui_internal.h>

#include "yart/interface/interface_internal.h"


namespace yart
{
    namespace Interface
    {
        bool InspectorPanel::Render(ImGuiWindow* window)
        {
            if (GUI::BeginTabBar("Scene")) {
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

                ImGui::Text("Hello from the Scene tab!");

                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Object")) {
                ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

                ImGui::Text("Hello from the Object tab!");

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            GUI::EndTabBar();

            return false;
        }

    } // namespace Interface
} // namespace yart

