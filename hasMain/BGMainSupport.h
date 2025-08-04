#pragma once

#include "Windows.h"

bool bg_EnableDocking();
bool bg_EnableViewPorts();

void BG_GetWinInfo(HWND hwnd);

void bg_LoadWindowParamsFromAppIni(float& pos_x, float& pos_y, float& ws_x, float& ws_y);
void bg_SaveWindowParamsToAppIni();

void bg_ShowDesktopResolution();

bool bg_LoadFonts();