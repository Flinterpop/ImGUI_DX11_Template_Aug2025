
#include "ImGui.h"

#include "Fonts/font_awesome_5.h"

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/color.h>

#include "AppIni.h"
#include "BGUtil.h"

#include "AppLogger.h"
//#include "ImBGUtil.h"

#include "TemplateApp.h"
#include "help.h"

#include "implot.h"

#include "ImGuiNotify.hpp"



void TemplateApp::InitializeApp()
{
    LoadMainAppStateFromApp_Ini();
};

void TemplateApp::ShutDownApp()
{
    SaveMainAppStateFromApp_Ini();
}

void TemplateApp::LoadMainAppStateFromApp_Ini()
{
    putsBlue("\tLoading App State");
    
    mb_map_debug = GetIniBool("map_debug", false);
    mb_showAbout = GetIniBool("showAbout", false);
    mb_showHelp = GetIniBool("showHelp", false);
    mb_showLog = GetIniBool("showLog", false);
    mb_showDemoWindow = GetIniBool("showDemoWindow", false);

    mb_showOptions = GetIniBool("mb_showOptions", false);

    char ff[40];
    std::string s = GetIniString("font", "");
    strcpy(ff, s.c_str());

    ImGuiIO& io = ImGui::GetIO();
    int _index = 0;
    for (ImFont* font : io.Fonts->Fonts)
    {
        if (!strcmp(font->GetDebugName(), ff))
        {
            io.FontDefault = font;
            //smallFontIndex = _index - 1;
        }
        _index++;
    }
    //if (smallFontIndex < 0) smallFontIndex = 0;

    putsBlue("\tCompleted Loading App State");
}


void TemplateApp::SaveMainAppStateFromApp_Ini()
{
    putsBlue("Saving App State");

    UpdateIniBool("map_debug", mb_map_debug);
    UpdateIniBool("showAbout", mb_showAbout);
    UpdateIniBool("showHelp", mb_showHelp);
    UpdateIniBool("showLog", mb_showLog);
    UpdateIniBool("showDemoWindow", mb_showDemoWindow);
    UpdateIniBool("mb_showOptions", mb_showOptions);

    ImFont* font_current = ImGui::GetFont();
    UpdateIniString("font", (char*)font_current->GetDebugName());

    putsBlue("\tCompleted Saving App State");
}




