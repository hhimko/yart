////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal GUI module definitions to hide implementation detail
/// @details Not supposed to be imported from outside the GUI module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gui_internal.h"


#include <cstring> // For memset

#include "yart/common/utils/yart_utils.h"
#include "yart/common/utils/glm_utils.h"
#include "font/IconsCodicons.h"
#include "input.h"


namespace yart
{
    GUI::GuiContext* GUI::GetGuiContext()
    {
        static yart::GUI::GuiContext s_context = { };  
        return &s_context;
    }

    ImVec2 GUI::LayoutSeparatorHandleEx(ImVec2 pos, ImVec2 size, ImGuiMouseCursor_ cursor) 
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        const ImRect bb(pos, { pos.x + size.x, pos.y + size.y });
        ImGui::ItemSize(size);

        const ImGuiID id = ImGui::GetID("SeparatorHandle");
        ImGui::ItemAdd(bb, id);

        bool hovered, held;
        ImGui::ButtonBehavior(bb, id, &hovered, &held);

        const ImU32 col = ImGui::GetColorU32(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);
        g->CurrentWindow->DrawList->AddRectFilled(bb.Min, bb.Max, col);

        if (hovered || held)
            ImGui::SetMouseCursor(cursor);

        if (held) {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left, 0.0f);
            ImGui::ResetMouseDragDelta();
            return drag;
        }

        return { 0.0f, 0.0f };
    }

    void GUI::LabelEx(const char* name, const char* text)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        // Retrieve current item flags and add the default dark style flag
        ctx->nextItemFlags |= GuiItemFlags_FrameStyleDark;
        GuiItemFlags flags = GetCurrentItemFlags(); 

        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return;


        ImRect text_bb, frame_bb;
        const ImRect total_bb = CalculateItemSizes(text_bb, frame_bb);

        const ImGuiID id = window->GetID(name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id))
            return;


        const bool total_hovered = g->ActiveId != id && (ImGui::ItemHoverable(total_bb, id) || g->NavId == id);
        const bool text_hovered = total_hovered && ImGui::IsMouseHoveringRect(text_bb.Min, text_bb.Max);
        const bool frame_hovered = total_hovered && ImGui::IsMouseHoveringRect(frame_bb.Min, frame_bb.Max);

        // Render the widget name text
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, name) && text_hovered)
            ImGui::SetTooltip(name);

        // Render the frame with label text
        const ImU32 frame_col = GetFrameColor(false, false); // Label is not responsive to the mouse cursor
        DrawItemFrame(window->DrawList, frame_bb.Min, frame_bb.Max, frame_col);
        if (GUI::DrawText(window->DrawList, frame_bb.Min, frame_bb.Max, text, YART_GUI_TEXT_ALIGN_LEFT, true) && frame_hovered)
            ImGui::SetTooltip(text);
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
    bool SliderExT(const char* name, ImGuiDataType data_type, T* p_val, const T* p_min, const T* p_max, const char* format, T arrow_step) 
    {
        // Retrieve current item flags
        GuiItemFlags flags = GUI::GetCurrentItemFlags(); 

        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;
        if (window->SkipItems)
            return false;


        ImRect text_bb, frame_bb;
        const ImRect total_bb = GUI::CalculateItemSizes(text_bb, frame_bb);

        const ImGuiID id = window->GetID(name);
        ImGui::ItemSize(total_bb);
        if (!ImGui::ItemAdd(total_bb, id))
            return false;


        static constexpr float arrow_frame_width = 14.0f;
        const ImRect frame_drag_bb = { { frame_bb.Min.x + arrow_frame_width, frame_bb.Min.y }, { frame_bb.Max.x - arrow_frame_width, frame_bb.Max.y } };

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
        if (GUI::DrawText(window->DrawList, text_bb.Min, text_bb.Max, name) && text_hovered)
            ImGui::SetTooltip(name);

        // During temp input, skip drawing the custom frame 
        if (temp_input_is_active)
            return ImGui::TempInputScalar(frame_bb, id, name, data_type, p_val, format, p_min, p_max);

        
        static constexpr float frame_separator_thickness = 1.0f;
        const float frame_rounding = g->Style.FrameRounding;

        const ImRect left_arrow_bb = { frame_bb.Min, { frame_drag_bb.Min.x - frame_separator_thickness, frame_bb.Max.y } };
        const ImRect right_arrow_bb = { { frame_drag_bb.Max.x + frame_separator_thickness, frame_bb.Min.y }, frame_bb.Max };

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

        ImU32 frame_col = GUI::GetFrameColor(left_arrow_hovered || frame_drag_nav, left_arrow_active);
        GUI::DrawItemFrame(window->DrawList, left_arrow_bb.Min, left_arrow_bb.Max, frame_col, ImDrawFlags_RoundCornersLeft);
        GUI::DrawLeftArrow(window->DrawList, left_arrow_bb.Min, left_arrow_bb.Max, arrow_padding, text_col);

        // - Drag frame
        frame_col = GUI::GetFrameColor(frame_drag_hovered || frame_drag_nav, g->ActiveId == id);
        GUI::DrawItemFrame(window->DrawList, frame_drag_bb.Min, frame_drag_bb.Max, frame_col, ImDrawFlags_RoundCornersNone);

        // - Drag frame slider highlight (only if clamped)
        if (p_min != nullptr && p_max != nullptr) {
            float t = (static_cast<float>(*p_val) - static_cast<float>(*p_min) + 1.0f) / (static_cast<float>(*p_max) - static_cast<float>(*p_min) + 1.0f);
            const ImVec2 highlight_p_min = frame_drag_bb.Min;
            const ImVec2 highlight_p_max = { highlight_p_min.x + t * frame_drag_bb.GetWidth(), frame_drag_bb.Max.y };

            GUI::DrawFrameHighlight(window->DrawList, highlight_p_min, highlight_p_max, t, (frame_drag_hovered || frame_drag_nav), g->ActiveId == id);
        }

        // - Right arrow frame
        frame_col = GUI::GetFrameColor(right_arrow_hovered || frame_drag_nav, right_arrow_active);
        GUI::DrawItemFrame(window->DrawList, right_arrow_bb.Min, right_arrow_bb.Max, frame_col, ImDrawFlags_RoundCornersRight);
        GUI::DrawRightArrow(window->DrawList, right_arrow_bb.Min, right_arrow_bb.Max, arrow_padding, text_col);


        // Display value using user-provided display format
        char value_buf[64];
        const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *p_val);
        ImGui::RenderTextClipped(frame_drag_bb.Min, frame_drag_bb.Max, value_buf, value_buf_end, nullptr, { 0.5f, 0.5f });

        ImGui::RenderNavHighlight(frame_bb, id, ImGuiNavHighlightFlags_TypeThin);
        made_changes |= ImGui::DragBehavior(id, data_type, p_val, 1.0f, p_min, p_max, format, ImGuiSliderFlags_AlwaysClamp);
        if (g->ActiveId == id)
            yart::GUI::Input::SetCursorLocked();

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

        const ImU32 col = ImGui::GetColorU32({ color.x, color.y, color.z, 1.0f });

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
                    ctx.selectedItemIndex = static_cast<uint8_t>(j);
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
                    ctx.selectedItemIndex = static_cast<uint8_t>(j);
                    break;
                }
            }
        }

        return true;
    }

    bool GUI::GradientEditorEx(GradientEditorContext& ctx)
    {
        // Retrieve current item flags
        GuiItemFlags flags = GetCurrentItemFlags(); 

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
        bool disable = ctx.selectedItemIndex >= ctx.values.size();
        if (disable)
            ImGui::BeginDisabled();

        float disabled_col[3] = { YART_GUI_COLOR_DARK_GRAY };
        static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
        state_updated |= GUI::ColorEdit("Stop color", disable ? disabled_col : (float*)&ctx.values[ctx.selectedItemIndex]);

        float location_as_percent = disable ? 0.0f : ctx.locations[ctx.selectedItemIndex] * 100.0f;
        if (GUI::SliderFloat("Stop location", &location_as_percent, 0.0f, 100.0f, "%.2f%%")) {
            float new_location = location_as_percent / 100.0f;
            if (UpdateGradientEditorLocations(ctx, ctx.selectedItemIndex, new_location))
                state_updated = true;
        }


        // -- RENDER + / - BUTTONS -- //
        const ImRect buttons_bb = {
            window->DC.CursorPos,
            { window->DC.CursorPos.x + ImGui::GetContentRegionAvail().x, window->DC.CursorPos.y + ImGui::GetFrameHeight() }
        };

        ImGui::ItemSize({ 0.0f, buttons_bb.GetHeight() });

        // - button
        {
            const ImVec2 p_max = { buttons_bb.Min.x + buttons_bb.GetWidth() / 2.0f - 1.0f, buttons_bb.Max.y };
            const ImGuiID minus_button_id = ImGui::GetID("##ColorEdit/buttons/-");
            ImGui::ItemAdd({ buttons_bb.Min, p_max }, minus_button_id);

            bool hovered, active;
            const bool clicked = ImGui::ButtonBehavior({ buttons_bb.Min, p_max }, minus_button_id, &hovered, &active);
            if (clicked && ctx.values.size() > 1) {
                ctx.values.erase(ctx.values.begin() + ctx.selectedItemIndex);
                ctx.locations.erase(ctx.locations.begin() + ctx.selectedItemIndex);
                ctx.selectedItemIndex = glm::max(ctx.selectedItemIndex - 1, 0);

                ctx.ids = nullptr; // force idx recalculation
                state_updated = true;
            }

            const ImU32 col = GUI::GetFrameColor(hovered, active);
            window->DrawList->AddRectFilled(buttons_bb.Min, p_max, col, g->Style.FrameRounding, ImDrawFlags_RoundCornersLeft);

            GUI::DrawText(window->DrawList, buttons_bb.Min, p_max, "-", YART_GUI_TEXT_ALIGN_CENTER, false);
            ImGui::RenderNavHighlight({ buttons_bb.Min, p_max }, minus_button_id, ImGuiNavHighlightFlags_TypeThin);
        }

        // + button
        {
            const ImVec2 p_min = { buttons_bb.Max.x - buttons_bb.GetWidth() / 2.0f + 1.0f, buttons_bb.Min.y };
            const ImGuiID plus_button_id = ImGui::GetID("##ColorEdit/buttons/+");
            ImGui::ItemAdd({ p_min, buttons_bb.Max }, plus_button_id);

            bool hovered, active;
            const bool clicked = ImGui::ButtonBehavior({ p_min, buttons_bb.Max }, plus_button_id, &hovered, &active);
            if (clicked && ctx.values.size() < 256) {
                float max_dist = ctx.locations[0];
                size_t new_index = 0;
                glm::vec3 new_val = ctx.values[0];
                float new_loc = max_dist / 2.0f;

                // pick an index with the biggest space between adjacent values
                for (size_t i = 0; i < ctx.locations.size(); i++) {
                    const float next_loc = i < ctx.locations.size() - 1 ? ctx.locations[i + 1] : 1.0f;
                    const float dist = next_loc - ctx.locations[i];

                    if (dist > max_dist) {
                        new_index = i + 1;
                        max_dist = dist;

                        const glm::vec3 grad[2] = {
                            ctx.values[i],
                            i < ctx.locations.size() - 1 ? ctx.values[i + 1] : ctx.values[i]
                        };

                        new_val = yart::utils::LinearGradient(grad, YART_ARRAYSIZE(grad), 0.5f);
                        new_loc = ctx.locations[i] + dist / 2.0f;
                    }
                }

                ctx.values.insert(ctx.values.begin() + new_index, new_val);
                ctx.locations.insert(ctx.locations.begin() + new_index, new_loc);
                ctx.selectedItemIndex = new_index;

                ctx.ids = nullptr; // force idx recalculation
                state_updated = true;
            }

            const ImU32 col = GUI::GetFrameColor(hovered, active);
            window->DrawList->AddRectFilled(p_min, buttons_bb.Max, col, g->Style.FrameRounding, ImDrawFlags_RoundCornersRight);

            GUI::DrawText(window->DrawList, p_min, buttons_bb.Max, "+", YART_GUI_TEXT_ALIGN_CENTER, false);
            ImGui::RenderNavHighlight({ p_min, buttons_bb.Max }, plus_button_id, ImGuiNavHighlightFlags_TypeThin);
        }


        if (disable)
            ImGui::EndDisabled();

        ImGui::EndGroup();
        return state_updated;
    }

    void GUI::DrawItemFrame(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 color, ImDrawFlags draw_flags)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

        // Fix ImGui corner flags
        if ((draw_flags & ImDrawFlags_RoundCornersMask_) == 0)
            draw_flags |= ImDrawFlags_RoundCornersDefault_;


        // GuiItemFlags enum rounding flags have higher priority over ImDrawFlags 
        if (ctx->currentItemFlags & GuiItemFlags_NoCornerRounding) {
            draw_flags &= ~ImDrawFlags_RoundCornersAll;
            draw_flags |= ImDrawFlags_RoundCornersNone;
        } else {
            // If frame rounding is requested by both flags, take their union 
            if (ctx->currentItemFlags & GuiItemFlags_CornersRoundTop)
                draw_flags &= (~ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersTop);
            if (ctx->currentItemFlags & GuiItemFlags_CornersRoundBottom)
                draw_flags &= (~ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersBottom);
        }

        // Render frame
        const float rounding = g->Style.FrameRounding;
        if (ctx->currentItemFlags & GuiItemFlags_FrameBorder) {
            // Frame border requires the background to shrink for a better visual effect     
            const ImVec2 p1 = { p_min.x + 0.5f, p_min.y + 0.5f };
            const ImVec2 p2 = { p_max.x - 0.5f, p_max.y - 0.5f };
            if (p2.x >= p1.x && p2.y >= p1.y)
                draw_list->AddRectFilled(p1, p2, color, rounding, draw_flags);

            // Render frame border
            const ImU32 border_col = ImGui::GetColorU32(ImGuiCol_Border);
            draw_list->AddRect(p_min, p_max, border_col, rounding, draw_flags);
        } else {
            draw_list->AddRectFilled(p_min, p_max, color, rounding, draw_flags);
        }
    }

    bool GUI::DrawText(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, const char* text, float align, bool frame_padding)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();

        const float width = p_max.x - p_min.x;
        if (width <= 0.0f)
            return false;

        // Calculate offset based on the alignment value
        const ImVec2 text_size = ImGui::CalcTextSize(text);
        const float offset_x = ImMax(0.0f, (p_max.x - p_min.x) * align - text_size.x * align);
        const float offset_y = (p_max.y - p_min.y - g->FontSize) / 2.0f;

        ImVec2 p0 = { p_min.x + offset_x, p_min.y + offset_y }; 
        ImVec2 p1 = p_max; 

        if (frame_padding) {
            const float adaptive_pad = ImClamp(width - text_size.x, 0.0f, 2.0f * g->Style.FramePadding.x) / 2.0f;
            p0.x += adaptive_pad;
            p1.x -= adaptive_pad;
        }

        ImGui::RenderTextEllipsis(draw_list, p0, p1, p1.x, p1.x, text, nullptr, &text_size);
        return text_size.x > (p_max.x - p_min.x);
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

    void GUI::DrawGradientRect(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 min_col, ImU32 max_col, float rounding, ImDrawFlags flags)
    {
        if (p_min.x >= p_max.x || p_min.y >= p_max.y)
            return;


        if (rounding >= 0.5f && !((flags & ImDrawFlags_RoundCornersMask_) == ImDrawFlags_RoundCornersNone)) {
            if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
                flags |= ImDrawFlags_RoundCornersAll;

            if (flags & ImDrawFlags_RoundCornersLeft) {
                p_min.x = ImCeil(p_min.x + rounding + 1.0f);

                const ImVec2 p1 = { p_min.x - rounding - 1.0f, p_min.y };
                const ImVec2 p2 = { p_min.x, p_max.y };
                draw_list->AddRectFilled(p1, p2, min_col, rounding, ImDrawFlags_RoundCornersLeft);
            }

            if (flags & ImDrawFlags_RoundCornersRight) {
                p_max.x = ImFloor(p_max.x - rounding - 1.0f);

                const ImVec2 p2 = { p_max.x + rounding + 1.0f, p_max.y };
                const ImVec2 p1 = { p_max.x, p_min.y };
                draw_list->AddRectFilled(p1, p2, max_col, rounding, ImDrawFlags_RoundCornersRight);
            }
        }

        draw_list->AddRectFilledMultiColor(p_min, p_max, min_col, max_col, max_col, min_col);
    }

    void GUI::DrawFrameHighlight(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, float t, bool hovered, bool active, ImDrawFlags flags)
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        GuiContext* ctx = GetGuiContext();

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

        const ImU32 min_col = ImGui::GetColorU32({ colors[0].x, colors[0].y, colors[0].z, YART_GUI_ALPHA_OPAQUE });
        const ImU32 max_col = ImGui::GetColorU32({ colors[1].x, colors[1].y, colors[1].z, YART_GUI_ALPHA_OPAQUE });

        const float rounding = g->Style.FrameRounding;
        if (ctx->currentItemFlags & GuiItemFlags_FrameBorder) {
            // A offset of 0.5 seems to work much better for alignment with border than 1.0 does 
            const ImVec2 p1 = { p_min.x + 0.5f, p_min.y + 0.5f };
            const ImVec2 p2 = { p_max.x - 0.5f, p_max.y - 0.5f };
            DrawGradientRect(draw_list, p1, p2, min_col, max_col, rounding, flags);
            draw_list->AddRect(p_min, p_max, ImGui::GetColorU32(ImGuiCol_Border), rounding, flags);
        } else {
            DrawGradientRect(draw_list, p_min, p_max, min_col, max_col, rounding, flags);
        }
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

    GuiItemFlags GUI::GetCurrentItemFlags()
    {
        GuiContext* ctx = GetGuiContext();
        GuiItemFlags flags = ctx->nextItemFlags;

        // Handle multi item group flags
        if (ctx->multiItemsCount > 0) {
            if (ctx->startMultiItems) {
                flags |= GuiItemFlags_CornersRoundTop;
                ctx->startMultiItems = false;
            } else if (ctx->multiItemsCount == 1) {
                flags |= GuiItemFlags_CornersRoundBottom;
            } else {
                flags |= GuiItemFlags_NoCornerRounding;
            }

            ctx->multiItemsCount--;
        }

        // Validate and fix flags
        if (flags != GuiItemFlags_None) {
            // Validate 
            // - Corner rounding flags should not be used with `GuiItemFlags_NoCornerRounding`
            static constexpr GuiItemFlags rounding_flags = GuiItemFlags_CornersRoundTop | GuiItemFlags_CornersRoundBottom;
            if ((flags & GuiItemFlags_NoCornerRounding) && (flags & rounding_flags))
                YART_ABORT("Invalid GuiItemFlags: Rounding flags mix-up\n");

            // Fix
            // - Full frame width makes the label hidden
            if (flags & GuiItemFlags_FullWidth)
                flags |= GuiItemFlags_HideLabel;
        }

        ctx->currentItemFlags = flags;
        ctx->nextItemFlags = GuiItemFlags_None;
        
        return flags;
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

    ImRect GUI::CalculateItemSizes(ImRect& text_bb, ImRect& frame_bb, bool square_frame) 
    {
        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiWindow* window = g->CurrentWindow;

        GuiItemFlags flags = GetGuiContext()->currentItemFlags; 
        const bool hide_name = flags & GuiItemFlags_HideLabel; 
        const bool is_full_width = flags & GuiItemFlags_FullWidth; 

        // Calculate the total bounding box of the widget
        const ImRect total_bb = { window->DC.CursorPos, { window->WorkRect.Max.x, window->DC.CursorPos.y + ImGui::GetFrameHeight() }};
        static const float text_width_percent = 0.4f; // TODO: This value should be calculated based on the current indent at some point

        const bool display_name = (flags & GuiItemFlags_HideLabel) == 0; 
        const float frame_spacing = display_name ? g->Style.ItemInnerSpacing.x : 0.0f;
        const float text_frame_width = !is_full_width ? IM_ROUND(total_bb.GetWidth() * text_width_percent) : 0.0f;
        const float max_frame_width = square_frame ? total_bb.GetHeight() : 0.0f;

        text_bb.Min = total_bb.Min;
        text_bb.Max = { total_bb.Min.x + text_frame_width - frame_spacing, total_bb.Max.y };
        frame_bb.Min = { text_bb.Max.x + frame_spacing, total_bb.Min.y };
        frame_bb.Max = (max_frame_width <= 0) ? total_bb.Max : ImVec2(ImMin(frame_bb.Min.x + max_frame_width, total_bb.Max.x), total_bb.Max.y);

        if (hide_name)
            text_bb.Max.x = text_bb.Min.x;

        // Return the total bounding box of the widget
        return total_bb;
    }

    ImU32 GUI::GetFrameColor(bool hovered, bool active)
    {
        GuiContext* ctx = GetGuiContext();

        // Dark frame style 
        if (ctx->currentItemFlags & GuiItemFlags_FrameStyleDark) {
            static constexpr ImVec4 hovered_col = { YART_GUI_COLOR_DARKER_GRAY, YART_GUI_ALPHA_OPAQUE };
            static constexpr ImVec4 bg_col = { YART_GUI_COLOR_DARKEST_GRAY, YART_GUI_ALPHA_OPAQUE };

            return ImGui::GetColorU32((hovered || active) ? hovered_col : bg_col);
        }

        // Light (default) frame style
        return ImGui::GetColorU32(active ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    }

} // namespace yart
