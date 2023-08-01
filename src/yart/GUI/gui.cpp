#include "gui.h"


#include "gui_internal.h"


static yart::GUI::GUIContext s_context;  


namespace yart
{
    GUI::GUIContext& GUI::GetCurrentContext()
    {
        return s_context;
    }

    void GUI::RegisterImGuiWindow(const char *window_name, imgui_callback_t callback)
    {
        GUIContext::ImGuiWindow window;
        window.name = window_name;
        window.callback = callback;

        s_context.registeredImGuiWindows.push_back(window);
    }

    void GUI::Render() 
    {
        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New");
			ImGui::MenuItem("Create");
			ImGui::EndMenu();
		}

        ImGui::EndMainMenuBar();

        // Render registered ImGui windows 
        for (auto &&window : s_context.registeredImGuiWindows) {
            ImGui::Begin(window.name);
            window.callback();
            ImGui::End();
        }
    }   
} // namespace yart