void TemplateApp::MainDraw()
{
    
    //this content provides a drawing context that is the entire window 

    ImVec2 canvas_pTL = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 m_canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available

    if (m_canvas_sz.x < 50.0f) m_canvas_sz.x = 50.0f;
    if (m_canvas_sz.y < 50.0f) m_canvas_sz.y = 50.0f;
    ImVec2 canvas_pBR = ImVec2(canvas_pTL.x + m_canvas_sz.x, canvas_pTL.y + m_canvas_sz.y);
    ImVec2 m_canvas_Centre = ImVec2((canvas_pBR.x - canvas_pTL.x) / 2 + canvas_pTL.x, (canvas_pBR.y - canvas_pTL.y) / 2 + canvas_pTL.y);

    // This will catch our mouse interactions
    ImGui::InvisibleButton("canvas", m_canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    bool mb_isMouseOverMap = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 g_displaySize = io.DisplaySize;


    //mouse position calculations
    double MouseDeltaXFromCentre = io.MousePos.x - m_canvas_Centre.x;
    double MouseDeltaYFromCentre = io.MousePos.y - m_canvas_Centre.y;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(canvas_pTL, canvas_pBR, IM_COL32(0, 0, 0, 255));


   




}



void TemplateApp::UpdateApp()
{
	CheckKeysPressed();



    //these 4 lines force the next ImGui::Begin to use the main viewport. 
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    std::string _MainWindowsTitle = "ImGuiDX11 Template"; //fmt::format("Map: {0} at Zm {1}  declutter: {2}###MainWindow", mngr.GetCurMapName().c_str(), m_Zoom, g_acmPtr->getDeclutter());

    ImGui::Begin(_MainWindowsTitle.c_str(), 0, mainWinFlags); //| ImGuiWindowFlags_NoResize);

    ShowAllMenuBars();

    MainDraw();

#pragma region DrawOverlays
    //DrawToolBar();
    //DrawToast();
#pragma endregion DrawOverlays


    ImGui::End();




#pragma region ShowOtherWindows
    if (mb_showLog)         ShowLogWindow(&mb_showLog);
    if (mb_showOptions)     ShowAppOptions(&mb_showOptions);
    if (mb_showAbout)       ShowAbout(&mb_showAbout);
    if (mb_showHelp)        ShowHelpWindow(&mb_showHelp);
    
    if (mb_showDemoWindow)  ImGui::ShowDemoWindow(&mb_showDemoWindow);
#pragma endregion OtherWindows



}


void TemplateApp::ShowAllMenuBars()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Re-Load App State"))
            {
                LoadMainAppStateFromApp_Ini();
                //DoLoadAllModulesStateFromApp_Ini();
            }
            ImGui::SetItemTooltip("Loads app.ini file.");

            if (ImGui::MenuItem("Save App State"))
            {
                SaveMainAppStateFromApp_Ini();
                //DoSaveAllModuleStateToApp_Ini();
            }
            ImGui::SetItemTooltip("Saves App (not scenario) state.  Creates new app.ini file if one does not exist.");


            ImGui::Separator();
            if (ImGui::MenuItem("App Options", NULL, mb_showOptions)) { mb_showOptions = !mb_showOptions; }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) ::PostQuitMessage(0);

            ImGui::EndMenu();
        }


        if (ImGui::BeginMenu("Test File Open/Close"))
        {
            static char TestFileName[MAX_PATH]{"*.*"};

            char *initialFolder = "logs";

            if (ImGui::MenuItem("Open LoadFile", NULL))
            {
                //std::string curdir = GetExeDirectory();
                //printf("CurDir is %s\r\n", curdir.c_str());

                bool fail = g_OpenFile(nullptr, TestFileName);
                if (false == fail) AddLog(TestFileName);
            }
            ImGui::SetItemTooltip("Open any sort of load file.");


            if (ImGui::MenuItem("Save LoadFile", NULL))
            {
                bool fail = g_SaveFile(nullptr, TestFileName);
                if (false == fail) AddLog(TestFileName);
            }
            ImGui::SetItemTooltip("Save current load file.");


            ImGui::EndMenu();
        }




        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Show Demo Window", NULL, mb_showDemoWindow)) { mb_showDemoWindow = !mb_showDemoWindow; }

            if (ImGui::MenuItem("Log", NULL, mb_showLog)) { mb_showLog = !mb_showLog; }
            if (ImGui::MenuItem("Debug", NULL, mb_map_debug)) { mb_map_debug = !mb_map_debug; }
            if (ImGui::MenuItem("Help", NULL, mb_showHelp)) { mb_showHelp = !mb_showHelp; }
            if (ImGui::MenuItem("About", NULL, mb_showAbout)) { mb_showAbout = !mb_showAbout; }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

}


void TemplateApp::CheckKeysPressed()
{
    if (ImGui::IsKeyPressed(ImGuiKey_W)) 
    { 
        ImGui::InsertNotification({ ImGuiToastType::Success, 3000, "That is a success! %s", "(Format here)" });
        ImGui::InsertNotification({ ImGuiToastType::Warning, 3000, "Hello World! This is a warning! %d", 0x1337 });
        ImGui::InsertNotification({ ImGuiToastType::Error, 3000, "Hello World! This is an error! 0x%X", 0xDEADBEEF });
        //ImGui::InsertNotification({ ImGuiToastType::Info, 3000, "Hello World! This is an info!" });
        ImGui::InsertNotification({ ImGuiToastType::Info, 3000, "Hi, I'm a long notification. I'm here to show you that you can write a lot of text in me. I'm also here to show you that I can wrap text, so you don't have to worry about that." });

        ImGui::InsertNotification({ ImGuiToastType::Error, 5000, "Click me!", []() {ImGui::InsertNotification({ImGuiToastType::Success, 3000, "Thanks for clicking!"});}, "Notification content" });
        
        
    }
}

