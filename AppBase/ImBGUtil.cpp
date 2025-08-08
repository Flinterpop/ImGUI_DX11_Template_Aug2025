
#include <string>
#include "ImBGUtil.h"
//#include "fonts/IconsFontAwesome5.h"

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/color.h>


bool glob_bInTest;

namespace fs = std::filesystem;

#ifdef PIMENU 
#include "imgui_internal.h"

// Return >= 0 on mouse release
// Optional int* p_selected display and update a currently selected item
int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected)
{
    int ret = -1;

    if (ImGui::BeginPopup(popup_id, ImGuiWindowFlags_NoDecoration))
    {
        const ImVec2 drag_delta = ImVec2(ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y);
        const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

        const ImGuiStyle& style = ImGui::GetStyle();
        const float RADIUS_MIN = 30.0f;
        const float RADIUS_MAX = 120.0f;
        const float RADIUS_INTERACT_MIN = 20.0f;    // Handle hit testing slightly below RADIUS_MIN
        const int ITEMS_MIN = 6;                    // If they are less than 6 items, we still make each item fill a 1/6 slice.

        // Draw background
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRectFullScreen();
        //draw_list->PathArcTo(center, (RADIUS_MIN + RADIUS_MAX) * 0.5f, 0.0f, IM_PI * 2.0f);
        //draw_list->PathStroke(IM_COL32(128, 128, 128, 128), ImDrawFlags_Closed, RADIUS_MAX - RADIUS_MIN);



        const float item_arc_span = 2 * IM_PI / ImMax(ITEMS_MIN, items_count);
        float drag_angle = ImAtan2(drag_delta.y, drag_delta.x);
        if (drag_angle < -0.5f * item_arc_span)
            drag_angle += 2.0f * IM_PI;
        //ImGui::Text("%f", drag_angle);    // [Debug]

        // Draw items
        int item_hovered = -1;
        for (int item_n = 0; item_n < items_count; item_n++)
        {
            const char* item_label = items[item_n];
            const float item_ang_min = item_arc_span * (item_n + 0.02f) - item_arc_span * 0.5f; // FIXME: Could calculate padding angle based on how many pixels they'll take
            const float item_ang_max = item_arc_span * (item_n + 0.98f) - item_arc_span * 0.5f;

            bool hovered = false;
            if (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN)
                if (drag_angle >= item_ang_min && drag_angle < item_ang_max)
                    hovered = true;

            bool selected = p_selected && (*p_selected == item_n);

            //draw solid segment
            draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max);
            draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min);
            draw_list->PathFillConvex(ImGui::GetColorU32(hovered ? ImGuiCol_HeaderHovered : selected ? ImGuiCol_HeaderActive : ImGuiCol_Header));

            //draw segment outline
            draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, item_ang_min, item_ang_max);
            draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, item_ang_max, item_ang_min);
            draw_list->PathStroke(ImColor(0, 0, 0, 255),0,2.0); //hovered ? ImGuiCol_HeaderHovered : selected ? ImGuiCol_HeaderActive : ImGuiCol_Header));

            ImVec2 text_size = ImGui::CalcTextSize(item_label);
            ImVec2 text_pos = ImVec2(
                center.x + cosf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
                center.y + sinf((item_ang_min + item_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);
            draw_list->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), item_label);

            if (hovered)
                item_hovered = item_n;
        }
        draw_list->PopClipRect();

        if (ImGui::IsMouseReleased(0))
        {
            ImGui::CloseCurrentPopup();
            ret = item_hovered;
            if (p_selected)
                *p_selected = item_hovered;
        }
        ImGui::EndPopup();
    }
    return ret;
}

#endif



void remove_spaces(char* s) {
    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}


void RoundButton(const char* label, ImVec2& textPos)
{
    // Calculate text size for background rectangle
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, label);

    // Draw white background rectangle with purple border
    ImGui::GetWindowDrawList()->AddCircleFilled(textPos, 20, 0xFF0000FF);
    ImGui::GetWindowDrawList()->AddCircle(textPos, 20, 0x000000FF);

    ImGui::GetWindowDrawList()->AddText(textPos, 0x000000FF, label);
}




ImVec2 DrawBoxedText(const char* label, ImVec2& textPos, ImU32 FGColour, ImU32 TextColour, ImU32 BGColour, float fontScale)
{
    // Calculate text size for background rectangle
    //ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * fontScale, FLT_MAX, 0.0f, label);
    ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize() * fontScale, FLT_MAX, 0.0f, label);



    // Draw background rectangle with  border
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(textPos.x - 2, textPos.y - 2),
        ImVec2(textPos.x + textSize.x + 2, textPos.y + textSize.y + 2),BGColour);

    ImGui::GetWindowDrawList()->AddRect(ImVec2(textPos.x - 2, textPos.y - 2),
        ImVec2(textPos.x + textSize.x + 2, textPos.y + textSize.y + 2),
        FGColour, //border
        0.0f,  // rounding
        0,     // flags
        1.0f   // thickness
    );

    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize()* fontScale, textPos, TextColour, label);
    return textSize;
}



//IMGUI HELPERS
void ImGui_TextRight(std::string text, int extra)
{
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - extra - ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
    if (posX > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(posX);
    ImGui::Text("%s", text);
}

void ImGui_TextCentre(std::string text)
{
    //auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
    auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2 - ImGui::CalcTextSize(text.c_str()).x / 2 - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);

    if (posX > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(posX);
    ImGui::Text("%s", text);
}


bool StopStartToggleButton(char* name, bool& b_toggle)
{
    char buf[30];
    if (b_toggle)
    {
        sprintf(buf, "Stop %s", name);
        if (RedButton(buf)) b_toggle = false;
    }
    else
    {
        sprintf(buf, "Start %s", name);
        if (GreenButton(buf)) b_toggle = true; 
    }
    return b_toggle;
}


bool RedButton(const char* name, const ImVec2& size_arg)
{
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    auto retVal = ImGui::Button(name, size_arg);
    ImGui::PopStyleColor(4);
    return retVal;
}


bool GreenButton(char* name, const ImVec2& size_arg)
{
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
    auto retVal = ImGui::Button(name, size_arg);
    ImGui::PopStyleColor(3);
    return retVal;
}



void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}



