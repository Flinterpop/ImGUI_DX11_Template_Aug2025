
#include "BGMainSupport.h"

#include <map>
#include <filesystem>


#include "BGUtil.h"
#include "AppIni.h"
#include "Fonts/Fonts.h"

#include "imgui_internal.h" //for ImGuiContext



static std::map<std::string, ImFont*> Fonts;  // font map
static float fontScale = 13.0;


static int horizontalRes = 0;
static int verticalRes = 0;

static bool b_useViewPorts = false;
static bool b_enableDocking = false;

bool bg_IsDockingEnabled()
{
    return b_enableDocking;
}

void bg_SetDocking(bool val)
{
    b_enableDocking = val;
}


bool bg_AreViewPortsEnabled()
{
    return b_useViewPorts;
}


void bg_EnableViewPorts(bool val)
{
    b_useViewPorts = val;
}



static float pos_x;
static float pos_y;
static float ws_x;
static float ws_y;



void BG_GetWinInfo(HWND hwnd)
{
    //from https://stackoverflow.com/questions/615551/how-to-get-width-and-height-from-createwindowex-window-c
    RECT rect2;
    if (GetWindowRect(hwnd, &rect2))
    {
        ws_x = rect2.right - rect2.left;
        ws_y = rect2.bottom - rect2.top;
        pos_x = rect2.left;
        pos_y = rect2.top;
        //putsGreen("\Save Window Position: pos_x = %0.0f pos_y = %0.0f", pos_x, pos_y);
        //putsGreen("\Save Window size: ws_x = %0.0f ws_y = %0.0f", ws_x, ws_y);
    }
    else
    {
        DWORD retVal = GetLastError();
        putsRed("GetWindowRect has failed");
        //std::cout << retVal << std::endl;
    }
}




void bg_SaveWindowParamsToAppIni()
{
    putsBlue("\tSave Window Position: pos_x = %0.0f pos_y = %0.0f", pos_x, pos_y);
    putsBlue("\tSave Window size: ws_x = %0.0f ws_y = %0.0f", ws_x, ws_y);

    UpdateIniBool("useViewPorts", b_useViewPorts);
    UpdateIniBool("enableDocking", b_enableDocking);
    UpdateIniReal("WinPos_X", pos_x);
    UpdateIniReal("WinPos_Y", pos_y);
    UpdateIniReal("WinSize_X", ws_x);
    UpdateIniReal("WinSize_Y", ws_y);

    ImGuiContext& g = *GImGui;
    UpdateIniReal("fontScale", g.Style.FontSizeBase);

    putsGreen("Saving g.Style.FontSizeBase as %f", g.Style.FontSizeBase);
}



void bg_LoadWindowParamsFromAppIni(float &pos_x, float & pos_y, float &ws_x, float &ws_y)
{
    fontScale = GetIniReal("fontScale", 15.0);
    fontScale = std::clamp(fontScale, 5.0f, 40.0f);

    b_useViewPorts = GetIniBool("useViewPorts", false);
    b_enableDocking = GetIniBool("enableDocking", false);
    //putsRed("b_useViewPorts is %c b_enableDocking is %c", b_useViewPorts ? 'T' : 'F', b_enableDocking?'T':'F');


    pos_x = GetIniReal("WinPos_X", 100);
    pos_y = GetIniReal("WinPos_Y", 100);
    if (pos_x > horizontalRes)
    {
        putsRed("Window X position off screen - setting to 100");
        pos_x = 100.0;
    }
    if (pos_y > verticalRes)
    {
        putsRed("Window Y position off screen - setting to 100");
        pos_y = 100.0;
    }

    ws_x = GetIniReal("WinSize_X", 1280);
    ws_y = GetIniReal("WinSize_Y", 800);
    if (ws_x < 400) ws_x = 1280;
    if (ws_y < 400) ws_y = 800;

    putsBlue("\tWindow Position: pos_x = %0.0f pos_y = %0.0f", pos_x, pos_y);
    putsBlue("\tWindow size: ws_x = %0.0f ws_y = %0.0f", ws_x, ws_y);
}



void bg_GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;


    const long nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    const long nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    horizontal = nScreenWidth;
    vertical = nScreenHeight;
}


void bg_ShowDesktopResolution()
{
    bg_GetDesktopResolution(horizontalRes, verticalRes);
    putsBlue("GetDesktopResolution\r\n\thorizontal: %d vertical: %d", horizontalRes, verticalRes);
}







// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
bool bg_LoadFonts()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FontSizeBase = fontScale; // 15.0f;  //startpoint
    style._NextFrameFontSizeBase = style.FontSizeBase; // FIXME: Temporary hack until we finish remaining work.

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = 14.0f;
    icons_config.GlyphOffset = ImVec2(0, 0);
    icons_config.OversampleH = 1;
    icons_config.OversampleV = 1;
    icons_config.FontDataOwnedByAtlas = false;

    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    
    ImStrncpy(font_cfg.Name, "Roboto Regular", 40);
    Fonts[font_cfg.Name] = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 15.0f, &font_cfg);
    
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
    config.FontDataOwnedByAtlas = false; //added to stop crashing when shutting down
    io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config, fa_ranges);
    

#ifndef RELEASE_VERSION
    if (std::filesystem::exists("fonts//roboto-medium.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//roboto-medium.ttf");
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config, fa_ranges);
    }

    if (std::filesystem::exists("fonts//verdana.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//verdana.ttf");
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config, fa_ranges);
    }

    if (std::filesystem::exists("fonts//16020_FUTURAM.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//16020_FUTURAM.ttf");//, 15.0f);
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config);
    }


    if (std::filesystem::exists("fonts//DroidSansMono.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//DroidSansMono.ttf");
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config);
    }

    if (std::filesystem::exists("fonts//Cousine-Regular.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//Cousine-Regular.ttf");
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config);
    }

    if (std::filesystem::exists("fonts//Anonymous.ttf"))
    {
        Fonts[font_cfg.Name] = io.Fonts->AddFontFromFileTTF("fonts//Anonymous.ttf");
        io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 15.0f, &config);
    }
#endif

    return false;//no errors
}








