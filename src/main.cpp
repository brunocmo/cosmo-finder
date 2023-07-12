#include <string>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/queue.h"
#include <unistd.h>
#include <thread>

#include "Gps6mv2.h"
#include "Controller.h"

#include "Wifi.h"

#define WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

WIFI::Wifi::state_e wifiState { WIFI::Wifi::state_e::NOT_INITIALIZED };
WIFI::Wifi Wifi;

void setupWifi()
{
    esp_event_loop_create_default();
    nvs_flash_init();

    Wifi.SetCredentials(WIFI_SSID, WIFI_PASS);
    Wifi.Init(); 
}

void runWifi( void* pTaskInstance )
{
    while( 1 )
    {
        wifiState = Wifi.GetState();

        switch (wifiState)
        {
        case WIFI::Wifi::state_e::READY_TO_CONNECT:
            std::cout << "Wifi Status: READY_TO_CONNECT\n";
            Wifi.Begin();
            break;
        case WIFI::Wifi::state_e::DISCONNECTED:
            std::cout << "Wifi Status: DISCONNECTED\n";
            Wifi.Begin();
            break;
        case WIFI::Wifi::state_e::CONNECTING:
            std::cout << "Wifi Status: CONNECTING\n";
            break;
        case WIFI::Wifi::state_e::WAITING_FOR_IP:
            std::cout << "Wifi Status: WAITING_FOR_IP\n";
            break;
        case WIFI::Wifi::state_e::ERROR:
            std::cout << "Wifi Status: ERROR\n";
            break;
        case WIFI::Wifi::state_e::CONNECTED:
            //std::cout << "Wifi Status: CONNECTED\n";
            // testServer();
            break;
        case WIFI::Wifi::state_e::NOT_INITIALIZED:
            std::cout << "Wifi Status: NOT_INITIALIZED\n";
            break;
        case WIFI::Wifi::state_e::INITIALIZED:
            std::cout << "Wifi Status: INITIALIZED\n";
            break;
        }
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}


extern "C" void app_main(void)
{
    setupWifi();
    xTaskCreate( &runWifi, "Run Wifi", 4096, NULL, tskIDLE_PRIORITY, NULL );

    while ( wifiState != WIFI::Wifi::state_e::CONNECTED )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    std::cout << "Wifi Status: CONNECTED\n";

    // Comms m_communication;
    // m_communication.Init();

    // vTaskDelay( 100 / portTICK_PERIOD_MS );

    // xTaskCreate( &Comms::RunTCPServer, "Run TCP/IP Comms", 4096, &m_communication, tskIDLE_PRIORITY, NULL );


    // char* teste1;

    // while( 1 )
    // {
    //     std::cout << "##### SHOWll!!!" << '\n';
    //     teste1 = m_communication.receiveCommand();
    //     if( m_communication.sizeOfBuffer != 0 )
    //     {
    //         m_communication.sendCommand( "Ok\0" );
    //         std::cout << "##Tamanho: " << std::dec << m_communication.sizeOfBuffer << '\n';
    //         for( int i{0}; i<m_communication.sizeOfBuffer; i++)
    //         {
    //             std::cout << std::hex << (int)*(teste1+i) << " ";
    //         }
    //         std::cout << '\n';
    //     }
    //     vTaskDelay( 2000 / portTICK_PERIOD_MS );
    // }

    Controller controller;
    controller.Init();
    controller.Run();
    
    while( wifiState == WIFI::Wifi::state_e::CONNECTED )
    {
        controller.machineState();
    }
}
