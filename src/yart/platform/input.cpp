////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the platform specific Input class for GLFW 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "input.h"


#include <iostream>

#include <imgui_internal.h>
#include <imgui.h>

#include "window.h"


// -- STATIC CONTEXT -- //
static float s_horizontalAxis;
static float s_verticalAxis;

static bool s_cursorLocked = false;
static bool s_cursorLockedLastFrame = false;
static bool s_cursorLockForce= false;
static ImVec2 s_mouseLockPos = { 0, 0 }; 


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

    void Input::SetCursorLocked(bool force)
    {
        // Hide the mouse cursor for the immediate frame
        ImGui::SetMouseCursor(ImGuiMouseCursor_None); 
        s_cursorLocked = true;
        s_cursorLockForce = force;

        if (!s_cursorLockedLastFrame) {
            ImGuiContext* g = ImGui::GetCurrentContext();
            s_mouseLockPos = g->IO.MousePos;
        }
    }

    ImVec2 Input::GetMouseMoveDelta()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        return g->IO.MouseDelta;
    }

    void Input::Update()
    {
        // Update movement axes
        s_horizontalAxis = 0.0f;
        s_verticalAxis = 0.0f;

        s_horizontalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_D);
        s_horizontalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_A);

        s_verticalAxis += 1.0f * ImGui::IsKeyDown(ImGuiKey_W);
        s_verticalAxis -= 1.0f * ImGui::IsKeyDown(ImGuiKey_S);

        // Update mouse state
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (s_cursorLocked) {
            if (s_cursorLockForce) {
                yart::Window& window = yart::Window::Get();
                
                double x, y;
                glfwGetCursorPos(window.m_window, &x, &y);
                g->IO.MouseDelta.x = (float)x - s_mouseLockPos.x;
                g->IO.MouseDelta.y = (float)y - s_mouseLockPos.y;

                glfwSetCursorPos(window.m_window, (double)s_mouseLockPos.x, (double)s_mouseLockPos.y);
            }
        } else if (s_cursorLockedLastFrame) {
            // Hide the cursor for one additional frame to mitigate flickering
            ImGui::SetMouseCursor(ImGuiMouseCursor_None); 
            g->IO.MousePos = s_mouseLockPos;
            g->IO.MousePosPrev = s_mouseLockPos;

            yart::Window& window = yart::Window::Get();
            glfwSetCursorPos(window.m_window, (double)s_mouseLockPos.x, (double)s_mouseLockPos.y);
        }

        s_cursorLockedLastFrame = s_cursorLocked;
        s_cursorLocked = false;
    }
} // namespace yart
