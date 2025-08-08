#pragma once

#include <string>


void ShowLogWindow(bool* p_open);
void LogToFile(std::string logMsg);
void AddLog(const char* fmt, ...);

