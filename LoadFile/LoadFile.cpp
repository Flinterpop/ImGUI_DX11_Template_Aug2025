
#include "imgui.h"

#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>


#include "AppLogger.h"
#include "BGUtil.h"
#include "help.h"

#include "Loadfile.h"

class LoadFile myLoadFile;



unsigned int GetIntFromWord(int word, int startBit, int Length)
{
    --word;
	//startBit is 0 thru 15, lowest bit is startBit
	if (word >= myLoadFile.SDWs.size())
	{
	    AddLog("Word Index too large for Loadfile length");
		return 0;
	}
	unsigned int RF = myLoadFile.SDWs[word];
	unsigned int mask = 0b0000000000000001 << startBit ;
	unsigned int sum = 0;
	for (int i = 0; i < Length; i++)
	{
		if (0!=(RF & mask)) sum += (1 << i);
		mask = mask << 1;
	}
	return sum;
}


void UpdateIntInWord(int word, int startBit, int Length, int value)
{
    --word;
	//startBit is 0 thru 15, lowest bit is startBit
	unsigned int RF = value;
	unsigned int mask = 0b0000000000000001 << startBit ;

	unsigned int One = 0b0000000000000001;

	for (int i = 0; i < Length; i++)
	{
		if (0 != (RF & (One<<i)))
		{
            myLoadFile.SDWs[word] |= mask;  //assert the bit
		}
		else
		{
		unsigned int unmask = ~mask;
        myLoadFile.SDWs[word] = myLoadFile.SDWs[word] & unmask;  //clear the bit
		}
		mask = mask << 1;
 	}
 }


int getVal(char c)
    {
        if ((c>=0x30 ) && (c<=0x39))
        {
            return (c - 0x30);
        }
        if ((c>=0x41) && (c<=0x46))
        {
            return (c - 55);
        }
        return 0; //check this
    }


void AddFirstTwoColumns(char *caption,int word)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("SDW%d",word);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(caption);
    ImGui::TableSetColumnIndex(2);
}

void AddCheckBox(char *caption,int word, int bit)
{
    AddFirstTwoColumns(caption,word);
    bool val= (1==GetIntFromWord(word, bit, 1))?true:false; 

    if (ImGui::Checkbox(caption,&val))
    {
        UpdateIntInWord(word,bit,1, val);
    }
}



void AddNumBox(char *caption,int word, int bit, int NumBits,int min=0, int max=0)
{
    AddFirstTwoColumns(caption,word);
    int val= GetIntFromWord(word, bit, NumBits);

    char buf[60];
    sprintf(buf,"##%s",caption);    

    ImGui::SetNextItemWidth(ImGui::GetFontSize()*10);
    if (ImGui::InputInt(buf,&val))
    {
        if ((0 != min) || (0 != max)) val = std::clamp(val,min,max);
        UpdateIntInWord(word,bit,NumBits, val);
    }
}

char tt[20];
bool firstPass= true;

void AddNetworkName()
{
    AddFirstTwoColumns("Network Name",1800);
    char nn[17];
    nn[1] = GetIntFromWord(1800, 8, 8);
    nn[0] = GetIntFromWord(1800, 0, 8);
    nn[3] = GetIntFromWord(1801, 8, 8);
    nn[2] = GetIntFromWord(1801, 0, 8);
    nn[5] = GetIntFromWord(1802, 8, 8);
    nn[4] = GetIntFromWord(1802, 0, 8);
    nn[7] = GetIntFromWord(1803, 8, 8);
    nn[6] = GetIntFromWord(1803, 0, 8);
    nn[9] = GetIntFromWord(1804, 8, 8);
    nn[8] = GetIntFromWord(1804, 0, 8);

    nn[11] = GetIntFromWord(1805, 8, 8);
    nn[10] = GetIntFromWord(1805, 0, 8);
    nn[13] = GetIntFromWord(1806, 8, 8);
    nn[12] = GetIntFromWord(1806, 0, 8);
    nn[15] = GetIntFromWord(1807, 8, 8);
    nn[14] = GetIntFromWord(1807, 0, 8);
    nn[16] = 0;
    if (firstPass) 
    {
        strcpy(tt,nn); 
        firstPass =false;
    }

    ImGui::SetNextItemWidth(ImGui::GetFontSize()*10);
    ImGui::InputText("##NN",tt,16);
    if (strcmp(nn,tt))
    {
        ImGui::SameLine();
        if (ImGui::Button("Commit Change"))
        {
            AddLog("nn[0] is %c  %d",nn[0],nn[0]);
            
            UpdateIntInWord(1800,8,8, tt[0]);
            UpdateIntInWord(1800,0,8, tt[1]);
            UpdateIntInWord(1801,8,8, tt[2]);
            UpdateIntInWord(1801,0,8, tt[3]);
            UpdateIntInWord(1802,8,8, tt[4]);
            UpdateIntInWord(1802,0,8, tt[5]);
            UpdateIntInWord(1803,8,8, tt[6]);
            UpdateIntInWord(1803,0,8, tt[7]);
            UpdateIntInWord(1804,8,8, tt[8]);
            UpdateIntInWord(1804,0,8, tt[9]);
            UpdateIntInWord(1805,8,8, tt[10]);
            UpdateIntInWord(1805,0,8, tt[11]);
            UpdateIntInWord(1806,8,8, tt[12]);
            UpdateIntInWord(1806,0,8, tt[13]);
            UpdateIntInWord(1807,8,8, tt[14]);
            UpdateIntInWord(1807,0,8, tt[15]);  
            firstPass=false;      
        }
    }


}

