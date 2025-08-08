
#include "ImGui.h"

#include "Fonts/font_awesome_5.h"

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/color.h>

#include "AppIni.h"
#include "BGUtil.h"

//#include "AppLogger.h"
//#include "ImBGUtil.h"

#include "TemplateApp.h"
#include "help.h"

#include "implot.h"

#include "ImageDX.h"

//#define STB_IMAGE_LOAD_IMPLEMENTATION
#include "stb_image.h"

#undef STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8


#include "stb_image_write.h"


//extern ID3D11DeviceContext* g_pd3dDeviceContext;


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
    putsBlue("Loading App State");
    
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



template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T)RAND_MAX;
    return min + scale * (max - min);
}

void TemplateApp::DrawImPlotExample()
{
    static double xs1[101], ys1[101], ys2[101], ys3[101];
    srand(0);
    for (int i = 0; i < 101; ++i) {
        xs1[i] = (float)i;
        ys1[i] = RandomRange(400.0, 450.0);
        ys2[i] = RandomRange(275.0, 350.0);
        ys3[i] = RandomRange(150.0, 225.0);
    }
    static bool show_lines = true;
    static bool show_fills = true;
    static float fill_ref = 0;
    static int shade_mode = 0;
    static ImPlotShadedFlags flags = 0;
    ImGui::Checkbox("Lines", &show_lines); ImGui::SameLine();
    ImGui::Checkbox("Fills", &show_fills);
    if (show_fills) {
        ImGui::SameLine();
        if (ImGui::RadioButton("To -INF", shade_mode == 0))
            shade_mode = 0;
        ImGui::SameLine();
        if (ImGui::RadioButton("To +INF", shade_mode == 1))
            shade_mode = 1;
        ImGui::SameLine();
        if (ImGui::RadioButton("To Ref", shade_mode == 2))
            shade_mode = 2;
        if (shade_mode == 2) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::DragFloat("##Ref", &fill_ref, 1, -100, 500);
        }
    }

    if (ImPlot::BeginPlot("Stock Prices")) {
        ImPlot::SetupAxes("Days", "Price");
        ImPlot::SetupAxesLimits(0, 100, 0, 500);
        if (show_fills) {
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("Stock 1", xs1, ys1, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PlotShaded("Stock 2", xs1, ys2, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PlotShaded("Stock 3", xs1, ys3, 101, shade_mode == 0 ? -INFINITY : shade_mode == 1 ? INFINITY : fill_ref, flags);
            ImPlot::PopStyleVar();
        }
        if (show_lines) {
            ImPlot::PlotLine("Stock 1", xs1, ys1, 101);
            ImPlot::PlotLine("Stock 2", xs1, ys2, 101);
            ImPlot::PlotLine("Stock 3", xs1, ys3, 101);
        }
        ImPlot::EndPlot();
    }
}


int Grid[16][32]{};

ImVec2 canvas_pTL{};
float cellWidth = 50.0f;

void drawCell(int r, int c)
{
    ImVec2 p1 = ImVec2(canvas_pTL.x + (float)(c * cellWidth), canvas_pTL.y + (r * cellWidth));
    ImVec2 p2 = ImVec2(p1.x + cellWidth, p1.y + cellWidth);
    if (1 == Grid[r][c]) ImGui::GetWindowDrawList()->AddRectFilled(p1, p2, IM_COL32(0, 0, 255, 255));
    ImGui::GetWindowDrawList()->AddRect(p1, p2, IM_COL32(0, 0, 0, 255));
}


#include <fstream> // Required for file stream operations
#include <iostream> // Required for console output

void readGrid()
{
    putsRed("Reading grid");
    std::ifstream is("output_text.txt");
    int cnt = 0, x;

    // Check if the file opening is successful
    if (!is) {
        putsRed("Failed to open the file");
        return;
    }

    for (int r = 0;r < 16;r++)
    {
        for (int c = 0;c < 32;c++)
        {
            int x; 
            is >> x;
            Grid[r][c] = x;
        }
    }

    // Close the file
    is.close();
}

void writeGrid()
{
    putsRed("Saving grid");
    std::ofstream outputFile("output_text.txt"); // Create or overwrite output_text.txt

    if (outputFile.is_open()) { // Check if the file was opened successfully
       
        for (int r = 0;r < 16;r++)
        {
            for (int c = 0;c < 32;c++)
            {
                outputFile << Grid[r][c] << std::endl;// << std::endl; // Write num1 and a newline
            }
        }

        outputFile.close(); // Close the file
        putsRed("Integers written to output_text.txt successfully");
    }
    else {
        putsRed("Error opening file for writing.");
    }
}





void PrintMaze()
{
    printf("Start here\r\n");

    std::ofstream outputFile("maze.cpp"); // Create or overwrite output_text.txt
    if (outputFile.is_open())
    { // Check if the file was opened successfully

        outputFile << "#include \"RayAppBG.h\"\r\n";

        outputFile << "void RayAppBG::SetGrid() { ";
        

        for (int r = 0;r < 16;r++)
        {
            for (int c = 0;c < 32;c++)
            {
                char buf[200];
                sprintf(buf, "Grid[%d][%d][0] = %d;\r\n", r, c, Grid[r][c]);
                outputFile << buf << std::endl;
            }
        }

        outputFile << "} ";

        outputFile.close(); // Close the file
    }
}


void clearGrid()
{
    printf("Start here\r\n");
    for (int r = 0;r < 16;r++)
    {
        for (int c = 0;c < 32;c++)
        {
            Grid[r][c] = 0;
        }
    }
}


void fillGrid()
{
    printf("Start here\r\n");
    for (int r = 0;r < 16;r++)
    {
        for (int c = 0;c < 32;c++)
        {
            Grid[r][c] = 1;
        }
    }


}

void TemplateApp::MainDraw()
{
    Grid[5][5] = 1;
    //DrawImPlotExample();

    //ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)SymbolImage.my_texture, ImVec2(100,300), ImVec2(500,700), { 0,0 }, { 1,1 }, ImColor(255, 255, 255, 255));



    canvas_pTL = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
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

    static int m_Zoom = 3;


    float mx = io.MousePos.x - canvas_pTL.x;
    float my = io.MousePos.y - canvas_pTL.y;
    int mCol = (int)(mx / cellWidth);
    int mRow = (int)(my / cellWidth);

    //if (mb_isMouseOverMap) //if mouse is over this window then check for Zoom
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_A)) 
        {
            Grid[mRow][mCol] = 1;

            //if (0 == Grid[mRow][mCol]) Grid[mRow][mCol] = 1;
            //else Grid[mRow][mCol] = 0;
            //Sleep(10);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_S))
        {
            Grid[mRow][mCol] = 0;
        }
   }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (0 == Grid[mRow][mCol]) Grid[mRow][mCol] = 1;
        else Grid[mRow][mCol] = 0;
    }



    //mouse position calculations
    double MouseDeltaXFromCentre = io.MousePos.x - m_canvas_Centre.x;
    double MouseDeltaYFromCentre = io.MousePos.y - m_canvas_Centre.y;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRect(canvas_pTL, canvas_pBR, IM_COL32(0, 0, 0, 255));


    for (int r = 0;r < 16;r++)
        for (int c = 0;c < 32;c++)
        {
            drawCell(r, c);
        }



    
    char buf[200];
    sprintf(buf,"TL: %d %d", (int)canvas_pTL.x, (int)canvas_pTL.y);
    draw_list->AddText(canvas_pTL, ImColor(0, 0, 0, 255), buf);
    
    sprintf(buf, "BR: %d %d", (int)canvas_pBR.x, (int)canvas_pBR.y);
    draw_list->AddText(ImVec2(canvas_pTL.x, canvas_pTL.y+30), ImColor(0, 0, 0, 255), buf);

    sprintf(buf, "Mouse Pos: %d %d", (int)mx, (int)my);
    draw_list->AddText(ImVec2(canvas_pTL.x, canvas_pTL.y + 60), ImColor(0, 0, 0, 255), buf);

    sprintf(buf, "Mouse Cell: %d %d", (int)mCol, (int)mRow);
    draw_list->AddText(ImVec2(canvas_pTL.x, canvas_pTL.y + 90), ImColor(0, 0, 0, 255), buf);






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
    //if (mb_showLog)         MyLog.Draw("App Log", &mb_showLog);
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

        if (ImGui::BeginMenu("Maze"))
        {
            if (ImGui::MenuItem("Load Maze")) { readGrid(); }
            if (ImGui::MenuItem("Write Maze cpp file")) { PrintMaze(); }
            if (ImGui::MenuItem("Save Maze")) { writeGrid(); }
            if (ImGui::MenuItem("Clear Maze")) { clearGrid(); }
            if (ImGui::MenuItem("Fill Maze")) { fillGrid(); }

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
    //if (ImGui::IsKeyPressed(ImGuiKey_W)) { writeImage();}
}

