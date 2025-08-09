#pragma once

#include "BG_SocketBase.h"

//USAGE
//Create a bg_TCPSocket():	bg_TCPSocket myTCP("192.168.1.135",30154); 
//Start listening:		    myTCP.StartTCPOpenThread();
//create a processing function wth this signature: subProcessTCPPacket(char* packet, int pLen);
//stop listening:			myTCP.StopTCPListenThread();

//Can send with:			bool SendTCPPacket(char* buffer, int bLen);


void subProcessTCPPacket(char* packet, int pLen);


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
	//bg_TCPSocket() {};

	bg_TCPSocket(char* host, int port)
	{
		TCP_Port = port;
		strncpy(TCP_Host, host, 30);
	};

	~bg_TCPSocket()
	{
		b_TCPThreadIsListening = false;
		if (b_TCP_SocketOpened) closesocket(TCPClientSocket);
		b_TCP_SocketOpened = false;
	};


	void StartTCPOpenThread()
	{
		CreateThread(NULL, 0, m_StaticThreadEntry, this, 0, NULL);
	};

	void StopTCPListenThread()
	{
		b_TCPThreadIsListening = false;
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



protected:
	SOCKET TCPClientSocket;
	char TCP_Host[30] = "192.168.1.132";
	int TCP_Port = 30154;
	bool b_TCP_SocketOpened = false;
	bool b_TCPThreadIsListening = false;

private:
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

		HANDLE hThread = CreateThread(NULL, 0, TCPListenThread, (LPVOID)&TCPClientSocket, 0, NULL);
		if (hThread == NULL)
		{
			printf("OpenSocketThread not created\r\n");
			return;
		}
		b_TCP_SocketOpened = true;
	}


	static DWORD WINAPI TCPListenThread(LPVOID lpParameter) 
	{
		bg_TCPSocket* pThis = static_cast<bg_TCPSocket*>(lpParameter);
		pThis->TCPListen(lpParameter);
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

			//printf("TCP Rx %d bytes\r\n", n);
			if ((n > 0) && (n < 1510))
			{
				buffer[n] = 0;
				if (false)
				{
					printf("%s", buffer); //doesn't flush without a newline
					fflush(stdout);// Force writing buffer to the stdout
				}

				//ProcessTCPPacket(buffer,n);
				subProcessTCPPacket(buffer, n);
			}
		}


		if (b_TCP_SocketOpened) closesocket(*TCPClientSocket);
		b_TCP_SocketOpened = false;
		b_TCPThreadIsListening = false;
	}



};

