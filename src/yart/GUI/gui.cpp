////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui.h"


#include "gui_internal.h"


namespace yart
{
    GUI::GuiContext* GUI::GetGUIContext()
    {
        static yart::GUI::GuiContext s_context = { };  
        return &s_context;
    }

    bool GUI::Render()
    {
        // Uncomment to display Dear ImGui's debug window
        // ImGui::ShowDemoWindow();


        // Refresh and update the context state
        GuiContext* ctx = GUI::GetGUIContext();
        ctx->madeChanges = false;

        static ImVec2 last_display_size = ImGui::GetIO().DisplaySize;
        ImVec2 display_size = ImGui::GetIO().DisplaySize;

        ctx->displaySizeDelta = { display_size.x - last_display_size.x, display_size.y - last_display_size.y};
        last_display_size = display_size;


        // Render the static layout
        GUI::RenderMainMenuBar();
        GUI::RenderMainContentFrame();

        // Render registered global callbacks
        for (auto callback : ctx->registeredCallbacks)
            ctx->madeChanges |= callback();

        return ctx->madeChanges;
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
        style.ItemInnerSpacing = { 6.0f, 4.0f };
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
            colors[ImGuiCol_TitleBgCollapsed]       = { YART_GUI_COLOR_DARKEST_GRAY,         YART_GUI_ALPHA_OPAQUE };
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
        GuiContext* ctx = GUI::GetGUIContext();

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
        GuiContext* ctx = GUI::GetGUIContext();
        ImGui::PushFont(ctx->iconsFont);
    }

    ImGuiID GUI::GetIDFormatted(const char* fmt, ...)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        va_list args;
        va_start(args, fmt);

        const char* str;
        ImFormatStringToTempBufferV(&str, nullptr, fmt, args);

