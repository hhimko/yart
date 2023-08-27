////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui.h"


#include "gui_internal.h"


namespace yart
{
    GUI::GuiContext* GUI::GetCurrentContext()
    {
        static yart::GUI::GuiContext s_context = { };  
        return &s_context;
    }

    void GUI::ApplyCustomStyle()
    {
        // -- APPLY YART STYLE VARS -- // 
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowTitleAlign = { 0.5f, 0.5f };
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ChildBorderSize = 1.0f;
        style.ChildRounding = 5.0f;
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


        // -- APPLY DEFAULT YART COLOR PALETTE -- // 
        ImVec4* colors = ImGui::GetStyle().Colors;

        // | |  Dear ImGui style identifier        | |  Color value                  | |              Alpha value  | |
        // +-+-------------------------------------+-+-------------------------------+-+---------------------------+-+
        {
            colors[ImGuiCol_Text]                   = { YART_GUI_COLOR_WHITE,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TextDisabled]           = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_WindowBg]               = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ChildBg]                = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PopupBg]                = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Border]                 = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_BorderShadow]           = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_FrameBg]                = { YART_GUI_COLOR_DARK_GRAY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgHovered]         = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgActive]          = { YART_GUI_COLOR_LIGHT_GRAY,           YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBg]                = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgActive]          = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgCollapsed]       = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_MenuBarBg]              = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarBg]            = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_ScrollbarGrab]          = { YART_GUI_COLOR_WHITE,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabHovered]   = { YART_GUI_COLOR_WHITE,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabActive]    = { YART_GUI_COLOR_WHITE,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_CheckMark]              = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrab]             = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrabActive]       = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Button]                 = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonHovered]          = { YART_GUI_COLOR_PRIMARY,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonActive]           = { YART_GUI_COLOR_LIGHT_PRIMARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Header]                 = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_HeaderHovered]          = { YART_GUI_COLOR_PRIMARY,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_HeaderActive]           = { YART_GUI_COLOR_LIGHT_PRIMARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Separator]              = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SeparatorHovered]       = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SeparatorActive]        = { YART_GUI_COLOR_TERTIARY,             YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ResizeGrip]             = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_ResizeGripHovered]      = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ResizeGripActive]       = { YART_GUI_COLOR_TERTIARY,             YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Tab]                    = { YART_GUI_COLOR_DARKEST_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabHovered]             = { YART_GUI_COLOR_DARK_GRAY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabActive]              = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabUnfocused]           = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TabUnfocusedActive]     = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotLines]              = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotLinesHovered]       = { YART_GUI_COLOR_TERTIARY,             YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotHistogram]          = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PlotHistogramHovered]   = { YART_GUI_COLOR_TERTIARY,             YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableHeaderBg]          = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableBorderStrong]      = { YART_GUI_COLOR_DARK_GRAY,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableBorderLight]       = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TableRowBg]             = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_TableRowBgAlt]          = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_MEDIUM };
            colors[ImGuiCol_TextSelectedBg]         = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_MEDIUM };
            colors[ImGuiCol_DragDropTarget]         = { YART_GUI_COLOR_TERTIARY,             YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavHighlight]           = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavWindowingHighlight]  = { YART_GUI_COLOR_DARK_TERTIARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_NavWindowingDimBg]      = { YART_GUI_COLOR_BLACK,                  YART_GUI_ALPHA_HIGH };
            colors[ImGuiCol_ModalWindowDimBg]       = { YART_GUI_COLOR_BLACK,                  YART_GUI_ALPHA_HIGH };
        }
    }

    void GUI::LoadFonts()
    {
        GuiContext* ctx = GUI::GetCurrentContext();

        ImGuiIO& io = ImGui::GetIO();
        static float icon_font_size = 16.0f;

        static const ImWchar icons_ranges[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };
        ImFontConfig icons_config = { }; 
        icons_config.PixelSnapH = true; 
        icons_config.GlyphMinAdvanceX = icon_font_size;
        ImFont* icons_font = io.Fonts->AddFontFromFileTTF("..\\res\\fonts\\" FONT_ICON_FILE_NAME_CI, icon_font_size, &icons_config, icons_ranges);

        ctx->iconsFont = icons_font;
    }

    void GUI::PushIconsFont()
    {
        GuiContext* ctx = GUI::GetCurrentContext();
        ImGui::PushFont(ctx->iconsFont);
    }

    ImVec2 GUI::GetMainViewportAreaPosition()
    {
        GuiContext* ctx = GetCurrentContext();
        return ctx->renderViewportAreaPos;
    }

    ImVec2 GUI::GetMainViewportAreaSize()
    {
        GuiContext* ctx = GetCurrentContext();

        float w = ctx->renderViewportAreaWidth;
        float h = ctx->renderViewportAreaHeight;

        return { w > 0 ? w : 1, h > 0 ? h : 1 }; 
    }

    bool GUI::IsMouseOverRenderViewport()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (g->HoveredWindow != nullptr) {
            GuiContext* ctx = GUI::GetCurrentContext();
            return g->HoveredWindow->ID == ctx->renderViewportWindowID;
        }

        return false;
    }

    void GUI::RegisterCallback(imgui_callback_t callback)
    {
        GuiContext* ctx = GetCurrentContext();
        ctx->registeredCallbacks.push_back(callback);
    }

    void GUI::RegisterInspectorWindow(const char *name, const char *icon, ImU32 color, imgui_callback_t callback)
    {
        GuiContext* ctx = GetCurrentContext();

        InspectorWindow item;
        item.name = name;
        item.icon = icon;
        item.color = color;
        item.callback = callback;

        ctx->inspectorWindows.push_back(item);
        ctx->activeInspectorWindow = &ctx->inspectorWindows.front();
    }

    bool GUI::RenderViewAxesWindow(const glm::vec3 &x_axis, const glm::vec3 &y_axis, const glm::vec3 &z_axis, glm::vec3& clicked_axis)
    {
        return GUI::RenderViewAxesWindowEx(x_axis, y_axis, z_axis, clicked_axis);
    }

    void GUI::Render()
    {
        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();


        // Update the display size delta
        GuiContext* ctx = GUI::GetCurrentContext();
        static ImVec2 last_display_size = ImGui::GetIO().DisplaySize;
        ImVec2 display_size = ImGui::GetIO().DisplaySize;

        ctx->displaySizeDelta = { display_size.x - last_display_size.x, display_size.y - last_display_size.y};
        last_display_size = display_size;

        // Render the static layout
        GUI::RenderMainMenuBar();
        GUI::RenderMainContentFrame();

        // Render registered global callbacks
        for (auto callback : ctx->registeredCallbacks)
            callback();
    }   

} // namespace yart
