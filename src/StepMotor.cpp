#include "StepMotor.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>

bool stop;

StepMotor::StepMotor() :
    m_movementList(),
    m_XAxis(),
    m_YAxis(),
    m_stopNow( false )

{
    init();
}

void StepMotor::setupPin( gpio_num_t pinSet )
{
    ESP_ERROR_CHECK(gpio_reset_pin(pinSet));
    ESP_ERROR_CHECK(gpio_set_direction(pinSet, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_pulldown_dis(pinSet));
    ESP_ERROR_CHECK(gpio_pullup_dis(pinSet));
}

void StepMotor::init()
{
    m_XAxis.m_stepGpio = static_cast<gpio_num_t> (X_STEP);
    m_XAxis.m_dirGpio = static_cast<gpio_num_t> (X_DIR);
    m_XAxis.m_log += "X";

    m_YAxis.m_stepGpio = static_cast<gpio_num_t> (Y_STEP);
    m_YAxis.m_dirGpio = static_cast<gpio_num_t> (Y_DIR);
    m_YAxis.m_log += "Y";

    setupPin( m_XAxis.m_stepGpio );
    setupPin( m_XAxis.m_dirGpio );
    setupPin( m_YAxis.m_stepGpio );
    setupPin( m_YAxis.m_dirGpio );
}

void StepMotor::Run( void* pTaskInstance )
{
    StepMotor* pTask = (StepMotor* ) pTaskInstance;

    while(1)
    {
        if( !pTask->m_movementList.empty() )
        {
            for( auto movementIterator = pTask->m_movementList.begin(); movementIterator != pTask->m_movementList.end(); ++movementIterator )
            {
                pTask->m_XAxis = std::get<0>( *movementIterator );
                pTask->m_YAxis = std::get<1>( *movementIterator );

                ESP_LOGI( pTask->m_XAxis.m_log.c_str(), "Moving %llu steps", pTask->m_XAxis.m_steps );
                gpio_set_level( pTask->m_XAxis.m_dirGpio, pTask->m_XAxis.m_direction );

                ESP_LOGI( pTask->m_YAxis.m_log.c_str(), "Moving %llu steps", pTask->m_YAxis.m_steps );
                gpio_set_level( pTask->m_YAxis.m_dirGpio, pTask->m_YAxis.m_direction );

                std::thread stepMotorMovement_X( StepMotor::slewing, pTask->m_XAxis, pTask->m_stopNow );
                std::thread stepMotorMovement_Y( StepMotor::slewing, pTask->m_YAxis, pTask->m_stopNow );

                stepMotorMovement_X.join();
                stepMotorMovement_Y.join();

                if( pTask->m_stopNow )
                {
                    pTask->m_movementList.clear();
                    pTask->m_stopNow = false;
                    break;
                }

                std::cout << pTask->m_movementList.size() << '\n';

                for( auto teste3 : pTask->m_movementList )
                {
                    std::cout << "lol " << std::get<0>( teste3 ).m_steps << '\n';
                    std::cout << "lol 2" << std::get<1>( teste3 ).m_steps << '\n';
                }

                movementIterator = pTask->m_movementList.begin();

                movementIterator = pTask->m_movementList.erase( movementIterator );
                --movementIterator;



                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void StepMotor::slewing( motorPasso axis, bool stop )
{
    while( axis.m_steps-- && !stop )
    {
        gpio_set_level( axis.m_stepGpio, 1 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        gpio_set_level( axis.m_stepGpio, 0 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
    }
}

void StepMotor::Movement( std::uint8_t XDirection, 
                          std::uint64_t XSteps,
                          std::uint8_t YDirection, 
                          std::uint64_t YSteps )
{

    motorPasso XAxis { XDirection, XSteps, 0, m_XAxis.m_stepGpio, m_XAxis.m_dirGpio, m_XAxis.m_log };
    motorPasso YAxis { YDirection, YSteps, 0, m_YAxis.m_stepGpio, m_YAxis.m_dirGpio, m_YAxis.m_log };

    std::uint64_t greaterValue = std::max( XSteps, YSteps );
    std::uint64_t lesserValue = std::min( XSteps, YSteps );

    std::uint64_t lessesSpeed{ 0 };

    if( lesserValue != 0 )
    {
        lessesSpeed = greaterValue/lesserValue * MAX_SPEED;
    }

    ESP_LOGI( "Teste", "Moving %llu steps", lessesSpeed );

    if( greaterValue == XSteps )
    {
        XAxis.m_speed = MAX_SPEED;
        YAxis.m_speed = lessesSpeed;
    }
    else
    {
        YAxis.m_speed = MAX_SPEED;
        XAxis.m_speed = lessesSpeed;       
    }

    m_movementList.push_back( { XAxis, YAxis } );
}

void StepMotor::Park()
{
    // TODO - Need another sensor ( GPIO INPUT ), to set park. Define which
    //        clockwise and speed we neet to reach it, and thread for boths
    //        axis cases.
}

void StepMotor::FullStop()
{
   ESP_LOGI( "STEPMOTOR", "FULL STOP" );
   m_stopNow = true;
}