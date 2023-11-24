////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief YART's immediate GUI wrapper on Dear ImGui
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui.h"


#include "gui_internal.h"


#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define SEPARATOR_HANDLE_THICKNESS 2.0f
#endif // #ifndef DOXYGEN_EXCLUDE 


namespace yart
{
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
        style.FrameRounding = 4.0f;
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
        style.DisabledAlpha = 0.5f;


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
        GuiContext* ctx = GUI::GetGuiContext();

        ImGuiIO& io = ImGui::GetIO();
        static float icon_font_size = 16.0f;

        static const ImWchar icons_ranges[] = { ICON_MIN_CI, ICON_MAX_CI, 0 };
        ImFontConfig icons_config = { }; 
        icons_config.PixelSnapH = true; 
        icons_config.GlyphMinAdvanceX = icon_font_size;
        ImFont* icons_font = io.Fonts->AddFontFromFileTTF("..\\res\\fonts\\" FONT_ICON_FILE_NAME_CI, icon_font_size, &icons_config, icons_ranges);

        ctx->iconsFont = icons_font;
    }

    void GUI::SetNextItemFlags(GuiItemFlags flags)
    {
        GuiContext* ctx = GUI::GetGuiContext();
        ctx->nextItemFlags = flags;
    }

    const ImFont* GUI::GetIconsFont()
    {
        GuiContext* ctx = GUI::GetGuiContext();
        return ctx->iconsFont;
    }

    void GUI::PushIconsFont()
    {
        GuiContext* ctx = GUI::GetGuiContext();
        ImGui::PushFont(ctx->iconsFont);
    }

    bool GUI::IsMouseHoveringCircle(const ImVec2& pos, float radius)
    {
        ImGuiIO& io = ImGui::GetIO();

        float dx = io.MousePos.x - pos.x;
        float dy = io.MousePos.y - pos.y;

        return dx * dx + dy * dy <= radius * radius;
    }

    ImVec2 GUI::GetDisplaySizeDelta()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        static int last_frame = g->FrameCount;
        static ImVec2 last_display_size = g->IO.DisplaySize;

        static ImVec2 display_size_delta = { g->IO.DisplaySize.x - last_display_size.x, g->IO.DisplaySize.y - last_display_size.y};
        if (last_frame == g->FrameCount)
            return display_size_delta;

        display_size_delta = { g->IO.DisplaySize.x - last_display_size.x, g->IO.DisplaySize.y - last_display_size.y};
        last_display_size = g->IO.DisplaySize;
        last_frame = g->FrameCount;

        return display_size_delta;
    }

    bool GUI::BeginLayout(LayoutContext& layout)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (layout.size <= 0.0f) {
            float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, layout.min_size);
            float content_avail = layout.direction == GUI::LayoutDirection::HORIZONTAL ? ImGui::GetContentRegionAvail().x : ImGui::GetContentRegionAvail().y;
            layout.size = ImMax((content_avail - SEPARATOR_HANDLE_THICKNESS) * layout.default_size_ratio, min_size);
        }

        // The whole layout is captured into a group to act as a single item 
        ImGui::BeginGroup();

        // Start capturing the fist segment
        ImVec2 old_item_spacing = g->Style.ItemSpacing;
        g->Style.ItemSpacing = { 0.0f, 0.0f };

        ImVec2 region = layout.direction == GUI::LayoutDirection::HORIZONTAL ? ImVec2(layout.size, 0) : ImVec2(0, layout.size);
        bool ret = ImGui::BeginChild("LayoutSegment_First", region, false, ImGuiWindowFlags_NoBackground);
            
        g->Style.ItemSpacing = old_item_spacing;

        return ret;
    }

    bool GUI::LayoutSeparator(LayoutContext& layout)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        // Finalize capturing the previous segment
        ImVec2 old_item_spacing = g->Style.ItemSpacing;
        g->Style.ItemSpacing = { 0.0f, 0.0f };
        ImGui::EndChild();

        // Draw and handle the separator
        ImGuiWindow* window = g->CurrentWindow;

        if (layout.direction == GUI::LayoutDirection::HORIZONTAL) ImGui::SameLine();
        ImVec2 separator_size = layout.direction == GUI::LayoutDirection::HORIZONTAL ? ImVec2(SEPARATOR_HANDLE_THICKNESS, ImGui::GetContentRegionAvail().y) : ImVec2(window->Size.x, SEPARATOR_HANDLE_THICKNESS);
        ImGuiMouseCursor_ cursor = layout.direction == GUI::LayoutDirection::HORIZONTAL ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS;

        ImVec2 drag = LayoutSeparatorHandleEx({ window->DC.CursorPos.x, window->DC.CursorPos.y }, separator_size, cursor);
        layout.size += layout.direction == GUI::LayoutDirection::HORIZONTAL ? drag.x : drag.y;

        const ImVec2 display_size_delta = GUI::GetDisplaySizeDelta();
        if (layout.preserveSecondSectionSize && drag.x == 0 && drag.y == 0)
            layout.size += layout.direction == GUI::LayoutDirection::HORIZONTAL ? display_size_delta.x : display_size_delta.y;


        float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, layout.min_size);
        if (layout.size < min_size) 
            layout.size = min_size;

        float content = layout.direction == GUI::LayoutDirection::HORIZONTAL ? window->Size.x : window->Size.y;
        float max_size = content - min_size - SEPARATOR_HANDLE_THICKNESS;
        if (max_size > min_size && layout.size > max_size)
            layout.size = max_size;

        // Start capturing the next segment
        if (layout.direction == GUI::LayoutDirection::HORIZONTAL) ImGui::SameLine();
        bool ret = ImGui::BeginChild("LayoutSegment_Second", { 0.0f, 0.0f }, false, ImGuiWindowFlags_NoBackground);
        g->Style.ItemSpacing = old_item_spacing;

        return ret;
    }

    void GUI::EndLayout(LayoutContext& layout)
    {
        (void)layout; // Unused

        // Finalize capturing the previous segment
        ImGui::EndChild();

        ImGui::EndGroup();
    }

    void GUI::Label(const char* name, const char* text, ...)
    {
        va_list args;
        va_start(args, text);

        char text_buffer[32];
        ImFormatStringV(text_buffer, YART_ARRAYSIZE(text_buffer), text, args);

        va_end(args);

        return LabelEx(name, text_buffer);
    }

    bool GUI::SliderInt(const char *name, int *p_val, const char *format, int arrow_step)
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

    bool GUI::SliderFloat(const char* name, float* p_val, float min, float max, const char* format, float arrow_step)
    {
        return GUI::SliderEx(name, ImGuiDataType_Float, (void*)p_val, (float*)&min, (float*)&max, format, (void*)&arrow_step);
    }

    bool GUI::SliderVec3(const char* names[3], glm::vec3* p_vals, const char* format, float arrow_step)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();
        bool made_changes = false;

        GuiItemFlags original_flags = GetCurrentItemFlags();

        const float backup_spacing = g->Style.ItemSpacing.y;
        g->Style.ItemSpacing.y = 1.0f;

        static constexpr size_t items = 3;
        for (int i = 0; i < items; ++i) {
            GuiItemFlags flags = original_flags | (
                i == 0 ? GuiItemFlags_CornersRoundTop : i == items - 1 ? GuiItemFlags_CornersRoundBottom : GuiItemFlags_NoCornerRounding
            );

            SetNextItemFlags(flags);

            static_assert(sizeof(glm::vec3) == items * sizeof(float));
            made_changes |= GUI::SliderFloat(names[i], &((float*)p_vals)[i], format, arrow_step);
        }

        g->Style.ItemSpacing.y = backup_spacing;

        return made_changes;
    }

    bool GUI::CheckBox(const char* name, bool* val)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        // Retrieve current item flags and add the default frame border flag
        ctx->nextItemFlags |= GuiItemFlags_FrameBorder;
        GuiItemFlags flags = GetCurrentItemFlags(); 

        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;


        ImRect text_bb, frame_bb;
        const ImRect total_bb = CalculateItemSizes(text_bb, frame_bb, true);

        const ImGuiID id = window->GetID(name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id))
            return false;


        const bool total_hovered = g->ActiveId != id && (ImGui::ItemHoverable(total_bb, id) || g->NavId == id);
        const bool text_hovered = total_hovered && ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max);
        bool item_changed = false;

        // During keyboard navigation, the state can be changed with arrow keys
        if (g->NavId == id && ImGui::NavMoveRequestButNoResultYet()) {
            if (g->NavMoveDir == ImGuiDir_Left && *val == true) {
                *val = false;
                ImGui::NavMoveRequestCancel();
                item_changed = true;
            } else if (g->NavMoveDir == ImGuiDir_Right && *val == false) {
                *val = true;
                ImGui::NavMoveRequestCancel();
                item_changed = true;
            }
        }

        // Render label text
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, name) && text_hovered)
            ImGui::SetTooltip(name);

        // Handle frame inputs
        bool hovered, active;
        if (ImGui::ButtonBehavior(frame_bb, id, &hovered, &active, ImGuiButtonFlags_PressedOnClick)) {
            *val ^= 1; // Toggle the checkbox state
            item_changed = true;
        }

        // Render frame
        if (*val) {
            static constexpr ImDrawFlags draw_flags = ImDrawFlags_RoundCornersAll;
            DrawFrameHighlight(window->DrawList, frame_bb.Min, frame_bb.Max, 1.0f, hovered, active, draw_flags);
        } else {
            const ImU32 frame_col = GetFrameColor(hovered, active);
            DrawItemFrame(window->DrawList, frame_bb.Min, frame_bb.Max, frame_col);
        }

        ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
        return item_changed;
    }

    bool GUI::ComboHeader(const char* name, const char* items[], size_t items_size, int* selected_item)
    {
        // Retrieve current item flags
        GuiItemFlags flags = GetCurrentItemFlags(); 

        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;


        ImRect text_bb, frame_bb;
        const ImRect total_bb = CalculateItemSizes(text_bb, frame_bb);

        ImGuiID id = GUI::GetIDFormatted("##ComboHeader/%s", name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id)) 
            return false;


        // When navigating with keyboard/gamepad, cycle over all the items
        bool item_changed = false;
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


        // Render the label text
        const bool text_hovered = ImGui::IsItemHovered() && ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max);
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, name) && text_hovered)
            ImGui::SetTooltip(name);

        // Render the individual header items
        ImVec2 p_min = frame_bb.Min;
        ImVec2 p_max = frame_bb.Max;
        for (int i = 0; i < items_size; ++i) {
            p_min.x = frame_bb.Min.x + frame_bb.GetWidth() * (i / static_cast<float>(items_size));
            p_max.x = frame_bb.Min.x + frame_bb.GetWidth() * ((i + 1) / static_cast<float>(items_size));

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
            bool held = hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
            
            const ImDrawFlags flags = i == 0 ? ImDrawFlags_RoundCornersLeft : i == items_size - 1 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone;
            if (i == *selected_item) {
                GUI::DrawFrameHighlight(window->DrawList, p_min, p_max, 1.0f, hovered_or_nav, held, flags);
            } else {
                const ImU32 frame_col = GetFrameColor(hovered_or_nav, held);
                DrawItemFrame(window->DrawList, p_min, p_max, frame_col, flags);
            }

            // Render header text
            if (GUI::DrawText(window->DrawList, { p_min.x + 2.0f, p_min.y }, { p_max.x - 2.0f, p_max.y }, items[i], YART_GUI_TEXT_ALIGN_CENTER) && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip(items[i]);
        }


        // Render header items separators
        const ImU32 separator_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARK_GRAY, YART_GUI_ALPHA_OPAQUE });
        for (int i = 0; i < items_size - 1; ++i) {
            p_min.x = frame_bb.Min.x + frame_bb.GetWidth() * ((i + 1) / static_cast<float>(items_size)) - 1.0f;
            p_max.x = p_min.x + 2.0f;

            window->DrawList->AddRectFilled(p_min, p_max, separator_col);
        }

        ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
        return item_changed;
    }

    bool GUI::ColorEdit(const char* name, float color[3])
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        // Retrieve current item flags and add the default border flag
        ctx->nextItemFlags |= GuiItemFlags_FrameBorder; 
        GuiItemFlags flags = GetCurrentItemFlags(); 

        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;

        // Calculate the total bounding box of the widget
        ImRect text_bb, frame_bb;
        const ImRect total_bb = CalculateItemSizes(text_bb, frame_bb);

        const ImGuiID id = window->GetID(name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id))
            return false;


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
            
            static constexpr ImGuiColorEditFlags flags = has_alpha ? ImGuiColorEditFlags_None : ImGuiColorEditFlags_NoAlpha;
            made_changes |= ImGui::ColorPicker4("##Picker", color, flags, &g->ColorPickerRef.x);

            ImGui::EndPopup();
        }

        if (set_current_color_edit_id)
            g->ColorEditCurrentID = 0;


        // Render the label text
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, name) && text_hovered)
            ImGui::SetTooltip(name);

        // Render the color button frame
        const ImU32 frame_col = ImGui::ColorConvertFloat4ToU32({ color[0], color[1], color[2], 1.0f });
        DrawItemFrame(window->DrawList, frame_bb.Min, frame_bb.Max, frame_col);

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

        ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
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

    bool GUI::BeginCollapsableSection(const char* name)
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

    bool GUI::BeginTabBar(const char* item_name)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        // The whole tab bar is recorded into a group to act as a single item
        ImGui::BeginGroup();

        ImVec2 backup_spacing = g->Style.ItemSpacing;
        g->Style.ItemSpacing = { 0.0f, 0.0f };

        ImGui::ItemSize({ g->Style.ChildRounding , 0.0f });
        ImGui::SameLine();

        ImVec4 backup_tab_active_color = g->Style.Colors[ImGuiCol_TabActive];
        ImVec2 backup_frame_padding = g->Style.FramePadding;
        g->Style.FramePadding = { 16.0f, backup_frame_padding.y };
        g->Style.Colors[ImGuiCol_TabActive] = { YART_GUI_COLOR_BLACK, YART_GUI_ALPHA_TRANSPARENT };
        ImGui::BeginTabBar("##TabBar", ImGuiTabBarFlags_AutoSelectNewTabs);
        g->Style.Colors[ImGuiCol_TabActive] = backup_tab_active_color;

        ImVec2 backup_inner_spacing = g->Style.ItemInnerSpacing;
        g->Style.ItemInnerSpacing = { 0.0f, 0.0f };
        g->Style.ItemSpacing = { 0.0f, -1.0f };
        bool open = ImGui::BeginTabItem(item_name, nullptr, ImGuiTabItemFlags_NoPushId);
        g->Style.ItemSpacing = backup_spacing;
        g->Style.ItemInnerSpacing = backup_inner_spacing;
        g->Style.FramePadding = backup_frame_padding;

        return open;
    }

    void GUI::EndTabBar() 
    {
        ImGui::EndTabBar();
        ImGui::EndGroup();
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

    void GUI::BeginMultiItem(uint8_t count)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        YART_ASSERT(!ctx->startMultiItems && "A multi-items group has already been started\n");
        ctx->startMultiItems = true;

        YART_ASSERT(ctx->multiItemsCount <= 0 && "Trying to begin a multi-items group within another group\n");
        YART_ASSERT(count > 1 && "Invalid item count passed to GUI::BeginMultiItem\n");
        ctx->multiItemsCount = count;

        // Push a small y spacing value for tighter packing of inner items
        const float x_spacing = g->Style.ItemSpacing.x;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { x_spacing, 1.0f });

        ImGui::BeginGroup();
    }

    void GUI::EndMultiItem()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        YART_ASSERT(ctx->multiItemsCount == 0 && "Premature or overdue multi-item group ending\n");

        // Pop the vertical item spacing pushed in `GUI::BeginMultiItem`
        ImGui::PopStyleVar();

        // Fix the spacing after the last item
        const float y_spacing = g->Style.ItemSpacing.y;
        g->CurrentWindow->DC.CursorPos.y += y_spacing - 1.0f;

        ImGui::EndGroup();
    }

} // namespace yart
