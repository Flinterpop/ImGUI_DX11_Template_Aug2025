
#include "BG_SocketBase.h"

#include "TemplateApp.h"
#include "AppLogger.h"

#include "bg_TCPSocket.h"
#include "bg_UDPSocket.h"
#include "bg_MulticastSocket.h"

#include <BGUtil.h>


void subProcessTCPPacket(char* packet, int pLen)
{
    AddLog("Rx TCP Packet");
    putsGreen("Rx TCP Packet");
}




bg_TCPSocket myTCP("192.168.1.135", 30154);
bg_UDPSocket myUDP("192.168.1.196", 5005);
bg_MulticastSocket* myMulticast;

void callbackMultiCastListener(char* _message, int messageSize)
{
    printf("Multicast Listener Received: %d\r\n", messageSize);
    //unsigned char* message = (unsigned char*)_message;
}



void TemplateApp::SocketTest()
{
    AddLog("Socket Test");

    getNetworkAdapterInfo();

    //TCP Test
    myTCP.StartTCPOpenThread();

    char *bPacket = "Big Test of TCP";
    myTCP.SendTCPPacket(bPacket, strlen(bPacket));


    Sleep(100);
    myTCP.StopTCPListenThread();


    //UDP Test
    myUDP.SendUDP(bPacket, strlen(bPacket));



    //MULTICAST test
    char MULTICAST_LISTEN_GROUP[20] = "239.5.237.1";
    int MULTICAST_LISTEN_PORT = 1581;
    const unsigned short LOOP_BACK = 1;
    const unsigned short TTL = 1;

    myMulticast = new bg_MulticastSocket();

    bool result = myMulticast->Intialize(MULTICAST_LISTEN_GROUP, MULTICAST_LISTEN_PORT, LOOP_BACK, TTL, &callbackMultiCastListener);
    if (result == false)
    {
        puts("Failed to initialize multicast socket.");
        return;
    }
    else if (true) puts("Opened CAT7 Listener");
    printf("Listening on %s:%d\r\n", MULTICAST_LISTEN_GROUP, MULTICAST_LISTEN_PORT);

    int bytesSent = 0;
    myMulticast->Send(bPacket, strlen(bPacket), bytesSent);
    printf("Sent %d bytes\r\n",bytesSent);
}

