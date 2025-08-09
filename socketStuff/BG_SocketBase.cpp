
#include <winsock2.h>
#include <windows.h>

#include "BG_SocketBase.h"

#include <iostream>
#include <iphlpapi.h>


// Link the project with "Ws2_32.lib".
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib") //used by get adapters


void getNetworkAdapterInfo()
{
    IP_ADAPTER_INFO* pAdapterInfo;
    ULONG            ulOutBufLen;
    DWORD            dwRetVal;

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) { //first call is meant to fail
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
        printf("GetAdaptersInfo call failed with %d\n", dwRetVal);
    }

    int anum = 0;
    PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
    while (pAdapter) {
        printf("Adapter # %02d\r\n", anum++);

        printf("\tAdapter Name: %s\n", pAdapter->AdapterName);
        printf("\tAdapter Desc: %s\n", pAdapter->Description);
        printf("\tAdapter Addr(s): ");  //this is the MAC Address
        for (UINT i = 0; i < pAdapter->AddressLength; i++) {
            if (i == (pAdapter->AddressLength - 1))
                printf("%.2X\n", (int)pAdapter->Address[i]);
            else
                printf("%.2X-", (int)pAdapter->Address[i]);
        }

       
        IP_ADDR_STRING* next = &pAdapter->IpAddressList;
        do {
            printf("\tIP Address: %s\n", next->IpAddress.String);
            printf("\tIP Mask: %s\n", next->IpMask.String);
            next = next->Next;
        } while (next != NULL);


        printf("\tGateway: %s\n", pAdapter->GatewayList.IpAddress.String);

        printf("\tDHCP Enabled: ");
        pAdapter->DhcpEnabled? printf(" Yes\n"): printf(" No\n");
        //printf("\t\tDHCP Server: %s\n", pAdapter->DhcpServer.IpAddress.String);

        pAdapter = pAdapter->Next;
        Sleep(1);
    }


    if (pAdapterInfo)  free(pAdapterInfo);

}

bool initialise_winsock()
{
    WSADATA ws;
    printf("Initialising Winsock...\r\n");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("\tInitialising Winsock Failed. Error Code: %d\r\n", WSAGetLastError());
        return true;
    }
    printf("\tWinsock Initialized...\r\n");
    return false;
}

void closeandclean_winsock()
{
    WSACleanup();
}

int isValidIp4(char* str) {
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */
    if (str == NULL)
        return 0;
    /* Process every character in string. */
    while (*str != '\0') {
        /* Segment changeover. */
        if (*str == '.') {
            /* Must have some digits in segment. */
            if (chcnt == 0)
                return 0;
            /* Limit number of segments. */
            if (++segs == 4)
                return 0;
            /* Reset segment values and restart loop. */
            chcnt = accum = 0;
            str++;
            continue;
        }
        /* Check numeric. */
        if ((*str < '0') || (*str > '9'))
            return 0;
        /* Accumulate and check segment. */
        if ((accum = accum * 10 + *str - '0') > 255)
            return 0;
        /* Advance other segment specific stuff and continue loop. */
        chcnt++;
        str++;
    }
    /* Check enough segments and enough characters in last segment. */
    if (segs != 3)
        return 0;
    if (chcnt == 0)
        return 0;
    /* Address okay. */
    return 1;
}


static wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}
