#include "windows.h"
#include <chrono>

#include "imgui.h"
#include "fonts\IconsFontAwesome5.h"

#include <string>
#include "stdio.h"
#include "BGUtil.h"




class UpTimeTracker
{
public:
    long long startTime;
    bool started = false;

    char m_StartTimeS[40];

    UpTimeTracker()
    {
        // Example of the very popular RFC 3339 format UTC time
        std::time_t time = std::time({});
        strftime(m_StartTimeS, 40, "Start DTG: %d %b %Y %Tz", std::gmtime(&time));
        putsGreen("Application Starting at %s", m_StartTimeS);

        //startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        started = true;
    }

public:
    long long getUptimeString(char * buf)
    {
        if (false == started)
        {
            putsRed("Uptimer not initialized\r\n");
            return 0;
        }

        //long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

        long long uptimeSecs = timeSinceEpoch - startTime;
        //int _secs = (int)(uptimeSecs / 1000);
        int _secs = (int)uptimeSecs;

        int hours = _secs / 3600;
        int mins = (_secs % 3600) / 60;
        int secs = (int)(_secs - (hours * 3600) - (mins * 60));
        sprintf(buf, "Uptime: %02d:%02d:%02d", hours, mins, secs);

        return timeSinceEpoch - startTime;
    }

    char * printStartTimeString()
    {
        if (false == started)
        {
            putsRed("Start Timer not initialized\r\n");
            return 0;
        }
        return m_StartTimeS;
    }



};


UpTimeTracker _UpTimeTracker;


void ShowAbout(bool *bAboutWindow)
{
    ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("About", bAboutWindow,_flags);
    
    //ImGui::PushItemWidth(35.0f * ImGui::GetFontSize());
        static const char* label = ICON_FA_BUG " ImGuiDX11 Template - " __DATE__;
        ImGui::Text(label);
        ImGui::Text("Written by B. Graham");
        ImGui::Text("Based on ImGui "); ImGui::SameLine(); ImGui::TextLinkOpenURL("https://github.com/ocornut/imgui", "https://github.com/ocornut/imgui");

        ImGui::Separator();
//    ImGui::PopItemWidth();
 

    ImGui::Separator();
    ImGui::TextWrapped("This is a starting project for IMGui Apps running on DirectX11.");
    ImGui::TextWrapped("It includes: ");
    ImGui::TextWrapped("\t-the FMT library.");
    ImGui::TextWrapped("\t-App State saving via SQLite.");
    ImGui::TextWrapped("\t-Help Window with context sensitive help option.");
    ImGui::TextWrapped("\t-About Window with uptimer");

    ImGui::Separator();

    
    
    ImGui::Text(_UpTimeTracker.printStartTimeString());
    char utime[40];
    auto ut = _UpTimeTracker.getUptimeString(utime);
    ImGui::Text(utime);

    if (ImGui::Button("Close")) *bAboutWindow = false;

    if (ImGui::CollapsingHeader("RoadMap"))
    {
        ImGui::SeparatorText("Large Features");
        ImGui::Text("TBD");

        ImGui::SeparatorText("Smaller Features");
        ImGui::Text("Font Awesome only works with Font loaded from Memory");
    }

    ImGui::End();
}
