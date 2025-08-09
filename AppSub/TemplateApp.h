#pragma once

#include <ImGuiAPP.h>

#include <ImageDX.h>




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
	void ShowAllMenuBars() override;
	void CheckKeysPressed() override;
	void SpecificAbout() override;
	void RoadMap() override;
	void InsertSpecificOptions() override;


	//void DrawToolBar();
	//void DrawToast();

		
	public: 
		void InitializeApp() override;
		void ShutDownApp() override;
		void UpdateApp() override;


		void LoadMainAppStateFromApp_Ini();
		void SaveMainAppStateFromApp_Ini();

		void MainDraw();


		void SocketTest();

		/*

		
		static SOCKET TCPClientSocket;
		char TCP_Host[30] = "192.168.1.135";
		int TCP_Port = 30154;
		bool b_TCP_SocketOpened = false;
		bool b_TCPThreadIsListening = false;
				
		void StartTCPOpenThread() {
			CreateThread(NULL, 0, m_StaticThreadEntry, this, 0, NULL);
		}

		static DWORD WINAPI m_StaticThreadEntry(LPVOID lpParam) {
			TemplateApp* pThis = static_cast<TemplateApp*>(lpParam);
			pThis->m_OpenTCPSocket(lpParam);
			return 0;
		}

		//https://www.bogotobogo.com/cplusplus/multithreading_win32A.php
		void m_OpenTCPSocket(LPVOID lpParameter)
		{
			TCPClientSocket = OpenTCPServerSocket((char*)TCP_Host, TCP_Port);
			if (EXIT_FAILURE == TCPClientSocket)
			{
				printf("Could not open TCP Client Socket (%s:%d)\r\n", TCP_Host, TCP_Port);
			}

			HANDLE hThread = CreateThread(NULL, 0, TCPListenThread, (LPVOID)&TCPClientSocket, 0, NULL);
			if (hThread == NULL)
			{
				printf("OpenSocketThread not created\r\n");
			}

		}


		static DWORD WINAPI TCPListenThread(LPVOID lpParam) {
			TemplateApp* pThis = static_cast<TemplateApp*>(lpParam);
			pThis->TCPListen(lpParam);

			return 0;
		}


		void TCPListen(LPVOID lpParameter)
		{
			char buffer[1510];
			SOCKET* TCPClientSocket = (SOCKET*)lpParameter;

			b_TCPThreadIsListening = true;
			while (b_TCPThreadIsListening) {
				int n = ::recv(*TCPClientSocket, buffer, 1510, 0);

				if (SOCKET_ERROR == n) continue;

				printf("TCP Rx %d bytes\r\n", n);
				if ((n > 0) && (n < 1510))
				{
					buffer[n] = 0;
					if (false)
					{
						printf("%s", buffer); //doesn't flush without a newline
						fflush(stdout);// Force writing buffer to the stdout
					}


					//Do Stuff to the packet
					AddLog("Rx TCP Packet");
					
				}
			}
			if (b_TCP_SocketOpened) closesocket(*TCPClientSocket);
			b_TCP_SocketOpened = false;

			b_TCPThreadIsListening = false;

		}


		*/


};

