
#include "BG_SocketBase.h"

#include "GAMS2App.h"
#include "AppLogger.h"

#include "bg_TCPSocket.h"
#include "bg_UDPSocket.h"
#include "bg_MulticastSocket.h"

#include <BGUtil.h>


class TestTDPSocket :public bg_TCPSocket
{
public:

    TestTDPSocket(char* b, int l) :bg_TCPSocket(b, l, TCPListenThread)
    {

    };

protected:
    static DWORD WINAPI TCPListenThread(LPVOID lpParameter)
    {
        TestTDPSocket* pThis = static_cast<TestTDPSocket*>(lpParameter);
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

            if ((n > 0) && (n < 1510))
            {
                buffer[n] = 0;
                if (false)
                {
                    printf("%s", buffer); //doesn't flush without a newline
                    fflush(stdout);// Force writing buffer to the stdout
                }

                printf("Process Packet: %d\r\n",n);
            }
        }
        if (b_TCP_SocketOpened) closesocket(*TCPClientSocket);
        b_TCP_SocketOpened = false;
        b_TCPThreadIsListening = false;
    };

};




class TestTDPSocket myTCP("192.168.1.135", 30154);


void GAMS2::StopTCPTest()
{
    myTCP.StopTCPListThread();
}

void GAMS2::StartTCPTest()
{
    myTCP.StartTCPOpenThread();

}
void GAMS2::SendTCPTest()
{
    char* bPacket = "Big Test of TCP";
    myTCP.SendTCPPacket(bPacket, strlen(bPacket));
}