        va_end(args);
        return window->GetID(str);
    }

    ImVec2 GUI::GetRenderViewportAreaPosition()
    {
        GuiContext* ctx = GetGUIContext();
        return ctx->renderViewportAreaPos;
    }

    ImVec2 GUI::GetRenderViewportAreaSize()
    {
        GuiContext* ctx = GetGUIContext();

        float w = ctx->renderViewportAreaWidth;
        float h = ctx->renderViewportAreaHeight;

        return { w > 0 ? w : 1, h > 0 ? h : 1 }; 
    }

    bool GUI::IsMouseOverRenderViewport()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (g->HoveredWindow != nullptr) {
            GuiContext* ctx = GUI::GetGUIContext();
            return g->HoveredWindow->ID == ctx->renderViewportWindowID;
        }

        return false;
    }

    void GUI::RegisterCallback(imgui_callback_t callback)
    {
        GuiContext* ctx = GetGUIContext();
        ctx->registeredCallbacks.push_back(callback);
    }

    void GUI::RegisterInspectorWindow(const char *name, const char *icon, ImU32 color, imgui_callback_t callback)
    {
        GuiContext* ctx = GetGUIContext();

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
            static const float separator_thickness = 1.0f;
            static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, YART_GUI_ALPHA_OPAQUE });
            const ImVec2 p1 = { window->DC.CursorPos.x, window->DC.CursorPos.y - g->Style.ItemSpacing.y };
            const ImVec2 p2 = { p1.x + window->SizeFull.x, p1.y + separator_thickness };
            window->DrawList->AddRectFilled(p1, p2, bg_col);

            window->ContentRegionRect.TranslateX(-g->Style.WindowPadding.x);
            window->WorkRect.TranslateX(-g->Style.WindowPadding.x);

            ImVec4 clip_rect = window->DrawList->_ClipRectStack.back();
            window->DrawList->PushClipRect({ clip_rect.x, clip_rect.y }, { window->ContentRegionRect.Max.x + 4.0f, clip_rect.w });

            window->DC.CursorPos.y += g->Style.ItemSpacing.y + separator_thickness;
        }

        ImGui::Indent(g->Style.WindowPadding.x);

        return open;
    }

    void GUI::EndCollapsableSection(bool was_open)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;

        const float rounding = g->Style.ChildRounding;

        ImGui::Unindent(g->Style.WindowPadding.x);
        if (was_open) {
            window->ContentRegionRect.TranslateX(g->Style.WindowPadding.x);
            window->WorkRect.TranslateX(g->Style.WindowPadding.x);
            window->DrawList->PopClipRect();

            ImGui::ItemSize({ 0.0f, rounding });
        }

        // Draw the background rect
        static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKER_GRAY, YART_GUI_ALPHA_OPAQUE });
        ImVec2 p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y - g->Style.ItemSpacing.y };
        ImVec2 p_max = { window->WorkRect.Max.x, p_min.y + window->ContentSize.y };
        window->DrawList->AddRectFilled(p_min, p_max, bg_col, 0);

        // Crude way to simulate rounded edges on the section foreground
        // This currently only works for ChildRounding value of 4
        ImVec2 p1 = { p_min.x, p_min.y - 3.0f };
        ImVec2 p2 = { p_min.x + 3.0f, p_min.y };
        window->DrawList->AddBezierQuadratic(p1, p_min, p2, bg_col, 1.0f);

        p1.x = p_max.x;
        p2.x = p_max.x - 3.0f;
        window->DrawList->AddBezierQuadratic(p1, { p_max.x, p_min.y }, p2, bg_col, 1.0f);
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

    bool GUI::DrawText(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float align, const char* text)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        // Calculate offset based on the alignment value
        const ImVec2 text_size = ImGui::CalcTextSize(text);
        const float offset_x = ImMax(0.0f, (p_max.x - p_min.x) * align - text_size.x * align);
        const float offset_y = (p_max.y - p_min.y - g->FontSize) / 2.0f;

        const ImVec2 p0 = { p_min.x + offset_x, p_min.y + offset_y }; 
        ImGui::RenderTextEllipsis(draw_list, p0, p_max, p_max.x, p_max.x, text, nullptr, &text_size);

        return text_size.x > (p_max.x - p_min.x);
    }

    void GUI::DrawLeftArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col)
    {
        if ((col & IM_COL32_A_MASK) == 0)
            return;

        const ImVec2 p0 = { p_max.x - padding.x, p_min.y + padding.y };
        const ImVec2 p2 = { p0.x, p_max.y - padding.y };
        const ImVec2 p1 = { p_min.x + padding.x, p0.y + (p2.y - p0.y) / 2.0f };

        draw_list->PathLineTo(p0);
        draw_list->PathLineTo(p1);
        draw_list->PathLineTo(p2);
        draw_list->PathStroke(col, ImDrawFlags_None, 1.0f);
    }

    void GUI::DrawRightArrow(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& padding, ImU32 col)
    {
        if ((col & IM_COL32_A_MASK) == 0)
            return;

        const ImVec2 p0 = { p_min.x + padding.x, p_min.y + padding.y };
        const ImVec2 p2 = { p0.x, p_max.y - padding.y };
        const ImVec2 p1 = { p_max.x - padding.x, p0.y + (p2.y - p0.y) / 2.0f };

        draw_list->PathLineTo(p0);
        draw_list->PathLineTo(p1);
        draw_list->PathLineTo(p2);
        draw_list->PathStroke(col, ImDrawFlags_None, 1.0f);
    }

    void GUI::DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, glm::vec3 const* values, float const* locations, size_t size, bool border)
    {
        const ImVec2 rect_size = { p_max.x - p_min.x, p_max.y - p_max.y };
        p_max.x = p_min.x + rect_size.x * locations[0];
        const float start_x = p_min.x;

        // If the first stops location is not 0.0f we draw a solid color at the beginning 
        if (p_min.x != p_max.x) {
            const ImU32 col = ImGui::ColorConvertFloat4ToU32({ values[0].x, values[0].y, values[0].z, 1.0f });
            draw_list->AddRectFilled(p_min, p_max, col);
        }

        // Draw the individual segments as color interpolated rects
        for (size_t i = 0; i < size - 1; ++i) {
            p_min.x = p_max.x;
            p_max.x = p_min.x + rect_size.x * (locations[i + 1] - locations[i]);

            const ImU32 col_min = ImGui::ColorConvertFloat4ToU32({ values[i    ].x, values[i    ].y, values[i    ].z, 1.0f });
            const ImU32 col_max = ImGui::ColorConvertFloat4ToU32({ values[i + 1].x, values[i + 1].y, values[i + 1].z, 1.0f });
            draw_list->AddRectFilledMultiColor(p_min, p_max, col_min, col_max, col_max, col_min);
        }

        // If the last stops location is not 1.0f we draw a solid color at the end
        if (p_max.x != p_min.x + rect_size.x) {
            p_min.x = p_max.x;
            p_max.x = start_x + rect_size.x;
            
            const glm::vec3& last = values[size - 1];
            const ImU32 col = ImGui::ColorConvertFloat4ToU32({ last.x, last.y, last.z, 1.0f });
            draw_list->AddRectFilled(p_min, p_max, col);
        }

        // Draw an optional border
        if (border) {
            ImGuiContext* g = ImGui::GetCurrentContext();
            const ImU32 border_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_Border]);
            draw_list->AddRect({ start_x - 1.0f, p_min.y }, p_max, border_col);
        }
    }

    bool GUI::SliderInt(const char* name, int* p_val, const char* format, int arrow_step)
    {
        return GUI::SliderEx(name, ImGuiDataType_S32, (void*)p_val, nullptr, nullptr, format, (void*)&arrow_step);
    }

    bool GUI::SliderInt(const char* name, int* p_val, int min, int max, const char* format, int arrow_step)
    {
        return GUI::SliderEx(name, ImGuiDataType_S32, (void*)p_val, (void*)&min, (void*)&max, format, (void*)&arrow_step);
    }

    bool GUI::SliderFloat(const char* name, float* p_val, const char* format, float arrow_step)
    {
        return GUI::SliderEx(name, ImGuiDataType_Float, (void*)p_val, nullptr, nullptr, format, (void*)&arrow_step);
    }

    bool GUI::SliderFloat(const char *name, float *p_val, float min, float max, const char *format, float arrow_step)
    {
        return GUI::SliderEx(name, ImGuiDataType_Float, (void*)p_val, (float*)&min, (float*)&max, format, (void*)&arrow_step);
    }

    bool GUI::ComboHeader(const char *name, const char *items[], size_t items_size, int *selected_item)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;

        ImVec2 p_min = window->DC.CursorPos;
        ImVec2 p_max = { window->WorkRect.Max.x, window->DC.CursorPos.y + ImGui::GetFrameHeight() };
        const ImRect total_bb = { p_min, p_max };

        ImGuiID id = GUI::GetIDFormatted("##ComboHeader/%s", name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id, nullptr)) 
            return false;


        bool item_changed = false;

        // When navigating with keyboard/gamepad, cycle over all the items
        bool nav_activated_by_code = (g->NavActivateId == id);
        bool nav_activated_by_inputs = (g->NavActivatePressedId == id);
        if (nav_activated_by_code || nav_activated_by_inputs) {
            *selected_item = (*selected_item + 1) % items_size;
            item_changed = true;
        }

        // During nav, the items can be cycled over with arrow keys
        if (g->NavId == id && ImGui::NavMoveRequestButNoResultYet()) {
            if (g->NavMoveDir == ImGuiDir_Left) {
                *selected_item = --(*selected_item) < 0 ? static_cast<int>(items_size - 1) : *selected_item;
                ImGui::NavMoveRequestCancel();
                item_changed = true;
            } else if (g->NavMoveDir == ImGuiDir_Right) {
                *selected_item = (*selected_item + 1) % items_size;
                ImGui::NavMoveRequestCancel();
                item_changed = true;
            }
        }


        // Render the individual header items
        const ImU32 frame_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_FrameBg]);
        const ImU32 frame_hovered_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_FrameBgHovered]);
        const float rounding = g->Style.FrameRounding;

        for (int i = 0; i < items_size; ++i) {
            p_min.x = total_bb.Min.x + total_bb.GetWidth() * (i / static_cast<float>(items_size));
            p_max.x = total_bb.Min.x + total_bb.GetWidth() * ((i + 1) / static_cast<float>(items_size));

            ImGuiID button_id = GUI::GetIDFormatted("##ComboHeader/%s/%d", name, i);
            ImGui::ItemAdd({ p_min, p_max }, button_id, nullptr, ImGuiItemFlags_NoNav);
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                ImGui::ClearActiveID();
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);

                if (*selected_item != i) {
                    *selected_item = i;
                    item_changed = true;
                }
            }

            const bool hovered = ImGui::IsItemHovered();
            const bool hovered_or_nav = (hovered || (g->NavId != 0 && g->NavId == id && g->NavDisableMouseHover));
            const ImDrawFlags flags = i == 0 ? ImDrawFlags_RoundCornersLeft : i == items_size - 1 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone;
            if (i == *selected_item) {
                bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
                GUI::DrawHighlightRect(window->DrawList, p_min, p_max, 1.0f, hovered_or_nav, held, rounding, flags);
            } else {
                const ImU32 col = hovered_or_nav ? frame_hovered_col : frame_col;
                window->DrawList->AddRectFilled(p_min, p_max, col, rounding, flags);
            }

            // Render header text
            if (GUI::DrawText(window->DrawList, { p_min.x + 2.0f, p_min.y }, { p_max.x - 2.0f, p_max.y }, 0.5f, items[i]) && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip(items[i]);
        }


        // Render header items separators
        const ImU32 separator_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, YART_GUI_ALPHA_OPAQUE });
        for (int i = 0; i < items_size - 1; ++i) {
            p_min.x = total_bb.Min.x + total_bb.GetWidth() * ((i + 1) / static_cast<float>(items_size)) - 1.0f;
            p_max.x = p_min.x + 2.0f;

            window->DrawList->AddRectFilled(p_min, p_max, separator_col);
        }

        ImGui::RenderNavHighlight(total_bb, id);
        return item_changed;
    }

    bool GUI::ColorEdit(const char* name, float color[3])
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;

        // Calculate the total bounding box of the widget
        const ImGuiID id = window->GetID(name);
        const ImRect total_bb = { window->DC.CursorPos, { window->WorkRect.Max.x, window->DC.CursorPos.y + ImGui::GetFrameHeight() }};

        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id))
            return false;


        const float item_spacing = g->Style.ItemInnerSpacing.x;
        static const float text_width_percent = 0.4f; // TODO: This value should be calculated based on the current indent at some point

        const ImRect text_bb = { total_bb.Min, { total_bb.Min.x + IM_ROUND(total_bb.GetWidth() * text_width_percent) - item_spacing, total_bb.Max.y }};
        const ImRect frame_bb = { { text_bb.Max.x + item_spacing, total_bb.Min.y }, total_bb.Max };

        const bool total_hovered = g->ActiveId != id && (ImGui::ItemHoverable(total_bb, id) || g->NavId == id);
        const bool text_hovered = total_hovered && ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max);

        const bool set_current_color_edit_id = (g->ColorEditCurrentID == 0);
        if (set_current_color_edit_id)
            g->ColorEditCurrentID = id;

        static constexpr bool has_alpha = false;

        bool frame_hovered = false, made_changes = false;
        if (ImGui::ButtonBehavior(frame_bb, id, &frame_hovered, nullptr)) {
            // Store current color and open a picker
            g->ColorPickerRef = { color[0], color[1], color[2], has_alpha ? color[3] : 1.0f };
            ImGui::OpenPopup("ColorPicker");
            ImGui::SetNextWindowPos({ frame_bb.Min.x,  frame_bb.Max.y + g->Style.ItemSpacing.y });
        }

        ImGuiWindow* picker_popup_window = nullptr;
        if (ImGui::BeginPopup("ColorPicker")) {
            picker_popup_window = g->CurrentWindow;

            ImGui::TextEx(name);
            ImGui::Spacing();
            
            // ImGui::SetNextItemWidth(18 * 12.0f); // Use 256 + bar sizes?
            static constexpr ImGuiColorEditFlags flags = has_alpha ? ImGuiColorEditFlags_None : ImGuiColorEditFlags_NoAlpha;
            made_changes |= ImGui::ColorPicker4("##Picker", color, flags, &g->ColorPickerRef.x);

            ImGui::EndPopup();
        }

        if (set_current_color_edit_id)
            g->ColorEditCurrentID = 0;


        // Render the label text
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, 0.0f, name) && text_hovered)
            ImGui::SetTooltip(name);

        // Render the color button frame
        const ImU32 col = ImGui::ColorConvertFloat4ToU32({ color[0], color[1], color[2], 1.0f });
        const ImU32 border_col = ImGui::GetColorU32(frame_hovered ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);
        window->DrawList->AddRectFilled({ frame_bb.Min.x, frame_bb.Min.y }, { frame_bb.Max.x, frame_bb.Max.y}, col, g->Style.FrameRounding);
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, border_col, g->Style.FrameRounding);

        // Drag and Drop Target
        // if (BeginDragDropTarget())
        // {
        //     bool accepted_drag_drop = false;
        //     if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
        //     {
        //         memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
        //         made_changes = accepted_drag_drop = true;
        //     }
        //     if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
        //     {
        //         memcpy((float*)col, payload->Data, sizeof(float) * components);
        //         made_changes = accepted_drag_drop = true;
        //     }

        //     // Drag-drop payloads are always RGB
        //     if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
        //         ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
        //     EndDragDropTarget();
        // }

        // When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
        if (picker_popup_window && g->ActiveId != 0 && g->ActiveIdWindow == picker_popup_window)
            g->LastItemData.ID = g->ActiveId;

        if (made_changes && id != 0)
            ImGui::MarkItemEdited(id);

        ImGui::RenderNavHighlight(frame_bb, id);
        return made_changes;
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
        const ImVec2 p1 = { window->Pos.x, window->DC.CursorPos.y + 2.0f };
        const ImVec2 p2 = { p1.x + ImGui::GetContentRegionAvail().x + 2.0f * padding, p1.y + thickness };

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
