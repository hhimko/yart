////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the platform specific Input class for GLFW 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "input.h"


#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "window.h"


// -- STATIC CONTEXT -- //
static float s_horizontalAxis;
static float s_verticalAxis;

static int s_current_mouse_mode = GLFW_CURSOR_NORMAL;
static glm::ivec2 s_mousePosition = { 0, 0 }; 
static glm::ivec2 s_mouseMoveDelta = { 0, 0 }; 


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

    void Input::SetCursorLocked(bool state)
    {
        yart::Window& window = yart::Window::Get();

        int glfw_input_mode = state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        if (glfw_input_mode == s_current_mouse_mode)
            return;

        s_current_mouse_mode = glfw_input_mode;
        glfwSetInputMode(window.m_window, GLFW_CURSOR, glfw_input_mode);

        s_mouseMoveDelta = { 0, 0 }; // Required to avoid jumping cursor issues 
    }

    const glm::ivec2 &Input::GetMouseMoveDelta()
    {
        return s_mouseMoveDelta;
    }

    void Input::Update()
    {
        yart::Window& window = yart::Window::Get();

        // Update movement axes
        s_horizontalAxis = 0.0f;
        s_verticalAxis = 0.0f;

        s_horizontalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_D);
        s_horizontalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_A);

        s_verticalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_W);
        s_verticalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_S);

        // Update mouse state
        double mouse_x, mouse_y;
        glfwGetCursorPos(window.m_window, &mouse_x, &mouse_y);
        glm::ivec2 mouse_pos = { mouse_x, mouse_y };

        s_mouseMoveDelta = s_mousePosition - mouse_pos;
        s_mousePosition = mouse_pos;
    }
} // namespace yart
