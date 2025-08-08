#pragma once

#include <Windows.h>
#include <string>

#include <filesystem>

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/color.h>



namespace fs = std::filesystem;


std::wstring convertUTF8ToWstring(const std::string& utf8_string);

bool GetOpenFileNameWithUtf8Support(HWND hwnd, char* fpath, char* initialFolder = nullptr);
bool GetSaveFileNameWithUtf8Support(HWND hwnd, char* fname, char* initialFolder = nullptr);

std::string utf16_to_utf8_winapi(const std::wstring& utf16_string);
int utf16_to_utf8(const std::wstring& utf16_string, char* dest);

void UnixTimeToFileTime(time_t t, LPFILETIME pft);
std::uintmax_t calculateTotalFileSize(const fs::path& directory);

int GetSecondsSinceMidnightZulu();
void GetHMSSinceMidnightZulu(int& hrs, int& mins, int& secs);

bool getFileNameNoExtension(const char* path, char* dest);
bool getFileNameExtension(const char* path, char* dest);

std::vector<std::string> LoadFileNamesInFolder(char* folderName, char* fileType);

float degreesToMils(float deg);


void putsColor(fmt::color col, const char* fmt, ...);
void putsRed(const char* fmt, ...);
void putsGreen(const char* fmt, ...);
void putsYellow(const char* fmt, ...);
void putsSilver(const char* fmt, ...);
void putsBlue(const char* fmt, ...);

void HexDump(int bufLen, unsigned char* buf);