#pragma once

#include <ImGuiAPP.h>

#include <ImageDX.h>




class GAMS2 : public ImGuiApp
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
	void ShowAllMenuBars() override;
	void CheckKeysPressed() override;
	void SpecificAbout() override;
	void RoadMap() override;
	void InsertSpecificOptions() override;


	//void DrawToolBar();
	//void DrawToast();

	enum APPMODE {LOAD_FILE_EDITOR, TEST};
	enum APPMODE  AppMode = LOAD_FILE_EDITOR;

		
	public: 
		void InitializeApp() override;
		void ShutDownApp() override;
		void UpdateApp() override;


		void LoadMainAppStateFromApp_Ini();
		void SaveMainAppStateFromApp_Ini();

		void TestWindow();

		void StartTCPTest();
		void SendTCPTest();
		void StopTCPTest();

		void BottomStatusBar();

};