void AddSTNBox(char *caption,int word, int bit, int NumBits)
{
    AddFirstTwoColumns(caption,word);
    int val= GetIntFromWord(word, bit, NumBits);

    char buf[60];
    sprintf(buf,"##%s",caption);    

    ImGui::SetNextItemWidth(ImGui::GetFontSize()*10);
    if (ImGui::InputInt(buf,&val))
    {
        val = std::clamp(val,0,32767);
        UpdateIntInWord(word,bit,NumBits, val);
    }
    ImGui::SameLine();
    sprintf(buf,"%d (decimal) = %x (hex) = %o (Octal)",val,val,val);
    ImGui::Text(buf);
}



bool AddPickList(char* caption, const char* Choices[], int numChoices, int word, int bit, int NumBits, int size =12)
{
    AddFirstTwoColumns(caption, word);
    int val= GetIntFromWord(word, bit, NumBits);

	const char* combo_preview_value = Choices[val];
	if (0 != size) ImGui::SetNextItemWidth(ImGui::GetFontSize() * size);
		
    char buf[60];
    sprintf(buf,"##%s",caption);    
    bool retVal = ImGui::BeginCombo(buf, combo_preview_value);
	if (retVal)
	{
		for (int n = 0; n < numChoices; n++)
		{
			const bool is_selected = (val == n);
			ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
			if (ImGui::Selectable(Choices[n], is_selected)) val = n;
			if (is_selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
        UpdateIntInWord(word,bit,NumBits, val);
	}
	return retVal;
}


void BuildUI()
{
    //word numbers start from 1 not 0
    const char *CommModes[] = {"Reserved for Terminal use (LVT2)","Mode 1","Mode 2","Mode 4"};
    int _length = sizeof(CommModes) / sizeof(CommModes[0]);
    AddPickList("Comm Mode", CommModes, _length, 1, 0, 2);

    AddCheckBox("Extended Range",1, 2);
    
    const char *IPFModes[] = {"Full EMC Protection", "Exercise EMC Protection", "Combat EMC Protection","Full EMC Protection 2"};  //bug in ICD??
    _length = sizeof(IPFModes) / sizeof(IPFModes[0]);
    AddPickList("IPF Mode", IPFModes, _length, 1, 3, 2);

    AddCheckBox("TACAN",1, 7);
    AddCheckBox("L16",1, 8);
    AddCheckBox("LTTI",1, 9);
    
    const char *TxModes[] = {"Not Used","Normal","Polling","TDMA Silent","Conditional Radio Silence","Not Used##1","Not Used##2","Reserved" };
    _length = sizeof(TxModes) / sizeof(TxModes[0]);
    AddPickList("Transmit Mode", TxModes, _length, 1, 10, 3);

    const char *TestModes[] = {"No Test Messages" , "Test Mode 1", "Test Mode 2" , "Not Used"};
    _length = sizeof(TestModes) / sizeof(TestModes[0]);
    AddPickList("Test Mode", TestModes, _length, 1, 13, 2);

    AddSTNBox("STN",2, 0, 15);


    const char *Nav[] = {"Not Used","Secondary User","Primary User","Not Used##1", "Reserved", "Reserved##2","Not Used##2"};
    _length = sizeof(Nav) / sizeof(Nav[0]);
    AddPickList("Nav", Nav, _length, 3, 10, 3);
    
    AddCheckBox("PR",3, 3);
    AddCheckBox("NTR",3, 4);
    
    const char *PPLIPool[] = {"Pool (A+B)", "Pool B"};
    _length = sizeof(PPLIPool) / sizeof(PPLIPool[0]);
    AddPickList("PPLI Pool", PPLIPool, _length, 3, 5, 1);

    const char *Power[] = {"Normal Power", "Low Power","High Power (HPA)","Mixed Power","Medium Power","Low Power (HPA)"};
    _length = sizeof(Power) / sizeof(Power[0]);
    AddPickList("Power", Power, _length, 3, 10, 3);
    
    AddCheckBox("Antenna Switch",3, 14);

    const char *ONMRMode[] = {"Accept all ONMR", "Reject All ONMR"};
    _length = sizeof(ONMRMode) / sizeof(ONMRMode[0]);
    AddPickList("ONMR", ONMRMode, _length, 4, 10, 1);

    AddCheckBox("Initial Entry Group",4, 9);
    AddCheckBox("REKEYING PROCESSING STATUS (RPS)",4, 8);
    AddCheckBox("OVER THE AIR REKEYING (OTAR) ",4, 7);
    AddNumBox("Sequence Number",4, 3, 3, 0,7);
    AddCheckBox("NET ENTRY TRANSMIT ENABLE (NETE)",4, 2);
    AddCheckBox("EXTERNAL TIME REFERENCE (ETR)",4, 1);

    const char *TRPMode[] = {"Host Data Bus", "Support Port"};
    _length = sizeof(TRPMode) / sizeof(TRPMode[0]);
    AddPickList("TAPE RECORDER PORT SELECTION (TRP)", TRPMode, _length, 4, 0, 1);

    AddNumBox("Platform Strength",5, 3, 4, 0,15);

    const char *PlatTypes[] = {"Not Used", "Not Used ##1","Air","Surface Maritime","Subsurface","Ground Point","Ground Track","Not Used ##2"};
    _length = sizeof(PlatTypes) / sizeof(PlatTypes[0]);
    AddPickList("Platform Type", PlatTypes, _length, 5, 0, 3    );

    //two word values
    AddNumBox("?? Station Latitude",6, 3, 3, 0,7);

    //two's complement Integer
    AddNumBox("Station Position Antenna Height",10, 0, 16);

    AddCheckBox("Position Validity (PV)",11, 15);
    AddNumBox("?? Height Undcertainty (m)",11, 5, 5,0,18288);   //coded
    AddNumBox("?? Position Undertainty (m)",11, 0, 5,0,18288);  //coded

    AddNumBox("ETR Cable Delay (12.5ns)",25, 0, 8,0,255);  //coded


    AddCheckBox("Set Activity Indicator (SAI)",30, 15);
    AddNumBox("Set Index Number",30, 0, 8,0,255);  //coded

    AddCheckBox("Bail Out Indicator",48,8);
    AddCheckBox("Flight Lead Indicator",48, 7);
    AddCheckBox("Airborne Indicator",48, 6);
    AddCheckBox("Simulation Indicator",48, 5);
    AddCheckBox("Flight Cmd \& Ctrl Indicator",48, 4);
    AddCheckBox("Emergency Indicator",48, 3);
    AddCheckBox("Force Tell Indicator",48, 2);
    AddCheckBox("Displaced Position or Mission Commander Indicator",48, 1);
    AddCheckBox("Field Exercise Indicator",48, 0);

    AddNumBox("Platform",49, 7, 6,0,64);  //coded

    AddNumBox("Platform Activity",49, 0, 6,0,64);  //coded

    AddNetworkName();



}



const int bufsize = 65536;  //excessive size for stack
char buffer[bufsize];

void Load_LoadFile(LPWSTR fname)
{
    HANDLE hFile = CreateFile(
        //L"C:\\path\\to\\your\\file.txt", // File name
        fname, 
        GENERIC_READ, // | GENERIC_WRITE,    // Desired access (read and write)
        0,                               // Share mode (no sharing)
        NULL,                            // Security attributes
        OPEN_ALWAYS,                     // Creation disposition (create if not exists, open if exists)
        FILE_ATTRIBUTE_NORMAL,           // File attributes
        NULL                             // Template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        // Handle error
        AddLog("INVALID_HANDLE_VALUE");
        return;
    }

    //const int bufsize=65536;  //excessive size for stack
    //char buffer[bufsize];
    DWORD bytesRead;
    if (!ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL)) {
        // Handle error
        AddLog("ReadFile Error");
        CloseHandle(hFile);
        return;
    }
    
    CloseHandle(hFile);

    char buf[200];
    sprintf(buf,"Read file %d", bytesRead);
    AddLog(buf);

    int rowLength =5;

	if ((0x0d==buffer[4]) &&(0x0a==buffer[5]) )
	{
		rowLength=6;
        AddLog("File has Windows style line endings (0D 0A)");
        AddLog("Windows (0D 0A)");
	}
	else 
    {
        AddLog("File has Unix style line endings (0A)");
        AddLog("Linux (0A)");
	}
    
    myLoadFile.m_NumRows = bytesRead/rowLength;

	if (myLoadFile.m_NumRows==1890) {
        //TE_NumLines->Font->Color = clWindowText;
        AddLog("File has 1890 words (rows) - Not JTRS");
	}
	else if (myLoadFile.m_NumRows==5910) {
        AddLog("File has 5910 words (rows) - JTRS");
	}
	else
	{
        sprintf(buf, "File has %d rows:  invalid!!!", myLoadFile.m_NumRows);
        AddLog(buf);
	}


    sprintf(buf,"Sanity check: %c%c%c%c -> %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3],  buffer[0],  buffer[1], buffer[2], buffer[3]);
    AddLog(buf);


    myLoadFile.SDWs.clear();

	for (int row = 0; row < myLoadFile.m_NumRows; row++)
	{
		int index = row * rowLength;
		int _1 = getVal(buffer[index]);  //ascii value becomes integer
		int _2 = getVal(buffer[index+1]);
		int _3 = getVal(buffer[index+2]);
		int _4 = getVal(buffer[index+3]);

		int wordi = _1* 4096 + _2*256 + _3*16 +_4;

        myLoadFile.SDWs.push_back(wordi);
	}

    sprintf(buf,"SDWs has length %d",(int)myLoadFile.SDWs.size());
    AddLog(buf);


	//for (int i = 0; i < m_NumRows; i++)
    for (int i = 0; i < 10; i++)
	{
		sprintf(buf,"SDW[%04d]: %04X",i+1, myLoadFile.SDWs[i]);
        AddLog(buf);
	}

}

void Save_LoadFile(LPWSTR fname)
{
    AddLog("Saving to %s", fname);
    HANDLE hFile = CreateFile(
        fname,
        GENERIC_WRITE,    // Desired access (read and write)
        0,                               // Share mode (no sharing)
        NULL,                            // Security attributes
        OPEN_ALWAYS,                     // Creation disposition (create if not exists, open if exists)
        FILE_ATTRIBUTE_NORMAL,           // File attributes
        NULL                             // Template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        // Handle error
        AddLog("Error on gettng save file name");
        return;
    }

    DWORD bytesWritten;
    char buf[10];
    for (auto a : myLoadFile.SDWs)
    {
        sprintf(buf,"%04X\r\n",a);
        if (!WriteFile(hFile, buf, strlen(buf), &bytesWritten, NULL)) 
            {
                // Handle error
                AddLog("Error on writing data");
                CloseHandle(hFile);
                return;
            }

    }
    
    CloseHandle(hFile);

}

void ShowLoadFileModeMenuBars()
{
       
    if (ImGui::BeginMenu("LoadFile"))
    {

        if (ImGui::MenuItem("Open LoadFile", NULL))
        {
            //bool g_OpenFile(HWND hwnd, char* fpath, char* filter = nullptr); 
            bool fail = g_OpenFile(nullptr, myLoadFile.fname, "pf Files(*.pf)\0");  //want to add HWND if possible
            AddLog("Loadfile to open is %s", myLoadFile.fname);
            std::wstring wfname = Utf8ToUtf16(myLoadFile.fname);

            Load_LoadFile((LPWSTR)wfname.c_str());
        }
        ImGui::SetItemTooltip("Open any sort of load file.");

        if (ImGui::MenuItem("Save LoadFile", NULL))
        {
            //bool fail = SaveFileWithUtf8Support(nullptr, fname);
            bool fail = g_OpenFile(nullptr, myLoadFile.fname, "pf Files(*.pf)\0");
            AddLog("Loadfile to open is %s", myLoadFile.fname);
            std::wstring wfname = Utf8ToUtf16(myLoadFile.fname);

            Save_LoadFile(wfname.data());
        }
        ImGui::SetItemTooltip("Save current load file.");

        if (ImGui::MenuItem("Clear Currently loaded  LoadFile", NULL)) myLoadFile.SDWs.clear();
         

        ImGui::EndMenu();
    }
    
}



void LoadFileEditorWindow()
{
    

    //Tool Bar
    ImGui::Text("File Name: %s", myLoadFile.fname);
    ImGui::SameLine();
    ImGui::Text("Size: %d ()", myLoadFile.m_NumRows);
    ImGui::SameLine();
    ImGui::Text("STN: %d  = %o (octal)", myLoadFile.myJU, myLoadFile.myJU);
    ImGui::SameLine(); ImGui::Checkbox("NTR", &myLoadFile.NTR);
    ImGui::SameLine();
    ImGui::Checkbox("ETR", &myLoadFile.ETR);
    ImGui::SameLine();
    ImGui::Checkbox("PR", &myLoadFile.PR);
    ImGui::SameLine();
    if (ImGui::Button("SNE")) {};
 


    ImGui::Text(myLoadFile.fname);

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

    if (ImGui::BeginTable("table1", 3, flags))
    {
        ImGui::TableSetupColumn("AP", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        if (myLoadFile.SDWs.size() > 10) BuildUI();

        ImGui::EndTable();
    }
    

}

