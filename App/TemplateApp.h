#pragma once


#include <ImGuiAPP.h>



class TemplateApp : public ImGuiApp
{
	
	#pragma region Region_1
		// items that are saved between runs in SQL Lite DB
		bool mb_map_debug = true;  
		bool mb_showAbout = false;
		bool mb_showHelp = false;
		bool mb_showOptions = false;
		bool mb_showLog = false;
		bool mb_showDemoWindow = true;


	#pragma endregion Region_1

public:



private:
	void ShowAllMenuBars();
	void CheckKeysPressed();
	//void DrawToolBar();
	//void DrawToast();

		
	public: 
		void InitializeApp() override;
		void ShutDownApp() override;
		void UpdateApp() override;
		void LoadMainAppStateFromApp_Ini();
		void SaveMainAppStateFromApp_Ini();
		void ShowAppOptions(bool* bOptionsWindow);

};



