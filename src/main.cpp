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
#include "StepMotor.h"

const std::string encoderLogi {"STEPMOTOR"};

extern "C" void app_main(void)
{
    StepMotor motor;

    xTaskCreate( &StepMotor::Run, "Run Motor", 4096, &motor, tskIDLE_PRIORITY, NULL );
    
    while( 1 )
    {
        motor.Movement( StepMotor::CLOCKWISE, 3000, StepMotor::COUNTERCLOCKWISE, 5 );
        motor.Movement( StepMotor::CLOCKWISE, 200, StepMotor::COUNTERCLOCKWISE, 1000 );

        motor.Park();
        std::this_thread::sleep_for( std::chrono::minutes( 2 ) );
    }

}
