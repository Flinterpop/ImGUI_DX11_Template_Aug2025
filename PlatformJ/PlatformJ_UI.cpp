
#include "bg_TCPSocket.h"

#include <string>

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/color.h>


#include "imgui.h"

#include "Platform_J.h"

#include "FOMSUpport.h"

//extern ADSBTCPSocket myPlatJ;

extern bool b_PlatformJ_SocketOpened;
//extern char Platform_J_Host[20];
//extern int Platform_J_TCP_Port;

extern int FOM_Counts[80];
extern int FIM_Counts[80];

extern struct _FOM03 FOM03;
extern struct _FOM04 FOM04;
extern struct _FOM05 FOM05;
extern struct _FOM06 FOM06;


void PrintFOM03()
{
	ImGui::Text("ETRNR: %c\r\n", FOM03.ETRNR ? 'T' : 'F');
	ImGui::Text("TG: %c\r\n", FOM03.TG ? 'T' : 'F');
	ImGui::Text("TF: %c\r\n", FOM03.TF ? 'T' : 'F');
	ImGui::Text("NKL: %c\r\n", FOM03.NKL ? 'T' : 'F');
	ImGui::Text("IPFF: %c\r\n", FOM03.IPFF ? 'T' : 'F');
	ImGui::Text("TOL: %c\r\n", FOM03.TOL ? 'T' : 'F');

	ImGui::Text("CurrentInitState: %d which is %s\r\n", FOM03.CurrentInitState, FOM03.CurrentInitState_SS[FOM03.CurrentInitState]);
	ImGui::Text("Init_Sets_Status: %d which is %s\r\n", FOM03.Init_Sets_Status, FOM03.InitSetsStatus_SS[FOM03.Init_Sets_Status]);
	ImGui::Text("BIT_Status: %d which is %s\r\n", FOM03.BIT_Status, FOM03.BS[FOM03.BIT_Status]);
	ImGui::Text("Init Set Status: %d which is %s\r\n", FOM03.Init_Sets_Status, FOM03.InitSetsStatus_SS[FOM03.Init_Sets_Status]);
	ImGui::Text("Net Entry Status: %d which is %s\r\n", FOM03.NetEntryStatus, FOM03.NE_STATUS[FOM03.NetEntryStatus]);
	ImGui::Text("NR - NAVIGATION RESET CONFIRMED: %c\r\n", FOM03.NR ? 'T' : 'F');
	ImGui::Text("Control PG_Status: %d which is %s\r\n", FOM03.ControlPG_Status, FOM03.CtrlPG_STATUS[FOM03.ControlPG_Status]);
	ImGui::Text("LTTI: %c\r\n", FOM03.LTTI ? 'T' : 'F');
	ImGui::Text("VOICE INDICATORS PORT 1 (VIP1): %d which is %s\r\n", FOM03.VIP1, FOM03.VIP1_STR[FOM03.VIP1]);
	ImGui::Text("Voice Channel A Status: %d which is %s\r\n", FOM03.VCASTAT, FOM03.VC_STATUS[FOM03.VCASTAT]);
	ImGui::Text("Voice Channel B Status: %d which is %s\r\n", FOM03.VCBSTAT, FOM03.VC_STATUS[FOM03.VCBSTAT]);
}


void _ShowPlatformJ(bool* pOpen)
{
    ImGui::Begin("Platform-J", pOpen);

	/*

	ImGui::PushItemWidth(10.0f * ImGui::GetFontSize());

	//sprintf(buf, "JREAP IP : % s", JREAP_IP);
	//ImGui::Text(buf); ImGui::SameLine(); HelpMarker("Can only change IP from DB");
	ImGuiInputTextFlags f = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsNoBlank;
	ImGui::InputText("Platform-J IP ", myPlatJ.TCP_Host, 24, f);

	ImGui::PopItemWidth();

	ImGui::PushItemWidth(10.0f * ImGui::GetFontSize());

	if (b_PlatformJ_SocketOpened)
	{
		ImGui::InputInt("Platform-J Port", &myPlatJ.TCP_Port, 1, 10, ImGuiInputTextFlags_ReadOnly); //ImGui::SameLine(); HelpMarker("Cannot Change port while any JREAP is running");
	}
	else
	{
		ImGui::InputInt("Platform-J Port", &myPlatJ.TCP_Port);
	}

	if (b_PlatformJ_SocketOpened)
	{
		if (ImGui::Button("Dis-Connect")) DisConnectPlatform_J();
	}
	else
	{
		if (ImGui::Button("Connect")) DoPlatform_JConnect();//ConnectPlatformJ_ViaThread();
	}


	std::string str;
	str = fmt::format("FOM03 Count {:d}", FOM_Counts[3]);
	ImGui::Text(str.c_str());

	str = fmt::format("FOM29 Count {:d}", FOM_Counts[29]);
	ImGui::Text(str.c_str());

	ImGui::Separator();

	PrintFOM03();
	*/

    ImGui::End();
}

