#pragma once

#include "BG_SocketBase.h"

//USAGE
//Create a bg_UDPSocket():	bg_UDPSocket myUDP("192.168.1.135",30154); 
//Can send with:			myUDP.SendUDP(char* buffer, int len)
//Close Socket:			    myUDP.CloseSocket();

//Not DONE:
//Start listening:		    myUDP.StartUDPOpenThread(); NOT IMPLEMENTED
//create a processing function wth this signature: subProcessUDPPacket(char* packet, int pLen);
//stop listening:			myUDP.CloseSocket();



//void subProcessUDPPacket(char* packet, int pLen);


struct bg_UDPSocket
{
	sockaddr_in m_sockaddr_in;
	SOCKET m_client_socket;
	bool isMulticast = false;
	bool isBroadcast = false;
	bool b_Open = false;

	char	UDP_Host[30] = "239.255.1.1";
	int		UDP_Port = 7002;


public:

    bg_UDPSocket(char* host, int port)
    {
        UDP_Port = port;
        strncpy(UDP_Host, host, 30);
        OpenSocket(host, port);

    };

    bool OpenSocket(char* host, int port)
    {
        if ((m_client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
        {
            printf("socket() failed with error code: %d", WSAGetLastError());
            return true;
        }
        // setup address structure
        memset((char*)&m_sockaddr_in, 0, sizeof(m_sockaddr_in));
        m_sockaddr_in.sin_family = AF_INET;
        m_sockaddr_in.sin_port = htons(port);
        m_sockaddr_in.sin_addr.S_un.S_addr = inet_addr(host);
        b_Open = true;
        return false;
    }

    bool CloseSocket()
    {
        closesocket(m_client_socket);
        b_Open = false;
        return false;
    }

    bool SendUDP(char* buffer, int len)
    {
        if (sendto(m_client_socket, (const char*)buffer, len, 0, (sockaddr*)&m_sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            printf("sendto() failed to send packet to %d with error code: %d\r\n", UDP_Port, WSAGetLastError());
            return true;
        }
        return false;
    }


};



