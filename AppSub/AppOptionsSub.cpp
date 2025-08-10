#include "imgui.h"

#include "GAMS2App.h"



void GAMS2::InsertSpecificOptions()
{
    bool someBool = false;
    ImGui::Checkbox("App Specific Dummy", &someBool);
}
