#pragma once

#include <windows.h>


//int ConnectPlatformJ_ViaThread();
int DisConnectPlatform_J();



void DoPlatform_JConnect();
static void registerFxMs();
//static DWORD WINAPI TCPListenThread(LPVOID lpParam);
//static void OpenSocketAndStartListenThreadThread(LPVOID lpParameter);


static bool parseRxPacket(unsigned char* buffer, int bufLength);
//static bool SendTCP(char* buffer, int len);


