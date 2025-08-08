#include "imgui.h"
#include <imgui_internal.h>

#include "fonts\IconsFontAwesome5.h"

#include "ImBGutil.h"
#include "BGMainSupport.h"  //needed for viewport and docking options

#include "ImGuiAPP.h"


extern bool glob_bInTest;       //in ImBGUtil

//extern int smallFontIndex;

// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string.
// Useful for quick combo boxes where the choices are known locally.
bool ShowStyleSelector(const char* label)
{
    static int style_idx = 1;
    if (ImGui::Combo(label, &style_idx, "Dark\0Light\0Classic\0"))
    {
        switch (style_idx)
        {
        case 0: ImGui::StyleColorsDark(); break;
        case 1: ImGui::StyleColorsLight(); break;
        case 2: ImGui::StyleColorsClassic(); break;
        }
        return true;
    }
    return false;
}

void ImGuiApp::ShowAppOptions(bool *bOptionsWindow)
{
    ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; //ImGuiWindowFlags_None;
    ImGui::Begin("Application Level Options ", bOptionsWindow,_flags);
       
    ImGui::Checkbox("Test Mode", &glob_bInTest);
        
    ImGui::Separator();
        
    bool useViewPorts = bg_AreViewPortsEnabled();
    if (ImGui::Checkbox("Enable Viewports - requires restart", &useViewPorts)) bg_EnableViewPorts(useViewPorts);
    
    bool enableDocking = bg_IsDockingEnabled();
    if (ImGui::Checkbox("Enable Docking - requires restart", &enableDocking)) bg_SetDocking(enableDocking);
    
    ImGui::PushItemWidth(12.0f * ImGui::GetFontSize());
    if (ShowStyleSelector("Colors##Selector"))
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameBorderSize = 1.0;
    }

    ImGui::SeparatorText("Main Font");
    ImGui::ShowFontSelector("Fonts##Selector");

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    ImGuiStyle& style = g.Style;

    if (ImGui::InputFloat("FontSizeBase", &style.FontSizeBase, 1.00f, 2.0f)) style._NextFrameFontSizeBase = style.FontSizeBase; // FIXME: Temporary hack until we finish remaining work.
    if (ImGui::DragFloat("FontSizeBased", &style.FontSizeBase, 0.20f, 5.0f, 100.0f, "%.0f")) style._NextFrameFontSizeBase = style.FontSizeBase; // FIXME: Temporary hack until we finish remaining work.

    ImGui::SameLine(0.0f, 0.0f); ImGui::Text(" (out %.2f)", ImGui::GetFontSize());

    ImGui::PopItemWidth();

    ImGui::Separator();
    InsertSpecificOptions();
    ImGui::Separator();

    static bool bShowFontAtlas = false;
    ImGui::Checkbox("Show Font Atlas", &bShowFontAtlas);
   

    ImGui::Separator();
    if (ImGui::Button(ICON_FA_WINDOW_CLOSE " Close")) *bOptionsWindow = false;

    ImGui::End();


    if (bShowFontAtlas)
    {
        ImGuiWindowFlags _flags = ImGuiWindowFlags_AlwaysAutoResize; 
        ImGui::Begin("Font Atlas", &bShowFontAtlas, _flags);
        
        ImGuiIO& io = ImGui::GetIO();
        ImFontAtlas* atlas = io.Fonts;
        HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
        ImGui::ShowFontAtlas(atlas);

        ImGui::End();
    }



}


