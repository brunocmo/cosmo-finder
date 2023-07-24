#pragma once

#include <iostream>
#include <unistd.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <esp_log.h>
#include <cstring>

#include <tuple>

namespace GPS
{
    class Gps6mv2
    {
    public:

        enum
        {
            TXD_PIN0 = GPIO_NUM_1,
            RXD_PIN0 = GPIO_NUM_3,
            TXD_PIN2 = GPIO_NUM_17,
            RXD_PIN2 = GPIO_NUM_16
        };

        Gps6mv2();
        ~Gps6mv2();

        void initUART2();

        void sendData();
        static void receiveData( void* pTaskInstance );

        bool HasGPSLocation();
        std::tuple< double, double, double > GpsInformation();

        bool readGPSLatitudePersistence();
        bool writeGPSLatitudePersistence( double latitude );

        bool readGPSLongitudePersistence();
        bool writeGPSLongitudePersistence( double longitude );

        bool readGPSAltitudePersistence();
        bool writeGPSAltitudePersistence( double altitude );


    private:
        uart_port_t uart_num;
        uart_config_t uart_config;
        int uart_buffer_size;
        double m_latitude;
        double m_longitude;
        double m_altitude;
        bool m_hasValue{false};

        QueueHandle_t uart_queue;
    };
}