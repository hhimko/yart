////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui.h"


#include "gui_internal.h"


static yart::GUI::GUIContext s_context;  


namespace yart
{
    GUI::GUIContext& GUI::GetCurrentContext()
    {
        return s_context;
    }

    void GUI::ApplyCustomStyle()
    {
        // -- APPLY YART STYLE VARS -- // 
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowTitleAlign = { 0.5f, 0.5f };
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ChildBorderSize = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.PopupBorderSize = 1.0f;
        style.PopupRounding = 0.0f;
        style.FrameRounding = 3.0f;
        style.WindowRounding = 2.0f;
        style.ScrollbarSize = 10.0f;
        style.ScrollbarRounding = 10.0f;
        style.GrabMinSize = 8.0f;
        style.GrabRounding = 1.0f;
        style.TabBorderSize = 0.0f;
        style.TabRounding = 5.0f;
        style.WindowPadding = { 6.0f, 6.0f };
        style.FramePadding = { 6.0f, 2.0f };
        style.SeparatorTextBorderSize = 1.0f;
        style.SeparatorTextPadding = { 18.0f, 2.0f };


        // -- APPLY YART COLOR PALETTE -- // 
        ImVec4* colors = ImGui::GetStyle().Colors;
        // | |  Dear ImGui style identifier        | |  Color value                 | |              Alpha value  | |
        // +-+-------------------------------------+-+------------------------------+-+---------------------------+-+
        {
            colors[ImGuiCol_Text]                   = { YART_GUI_COLOR_WHITE,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TextDisabled]           = { YART_GUI_COLOR_LIGHTER_GRAY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_WindowBg]               = { YART_GUI_COLOR_BLACK,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ChildBg]                = { YART_GUI_COLOR_BLACK,         YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_PopupBg]                = { YART_GUI_COLOR_BLACK,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Border]                 = { YART_GUI_COLOR_DARK_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_BorderShadow]           = { YART_GUI_COLOR_BLACK,         YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_FrameBg]                = { YART_GUI_COLOR_DARK_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgHovered]         = { YART_GUI_COLOR_GRAY,               YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgActive]          = { YART_GUI_COLOR_LIGHT_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBg]                = { YART_GUI_COLOR_BLACK,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgActive]          = { YART_GUI_COLOR_DARK_PRIMARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgCollapsed]       = { YART_GUI_COLOR_DARKER_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_MenuBarBg]              = { YART_GUI_COLOR_DARKER_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarBg]            = { YART_GUI_COLOR_BLACK,         YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_ScrollbarGrab]          = { YART_GUI_COLOR_WHITE,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabHovered]   = { YART_GUI_COLOR_WHITE,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabActive]    = { YART_GUI_COLOR_WHITE,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_CheckMark]              = { YART_GUI_COLOR_LIGHTEST_GRAY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrab]             = { YART_GUI_COLOR_LIGHTER_GRAY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrabActive]       = { YART_GUI_COLOR_LIGHTEST_GRAY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Button]                 = { YART_GUI_COLOR_DARK_PRIMARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonHovered]          = { YART_GUI_COLOR_PRIMARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonActive]           = { YART_GUI_COLOR_LIGHT_PRIMARY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Header]                 = { YART_GUI_COLOR_DARK_PRIMARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_HeaderHovered]          = { YART_GUI_COLOR_PRIMARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_HeaderActive]           = { YART_GUI_COLOR_LIGHT_PRIMARY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Separator]              = { YART_GUI_COLOR_LIGHT_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SeparatorHovered]       = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SeparatorActive]        = { YART_GUI_COLOR_TRINARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ResizeGrip]             = { YART_GUI_COLOR_DARK_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ResizeGripHovered]      = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ResizeGripActive]       = { YART_GUI_COLOR_TRINARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Tab]                    = { YART_GUI_COLOR_DARK_SECONDARY,     YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabHovered]             = { YART_GUI_COLOR_LIGHT_SECONDARY,    YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabActive]              = { YART_GUI_COLOR_SECONDARY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabUnfocused]           = { YART_GUI_COLOR_LIGHTER_GRAY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabUnfocusedActive]     = { YART_GUI_COLOR_LIGHTEST_GRAY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotLines]              = { YART_GUI_COLOR_LIGHTEST_GRAY,      YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotLinesHovered]       = { YART_GUI_COLOR_TRINARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotHistogram]          = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotHistogramHovered]   = { YART_GUI_COLOR_TRINARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableHeaderBg]          = { YART_GUI_COLOR_DARK_PRIMARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableBorderStrong]      = { YART_GUI_COLOR_DARK_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableBorderLight]       = { YART_GUI_COLOR_DARKER_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableRowBg]             = { YART_GUI_COLOR_BLACK,         YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_TableRowBgAlt]          = { YART_GUI_COLOR_DARKER_GRAY,        YART_GUI_ALPHA_MEDIUM };
            colors[ImGuiCol_TextSelectedBg]         = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_MEDIUM };
            colors[ImGuiCol_DragDropTarget]         = { YART_GUI_COLOR_TRINARY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavHighlight]           = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavWindowingHighlight]  = { YART_GUI_COLOR_DARK_TRINARY,       YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavWindowingDimBg]      = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_HIGH };
            colors[ImGuiCol_ModalWindowDimBg]       = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_HIGH };
        }
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
        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();

        // Render registered ImGui windows 
        for (auto &&window : s_context.registeredImGuiWindows) {
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Window title bottom border on
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 4.0f }); // Window title vertical padding +2 
            ImGui::Begin(window.name);
            ImGui::PopStyleVar(2);

            window.callback();
            ImGui::End();
        }


        // -- Render Main Menu Bar -- // 
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, { 0.0f, 0.0f, 0.0f, 1.0f });
        ImGui::BeginMainMenuBar();
        ImGui::PopStyleColor();

        if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New");
			ImGui::MenuItem("Create");
			ImGui::EndMenu();
		}

        ImGui::EndMainMenuBar();
    }   
} // namespace yart
