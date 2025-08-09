#pragma once

#include "BG_SocketBase.h"

class bg_MulticastSocket
{
private:
	SOCKET m_socketHandle{};

	struct sockaddr_in m_bindAddress {};
	struct sockaddr_in m_multicastGroupAddress {};

	struct ip_mreq m_multicastGroupConfiguration {};

	std::mutex m_socketLock{};
	std::thread m_receptionThread{};

	bool m_isSocketInitialized{ false };
	bool m_isSocketCreated{ false };
	bool m_isGroupJoined{ false };
	bool m_isReceptionThreadStarted{ false };
	bool m_threadJoinRequested{ false };

	std::function<void(char* message, int messageSize)> m_onReceive;



public:

	bg_MulticastSocket::bg_MulticastSocket()
	{
		printf("In MulticastSocket::Constructor\r\n");
	};

	bg_MulticastSocket::~bg_MulticastSocket()
	{
		printf("In MulticastSocket::Destructor()\r\n");
		CleanUp();
	};


	void bg_MulticastSocket::CleanUp()
	{
		printf("In MulticastSocket::CleanUp()\r\n");

		// Join Reception Thread with Main Thread.
		if (m_isReceptionThreadStarted == true)
		{
			m_threadJoinRequested = true;
			m_receptionThread.join();
		}

		// Leave Multicast Group.
		LeaveGroup();

		// Close Socket.
		if (m_isSocketCreated) closesocket(m_socketHandle);

		// Clean-up Windows Sockets.
		//if (m_isSocketInitialized == true) WSACleanup();

		// Reset flags.
		m_isReceptionThreadStarted = false;
		m_isSocketCreated = false;
		m_isSocketInitialized = false;
	};


	bool Intialize(
		const char* multicastGroupString,
		int multicastPort,
		unsigned short loopBack,
		unsigned short ttl,
		std::function<void(char* message, int messageSize)> onReceive);

	bool LeaveGroup();
	bool Send(char* message, int messageSize, int& bytesSent);
};
