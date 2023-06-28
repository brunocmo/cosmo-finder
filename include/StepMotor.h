#pragma once

#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "driver/gptimer.h"

class StepMotor
{
public:
    struct motorPasso
    {
        std::uint8_t m_direction;
        std::uint64_t m_steps;
        std::int64_t m_speed;
        gpio_num_t m_stepGpio = GPIO_NUM_NC;
        gpio_num_t m_dirGpio = GPIO_NUM_NC;
        std::string m_log = "STEPMOTOR";
    };
    
    enum
    {
        MIN_SPEED = 500000,
        MAX_SPEED = 650
    };

    enum
    {
        CLOCKWISE,
        COUNTERCLOCKWISE
    };

    enum AxisValues
    {
        X_STEP = GPIO_NUM_12,
        X_DIR = GPIO_NUM_14,
        Y_STEP = GPIO_NUM_27,
        Y_DIR = GPIO_NUM_26
    };

    StepMotor();
    ~StepMotor() = default;

    static void Run( void* pTaskInstance );
    void Movement( std::uint8_t XDirection, 
                   std::uint64_t XSteps,
                   std::uint8_t YDirection, 
                   std::uint64_t YSteps );
    void Park();
    void FullStop();
    std::vector<std::tuple<motorPasso, motorPasso>> m_movementList;

private:


    void init();
    void setupPin( gpio_num_t pinSet );
    static void slewing( motorPasso axis, bool stop );

    motorPasso m_XAxis;
    motorPasso m_YAxis;
    bool m_stopNow;

};  