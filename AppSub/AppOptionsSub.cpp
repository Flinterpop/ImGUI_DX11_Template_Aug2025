#include "imgui.h"

#include "TemplateApp.h"



void TemplateApp::InsertSpecificOptions()
{
    bool someBool = false;
    ImGui::Checkbox("App Specific Dummy", &someBool);
}
