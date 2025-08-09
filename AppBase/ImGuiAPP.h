#pragma once

#include <string>

#include "ImGui.h"

#include "fonts\font_awesome_5.h"

class ImGuiApp
{
protected:
	ImGuiWindowFlags mainWinFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;// | ImGuiWindowFlags_NoSavedSettings);
	virtual void ShowAllMenuBars() {};
	virtual void CheckKeysPressed() {};
	
	void ShowAbout(bool* bAboutWindow);
	virtual void SpecificAbout() {};
	virtual void RoadMap() {};

	void ShowAppOptions(bool* bOptionsWindow);
	virtual void InsertSpecificOptions() {};


public:
	virtual void InitializeApp() {};
	virtual void ShutDownApp() {};
	virtual void UpdateApp() {};

	std::string AppName{ ICON_FA_GLOBE " Default Name " __DATE__};

};

