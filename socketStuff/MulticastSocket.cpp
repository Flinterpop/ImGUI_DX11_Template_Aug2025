
#include "MulticastSocket.h"
#include <iostream>




bool MulticastSocket::Intialize(const char* multicastGroupString, int multicastPort, unsigned short loopBack, unsigned short ttl,
    std::function<void(char* message, int messageSize)> onReceive)
{
 
    /*
    // Initialize Windows Sockets v2.2.
    WSADATA wsaData{};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        std::cout << "WSAStartup() failed with error " << GetLastError() << std::endl;
        return false;
    }
    */

    m_isSocketInitialized = false;

    // Create Socket Handle: (Family: IPv4, Socket Type: Datagram, Protocol: UDP)
    m_socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socketHandle == SOCKET_ERROR)
    {
        std::cout << "socket() failed with error " << GetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Socket is created.
    m_isSocketCreated = true;

    // Allow address reusability for other Apps.
    unsigned int allowReuse{ 1 };

    int result = setsockopt(
        m_socketHandle,
        SOL_SOCKET,
        SO_REUSEADDR,
        (char*)&allowReuse,
        sizeof(allowReuse));

    if (result == SOCKET_ERROR)
    {
        std::cout << "setsockopt() failed for socket reuse with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Bind socket with any Interface Address.
    ZeroMemory(&m_bindAddress, sizeof(m_bindAddress));

    m_bindAddress.sin_family = AF_INET;
    m_bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    m_bindAddress.sin_port = htons((unsigned short)multicastPort);

    result = bind(m_socketHandle, (struct sockaddr*)&m_bindAddress, sizeof(m_bindAddress));

    if (result == SOCKET_ERROR) {
        std::cout << "bind() failed with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Join Multicast Group.
    unsigned long ipV4address{};
    inet_pton(AF_INET, multicastGroupString, &ipV4address);

    m_multicastGroupConfiguration.imr_multiaddr.s_addr = ipV4address;
    m_multicastGroupConfiguration.imr_interface.s_addr = htonl(INADDR_ANY);

    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_ADD_MEMBERSHIP,
        (char*)&m_multicastGroupConfiguration,
        sizeof(m_multicastGroupConfiguration));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for joining group with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Set Loop-Back Value.
    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_MULTICAST_LOOP,
        (char*)&loopBack,
        sizeof(loopBack));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for loop-back with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return false;
    }

    // Set TTL.
    result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_MULTICAST_TTL,
        (char*)&ttl,
        sizeof(ttl));

    if (result == SOCKET_ERROR) {
        std::cout << "setcsockopt() failed for TTL with error " << WSAGetLastError() << std::endl;
        CleanUp();
        return 1;
    }

    // Assign Callback.
    m_onReceive = onReceive;

    // Create Reception Thread.
    m_receptionThread = std::thread([this](void* threadParams) {
        // Register Notification for Network Event: FD_READ.
        WSAEVENT networkEvent = WSACreateEvent();

        int result = WSAEventSelect(
            m_socketHandle, 
            networkEvent, 
            FD_READ);

        if (result == SOCKET_ERROR)
        {
            std::cout << "WSAEventSelect() failed with error " << WSAGetLastError() << std::endl;
            std::cout << "Termintating Reception Thread after failing to Select Network Events." << std::endl;
            return;
        }

        // Handle Network Events.
        while (true)
        {
            // If Main Thread is waiting for Reception Thread to Join.
            if (m_threadJoinRequested == true)
            {
                std::cout << "Termintating Reception Thread after Join Requested." << std::endl;
                break;
            }

            // Receive Notification of Network Events.
            WSANETWORKEVENTS networkEvents{};
            WSAEnumNetworkEvents(m_socketHandle, networkEvent, &networkEvents);

            // Check if FD_READ has been notified.
            if (networkEvents.lNetworkEvents & FD_READ)
            {
                if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
                {
                    std::cout << "FD_READ failed with error " << networkEvents.iErrorCode[FD_READ_BIT] << std::endl;
                    continue;
                }
                else
                {
                    // Lock Socket Handle while receving data.
                    m_socketLock.lock();

                    char messageBuffer[BUFFER_SIZE];

                    struct sockaddr_in sourceAddress {};
                    int addressLength = sizeof(sourceAddress);

                    // Receive bytes.
                    int bytesRead = recvfrom(
                        m_socketHandle,
                        messageBuffer,
                        BUFFER_SIZE,
                        0,
                        (struct sockaddr*)&sourceAddress,
                        &addressLength);

                    if (bytesRead < 0) {
                        std::cout << "recvfrom() failed with error " << GetLastError() << std::endl;
                        m_socketLock.unlock();
                        continue;
                    }
                    else
                    {
                        // Terminate Byte Stream.
                        messageBuffer[std::abs(bytesRead)] = '\0';
                    }
                    
                    // Reception Callback.
                    if (m_onReceive)
                        m_onReceive(messageBuffer, bytesRead);

                    // Unlock Socket Handle.
                    m_socketLock.unlock();
                }
            }
        }
        }, nullptr);

    m_isReceptionThreadStarted = true;

    // Create Multicast Group Address.
    ZeroMemory(&m_multicastGroupAddress, sizeof(m_multicastGroupAddress));

    m_multicastGroupAddress.sin_family = AF_INET;
    m_multicastGroupAddress.sin_addr.s_addr = ipV4address;
    m_multicastGroupAddress.sin_port = htons(multicastPort);

	return true;
}

bool MulticastSocket::LeaveGroup()
{
    // Return if Socket is not created.
    if (m_isSocketCreated == false) return false;

    // Return if Multicast Group is not joined.
    if (m_isGroupJoined == false) return false;

    // Leave Group.
    int result = setsockopt(
        m_socketHandle,
        IPPROTO_IP,
        IP_DROP_MEMBERSHIP,
        (char*)&m_multicastGroupConfiguration,
        sizeof(m_multicastGroupConfiguration));

    if (result == SOCKET_ERROR) 
    {
        std::cout << "setcsockopt() failed for leaving group with error " << WSAGetLastError() << std::endl;
        m_isGroupJoined = false;
        return false;
    }

    // Update flag.
    m_isGroupJoined = false;
    
    return true;
}

bool MulticastSocket::Send(char* message, int messageSize, int& bytesSent)
{
    // Return if socket is not created.
    if (m_isSocketCreated == false)  return false;

    // Lock Socket Handle for transmission.
    m_socketLock.lock();

    // Send bytes.
    bytesSent = sendto(
        m_socketHandle,
        message,
        messageSize,
        0,
        (struct sockaddr*)&m_multicastGroupAddress,
        (int)sizeof(m_multicastGroupAddress));


    if (bytesSent < 0) {
        std::cout << "sendto() failed with error " << WSAGetLastError() << std::endl;
        m_socketLock.unlock();
        return false;
    }

    std::cout << "Bytes Sent: " << bytesSent << std::endl;
    std::cout << "Message: " << message << std::endl << std::endl;

    // Unlock Socket Handle.
    m_socketLock.unlock();

    return true;
}
