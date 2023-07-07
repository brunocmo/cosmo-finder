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

class Comms
{
public:
    Comms();
    ~Comms() = default;
    
    void Init();

    static void RunTCPServer( void* pTaskInstance );

    void sendCommand( std::string message );
    std::string receiveCommand();

    std::string receiveBuffer;
    std::string sendBuffer; 

	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
    int sockedDescriptior;
	unsigned short servidorPorta = 16188;
	socklen_t clienteLength;
    int connectionDescriptor;

};