
#include "bg_TCPSocket.h"

#include <windows.h>
#include <process.h>
#include <iostream>
#include <vector>

#include "Platform_J.h"

#include "FOMSupport.h"



SOCKET OpenTCPServerSocket(char* host, u_short port);  //should be included by including SocketStuff.h but causes multiple winsock2.h problems


class ADSBTCPSocket :public bg_TCPSocket
{
public:
    ADSBTCPSocket(char* host, int port) : bg_TCPSocket(host, port)
    {
    };


    //void subProcessTCPPacket(char* packet, int pLen) override;

};



ADSBTCPSocket myPlatJ("192.168.1.196", 2001);

bool g_debug = false;
bool b_PlatformJ_SocketOpened = false;
bool b_TCPThreadIsListening = false;


//static SOCKET TCPClientSocket;
//char Platform_J_Host[20] = "192.168.123.1";
//int Platform_J_TCP_Port = 50010;

int FOM_Counts[80];
int FIM_Counts[80];

struct _FOM03 FOM03;
struct _FOM04 FOM04;
struct _FOM05 FOM05;
struct _FOM06 FOM06;



/*
int ConnectPlatformJ_ViaThread() //const char* _host, unsigned int _port)
{
    if (true == b_PlatformJ_SocketOpened)
    {
        printf("Platform-J  Socket already open!\r\n");
        return EXIT_SUCCESS;
    }
    HANDLE myHandle = (HANDLE)_beginthread(&OpenSocketAndStartListenThreadThread, 0, 0);
    return EXIT_SUCCESS;
}

//https://www.bogotobogo.com/cplusplus/multithreading_win32A.php
static void OpenSocketAndStartListenThreadThread(LPVOID lpParameter)
{
    TCPClientSocket = OpenTCPServerSocket((char*)Platform_J_Host, Platform_J_TCP_Port);
    if (EXIT_FAILURE == TCPClientSocket)
    {
        printf("Could not open TCP Client Socket (%s:%d for Platform-J)\r\n", Platform_J_Host, Platform_J_TCP_Port);
        return;
        //return EXIT_FAILURE;
    }

    HANDLE hThread = CreateThread(NULL, 0, TCPListenThread, (LPVOID)&TCPClientSocket, 0, NULL);
    if (hThread == NULL)
    {
        printf("Platform-J Listen Thread not created\r\n");
        int retVal = shutdown(TCPClientSocket, 0x02);//#define SD_RECIEVE 0x00   SD_SEND 0x01  SD_BOTH 0x02 
        closesocket(TCPClientSocket);
        return;
        //return EXIT_FAILURE;
    }
    DoPlatform_JConnect();
    registerFxMs();
    b_PlatformJ_SocketOpened = true;
    //return EXIT_SUCCESS;
}
*/


int DisConnectPlatform_J()
{
    //A common pattern for a graceful shutdown involves calling shutdown(s, SD_SEND) to signal 
    // the end of data transmission, waiting for any remaining incoming data (e.g., by receiving a zero-byte indication),
    // and then finally calling closesocket(s) to release the resources.

    //ToDo Improve shutdown - maybe use a thread?

    if (true == b_PlatformJ_SocketOpened)
    {
        //maybe add deregister FxMs??
        //int retVal = shutdown(TCPClientSocket, 0x02);//#define SD_RECIEVE 0x00   SD_SEND 0x01  SD_BOTH 0x02 
        //closesocket(TCPClientSocket);
        myPlatJ.StopTCPListenThread();

        b_PlatformJ_SocketOpened = false;

    }
    return 0;
}


void DoPlatform_JConnect()
{
    myPlatJ.StartTCPOpenThread();

    //contents of FIM58     Connection Request FIM (see 9.5.2.4.3) 
    int FIM_ID = 0x58; //Connection Request
    int Length = 2;
    int requestNumber = 1;
    int RequestType = 0;

    char ConnectionRequestFIM58[] = { 0x74, 0x02, 0x01, 0x00,0x00 ,0x00 };
    //SendTCP(ConnectionRequestFIM58, 6);
    myPlatJ.SendTCPPacket(ConnectionRequestFIM58, 6);

    //Sleep(3000);

    //FOM61 Connection Request Response FOM (see 0)    //TBC
    char FOM61[] = { 0x7A, 0x02, 0x01, 0x00,0x00 ,0x00 };
    //SendTCP(FOM61, 6);
    myPlatJ.SendTCPPacket(FOM61, 6);
    //Sleep(3000);
}


