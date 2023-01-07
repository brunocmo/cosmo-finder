#pragma once

#include <iostream>
#include <unistd.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include <cstring>

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

        void initUART0();
        void initUART2();

        void sendData();
        void receiveData();
    private:
        uart_port_t uart_num;
        uart_config_t uart_config;
        int uart_buffer_size;
        QueueHandle_t uart_queue;
    };
}