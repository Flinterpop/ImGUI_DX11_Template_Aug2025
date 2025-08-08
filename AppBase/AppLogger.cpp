

#include <fstream>  //for time
#include <vector>
#include <imgui.h>

#include "AppLogger.h"



static std::vector<std::string> lines;


static inline std::string getCurrentDateTime(std::string s) {
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80] = "";
    tstruct = *localtime(&now);
    if (s == "now") strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date") strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};



void LogToFile(std::string logMsg)
{
    std::string filePath = "logs//AppLog_" + getCurrentDateTime("date") + ".log";
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);

    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
}

void AddLog(const char* fmt, ...)
{
    if (lines.size() > 500) lines.clear();

    va_list args;
    va_start(args, fmt);
    char buf[200];
    vsprintf(buf, fmt, args);
    lines.emplace_back(std::string(buf));
    va_end(args);
}

void ShowLogWindow(bool* p_open)
{
    ImGui::Begin("App Log", p_open);

    if (ImGui::Button("Clear")) lines.clear();
    //ImGui::SameLine();
    //bool copy = ImGui::Button("Copy");


    if (ImGui::Button("Write Test Item"))
    {
        AddLog("Log Line Test item");
        LogToFile("It was the best of times and so on");
    }

    ImGui::Separator();

    for (auto s : lines)
    {
        ImGui::TextWrapped(s.c_str());
    }


    ImGui::End();
}



