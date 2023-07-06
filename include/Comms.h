#pragma once

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <vector>

class Comms
{
public:
    Comms();
    ~Comms() = default;
    
    void init();

    void RunTCPServer();

    bool sendCommand( std::string message );
    std::string receiveCommand();

    unsigned char receiveBuffer[255];
    std::string sendBuffer;    

	struct sockaddr_in servidorAddr;
	struct sockaddr_in clienteAddr;
    int sockedDescriptior;
	unsigned short servidorPorta = 16188;
	socklen_t clienteLength;
    int connectionDescriptor;

};