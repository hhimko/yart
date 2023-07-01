#include "input.h"

#include <imgui.h>


// -- STATIC CONTEXT -- //
static float s_horizontalAxis;
static float s_verticalAxis;


namespace yart
{
    float Input::GetHorizontalAxis() 
    {
        return s_horizontalAxis;
    }

    float Input::GetVerticalAxis()
    {
        return s_verticalAxis;
    }

    void Input::Update()
    {
        // Update movement axes
        s_horizontalAxis = 0.0f;
        s_verticalAxis = 0.0f;

        s_horizontalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_RightArrow);
        s_horizontalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_LeftArrow);

        s_verticalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_UpArrow);
        s_verticalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_DownArrow);
    }
} // namespace yart
