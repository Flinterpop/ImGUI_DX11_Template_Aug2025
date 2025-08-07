
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

void ShowAbout(bool* bAboutWindow);  //protoype from About.cpp
extern ID3D11DeviceContext* g_pd3dDeviceContext;


void TemplateApp::InitializeApp()
{
    LoadMainAppStateFromApp_Ini();

    
    
    
    

    char* fname = "SFGPEVUL--MOCAG.png";
    if (SymbolImage.LoadFromFile(fname))
    {
        putsGreen("Loaded %s\r\nWidth: %d Height: %d\r\n", fname, SymbolImage.Width, SymbolImage.Height);
    }


  
    img = stbi_load(fname, &width, &height, &channels, 0);
    if (img == NULL) 
    {
        putsRed("Error in loading the image\n");
        return;
    }
    putsBlue("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);


    stbi_write_png("SFGPEVUL--MOCAG_2.png", width, height, channels, img, width * channels);





};

void TemplateApp::ShutDownApp()
{
    SaveMainAppStateFromApp_Ini();
}

void TemplateApp::writeImage()
{

    //from https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/

    size_t img_size = width * height * channels;
    // Convert the input image to sepia
    unsigned char* sepia_img = (unsigned char* )malloc(img_size);
    if (sepia_img == NULL) 
    {
        putsRed("Unable to allocate memory for the sepia image.\n");
        return;
    }
    
    // Sepia filter coefficients from https://stackoverflow.com/questions/1061093/how-is-a-sepia-tone-created
    for (unsigned char* p = img, *pg = sepia_img; p != img + img_size; p += channels, pg += channels)
    {
        *pg = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if (channels == 4) *(pg + 3) = *(p + 3);
    
    }

    stbi_write_png("SFGPEVUL--MOCAG_sepia.png", width, height, channels, sepia_img, width * channels);
    free(sepia_img);


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

void TemplateApp::MainDraw()
{
    DrawImPlotExample();

    ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)SymbolImage.my_texture, ImVec2(100,300), ImVec2(500,700), { 0,0 }, { 1,1 }, ImColor(255, 255, 255, 255));

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
    if (ImGui::IsKeyPressed(ImGuiKey_W)) { writeImage();}
}

