#pragma once

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <esp_log.h>

class Comms
{
public:
    Comms();
    ~Comms() = default;
    
    void Init();

    static void RunTCPServer( void* pTaskInstance );

    void sendCommand( const char* message );
    char* receiveCommand();

    char receiveBuffer[255];
    int sizeOfBuffer;
    char sendBuffer[28]; 

    char tempReceiveBuffer[255];

	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
    int sockedDescriptior;
	unsigned short servidorPorta = 16188;
	socklen_t clienteLength;
    int connectionDescriptor;
    bool hasSendValue;

};