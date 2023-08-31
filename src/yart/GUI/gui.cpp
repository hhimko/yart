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

    void GUI::Render()
    {
        // Uncomment to display Dear ImGui's debug window
        ImGui::ShowDemoWindow();


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

    bool GUI::RenderViewAxesWindow(const glm::vec3 &x_axis, const glm::vec3 &y_axis, const glm::vec3 &z_axis, glm::vec3& clicked_axis)
    {
        return GUI::RenderViewAxesWindowEx(x_axis, y_axis, z_axis, clicked_axis);
    }

    void GUI::ApplyCustomStyle()
    {
        // -- APPLY YART STYLE VARS -- // 
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.WindowTitleAlign = { 0.5f, 0.5f };
        style.WindowMenuButtonPosition = ImGuiDir_None;
        style.ChildBorderSize = 1.0f;
        style.ChildRounding = 4.0f;
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
            colors[ImGuiCol_Text]                   = { YART_GUI_TEXT_COLOR_LIGHT,           YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TextDisabled]           = { YART_GUI_TEXT_COLOR_DARK,            YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_WindowBg]               = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ChildBg]                = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_PopupBg]                = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Border]                 = { YART_GUI_COLOR_LIGHT_GRAY,           YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_BorderShadow]           = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_FrameBg]                = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgHovered]         = { YART_GUI_COLOR_LIGHT_GRAY,           YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_FrameBgActive]          = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBg]                = { YART_GUI_COLOR_BLACK,                YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgActive]          = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_TitleBgCollapsed]       = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_MenuBarBg]              = { YART_GUI_COLOR_DARKER_GRAY,          YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarBg]            = { YART_GUI_COLOR_DARKEST_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrab]          = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabHovered]   = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ScrollbarGrabActive]    = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_CheckMark]              = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrab]             = { YART_GUI_COLOR_LIGHTER_GRAY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_SliderGrabActive]       = { YART_GUI_COLOR_LIGHTEST_GRAY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Button]                 = { YART_GUI_COLOR_DARK_PRIMARY,         YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonHovered]          = { YART_GUI_COLOR_PRIMARY,              YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_ButtonActive]           = { YART_GUI_COLOR_LIGHT_PRIMARY,        YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Header]                 = { YART_GUI_COLOR_BLACK,           YART_GUI_ALPHA_TRANSPARENT };
            colors[ImGuiCol_HeaderHovered]          = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_HeaderActive]           = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
            colors[ImGuiCol_Separator]              = { YART_GUI_COLOR_LIGHT_GRAY,           YART_GUI_ALPHA_OPAQUE };
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
            colors[ImGuiCol_TableBorderStrong]      = { YART_GUI_COLOR_GRAY,                 YART_GUI_ALPHA_OPAQUE };
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

    ImVec2 GUI::GetRenderViewportAreaPosition()
    {
        GuiContext* ctx = GetCurrentContext();
        return ctx->renderViewportAreaPos;
    }

    ImVec2 GUI::GetRenderViewportAreaSize()
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

    bool GUI::BeginCollapsableSection(const char *name)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;

        static const ImU32 fg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, YART_GUI_ALPHA_OPAQUE });
        const float rounding = g->Style.ChildRounding;

        const ImVec2 p_min = window->DC.CursorPos;
        const ImVec2 p_max = { window->WorkRect.Max.x, p_min.y + window->ContentSize.y };
        window->DrawList->AddRectFilled(p_min, p_max, fg_col, rounding);

        const ImVec2 backup_frame_padding = g->Style.FramePadding;
        const float backup_frame_rounding = g->Style.FrameRounding;
        g->Style.FramePadding = { backup_frame_padding.x, rounding };
        g->Style.FrameRounding = rounding;
        
        window->DC.CursorPos.x += ImFloor(g->Style.WindowPadding.x / 2.0f) - 1.0f;
        window->WorkRect.Max.x -= ImFloor(g->Style.WindowPadding.x / 2.0f);
        bool open = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_SpanAvailWidth);
        window->WorkRect.Max.x += ImFloor(g->Style.WindowPadding.x / 2.0f);
        window->DC.CursorPos.x -= ImFloor(g->Style.WindowPadding.x / 2.0f) - 1.0f;

        g->Style.FrameRounding = backup_frame_rounding;
        g->Style.FramePadding = backup_frame_padding;

        
        if (open) {
            // Draw the header separator line
            static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, YART_GUI_ALPHA_OPAQUE });
            const ImVec2 p1 = { window->DC.CursorPos.x, window->DC.CursorPos.y - g->Style.ItemSpacing.y };
            const ImVec2 p2 = { p1.x + window->SizeFull.x, p1.y };
            window->DrawList->AddLine(p1, p2, bg_col);

            window->ContentRegionRect.TranslateX(-g->Style.WindowPadding.x);

            ImVec4 clip_rect = window->DrawList->_ClipRectStack.back();
            window->DrawList->PushClipRect({ clip_rect.x, clip_rect.y }, { window->ContentRegionRect.Max.x, clip_rect.w });

            window->DC.CursorPos.y += g->Style.ItemSpacing.y;
        }

        ImGui::Indent(g->Style.WindowPadding.x);
        
        return open;
    }

    void GUI::EndCollapsableSection(bool was_open)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;

        ImGui::Unindent(g->Style.WindowPadding.x);

        if (was_open) {
            window->ContentRegionRect.TranslateX(g->Style.WindowPadding.x);
            window->DrawList->PopClipRect();

            const float rounding = g->Style.ChildRounding;
            ImGui::ItemSize({ 0.0f, rounding });
        }

        // Draw the background rect
        static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, YART_GUI_ALPHA_OPAQUE });
        ImVec2 p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y - g->Style.ItemSpacing.y };
        ImVec2 p_max = { window->WorkRect.Max.x, p_min.y + window->ContentSize.y };
        window->DrawList->AddRectFilled(p_min, p_max, bg_col, 0);
    }

    void GUI::BeginFrame(const char* name, uint32_t rows)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return;


        // -- RENDER CHILD FRAME -- //
        const ImVec4 backup_child_bg = g->Style.Colors[ImGuiCol_ChildBg];
        g->Style.Colors[ImGuiCol_ChildBg] = { YART_GUI_COLOR_DARK_GRAY, YART_GUI_ALPHA_OPAQUE };

        const float header_height = ImGui::GetTextLineHeight();
        float frame_height = rows * ImGui::GetFrameHeightWithSpacing() + g->Style.WindowPadding.y * 2.0f - g->Style.ItemSpacing.y;
        frame_height += header_height / 2.0f - g->Style.WindowPadding.y + g->Style.ItemSpacing.y;
        window->DC.CursorPos.y += header_height / 2.0f;
        
        static constexpr ImGuiWindowFlags flags = (
            ImGuiWindowFlags_NavFlattened | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding
        );

        ImGui::BeginChild(name, { 0.0f, frame_height }, true, flags);
        window = g->CurrentWindow;

        g->Style.Colors[ImGuiCol_ChildBg] = backup_child_bg;


        // -- RENDER HEADER TITLE -- //
        const ImVec4 backup_clip_rect = window->DrawList->_ClipRectStack.back();
        window->DrawList->PopClipRect();
        window->DrawList->PushClipRect({ backup_clip_rect.x, 0 }, { backup_clip_rect.z, backup_clip_rect.w });
        window->DC.CursorPos.y -= g->Style.WindowPadding.y + header_height / 2.0f;

        static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, YART_GUI_ALPHA_OPAQUE });
        const ImU32 text_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_Text]);
        const float text_width = ImGui::CalcTextSize(name).x;
        const float frame_padding = g->Style.FramePadding.x;
        const ImVec2 p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y };
        const ImVec2 p_max = { p_min.x + 2.0f * frame_padding + text_width, p_min.y + header_height };
        const ImVec2 text_pos = { p_min.x + frame_padding, p_min.y };

        // The title is rendered twice, first in the parent window to avoid clipping issues
        window->ParentWindow->DrawList->PushClipRect({ backup_clip_rect.x, 0 }, { backup_clip_rect.z, backup_clip_rect.w });
        window->ParentWindow->DrawList->AddRectFilled(p_min, p_max, bg_col);
        window->ParentWindow->DrawList->AddText(text_pos, text_col, name);
        window->ParentWindow->DrawList->PopClipRect();
        
        window->DrawList->AddRectFilled(p_min, p_max, bg_col);
        window->DrawList->AddText(text_pos, text_col, name);
        
        ImGui::ItemSize({ 0.0f, header_height });

        window->DrawList->PopClipRect();
        window->DrawList->PushClipRect({ backup_clip_rect.x, backup_clip_rect.y }, { backup_clip_rect.z, backup_clip_rect.w });

    }

    void GUI::EndFrame()
    {
        ImGui::EndChild();
    }

    void GUI::DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, glm::vec3 const* values, float const* locations, size_t size, bool border)
    {
        const ImVec2 rect_size = { p_max.x - p_min.x, p_max.y - p_max.y };
        p_max.x = p_min.x + rect_size.x * locations[0];
        const float start_x = p_min.x;

        if (p_min.x != p_max.x) {
            ImU32 col = ImGui::ColorConvertFloat4ToU32({ values[0].x, values[0].y, values[0].z, 1.0f });
            draw_list->AddRectFilled(p_min, p_max, col);
        }

        for (size_t i = 0; i < size - 1; ++i) {
            p_min.x = p_max.x;
            p_max.x = p_min.x + rect_size.x * (locations[i + 1] - locations[i]);

            ImU32 col_min = ImGui::ColorConvertFloat4ToU32({ values[i    ].x, values[i    ].y, values[i    ].z, 1.0f });
            ImU32 col_max = ImGui::ColorConvertFloat4ToU32({ values[i + 1].x, values[i + 1].y, values[i + 1].z, 1.0f });
            draw_list->AddRectFilledMultiColor(p_min, p_max, col_min, col_max, col_max, col_min);
        }

        if (p_max.x != p_min.x + rect_size.x) {
            p_min.x = p_max.x;
            p_max.x = start_x + rect_size.x;
            
            const glm::vec3& last = values[size - 1];
            ImU32 col = ImGui::ColorConvertFloat4ToU32({ last.x, last.y, last.z, 1.0f });
            draw_list->AddRectFilled(p_min, p_max, col);
        }

        if (border) {
            ImGuiContext* g = ImGui::GetCurrentContext();
            const ImU32 border_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_Border]);
            draw_list->AddRect({ start_x - 1.0f, p_min.y }, p_max, border_col);
        }
    }

    bool GUI::GradientEditor(GradientEditorContext& ctx)
    {
        return GUI::GradientEditorEx(ctx);
    }

    void GUI::FullWidthSeparator(float thickness)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;

        const float padding = g->Style.WindowPadding.x;
        ImVec2 p1 = { window->Pos.x, window->DC.CursorPos.y + 2.0f };
        ImVec2 p2 = { p1.x + ImGui::GetContentRegionAvail().x + 2.0f * padding, p1.y + thickness };

        const ImVec4 backup_clip_rect = g->CurrentWindow->DrawList->_ClipRectStack.back();
        g->CurrentWindow->DrawList->PopClipRect();

        g->CurrentWindow->DrawList->PushClipRect({ backup_clip_rect.x - padding, backup_clip_rect.y }, { backup_clip_rect.z + padding, backup_clip_rect.w });
        static const ImU32 col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKEST_GRAY, YART_GUI_ALPHA_OPAQUE });
        g->CurrentWindow->DrawList->AddRectFilled(p1, p2, col);
        g->CurrentWindow->DrawList->PopClipRect();

        g->CurrentWindow->DrawList->PushClipRect({ backup_clip_rect.x, backup_clip_rect.y }, { backup_clip_rect.z, backup_clip_rect.w });

        ImGui::ItemSize({ 0.0f, thickness });
    }

} // namespace yart
