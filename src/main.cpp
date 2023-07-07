#include <string>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "nvs_flash.h"

#include <esp_log.h>
#include <stdio.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/queue.h"
#include <unistd.h>
#include <thread>

#include "Gps6mv2.h"
#include "Controller.h"

extern "C" void app_main(void)
{
    Controller controller;

    controller.Run();
    
    while( 1 )
    {
        controller.machineState();
    }
}
