////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI objects and methods definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui_internal.h"


/// @brief Helper macro useful for opening `break`-able context scopes
#define WITH(handle) for(auto* _h = handle; _h != nullptr; _h = nullptr)

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define TEMP_BUFFER_SIZE 5

    #define HOVER_RECT_PADDING 2.0f
    #define LAYOUT_SEGMENT_MIN_SIZE 50.0f
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
            float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, LAYOUT_SEGMENT_MIN_SIZE);
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

        ImGuiID id = ImGui::GetID("SeparatorHandle");
        ImGui::ItemAdd(bb, id);

        bool hovered, held;
        ImGui::ButtonBehavior(bb, id, &hovered, &held);

        ImVec4 col = held ? g->Style.Colors[ImGuiCol_SeparatorActive] : (hovered ? g->Style.Colors[ImGuiCol_SeparatorHovered] : g->Style.Colors[ImGuiCol_Separator]);
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


        float min_size = ImMax(g->Style.ChildRounding * 2.0f + 2.0f, LAYOUT_SEGMENT_MIN_SIZE);
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
        // Render the scene+context menu layout
        static LayoutState vertical_layout = { };
        vertical_layout.direction = GUI::LayoutDir::VERTICAL;
        vertical_layout.default_size_ratio = 1.0f / 3.0f;
        vertical_layout.window_flags = ImGuiWindowFlags_AlwaysUseWindowPadding;

        GUI::BeginLayout(vertical_layout);

        GUI::LayoutSeparator(vertical_layout);

        GUI::EndLayout(vertical_layout);
    }

    void GUI::RenderWindow(const GuiWindow &window)
    {
        // Local alpha multiplier for the whole window
        float window_alpha = YART_GUI_ALPHA_OPAQUE;
        
        // Try querying the window state prior to ImGui::Begin() to apply custom style
        ImGuiWindow* imgui_window = ImGui::FindWindowByName(window.name);
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
        ImGui::Begin(window.name);

        ImGui::PopStyleVar(2);
        

        // Draw window contents 
        ImGuiStyle* style = &ImGui::GetStyle();

        float restore_alpha = style->Alpha;
        style->Alpha = window_alpha;

        // Call the window's render callback
        window.callback();

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
        draw_list->AddCircleFilled({handle_pos.x, handle_pos.y}, handle_radius - 0.5f, inner_col, 0);
        draw_list->AddCircle({handle_pos.x, handle_pos.y}, handle_radius, outer_col, 0, handle_thickness);
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
        static const ImU32 background_color = ImGui::ColorConvertFloat4ToU32({ YART_GUI_COLOR_GRAY, YART_GUI_ALPHA_MEDIUM });

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

} // namespace yart
