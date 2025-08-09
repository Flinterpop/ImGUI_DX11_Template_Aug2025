#pragma once

#include "windows.h"
#include <filesystem>

#include "imgui.h"
#include <string>


namespace fs = std::filesystem;

#ifdef PIMENU 
int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, const char** items, int items_count, int* p_selected);
#endif

std::uintmax_t calculateTotalFileSize(const fs::path& directory);


void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs);

int GetSecondsSinceMidnightZulu();


void remove_spaces(char* s);

struct ImVec2bg
{
    float                                   x, y;
    constexpr ImVec2bg() : x(0.0f), y(0.0f) {}
    constexpr ImVec2bg(float _x, float _y) : x(_x), y(_y) {}
    float& operator[] (size_t idx) { IM_ASSERT(idx == 0 || idx == 1); return ((float*)(void*)(char*)this)[idx]; } // We very rarely use this [] operator, so the assert overhead is fine.
    float  operator[] (size_t idx) const { IM_ASSERT(idx == 0 || idx == 1); return ((const float*)(const void*)(const char*)this)[idx]; }
    ImVec2bg& operator+=(const ImVec2bg& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        return *this;
    }

};


void RoundButton(const char* label, ImVec2& textPos);
ImVec2 DrawBoxedText(const char* label, ImVec2& textPos, ImU32 FGColour = ImColor(0, 0, 0, 255), ImU32 TextColour = ImColor(0, 0, 0, 255), ImU32 BGColour = ImColor(255, 255, 255, 255),float fontScale = 0.8f);


bool RedButton(const char* name, const ImVec2& size = ImVec2(0, 0));
bool GreenButton(char* name, const ImVec2& size = ImVec2(0, 0));
bool StopStartToggleButton(char* name, bool& b_toggle);

void HelpMarker(const char* desc);

void UnixTimeToFileTime(time_t t, LPFILETIME pft);

void ImGui_TextRight(std::string text, int extra = 0);
void ImGui_TextCentre(std::string text);

