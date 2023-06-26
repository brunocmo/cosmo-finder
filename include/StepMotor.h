#pragma once

#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include <string>
#include "driver/gptimer.h"

class StepMotor
{

const std::string stepMotorLOGI {"STEPMOTOR"};

public:
    StepMotor();
    ~StepMotor() = default;
    static void testOutput( void * params );


    void setupPin( gpio_num_t pinSet );
    void init();



    typedef struct
    {
    uint64_t event_count;
    } example_queue_element_t;
    
    static void control_loop(void* args);
    bool step_state = 0;

    void gptimer_init();



};


