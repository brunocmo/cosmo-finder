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

// QueueHandle_t filaDeInterrupcao;

// static void IRAM_ATTR gpio_isr_handler(void *args)
// {
//   int pino = (int)args;
//   xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
// }

// void encoder( void * params ) {

//     gpio_num_t pino;
//     int esquerda = 0;
//     int direita = 0;

//     int oldAState = gpio_get_level(GPIO_NUM_34);
//     int oldBState = gpio_get_level(GPIO_NUM_35);

//     while( 1 )
//     {   
//         if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
//         {
//             int newAState = gpio_get_level(GPIO_NUM_34);
//             int newBState = gpio_get_level(GPIO_NUM_35);

//             // Determine the direction based on the transition of the encoder states
//             if (oldAState == 0 && newAState == 1) {
//                 if (oldBState == 0 && newBState == 0) {
//                 direita++;  // Clockwise
//                 printf("Girei direita: %d, esquerda: %d. OUTPUT: %d\n", direita, esquerda, pino);
//                 } else if (oldBState == 1 && newBState == 1) {
//                 esquerda++;  // Counterclockwise
//                 printf("Girei direita: %d, esquerda: %d. OUTPUT: %d\n", direita, esquerda, pino);
//                 }
//             }

//             oldAState = newAState;
//             oldBState = newBState;

//             gpio_isr_handler_remove(pino);

//             // Habilitar novamente a interrupção
//             gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
//         }
//     }
// }

extern "C" void app_main(void)
{
//     ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_34));
//     ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_34, GPIO_MODE_INPUT));
//     ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_34));
//     ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_34));

//     ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_35));
//     ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_35, GPIO_MODE_INPUT));
//     ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_35));
//     ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_35));

//   // Configura pino para interrupção
//     ESP_ERROR_CHECK( gpio_set_intr_type(GPIO_NUM_34, GPIO_INTR_ANYEDGE ) );
//     ESP_ERROR_CHECK( gpio_set_intr_type(GPIO_NUM_35, GPIO_INTR_ANYEDGE) );

    //GPS::Gps6mv2 testeGPS;
    // xTaskCreate( &encoder, "Encoder", 4096, NULL, 1, NULL);   
    // xTaskCreate( &encoder, "Encoder", 4096, NULL, 1, NULL );
    // filaDeInterrupcao = xQueueCreate(1000, sizeof(int));

    // gpio_install_isr_service(0);
    // gpio_isr_handler_add(GPIO_NUM_34, gpio_isr_handler, (void *) GPIO_NUM_34);
    // gpio_isr_handler_add(GPIO_NUM_35, gpio_isr_handler, (void *) GPIO_NUM_35);

    StepMotor motor;

    xTaskCreate( &StepMotor::Run, "Run Motor", 4096, &motor, tskIDLE_PRIORITY, NULL );

    int counter{ 0 };
    
    while( 1 )
    {
        motor.Movement( StepMotor::CLOCKWISE, 3000, StepMotor::COUNTERCLOCKWISE, 5 );
        motor.Movement( StepMotor::CLOCKWISE, 200, StepMotor::COUNTERCLOCKWISE, 1000 );
        motor.Movement( StepMotor::COUNTERCLOCKWISE, 5000, StepMotor::CLOCKWISE, 5000 );
        motor.Movement( StepMotor::COUNTERCLOCKWISE, 0, StepMotor::CLOCKWISE, 600 );
        motor.Movement( StepMotor::COUNTERCLOCKWISE, 400, StepMotor::CLOCKWISE, 0 );

        if( counter % 2 == 1 )
        {
            std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
            motor.FullStop();
        }

        std::this_thread::sleep_for( std::chrono::seconds( 30 ) );
        ++counter;
    }

}
