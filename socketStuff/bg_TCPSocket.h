#pragma once

#include "BG_SocketBase.h"

//USAGE
//Create a bg_TCPSocket():	bg_TCPSocket myTCP("192.168.1.135",30154); 
//Start listening:		    myTCP.StartTCPOpenThread();
//create a processing function wth this signature: subProcessTCPPacket(char* packet, int pLen);
//stop listening:			myTCP.StopTCPListenThread();

//Can send with:			bool SendTCPPacket(char* buffer, int bLen);


static SOCKET OpenTCPServerSocket(char* host, u_short port)
{
	printf("Trying to open TCP Socket %s:%d...\r\n", host, port);
	SOCKET TCPClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPClientSocket == INVALID_SOCKET) {
		printf("Error, cannot create TCP socket in OpenTCPServerSocket()\r\n");
		return EXIT_FAILURE;
	}

	// setup address structure
	SOCKADDR_IN  addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_port = htons(port);

	// Attemp to connect to server and exit on failure. 
	int connval = connect(TCPClientSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
	if (connval == SOCKET_ERROR) {
		printf("Error: cannot connect to server in OpenTCPServerSocket()\r\n");
		//Returns status code other than zero
		return EXIT_FAILURE;
	}
	printf("Created and connected to TCP Server at %s:%d\r\n", host, port);
	return TCPClientSocket;
}



class bg_TCPSocket
{
public:
	bg_TCPSocket(char* host, int port, DWORD (*f)(LPVOID lpParameter)) //needs a function pointer to the owners listen thread
	{
		TCP_Port = port;
		strncpy(TCP_Host, host, 30);
		ListenThreadFuncPtr = f;
	};

	~bg_TCPSocket()
	{
		b_TCPThreadIsListening = false;
		if (b_TCP_SocketOpened) closesocket(TCPClientSocket);
		b_TCP_SocketOpened = false;
	};

	void StopTCPListThread()
	{
		b_TCPThreadIsListening = false;
		if (b_TCP_SocketOpened) closesocket(TCPClientSocket);
		b_TCP_SocketOpened = false;
	};

	void StartTCPOpenThread()
	{
		CreateThread(NULL, 0, m_StaticThreadEntry, this, 0, NULL);
	};


	bool SendTCPPacket(char* buffer, int bLen)
	{
		int bytesSent = send(TCPClientSocket, buffer, bLen, 0);
		if (bytesSent == SOCKET_ERROR) 
		{
			printf("Error Sending TCP Packet of size %d ->%s",bLen,buffer);
			return true;
		}
		return false;
	}


	char TCP_Host[30] = "192.168.1.132";
	int TCP_Port = 30154;

protected:
	
	SOCKET TCPClientSocket;
	bool b_TCP_SocketOpened = false;
	bool b_TCPThreadIsListening = false;
	
	DWORD (*ListenThreadFuncPtr)(LPVOID lpParameter);


	static DWORD WINAPI m_StaticThreadEntry(LPVOID lpParam) 
	{
		bg_TCPSocket* pThis = static_cast<bg_TCPSocket*>(lpParam);
		pThis->m_OpenTCPSocket(lpParam);
		return 0;
	}

	void m_OpenTCPSocket(LPVOID lpParameter)
	{
		b_TCP_SocketOpened = false;
		TCPClientSocket = OpenTCPServerSocket((char*)TCP_Host, TCP_Port);
		if (EXIT_FAILURE == TCPClientSocket)
		{
			printf("Could not open TCP Client Socket (%s:%d)\r\n", TCP_Host, TCP_Port);
			return;
		}

		HANDLE hThread = CreateThread(NULL, 0, ListenThreadFuncPtr, (LPVOID)&TCPClientSocket, 0, NULL);
		if (hThread == NULL)
		{
			printf("OpenSocketThread not created\r\n");
			return;
		}
		b_TCP_SocketOpened = true;
		puts("Socket Opened");
	};

};

