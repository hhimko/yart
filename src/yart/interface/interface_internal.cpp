////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal Interface module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the Interface module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "interface_internal.h"


#include "yart/GUI/gui.h"


/// @brief Helper macro useful for opening `break`-able context scopes
#define WITH(handle) for(auto* _h = handle; _h != nullptr; _h = nullptr)

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define HOVER_RECT_PADDING 2.0f

    #define AXIS_POSITIVE_X 0
    #define AXIS_POSITIVE_Y 1
    #define AXIS_POSITIVE_Z 2
    #define AXIS_NEGATIVE_X 3
    #define AXIS_NEGATIVE_Y 4
    #define AXIS_NEGATIVE_Z 5
#endif // ifndef DOXYGEN_EXCLUDE 


namespace yart
{
    Interface::InterfaceContext* Interface::GetInterfaceContext()
    {
        static yart::Interface::InterfaceContext context = { };  
        return &context;
    }

    float Interface::RenderMainMenuBar()
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

        // Store and return the menu bar height, used for determining content area sizes
        float menu_bar_height = g->CurrentWindow->Size.y;

        ImGui::EndMainMenuBar();
        return menu_bar_height;
    }

    void Interface::RenderMainContentArea(const float menu_bar_height)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        InterfaceContext* ctx = Interface::GetInterfaceContext();

        // Main content window area is essentially the display area minus the main menu bar size 
        ImVec2 display_size = g->IO.DisplaySize;
        ImGui::SetNextWindowPos({ 0.0f, menu_bar_height });
        ImGui::SetNextWindowSize({ display_size.x, display_size.y - menu_bar_height });

        static constexpr ImGuiWindowFlags flags = (
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground
        );
        
        ImVec2 backup_padding = g->Style.WindowPadding;
        g->Style.WindowPadding = { 0.0f, 0.0f };
        ImGui::Begin("YART_MainContentFrame", nullptr, flags);
        g->Style.WindowPadding = backup_padding;

        // Render the main frame layout
        static GUI::LayoutContext horizontal_layout = { };
        horizontal_layout.direction = GUI::LayoutDir::HORIZONTAL;
        horizontal_layout.preserveSecondSectionSize = true;
        horizontal_layout.default_size_ratio = 3.0f / 4.0f;
        horizontal_layout.window_flags = ImGuiWindowFlags_NoBackground;

        GUI::BeginLayout(horizontal_layout);
        {
            ImGuiWindow* viewport_window = g->CurrentWindow;

            // Store the viewport window ID and bounding box
            ctx->renderViewportWindowID = viewport_window->ID;
            ctx->renderViewportArea = viewport_window->Rect();

            // Render the viewport image
            ImTextureID viewport_texture = ctx->renderViewport->GetImTextureID();
            ImGui::GetBackgroundDrawList()->AddImage(viewport_texture, ctx->renderViewportArea.Min, ctx->renderViewportArea.Max);
        }
        GUI::LayoutSeparator(horizontal_layout);
        {
            Interface::RenderContextWindow();
        }
        GUI::EndLayout(horizontal_layout);

        ImGui::End();
    }

    void Interface::RenderContextWindow()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ImGuiContext* g = ImGui::GetCurrentContext();

        // Render the scene+context menu layout
        static GUI::LayoutContext vertical_layout = { };
        vertical_layout.direction = GUI::LayoutDir::VERTICAL;
        vertical_layout.default_size_ratio = 1.0f / 3.0f;
        vertical_layout.window_flags = ImGuiWindowFlags_NoBackground;
        vertical_layout.min_size = 50.0f;

        // Scene + Object inspectors section
        GUI::BeginLayout(vertical_layout);

        if (GUI::BeginTabBar("Scene")) {
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
        GUI::EndTabBar();

        // Master inspector section
        GUI::LayoutSeparator(vertical_layout);

        Interface::RenderInspectorNavBar();
        InspectorSection* active_section = ctx->activeInspectorSection;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
        ImGui::SameLine();
        ImGui::PopStyleVar();

        const char* name = "Inspector";
        if (active_section != nullptr)
            ImFormatStringToTempBuffer(&name, nullptr, "%s###InspectorTabItem", active_section->name);

        bool open = GUI::BeginTabBar(name);
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
            static ImU32 bg_col = ImGui::GetColorU32(ImGuiCol_ChildBg);
            g->CurrentWindow->DrawList->AddRectFilled(p_min, p_max, bg_col);
            g->CurrentWindow->DrawList->PushClipRect({ backup_clip_rect.x, backup_clip_rect.y }, { backup_clip_rect.z, backup_clip_rect.w });

            // Render the currently active inspector item
            if (active_section != nullptr)
                ctx->madeChanges |= active_section->callback();

            ImGui::EndChild();
            if (open)
                ImGui::EndTabItem();
        }
        GUI::EndTabBar();

        GUI::EndLayout(vertical_layout);
    }

    void Interface::RenderInspectorNavBar()
    {
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ImGuiContext* g = ImGui::GetCurrentContext();

        const float window_y_offset = ImGui::GetFrameHeight() - 1.0f;
        static constexpr float icon_button_outer_padding = 3.0f;
        static constexpr float icon_button_inner_padding = 4.0f;
        const float size = GUI::GetIconsFont()->FontSize;
       
        const float window_width = size + 2.0f * icon_button_outer_padding + 2.0f * icon_button_inner_padding;
        static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::BeginChild("##SideNavBar", { window_width, 0.0f }, false, flags);

        // Draw background without right edge rounding
        ImGuiWindow* window = g->CurrentWindow;
        const float child_rounding = g->Style.ChildRounding;
        ImVec2 p_min = { window->Pos.x, window->Pos.y + window_y_offset };
        ImVec2 p_max = { p_min.x + window->Size.x + child_rounding, p_min.y + window->Size.y };

        static const ImU32 bg_col = ImGui::GetColorU32({ YART_GUI_COLOR_DARKEST_GRAY, YART_GUI_ALPHA_OPAQUE });
        window->DrawList->AddRectFilled(p_min, p_max, bg_col, child_rounding);

        // Render menu item icons
        static constexpr float item_spacing = 4.0f; 
        window->DC.CursorPos.y += window_y_offset;

        for (auto&& item : ctx->registeredInspectorSections) {
            window->DC.CursorPos.x += icon_button_outer_padding;
            window->DC.CursorPos.y += item_spacing;

            p_min = { window->DC.CursorPos.x, window->DC.CursorPos.y + 1.0f };
            p_max = { p_min.x + 2.0f * icon_button_inner_padding + icon_button_outer_padding + size, p_min.y + 2.0f * icon_button_inner_padding + size };

            const ImGuiID id = ImGui::GetID(item.name);
            ImGui::ItemAdd({ p_min, p_max }, id);

            bool hovered, held;
            bool clicked = ImGui::ButtonBehavior({ p_min, p_max }, id, &hovered, &held);

            bool active = &item == ctx->activeInspectorSection;
            if (!active && clicked) {
                ctx->activeInspectorSection = &item;
                active = true;
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip(item.name);

            // Render background
            const ImU32 col = ImGui::GetColorU32(hovered ? ImGuiCol_TabHovered : active ? ImGuiCol_TabActive : ImGuiCol_Tab);
            window->DrawList->AddRectFilled(p_min, p_max, col, child_rounding, ImDrawFlags_RoundCornersLeft);

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

    bool Interface::RenderViewAxesWindow(const glm::vec3& x_axis, const glm::vec3& y_axis, const glm::vec3& z_axis, glm::vec3& clicked_axis)
    {
        static constexpr ImVec2 window_size = { 75.0f, 75.0f }; // Expected to be a square
        static constexpr ImVec2 window_margin = { 25.0f, 15.0f };

        static const ImGuiWindowFlags window_flags = (
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground
        );

        // Set a constant window size and position
        InterfaceContext* ctx = Interface::GetInterfaceContext();
        ImRect viewport_area = ctx->renderViewportArea;

        ImVec2 window_center = {
            viewport_area.Min.x + viewport_area.GetWidth() - window_size.x / 2 - window_margin.x, 
            viewport_area.Min.y + window_size.y / 2 + window_margin.y 
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
        if (Interface::IsMouseOverRenderViewport())
            hovered = GUI::IsMouseHoveringCircle(window_center, circle_radius);

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

    void Interface::RenderWindow(const char *name, callback_t callback)
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
} // namespace yart
