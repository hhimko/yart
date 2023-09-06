////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects and methods definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui_internal.h"


#include "yart/utils/glm_utils.h"
#include "yart/utils/yart_utils.h"
#include "yart/platform/input.h"
#include "font/IconsCodicons.h"


/// @brief Helper macro useful for opening `break`-able context scopes
#define WITH(handle) for(auto* _h = handle; _h != nullptr; _h = nullptr)

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define TEMP_BUFFER_SIZE 5

    #define HOVER_RECT_PADDING 2.0f
    #define SEPARATOR_HANDLE_THICKNESS 2.0f

    #define AXIS_POSITIVE_X 0
    #define AXIS_POSITIVE_Y 1
    #define AXIS_POSITIVE_Z 2
    #define AXIS_NEGATIVE_X 3
    #define AXIS_NEGATIVE_Y 4
    #define AXIS_NEGATIVE_Z 5
#endif // #ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    bool GUI::BeginLayout(LayoutState& layout)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        if (layout.size <= 0.0f) {
            float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, layout.min_size);
            float content_avail = layout.direction == GUI::LayoutDir::HORIZONTAL ? ImGui::GetContentRegionAvail().x : ImGui::GetContentRegionAvail().y;
            layout.size = ImMax((content_avail - SEPARATOR_HANDLE_THICKNESS) * layout.default_size_ratio, min_size);
        }

        // The whole layout is captured into a group to act as a single item 
        ImGui::BeginGroup();

        // Start capturing the fist segment
        ImVec2 old_item_spacing = g->Style.ItemSpacing;
        g->Style.ItemSpacing = { 0.0f, 0.0f };

        ImVec2 region = layout.direction == GUI::LayoutDir::HORIZONTAL ? ImVec2(layout.size, 0) : ImVec2(0, layout.size);
        bool ret = ImGui::BeginChild("LayoutSegment_First", region, false, layout.window_flags);
            
        g->Style.ItemSpacing = old_item_spacing;

        return ret;
    }

    /// @brief Draw the separator handle and handle its inputs
    /// @param pos Start position of the handle's bounding box
    /// @param size Size of the handle's bounding box
    /// @param cursor Cursor to be used while the handle is hovered over
    /// @return Mouse drag amount since last frame 
    ImVec2 SeparatorHandle(ImVec2 pos, ImVec2 size, ImGuiMouseCursor_ cursor) 
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        const ImRect bb(pos, { pos.x + size.x, pos.y + size.y });
        ImGui::ItemSize(size);

        const ImGuiID id = ImGui::GetID("SeparatorHandle");
        ImGui::ItemAdd(bb, id);

        bool hovered, held;
        ImGui::ButtonBehavior(bb, id, &hovered, &held);

        const ImVec4 col = held ? g->Style.Colors[ImGuiCol_ResizeGripActive] : (hovered ? g->Style.Colors[ImGuiCol_ResizeGripHovered] : g->Style.Colors[ImGuiCol_ResizeGrip]);
        ImGui::GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, ImGui::ColorConvertFloat4ToU32(col));

        if (hovered || held)
            ImGui::SetMouseCursor(cursor);

        if (held) {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
            ImGui::ResetMouseDragDelta();
            return drag;
        }

        return {0.0f, 0.0f};
    }

    bool GUI::LayoutSeparator(LayoutState& layout)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        // Finalize capturing the previous segment
        ImVec2 old_item_spacing = g->Style.ItemSpacing;
        g->Style.ItemSpacing = { 0.0f, 0.0f };
        ImGui::EndChild();

        // Draw and handle the separator
        ImGuiWindow* window = g->CurrentWindow;

        if (layout.direction == GUI::LayoutDir::HORIZONTAL) ImGui::SameLine();
        ImVec2 separator_size = layout.direction == GUI::LayoutDir::HORIZONTAL ? ImVec2(SEPARATOR_HANDLE_THICKNESS, ImGui::GetContentRegionAvail().y) : ImVec2(window->Size.x, SEPARATOR_HANDLE_THICKNESS);
        ImGuiMouseCursor_ cursor = layout.direction == GUI::LayoutDir::HORIZONTAL ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS;

        ImVec2 drag = SeparatorHandle({ window->DC.CursorPos.x, window->DC.CursorPos.y }, separator_size, cursor);
        layout.size += layout.direction == GUI::LayoutDir::HORIZONTAL ? drag.x : drag.y;

        GuiContext* ctx = GUI::GetCurrentContext();
        if (layout.preserveSecondSectionSize && drag.x == 0 && drag.y == 0)
            layout.size += layout.direction == GUI::LayoutDir::HORIZONTAL ? ctx->displaySizeDelta.x : ctx->displaySizeDelta.y;


        float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, layout.min_size);
        if (layout.size < min_size) 
            layout.size = min_size;

        float content = layout.direction == GUI::LayoutDir::HORIZONTAL ? window->Size.x : window->Size.y;
        float max_size = content - min_size - SEPARATOR_HANDLE_THICKNESS;
        if (max_size > min_size && layout.size > max_size)
            layout.size = max_size;

        // Start capturing the next segment
        if (layout.direction == GUI::LayoutDir::HORIZONTAL) ImGui::SameLine();
        bool ret = ImGui::BeginChild("LayoutSegment_Second", { 0.0f, 0.0f }, false, layout.window_flags);
        g->Style.ItemSpacing = old_item_spacing;

        return ret;
    }

    void GUI::EndLayout(LayoutState& layout)
    {
        (void)layout; // Unused

        // Finalize capturing the previous segment
        ImGui::EndChild();

        ImGui::EndGroup();
    }

    bool GUI::BeginCustomTabBar(const char* item_name)
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

    void GUI::EndCustomTabBar() 
    {
        ImGui::EndTabBar();
        ImGui::EndGroup();
    }

    bool GUI::IsMouseHoveringCircle(const ImVec2 &pos, float radius)
    {
        ImGuiIO& io = ImGui::GetIO();
        float dx = io.MousePos.x - pos.x;
        float dy = io.MousePos.y - pos.y;
        return dx * dx + dy * dy <= radius * radius;
    }

    void GUI::RenderMainMenuBar()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        ImVec4 backup_color = g->Style.Colors[ImGuiCol_MenuBarBg];
        g->Style.Colors[ImGuiCol_MenuBarBg] = { YART_GUI_COLOR_BLACK, YART_GUI_ALPHA_OPAQUE };
        ImGui::BeginMainMenuBar();
        g->Style.Colors[ImGuiCol_MenuBarBg] = backup_color;

        // Render menu items
        if (ImGui::BeginMenu("File")) {
			ImGui::MenuItem("New");
			ImGui::MenuItem("Create");
            ImGui::EndMenu();
		}

        // Store the size of the main menu bar for content area computation
        GuiContext* ctx = GUI::GetCurrentContext();
        ctx->mainMenuBarHeight = ImGui::GetWindowSize().y;

        ImGui::EndMainMenuBar();
    }

    void GUI::RenderMainContentFrame()
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GUI::GetCurrentContext();

        ImVec2 display_size = ImGui::GetIO().DisplaySize;
        float menu_bar_height = ctx->mainMenuBarHeight;

        ctx->renderViewportAreaPos = { 0.0f, menu_bar_height };
        ctx->renderViewportAreaHeight = display_size.y - menu_bar_height;
        ImGui::SetNextWindowPos(ctx->renderViewportAreaPos, ImGuiCond_None, { 0.0f, 0.0f });
        ImGui::SetNextWindowSize({ display_size.x, display_size.y - menu_bar_height }, ImGuiCond_None);

        static constexpr ImGuiWindowFlags flags = (
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground
        );
        
        ImVec2 backup_padding = g->Style.WindowPadding;
        g->Style.WindowPadding = {0,0};
        ImGui::Begin("YART_MainContentFrame", nullptr, flags);
        g->Style.WindowPadding = backup_padding;

        // Render the main frame layout
        static LayoutState horizontal_layout = { };
        horizontal_layout.direction = GUI::LayoutDir::HORIZONTAL;
        horizontal_layout.preserveSecondSectionSize = true;
        horizontal_layout.default_size_ratio = 3.0f / 4.0f;
        horizontal_layout.window_flags = ImGuiWindowFlags_NoBackground;

        GUI::BeginLayout(horizontal_layout);

        // Store the viewport window ID 
        ctx->renderViewportWindowID = g->CurrentWindow->ID;

        GUI::LayoutSeparator(horizontal_layout);

        GUI::RenderContextWindow();

        GUI::EndLayout(horizontal_layout);
        ImGui::End();

        // Store the viewport area size for renderer 
        ctx->renderViewportAreaWidth = horizontal_layout.size;
    }

    void GUI::RenderContextWindow()
    {
        GuiContext* ctx = GUI::GetCurrentContext();
        ImGuiContext* g = ImGui::GetCurrentContext();

        // Render the scene+context menu layout
        static LayoutState vertical_layout = { };
        vertical_layout.direction = GUI::LayoutDir::VERTICAL;
        vertical_layout.default_size_ratio = 1.0f / 3.0f;
        vertical_layout.window_flags = ImGuiWindowFlags_NoBackground;
        vertical_layout.min_size = 50.0f;

        // Scene + Object inspectors section
        GUI::BeginLayout(vertical_layout);

        if (GUI::BeginCustomTabBar("Scene")) {
            ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

            ImGui::Text("Hello from the Scene tab!");

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Object")) {
            ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);

            ImGui::Text("Hello from the Object tab!");

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        GUI::EndCustomTabBar();

        // Master inspector section
        GUI::LayoutSeparator(vertical_layout);

        GUI::RenderInspectorNavBar();
        InspectorWindow* active_item = ctx->activeInspectorWindow;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
        ImGui::SameLine();
        ImGui::PopStyleVar();

        const char* name = "Inspector";
        if (active_item != nullptr)
            ImFormatStringToTempBuffer(&name, nullptr, "%s###InspectorTabItem", active_item->name);

        bool open = GUI::BeginCustomTabBar(name);
        {
            static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysUseWindowPadding;
            ImGui::BeginChild("##Content", { 0.0f, 0.0f }, false, flags);

            // Draw background without left edge rounding
            ImGuiWindow* window = g->CurrentWindow;
            float child_rounding = g->Style.ChildRounding;
            ImVec2 p_min = { window->Pos.x, window->Pos.y };
            ImVec2 p_max = { p_min.x + child_rounding, p_min.y + window->Size.y };

            const ImVec4 backup_clip_rect = g->CurrentWindow->DrawList->_ClipRectStack.back();
            g->CurrentWindow->DrawList->PopClipRect();
            static ImU32 bg_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_ChildBg]);
            g->CurrentWindow->DrawList->AddRectFilled(p_min, p_max, bg_col);
            g->CurrentWindow->DrawList->PushClipRect({ backup_clip_rect.x, backup_clip_rect.y }, { backup_clip_rect.z, backup_clip_rect.w });

            // Render the currently active inspector item
            if (active_item != nullptr)
                ctx->madeChanges |= active_item->callback();

            ImGui::EndChild();
            if (open)
                ImGui::EndTabItem();
        }
        GUI::EndCustomTabBar();

        GUI::EndLayout(vertical_layout);
    }

    void GUI::RenderInspectorNavBar()
    {
        GuiContext* ctx = GUI::GetCurrentContext();
        ImGuiContext* g = ImGui::GetCurrentContext();

        const float window_y_offset = ImGui::GetFrameHeight() - 1.0f;
        static constexpr float icon_button_outer_padding = 3.0f;
        static constexpr float icon_button_inner_padding = 4.0f;
        const float size = ctx->iconsFont->FontSize;
       
        const float window_width = size + 2.0f * icon_button_outer_padding + 2.0f * icon_button_inner_padding;
        static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::BeginChild("##SideNavBar", { window_width, 0.0f }, false, flags);

        // Draw background without right edge rounding
        ImGuiWindow* window = g->CurrentWindow;
        const float child_rounding = g->Style.ChildRounding;
        ImVec2 p_min = { window->Pos.x, window->Pos.y + window_y_offset };
        ImVec2 p_max = { p_min.x + window->Size.x + child_rounding, p_min.y + window->Size.y };

        static const ImU32 bg_col = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_DARKEST_GRAY, YART_GUI_ALPHA_OPAQUE });
        window->DrawList->AddRectFilled(p_min, p_max, bg_col, child_rounding);

        // Render menu item icons
        static constexpr float item_spacing = 4.0f; 
        window->DC.CursorPos.y += window_y_offset;

        for (auto&& item : ctx->inspectorWindows) {
            window->DC.CursorPos.x += icon_button_outer_padding;
            window->DC.CursorPos.y += item_spacing;

            p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y + 1.0f };
            p_max = { p_min.x + 2.0f * icon_button_inner_padding + icon_button_outer_padding + size, p_min.y + 2.0f * icon_button_inner_padding + size };

            const ImGuiID id = ImGui::GetID(item.name);
            ImGui::ItemAdd({ p_min, p_max }, id);

            bool hovered, held;
            bool clicked = ImGui::ButtonBehavior({ p_min, p_max }, id, &hovered, &held);

            bool active = &item == ctx->activeInspectorWindow;
            if (!active && clicked) {
                ctx->activeInspectorWindow = &item;
                active = true;
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip(item.name);

            // Render background
            const ImU32 normal_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_Tab]);
            const ImU32 active_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_TabActive]);
            const ImU32 hovered_col = ImGui::ColorConvertFloat4ToU32(g->Style.Colors[ImGuiCol_TabHovered]);

            const ImU32 color = hovered ? hovered_col : active ? active_col : normal_col;
            window->DrawList->AddRectFilled(p_min, p_max, color, child_rounding, ImDrawFlags_RoundCornersLeft);

            // Render the icon
            window->DC.CursorPos.x += icon_button_inner_padding;
            window->DC.CursorPos.y += icon_button_inner_padding;

            ImVec4 backup_text_color = g->Style.Colors[ImGuiCol_Text];
            g->Style.Colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(item.color);
            GUI::PushIconsFont();
            ImGui::Text(item.icon);
            ImGui::PopFont();
            g->Style.Colors[ImGuiCol_Text] = backup_text_color;
        }
        
        ImGui::EndChild();
    }

    void GUI::RenderWindow(const char* name, imgui_callback_t callback)
    {
        // Local alpha multiplier for the whole window
        float window_alpha = YART_GUI_ALPHA_OPAQUE;
        
        // Try querying the window state prior to ImGui::Begin() to apply custom style
        ImGuiWindow* imgui_window = ImGui::FindWindowByName(name);
        WITH(imgui_window) {
            auto* active_window = ImGui::GetCurrentContext()->NavWindow;
            if (imgui_window == active_window)
                break;

            auto size = imgui_window->Size;
            auto pos = imgui_window->Pos;

            if (ImGui::IsMouseHoveringRect(
                {pos.x - HOVER_RECT_PADDING, pos.y - HOVER_RECT_PADDING}, 
                {pos.x + size.x + HOVER_RECT_PADDING, pos.y + size.y + HOVER_RECT_PADDING}, false)
            ) {
                window_alpha = YART_GUI_ALPHA_HIGH;
                break;
            }

            window_alpha = YART_GUI_ALPHA_MEDIUM;
        }


        // Open the window
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Window title bottom border on
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 6.0f, 4.0f }); // Window title vertical padding +2 
        
        ImGui::SetNextWindowBgAlpha(window_alpha);
        ImGui::Begin(name);

        ImGui::PopStyleVar(2);

        // Draw window contents 
        ImGuiStyle* style = &ImGui::GetStyle();
        float restore_alpha = style->Alpha;
        style->Alpha = window_alpha;

        // Call the window's render callback
        callback();

        // Restore global alpha value
        style->Alpha = restore_alpha;

        ImGui::End();
    }

    /// @brief Positive view axis rendering helper function
    /// @param draw_list ImGui window draw list
    /// @param win_pos Center window position of the view axes context window
    /// @param axis Normalized axis direction
    /// @param color Color of the axis
    /// @param length Maximum axis length in pixels 
    /// @param hovered Whether the axis handle is currently hovered
    void DrawPositiveViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
    {
        static constexpr float axis_thickness = 2.5f;
        static constexpr float handle_radius = 6.5f;

        float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
        ImU32 col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });

        glm::vec3 handle_pos = win_pos + axis * length;
        draw_list->AddLine({ win_pos.x, win_pos.y }, { handle_pos.x, handle_pos.y }, col, axis_thickness);
        draw_list->AddCircleFilled({ handle_pos.x, handle_pos.y }, handle_radius, col);
    }

    /// @brief Negative view axis rendering helper function
    /// @param draw_list ImGui window draw list
    /// @param win_pos Center window position of the view axes context window
    /// @param axis Normalized axis direction
    /// @param color Color of the axis
    /// @param length Maximum axis length in pixels 
    /// @param hovered Whether the axis handle is currently hovered
    void DrawNegativeViewAxisH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis, const glm::vec3& color, float length, bool hovered)
    {
        static constexpr float handle_thickness = 2.0f;
        static constexpr float handle_radius = 7.0f;

        float col_mul = hovered ? 1.25f : axis.z / 5.0f + 0.7f;
        ImU32 outer_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul, color.y * col_mul, color.z * col_mul, 1.0f });
        ImU32 inner_col = ImGui::ColorConvertFloat4ToU32({ color.x * col_mul * 0.2f, color.y * col_mul * 0.2f, color.z * col_mul * 0.2f, 1.0f });

        glm::vec3 handle_pos = win_pos + axis * length;
        draw_list->AddCircleFilled({ handle_pos.x, handle_pos.y }, handle_radius - 0.5f, inner_col, 0);
        draw_list->AddCircle({ handle_pos.x, handle_pos.y }, handle_radius, outer_col, 0, handle_thickness);
    }

    /// @brief View axes rendering helper function
    /// @param draw_list ImGui window draw list
    /// @param win_pos Center window position of the view axes context window
    /// @param axis0 First axis in the drawing order (back)
    /// @param axis1 Second axis in the drawing order (middle)
    /// @param axis2 Third axis in the drawing order (front)
    /// @param order The order in which the X, Y and Z axes will be drawn
    /// @param length Maximum axis length in pixels 
    /// @param active Whether the view axes context window is currently active
    /// @param swap Wether the negative axes should be rendered prior to the positive ones
    /// @param clicked_axis Output variable set to a base axis clicked by the user
    /// @return Whether the user has clicked on an axis and the `clicked_axis` output variable has been set 
    bool DrawViewAxesH(ImDrawList* draw_list, const glm::vec3& win_pos, const glm::vec3& axis0, const glm::vec3& axis1, const glm::vec3& axis2, const int* order, float length, bool active, bool swap, glm::vec3& clicked_axis)
    {
        static constexpr glm::vec3 axes_colors_LUT[] = {
            { 244.0f / 255.0f, 36.0f  / 255.0f, 84.0f  / 255.0f }, // + X-axis 
            { 84.0f  / 255.0f, 244.0f / 255.0f, 36.0f  / 255.0f }, // + Y-axis 
            { 36.0f  / 255.0f, 84.0f  / 255.0f, 244.0f / 255.0f }, // + Z-axis 
            { 247.0f / 255.0f, 99.0f  / 255.0f, 133.0f / 255.0f }, // - X-axis 
            { 133.0f / 255.0f, 247.0f / 255.0f, 99.0f  / 255.0f }, // - Y-axis 
            { 99.0f  / 255.0f, 133.0f / 255.0f, 247.0f / 255.0f }, // - Z-axis 
        };


        // No need to render negative view axes and check for input if the window is not active
        if (!active) {
            DrawPositiveViewAxisH(draw_list, win_pos, axis0, axes_colors_LUT[order[0]], length, false);
            DrawPositiveViewAxisH(draw_list, win_pos, axis1, axes_colors_LUT[order[1]], length, false);
            DrawPositiveViewAxisH(draw_list, win_pos, axis2, axes_colors_LUT[order[2]], length, false);

            return false;
        }

        glm::vec3* axes;
        if (!swap){
            glm::vec3 p_axes[6] = { axis0, axis1, axis2, -axis2, -axis1, -axis0 };
            axes = p_axes;
        } else {
            glm::vec3 n_axes[6] = { -axis2, -axis1, -axis0, axis0, axis1, axis2 };
            axes = n_axes;
        }

        // Mouse hover tests have to be done first, in reverse order to rendering
        int hovered_axis_index = -1;
        for (int i=5; i >= 0; --i) {
            if (GUI::IsMouseHoveringCircle({ win_pos.x + axes[i].x * length, win_pos.y + axes[i].y * length }, 6.5f)) {
                hovered_axis_index = i;
                break;
            }
        }

        // Render the individual axes
        for (int i=0; i < 6; ++i) {
            if (swap && i < 3 || !swap && i >= 3)
                DrawNegativeViewAxisH(draw_list, win_pos, axes[i], axes_colors_LUT[order[(5 - i) % 3] + 3], length, i == hovered_axis_index);
            else
                DrawPositiveViewAxisH(draw_list, win_pos, axes[i], axes_colors_LUT[order[i % 3]], length, i == hovered_axis_index);
        }

        // Return the clicked axis
        if (hovered_axis_index >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            int hovered_axis;
            if (swap && hovered_axis_index < 3 || !swap && hovered_axis_index >= 3)
                hovered_axis = order[(5 - hovered_axis_index) % 3] + 3;
            else 
                hovered_axis = order[hovered_axis_index % 3];

            switch (hovered_axis) {
                case AXIS_POSITIVE_X:
                    clicked_axis = {1, 0, 0};
                    return true;
                case AXIS_POSITIVE_Y:
                    clicked_axis = {0, 1, 0};
                    return true;
                case AXIS_POSITIVE_Z:
                    clicked_axis = {0, 0, 1};
                    return true;
                case AXIS_NEGATIVE_X:
                    clicked_axis = {-1, 0, 0};
                    return true;
                case AXIS_NEGATIVE_Y:
                    clicked_axis = {0, -1, 0};
                    return true;
                case AXIS_NEGATIVE_Z:
                    clicked_axis = {0, 0, -1};
                    return true;
            }
        }

        return false;
    }

    bool GUI::RenderViewAxesWindowEx(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis)
    {
        static constexpr ImVec2 window_size = { 75.0f, 75.0f }; // Expected to be a square
        static constexpr ImVec2 window_margin = { 25.0f, 15.0f };

        static const ImGuiWindowFlags window_flags = (
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground
        );

        // Set a constant window size and position
        GuiContext* ctx = GUI::GetCurrentContext();
        ImVec2 viewport_pos = ctx->renderViewportAreaPos;
        ImVec2 window_center = {
            viewport_pos.x + ctx->renderViewportAreaWidth - window_size.x / 2 - window_margin.x, 
            viewport_pos.y + window_size.y / 2 + window_margin.y 
        };

        ImGui::SetNextWindowPos(window_center, ImGuiCond_None, { 0.5f, 0.5f });
        ImGui::SetNextWindowSize(window_size);

        // Open the window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::Begin("View Axes Context", nullptr, window_flags);
        ImGui::PopStyleVar();

        // Draw window contents
        auto* imgui_window = ImGui::GetCurrentWindow();
        ImDrawList* draw_list = imgui_window->DrawList;
        static constexpr float circle_radius = window_size.x / 2;
        
        bool hovered = false;
        if (GUI::IsMouseOverRenderViewport())
            hovered = IsMouseHoveringCircle(window_center, circle_radius);

        // Background
        static const ImU32 background_color = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_LIGHTER_GRAY, YART_GUI_ALPHA_MEDIUM });

        if (hovered)
            draw_list->AddCircleFilled(window_center, circle_radius, background_color);

        // Axes
        glm::vec3 center(window_center.x, window_center.y, 0);
        static constexpr float axis_length = circle_radius - 10.0f;

        // Z-sort the axes in draw order
        bool clicked;
        bool swap = (y_axis.z > -0.8f && x_axis.z + y_axis.z + z_axis.z >= -0.5f); // Wether the negative axes should be rendered first
        if (x_axis.z > y_axis.z) {
            if (y_axis.z > z_axis.z) {
                static constexpr int order[3] = { AXIS_POSITIVE_Z, AXIS_POSITIVE_Y, AXIS_POSITIVE_X };
                clicked = DrawViewAxesH(draw_list, center, z_axis, y_axis, x_axis, order, axis_length, hovered, swap, clicked_axis);
            } else {
                if (x_axis.z > z_axis.z) {
                    static constexpr int order[3] = { AXIS_POSITIVE_Y, AXIS_POSITIVE_Z, AXIS_POSITIVE_X };
                    clicked = DrawViewAxesH(draw_list, center, y_axis, z_axis, x_axis, order, axis_length, hovered, swap, clicked_axis);
                } else {
                    static constexpr int order[3] = { AXIS_POSITIVE_Y, AXIS_POSITIVE_X, AXIS_POSITIVE_Z };
                    clicked = DrawViewAxesH(draw_list, center, y_axis, x_axis, z_axis, order, axis_length, hovered, swap, clicked_axis);
                }
            }
        } else {
            if (x_axis.z > z_axis.z) {
                static constexpr int order[3] = { AXIS_POSITIVE_Z, AXIS_POSITIVE_X, AXIS_POSITIVE_Y };
                clicked = DrawViewAxesH(draw_list, center, z_axis, x_axis, y_axis, order, axis_length, hovered, swap, clicked_axis);
            } else {
                if (y_axis.z > z_axis.z) {
                    static constexpr int order[3] = { AXIS_POSITIVE_X, AXIS_POSITIVE_Z, AXIS_POSITIVE_Y };
                    clicked = DrawViewAxesH(draw_list, center, x_axis, z_axis, y_axis, order, axis_length, hovered, swap, clicked_axis);
                } else {
                    static constexpr int order[3] = { AXIS_POSITIVE_X, AXIS_POSITIVE_Y, AXIS_POSITIVE_Z };
                    clicked = DrawViewAxesH(draw_list, center, x_axis, y_axis, z_axis, order, axis_length, hovered, swap, clicked_axis);
                }
            }
        }

        ImGui::End();
        return clicked;
    }

    void GUI::DrawHighlightRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, float t, bool hovered, bool active, float rounding, ImDrawFlags flags)
    {
        if (p_min.x >= p_max.x || p_min.y >= p_max.y)
            return;


        // Compute the color values for each side
        glm::vec3 colors[2] = {
            // Start color
            active ? glm::vec3(YART_GUI_COLOR_LIGHT_PRIMARY) :
            hovered ? glm::vec3(YART_GUI_COLOR_PRIMARY) :
            glm::vec3(YART_GUI_COLOR_DARK_PRIMARY),

            // End color
            active ? glm::vec3(YART_GUI_COLOR_LIGHT_SECONDARY) :
            hovered ? glm::vec3(YART_GUI_COLOR_SECONDARY) :
            glm::vec3(YART_GUI_COLOR_DARK_SECONDARY)
        };

        // Interpolate the end color by the t value
        if (t != 1.0f)
            colors[1] = yart::utils::LinearGradient(colors, 2, t);

        const ImU32 start_col = ImGui::ColorConvertFloat4ToU32({ colors[0].x, colors[0].y, colors[0].z, YART_GUI_ALPHA_OPAQUE });
        const ImU32 end_col = ImGui::ColorConvertFloat4ToU32({ colors[1].x, colors[1].y, colors[1].z, YART_GUI_ALPHA_OPAQUE });


        if (rounding >= 0.5f && !((flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone)) {
            if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
                flags |= ImDrawFlags_RoundCornersAll;

            if (flags & ImDrawFlags_RoundCornersLeft) {
                p_min.x += rounding;

                const ImVec2 p1 = { p_min.x - rounding, p_min.y };
                const ImVec2 p2 = { p_min.x + 1.0f, p_max.y };
                draw_list->AddRectFilled(p1, p2, start_col, rounding, flags & ImDrawFlags_RoundCornersLeft);
            }

            if (flags & ImDrawFlags_RoundCornersRight) {
                p_max.x -= rounding;

                const ImVec2 p1 = { p_max.x - 1.0f, p_min.y };
                const ImVec2 p2 = { p_max.x + rounding, p_max.y };
                draw_list->AddRectFilled(p1, p2, end_col, rounding, flags & ImDrawFlags_RoundCornersRight);
            }
        }

        draw_list->AddRectFilledMultiColor(p_min, p_max, start_col, end_col, end_col, start_col);
    }

    /// @brief Internal generic function for rendering a YART GUI style slider widget
    /// @param name Label text displayed next to the slider
    /// @param data_type Type of the slider variable. Should always match the generic `T` param
    /// @param p_val Pointer to the controlled value
    /// @param p_min Pointer to the minimal value. Can be `NULL`
    /// @param p_max Pointer to the maximal value. Can be `NULL`
    /// @param format Format in which to display the value
    /// @param arrow_step The step of change in value when using the frame arrows
    /// @tparam T Type of the slider variable. Should always match the `data_type` param
    /// @tparam SignedT Signed version of the generic `T` param    
    /// @return Whether the input value has changed  
    template<typename T, typename SignedT>
    bool SliderExT(const char* name, ImGuiDataType data_type, T* p_val, const T* p_min, const T* p_max, const char *format, T arrow_step) 
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
        static constexpr float arrow_frame_width = 14.0f;
        static const float text_width_percent = 0.4f; // TODO: This value should be calculated based on the current indent at some point

        const ImRect text_bb = { total_bb.Min, { total_bb.Min.x + IM_ROUND(total_bb.GetWidth() * text_width_percent) - item_spacing, total_bb.Max.y }};
        const ImRect frame_total_bb = { { text_bb.Max.x + item_spacing, total_bb.Min.y }, total_bb.Max };
        const ImRect frame_drag_bb = { { frame_total_bb.Min.x + arrow_frame_width, frame_total_bb.Min.y }, { frame_total_bb.Max.x - arrow_frame_width, frame_total_bb.Max.y } };


        const bool total_hovered = g->ActiveId != id && (ImGui::ItemHoverable(total_bb, id) || g->NavId == id);
        const bool text_hovered = total_hovered && ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max);
        const bool frame_drag_hovered = total_hovered && ImGui::IsMouseHoveringRect(frame_drag_bb.Min, frame_drag_bb.Max);

        bool temp_input_is_active = ImGui::TempInputIsActive(id);
        if (!temp_input_is_active) {
            // Tabbing / Ctrl-clicking / double-clicking turns the widget into an InputText
            const bool input_requested_by_tabbing = (g->LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = frame_drag_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g->NavActivateId == id);

            if (make_active) {
                if (clicked)
                    ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);

                const bool double_clicked = (frame_drag_hovered && g->IO.MouseClickedCount[0] == 2 && ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
                if (input_requested_by_tabbing || (clicked && g->IO.KeyCtrl) || double_clicked || (g->NavActivateId == id && (g->NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

                if (!temp_input_is_active) {
                    ImGui::SetActiveID(id, window);
                    ImGui::SetFocusID(id, window);
                    ImGui::FocusWindow(window);
                    g->ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
                }
            }
        }

        // Render the label text
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, 0.0f, name) && text_hovered)
            ImGui::SetTooltip(name);

        // During temp input, skip drawing the custom frame 
        if (temp_input_is_active)
            return ImGui::TempInputScalar(frame_total_bb, id, name, data_type, p_val, format, p_min, p_max);

        
        static constexpr float frame_separator_thickness = 1.0f;
        const float frame_rounding = g->Style.FrameRounding;

        const ImRect left_arrow_bb = { frame_total_bb.Min, { frame_drag_bb.Min.x - frame_separator_thickness, frame_total_bb.Max.y } };
        const ImRect right_arrow_bb = { { frame_drag_bb.Max.x + frame_separator_thickness, frame_total_bb.Min.y }, frame_total_bb.Max };

        const bool left_arrow_hovered = total_hovered && !frame_drag_hovered && ImGui::IsMouseHoveringRect(left_arrow_bb.Min, left_arrow_bb.Max);
        const bool left_arrow_active = g->ActiveId != id && left_arrow_hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);
        const bool right_arrow_hovered = total_hovered && !frame_drag_hovered && ImGui::IsMouseHoveringRect(right_arrow_bb.Min, right_arrow_bb.Max);
        const bool right_arrow_active = g->ActiveId != id && right_arrow_hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left);

        // Handle frame arrows 
        bool made_changes = false;
        if (left_arrow_active || right_arrow_active) {
            const bool left_arrow_clicked = left_arrow_active && ImGui::IsMouseClicked(ImGuiMouseButton_Left, true);
            const bool right_arrow_clicked = right_arrow_active && ImGui::IsMouseClicked(ImGuiMouseButton_Left, true);

            T step = left_arrow_clicked ? -static_cast<SignedT>(arrow_step) : right_arrow_clicked ? arrow_step : static_cast<T>(0.0);
            if (step != static_cast<T>(0.0)) {
                ImGui::ClearActiveID();
                ImGui::SetFocusID(id, window);
                ImGui::FocusWindow(window);

                if (g->IO.KeyShift) {
                    // Shift-clicking multiplies the step 10x
                    step *= static_cast<T>(10.0);
                } else if(g->IO.KeyAlt) {
                    // Alt-clicking divides the step 10x, this might not work for ints, so the value is clamped
                    T alt_step = step / static_cast<T>(10.0);
                    if (alt_step == static_cast<T>(0.0))
                        alt_step = step;

                    step = alt_step;
                }

                T old_val = *p_val;
                *p_val += step;
                if (p_min != nullptr && *p_val < *p_min)
                    *p_val = *p_min;
                if (p_max != nullptr && *p_val > *p_max)
                    *p_val = *p_max;

                made_changes |= (*p_val != old_val);
            }
        }

        // Render the item frame with arrows
        // - Left arrow frame
        const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
        const ImVec2 arrow_padding = { 5.0f, g->Style.FramePadding.y + 3.0f };
        const bool frame_drag_nav = (g->NavId != 0 && g->NavId == id && g->NavDisableMouseHover);

        ImU32 col = ImGui::GetColorU32(left_arrow_active ? ImGuiCol_FrameBgActive : (left_arrow_hovered || frame_drag_nav) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        window->DrawList->AddRectFilled(left_arrow_bb.Min, left_arrow_bb.Max, col, frame_rounding, ImDrawFlags_RoundCornersLeft);
        GUI::DrawLeftArrow(window->DrawList, left_arrow_bb.Min, left_arrow_bb.Max, arrow_padding, text_col);

        // - Drag frame
        col = ImGui::GetColorU32(g->ActiveId == id ? ImGuiCol_FrameBgActive : (frame_drag_hovered || frame_drag_nav) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        window->DrawList->AddRectFilled(frame_drag_bb.Min, frame_drag_bb.Max, col);

        // - Drag frame slider highlight (only if clamped)
        if (p_min != nullptr && p_max != nullptr) {
            float t = (static_cast<float>(*p_val) - static_cast<float>(*p_min) + 1.0f) / (static_cast<float>(*p_max) - static_cast<float>(*p_min) + 1.0f);
            const ImVec2 highlight_p_min = frame_drag_bb.Min;
            const ImVec2 highlight_p_max = { highlight_p_min.x + t * frame_drag_bb.GetWidth(), frame_drag_bb.Max.y };

            GUI::DrawHighlightRect(window->DrawList, highlight_p_min, highlight_p_max, t, (frame_drag_hovered || frame_drag_nav), g->ActiveId == id);
        }

        // - Right arrow frame
        col = ImGui::GetColorU32(right_arrow_active ? ImGuiCol_FrameBgActive : (right_arrow_hovered || frame_drag_nav) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        window->DrawList->AddRectFilled(right_arrow_bb.Min, right_arrow_bb.Max, col, frame_rounding, ImDrawFlags_RoundCornersRight);
        GUI::DrawRightArrow(window->DrawList, right_arrow_bb.Min, right_arrow_bb.Max, arrow_padding, text_col);


        // Display value using user-provided display format
        char value_buf[64];
        const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *p_val);
        ImGui::RenderTextClipped(frame_drag_bb.Min, frame_drag_bb.Max, value_buf, value_buf_end, nullptr, { 0.5f, 0.5f });

        ImGui::RenderNavHighlight(frame_total_bb, id);
        made_changes |= ImGui::DragBehavior(id, data_type, p_val, 1.0f, p_min, p_max, format, ImGuiSliderFlags_AlwaysClamp);
        if (g->ActiveId == id)
            Input::SetCursorLocked();

        return made_changes;
    }

    bool GUI::SliderEx(const char* name, ImGuiDataType data_type, void* p_val, const void* p_min, const void* p_max, const char* format, const void* p_arrow_step)
    {
        YART_ASSERT(p_arrow_step != nullptr);

        switch (data_type) {
            case ImGuiDataType_S8:     return SliderExT<ImS8, ImS8>(name, data_type, (ImS8*)p_val, (ImS8*)p_min, (ImS8*)p_max, format, *(ImS8*)p_arrow_step);
            case ImGuiDataType_U8:     return SliderExT<ImU8, ImS8>(name, data_type, (ImU8*)p_val, (ImU8*)p_min, (ImU8*)p_max, format, *(ImU8*)p_arrow_step);
            case ImGuiDataType_S16:    return SliderExT<ImS16, ImS16>(name, data_type, (ImS16*)p_val, (ImS16*)p_min, (ImS16*)p_max, format, *(ImS16*)p_arrow_step);
            case ImGuiDataType_U16:    return SliderExT<ImU16, ImS16>(name, data_type, (ImU16*)p_val, (ImU16*)p_min, (ImU16*)p_max, format, *(ImU16*)p_arrow_step);
            case ImGuiDataType_S32:    return SliderExT<ImS32, ImS32>(name, data_type, (ImS32*)p_val, (ImS32*)p_min, (ImS32*)p_max, format, *(ImS32*)p_arrow_step);
            case ImGuiDataType_U32:    return SliderExT<ImU32, ImS32>(name, data_type, (ImU32*)p_val, (ImU32*)p_min, (ImU32*)p_max, format, *(ImU32*)p_arrow_step);
            case ImGuiDataType_S64:    return SliderExT<ImS64, ImS64>(name, data_type, (ImS64*)p_val, (ImS64*)p_min, (ImS64*)p_max, format, *(ImS64*)p_arrow_step);
            case ImGuiDataType_U64:    return SliderExT<ImU64, ImS64>(name, data_type, (ImU64*)p_val, (ImU64*)p_min, (ImU64*)p_max, format, *(ImU64*)p_arrow_step);
            case ImGuiDataType_Float:  return SliderExT<float, float>(name, data_type, (float*)p_val, (float*)p_min, (float*)p_max, format, *(float*)p_arrow_step);
            case ImGuiDataType_Double: return SliderExT<double, double>(name, data_type, (double*)p_val, (double*)p_min, (double*)p_max, format, *(double*)p_arrow_step);
            case ImGuiDataType_COUNT:  
                break;
        }

        YART_UNREACHABLE();
        return false;
    }

    /// @brief Draw the GradientEditor sampling point handle
    /// @param color Color of the sampling point
    /// @param pos Top-left corner position of where to draw the handle
    /// @param size Sie of the handle
    /// @param border_col Color of the handle border
    void GradientSamplingPointHandle(const glm::vec3& color, const ImVec2& pos, const ImVec2& size, ImU32 border_col)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImDrawList* draw_list = g->CurrentWindow->DrawList;

        const ImU32 col = ImGui::ColorConvertFloat4ToU32({ color.x, color.y, color.z, 1.0f });

        ImVec2 p1 = pos;
        ImVec2 p2 = { p1.x + size.x, p1.y + size.x };

        draw_list->AddRectFilled({ p1.x + 1.0f, p1.y + 1.0f }, { p2.x - 1.0f, p2.y - 1.0f }, col);
        draw_list->AddRect(p1, p2, border_col);

        p1.y = p2.y += 1.0f;
        draw_list->AddTriangleFilled(p1, p2, { pos.x + size.x / 2.0f, pos.y + size.y }, border_col);
    }

    /// @brief Update a single location of the gradient editor stops and sort the data if necessary
    /// @param ctx GradientEditor widget context object
    /// @param i Index of the changed location
    /// @param new_loc New location value
    /// @return Whether the location value has changed
    bool UpdateGradientEditorLocations(GUI::GradientEditorContext& ctx, int i, float new_loc)
    {
        float old_loc = ctx.locations[i];
        ctx.locations[i] = new_loc;

        if (new_loc == old_loc)
            return false;

        if (new_loc < old_loc) {
            for (int j = 0; j < i; ++j) {
                if (new_loc < ctx.locations[j]) {
                    glm::vec3 temp_val = ctx.values[i];
                    ImGuiID temp_id = ctx.ids[i];

                    // Shift all the remaining values
                    for (int k = i; k > j; --k) {
                        ctx.locations[k] = ctx.locations[k - 1];
                        ctx.values[k] = ctx.values[k - 1];
                        ctx.ids[k] = ctx.ids[k - 1];
                    }

                    ctx.values[j] = temp_val;
                    ctx.ids[j] = temp_id;
                    i = j;
                    break;
                }
            }
        } else {
            for (int j = static_cast<int>(ctx.locations.size()) - 1; j > i; --j) {
                if (new_loc > ctx.locations[j]) {
                    glm::vec3 temp_val = ctx.values[i];
                    ImGuiID temp_id = ctx.ids[i];

                    // Shift all the remaining values
                    for (int k = i; k < j; ++k) {
                        ctx.locations[k] = ctx.locations[k + 1];
                        ctx.values[k] = ctx.values[k + 1];
                        ctx.ids[k] = ctx.ids[k + 1];
                    }

                    ctx.values[j] = temp_val;
                    ctx.ids[j] = temp_id;
                    i = j;
                    break;
                }
            }
        }

        return true;
    }

    bool GUI::GradientEditorEx(GradientEditorContext& ctx)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        ImDrawList* draw_list = window->DrawList;

        bool state_updated = false; // Whether the gradient has changed since last frame 
        if (window->SkipItems)
            return state_updated;


        // Generate picker IDs if nonexistant 
        if (ctx.ids == nullptr) {
            ctx.ids = std::make_unique<ImGuiID[]>(ctx.values.size());

            for (int i = 0; i < ctx.values.size(); ++i) {
                const char* temp_name;
                ImFormatStringToTempBuffer(&temp_name, nullptr, "##ColorEdit/%d", i);
                ctx.ids[i] = window->GetID(temp_name);
            }
        }


        // Compute sizes and bounding boxes
        static constexpr ImVec2 picker_size = { 10.0f, 16.0f }; // Height of the triangle arrow is `picker_size.y - picker_size.x`
        const ImVec2 gradient_size = { ImGui::GetContentRegionAvail().x - picker_size.x, ImGui::GetFrameHeight() };

        const ImVec2 cursor_pos = window->DC.CursorPos;
        const ImVec2 gradient_p_min = { 
            cursor_pos.x + picker_size.x / 2.0f,
            cursor_pos.y + picker_size.y + 1.0f
        };

        const ImVec2 gradient_p_max = { 
            gradient_p_min.x + gradient_size.x, 
            gradient_p_min.y + gradient_size.y 
        };

        ImGui::BeginGroup();
        ImGui::ItemSize({ 0.0f, picker_size.y + gradient_size.y + 1.0f });


        // -- HANDLE COLOR PICKER INPUTS -- //
        int hovered_idx = -1;
        for (int i = static_cast<int>(ctx.values.size()) - 1; i >= 0; --i) {
            const ImVec4 bb = { 
                cursor_pos.x + glm::round(ctx.locations[i] * gradient_size.x), cursor_pos.y,
                cursor_pos.x + glm::round(ctx.locations[i] * gradient_size.x) + picker_size.x, cursor_pos.y + picker_size.y
            };
            
            ImGuiID id = ctx.ids[i];
            ImGui::ItemAdd(bb, id);

            bool hovered, held;
            bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);

            if (hovered || held) {
                // Move the handle location
                if (held) {
                    float x_pos = ImClamp(g->IO.MousePos.x, cursor_pos.x, gradient_p_min.x + gradient_p_max.x); 
                    float new_loc = ImClamp((x_pos - gradient_p_min.x) / gradient_size.x, 0.0f, 1.0f);

                    // Update the gradient locations state and sort the data if necessary
                    if (UpdateGradientEditorLocations(ctx, i, new_loc))
                        state_updated = true;
                }

                hovered_idx = i;
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

                if (clicked || held)
                    ctx.selectedItemIndex = static_cast<uint8_t>(i);

                break;
            }
        }
        

        // -- RENDER GRADIENT RECT AND COLOR PICKER HANDLES -- //
        for (size_t i = 0; i < ctx.values.size(); ++i) {
            const ImVec2 p_min = { cursor_pos.x + glm::round(ctx.locations[i] * gradient_size.x), cursor_pos.y };

            const ImU32 border_col = (i == ctx.selectedItemIndex) ? 0xFFFFFFFF : ImGui::GetColorU32((i == hovered_idx) ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);
            GradientSamplingPointHandle(ctx.values[i], p_min, picker_size, border_col);
        }

        GUI::DrawGradientRect(draw_list, gradient_p_min, gradient_p_max, ctx.values.data(), ctx.locations.data(), ctx.values.size(), true);


        // -- RENDER HANDLE STATE CONTROLS -- //
        GUI::BeginFrame("Stop 1", 1);
        {
            static float f;
            ImGui::DragFloat("idk1", &f);
        }
        GUI::EndFrame();

        
        ImGui::EndGroup();
        return state_updated;
    }

} // namespace yart
