#include "StepMotor.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>

bool stop = false;
bool isPark = false;

StepMotor::StepMotor() :
    m_movementList(),
    m_XAxis(),
    m_YAxis()
{
    init();
}

void StepMotor::setupPinOut( gpio_num_t pinSet )
{
    ESP_ERROR_CHECK(gpio_reset_pin(pinSet));
    ESP_ERROR_CHECK(gpio_set_direction(pinSet, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_pulldown_dis(pinSet));
    ESP_ERROR_CHECK(gpio_pullup_dis(pinSet));
}

void StepMotor::setupPinIn( gpio_num_t pinSet, bool isPullUp )
{
    ESP_ERROR_CHECK(gpio_reset_pin(pinSet));
    ESP_ERROR_CHECK(gpio_set_direction(pinSet, GPIO_MODE_INPUT));

    if( isPullUp )
    {
        ESP_ERROR_CHECK(gpio_pulldown_dis(pinSet));
        ESP_ERROR_CHECK(gpio_pullup_en(pinSet));
    }
    else
    {
        ESP_ERROR_CHECK(gpio_pulldown_en(pinSet));
        ESP_ERROR_CHECK(gpio_pullup_dis(pinSet));
    }
}

void StepMotor::init()
{
    m_XAxis.m_stepGpio = static_cast<gpio_num_t> (X_STEP);
    m_XAxis.m_dirGpio = static_cast<gpio_num_t> (X_DIR);
    m_XAxis.m_log += "X";

    m_YAxis.m_stepGpio = static_cast<gpio_num_t> (Y_STEP);
    m_YAxis.m_dirGpio = static_cast<gpio_num_t> (Y_DIR);
    m_YAxis.m_log += "Y";

    setupPinOut( m_XAxis.m_stepGpio );
    setupPinOut( m_XAxis.m_dirGpio );
    setupPinOut( m_YAxis.m_stepGpio );
    setupPinOut( m_YAxis.m_dirGpio );

    setupPinOut( static_cast<gpio_num_t>( ENABLE_PIN ) );

    setupPinIn( static_cast<gpio_num_t> ( AZ_FRONT ), true );
    setupPinIn( static_cast<gpio_num_t> ( AZ_BACK ), true );
    setupPinIn( static_cast<gpio_num_t> ( ALT ), true );
}

void StepMotor::Run( void* pTaskInstance )
{
    StepMotor* pTask = (StepMotor* ) pTaskInstance;

    while(1)
    {
        if( !pTask->m_movementList.empty() && !isPark )
        {
            for( auto movementIterator = pTask->m_movementList.begin(); movementIterator != pTask->m_movementList.end(); ++movementIterator )
            {
                pTask->m_XAxis = std::get<0>( *movementIterator );
                pTask->m_YAxis = std::get<1>( *movementIterator );

                ESP_LOGI( pTask->m_XAxis.m_log.c_str(), "Moving %llu steps", pTask->m_XAxis.m_steps );
                gpio_set_level( pTask->m_XAxis.m_dirGpio, pTask->m_XAxis.m_direction );

                ESP_LOGI( pTask->m_YAxis.m_log.c_str(), "Moving %llu steps", pTask->m_YAxis.m_steps );
                gpio_set_level( pTask->m_YAxis.m_dirGpio, pTask->m_YAxis.m_direction );

                gpio_set_level( static_cast<gpio_num_t>( ENABLE_PIN ), 0 );
                std::thread stepMotorMovement_X( StepMotor::slewing, pTask->m_XAxis );
                std::thread stepMotorMovement_Y( StepMotor::slewing, pTask->m_YAxis );

                stepMotorMovement_X.join();
                stepMotorMovement_Y.join();

                gpio_set_level( static_cast<gpio_num_t>( ENABLE_PIN ), 1 );

                if( stop )
                {
                    pTask->m_movementList.clear();
                    stop = false;
                    break;
                }

                movementIterator = pTask->m_movementList.begin();
                movementIterator = pTask->m_movementList.erase( movementIterator );
                --movementIterator;

                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        }

        if( isPark )
        {
            ESP_LOGI( "PARK", "Park start" );

            gpio_set_level( pTask->m_XAxis.m_dirGpio, CLOCKWISE );
            gpio_set_level( pTask->m_YAxis.m_dirGpio, COUNTERCLOCKWISE );

            pTask->m_XAxis.m_steps = ANGLE_180;
            pTask->m_XAxis.m_speed = MAX_SPEED;
            pTask->m_YAxis.m_steps = ANGLE_90;
            pTask->m_YAxis.m_speed = MAX_SPEED;

            gpio_set_level( static_cast<gpio_num_t>( ENABLE_PIN ), 0 );
            std::thread stepMotorMovement_X( StepMotor::parkSlewing, pTask->m_XAxis );
            std::thread stepMotorMovement_Y( StepMotor::parkSlewing, pTask->m_YAxis );

            stepMotorMovement_X.join();
            stepMotorMovement_Y.join();

            gpio_set_level( static_cast<gpio_num_t>( ENABLE_PIN ), 1 );

            isPark = false;
            stop = false;
            ESP_LOGI( "PARK", "Park ended" );

        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void StepMotor::slewing( motorPasso axis )
{
    while( axis.m_steps-- && !stop )
    {
        gpio_set_level( axis.m_stepGpio, 1 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        gpio_set_level( axis.m_stepGpio, 0 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
    }
}

void StepMotor::parkSlewing( motorPasso axis )
{
    bool changedOnce = false;
    while( axis.m_steps )
    {
        if( axis.m_dirGpio == static_cast<gpio_num_t> (X_DIR) )
        {
            if( !gpio_get_level( static_cast<gpio_num_t> ( AZ_FRONT ) ) )
            {
                ESP_LOGI( "PARK", "STOP AZ_FRONT" );
                break;
            }

            if( !changedOnce && !gpio_get_level( static_cast<gpio_num_t> ( AZ_BACK ) ) )
            {
                ESP_LOGI( "PARK", "STOP AZ_BACK" );
                axis.m_direction = COUNTERCLOCKWISE;
                axis.m_steps = ANGLE_180 + 1;
                gpio_set_level( axis.m_dirGpio, axis.m_direction );
                changedOnce = true;
            }
        }
        else
        {
            if( !gpio_get_level( static_cast<gpio_num_t> ( ALT ) ) )
            {
                ESP_LOGI( "PARK", "STOP ALT" );
                break;
            }
        }

        gpio_set_level( axis.m_stepGpio, 1 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        gpio_set_level( axis.m_stepGpio, 0 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        --axis.m_steps;
    }

    if( axis.m_steps == 0 )
    {
        ESP_LOGE( "PARK", "Something went wrong, check Hall Sensor Pinout" );
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
    FullStop();
    isPark = true;
    ESP_LOGI( "PARK", "Park initiated" );
}

void StepMotor::FullStop()
{
   ESP_LOGI( "STEPMOTOR", "FULL STOP" );
   stop = true;
}