#pragma once

#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>


#include <functional>
#include <thread>
#include <mutex>

// Link the project with "Ws2_32.lib".
//#pragma comment (lib, "Ws2_32.lib")

constexpr auto BUFFER_SIZE = 1510;

class MulticastSocket
{
private:
	SOCKET m_socketHandle{};

	struct sockaddr_in m_bindAddress{};
	struct sockaddr_in m_multicastGroupAddress{};
	
	struct ip_mreq m_multicastGroupConfiguration{};
	
	std::mutex m_socketLock{};
	std::thread m_receptionThread{};

	bool m_isSocketInitialized{ false };
	bool m_isSocketCreated{ false };
	bool m_isGroupJoined{ false };
	bool m_isReceptionThreadStarted{ false };
	bool m_threadJoinRequested{ false };

	std::function<void(char* message, int messageSize)> m_onReceive;

	

public:
	MulticastSocket();
	virtual ~MulticastSocket();
	void CleanUp();

	bool Intialize(
		const char* multicastGroupString,
		int multicastPort,
		unsigned short loopBack,
		unsigned short ttl,
		std::function<void(char* message, int messageSize)> onReceive);

	bool LeaveGroup();
	bool Send(char* message, int messageSize, int& bytesSent);
};