void registerFxMs()  //might be RegisterFOMs
{
    char FIM55[] = {  //Request For FxM (De) Registration FIM (see 9.5.2.4.1) 
        //FIM55, length 9 
        0x6e, 0x09, 

        ///then 9 pairs of FOMs
        //first number contains 2 BIT fields 
        // bit 8 -> 0 = FIM, 1 = FOM
        // bit 9 -> 0 = Registration Requested, 1 = DeRegistration Requested
        //so 
        //  0x00 = Register FIM
        //  0x01 = Register FOM
        //  0x02 = DeRegister FIM
        //  0x03 = DeRegister FOM
        0x02, 0x00, 
        0x00, 0x00, 
        0x00, 0x06,
        0x01, 0x02,
        0x01, 0x03, 
        0x01, 0x04, 
        0x01, 0x1d,
        0x00, 0x02, 
        0x00, 0x03
    };

    char FOM58[] = { //Response To FxM (De) Registration FOM (see 9.5.2.5.1)  //TBC
        0x74, 0x09, 
        0x02, 0x00,
        0x00, 0x00, 
        0x00, 0x06,
        0x01, 0x02,
        0x01, 0x03, 
        0x01, 0x04, 
        0x01, 0x1d,
        0x00, 0x02, 
        0x00, 0x03
    };

    char FIM03[] = { //Init. Data Change FIM     //TBC
        0x06, 0x03,   //FIM03 with 3 words
        0x40, 0x00, 
        0x20, 0x00, 
        0x40, 0x00 };


    myPlatJ.SendTCPPacket(FIM55, 20);
    myPlatJ.SendTCPPacket(FOM58, 20);
    myPlatJ.SendTCPPacket(FIM03, 8);
}


void _subProcessTCPPacket(char* packet, int pLen) 
{
    parseRxPacket((unsigned char*)packet, pLen);
}



static bool parseRxPacket(unsigned char* buffer, int bufLength)
{
    printf("===================================\r\nRx Packet\r\n");
    
    //parse BIM
    //BxM words are 16 bits. In the buffer word 0 is comprised of buffer[0] and buffer[1]. buffer[0] is MSByte - has bits 8 thru 15, buffer[1] is LSByte - has bits 0 thru 7
    char FxM_ID = buffer[0]>>1;  //bits 9 thru 15 of BxM word 0
    char FxM_LengthInWords = buffer[1] + (buffer[0] & 0b00000001)*256; //all of BxM word 1 plus bits 0 thru 8 of BxM word 0 
    int FxM_LengthInBytes = FxM_LengthInWords * 2; //multiply by 2 because we are dealing with bytes not 16 bit words
    printf("FxM ID: %02d  Length: %d (16 but words)\r\n",FxM_ID, FxM_LengthInWords);

    //debug
    if (g_debug)
    {
        for (int i = 0;i < FxM_LengthInBytes;i++)
        {
            char FxM_Data = buffer[2 + i];
            printf("FxM Data[%d]: %0x\r\n", i, (unsigned char)FxM_Data);
        }

    }
    
    FOM_Counts[FxM_ID]++;


    //Parse the FxM
    std::vector<bool>* bitstring = FxM_BuildBitStringFromByteArray((const unsigned char*)&buffer[2], FxM_LengthInBytes);

    if (03 == FxM_ID)
    {
        //struct _FOM03 FOM03;
        ParseFOM03(FOM03, bitstring, FxM_LengthInWords);
        PrintFOM03(FOM03);
    }
    
    else if (04 == FxM_ID)
    {
        //struct _FOM04 FOM04;
        ParseFOM04(FOM04, bitstring, FxM_LengthInWords);
        PrintFOM04(FOM04);
    }

    else if (05 == FxM_ID) // Initialization Data Change Acknowledgement FOM
    {
        //This FOM is used by the Link 16 waveform, when required by the Host in an Initialization Data 
        //Change FIM, to report acknowledgement status on the acceptance of initialization data changes on
        //the Current Use Set.

        //struct _FOM05 FOM05;
        ParseFOM05(FOM05, bitstring, FxM_LengthInWords);
        PrintFOM05(FOM05);
    }

    else if (06 == FxM_ID) //  Initialization Data Change Notification FOM
    {
        //This FOM shall be used to notify the Host of changes made on the Current Use Set by sources other 
        //than the Host(over the Air, a second Host, Link 16 waveform automatic init Data Change).

        //struct _FOM06 FOM06;
        ParseFOM06(FOM06, bitstring, FxM_LengthInWords);
        PrintFOM06(FOM06);
    }

    else if (12 == FxM_ID)
    {
        printf("Rx FOM12 - Transmission Queue Status FOM Message Definition of length %d\r\n", FxM_LengthInWords);
        printf("Parser Not Implemented\r\n");

    }
    else if (29 == FxM_ID)
    {
        printf("Rx FOM29 - Common BIT FOM of length %d\r\n", FxM_LengthInWords);
        printf("Parser Not Implemented\r\n");
    }

    else
    {
        printf("Rx FOM%d - of length %d\r\n", FxM_ID,FxM_LengthInWords);
        printf("Parser Not Implemented\r\n");
    }


    printf("FOM Counts\r\n");
    printf("FOM03: %03d\r\n", FOM_Counts[3]);
    printf("FOM04: %03d\r\n", FOM_Counts[4]);
    printf("FOM05: %03d\r\n", FOM_Counts[5]);
    printf("FOM06: %03d\r\n", FOM_Counts[6]);
    printf("FOM12: %03d\r\n", FOM_Counts[12]);
    printf("FOM29: %03d\r\n", FOM_Counts[29]);
    return false;
}