#pragma once

#include "ImGui.h"

class ImGuiApp
{
protected:
	ImGuiWindowFlags mainWinFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;// | ImGuiWindowFlags_NoSavedSettings);

public:
	virtual void InitializeApp() {};
	virtual void ShutDownApp() {};
	virtual void UpdateApp() {};


};

