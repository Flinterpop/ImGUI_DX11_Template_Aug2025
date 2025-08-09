#ifndef SOCKETSTUFF_H
#define SOCKETSTUFF_H


#include <winsock2.h>
#include <windows.h>

#include <Ws2tcpip.h>

#include <functional>
#include <thread>
#include <mutex>


constexpr auto BUFFER_SIZE = 1510;

void getNetworkAdapterInfo();
bool initialise_winsock();
void closeandclean_winsock();
int isValidIp4(char* str);


#endif



