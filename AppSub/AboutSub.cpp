#include "imgui.h"

#include <string>
#include "stdio.h"

#include "GAMS2App.h"

#include "BGUtil.h"

extern const char* g_AppName;  //defined in hasMainImGuiDX11.cpp




void GAMS2::SpecificAbout()
{
    ImGui::TextWrapped("This is a starting project for IMGui Apps running on DirectX11.");
    ImGui::TextWrapped("It includes: ");
    ImGui::TextWrapped("\t-the FMT library.");
    ImGui::TextWrapped("\t-App State saving via SQLite.");
    ImGui::TextWrapped("\t-Help Window with context sensitive help option.");
    ImGui::TextWrapped("\t-About Window with uptimer");
}

void GAMS2::RoadMap()
{
    //Start CUSTOM CODE here
    ImGui::SeparatorText("Large Features");
    ImGui::Text("TBD");

    ImGui::SeparatorText("Smaller Features");
    //End CUSTOM CODE here
}


