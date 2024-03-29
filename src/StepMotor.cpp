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

void StepMotor::Init()
{
    m_XAxis.m_stepGpio = static_cast<gpio_num_t> (X_STEP);
    m_XAxis.m_dirGpio = static_cast<gpio_num_t> (X_DIR);
    m_XAxis.m_log += "X";

    m_YAxis.m_stepGpio = static_cast<gpio_num_t> (Y_STEP);
    m_YAxis.m_dirGpio = static_cast<gpio_num_t> (Y_DIR);
    m_YAxis.m_log += "Y";

    setupPinOut( m_XAxis.m_stepGpio );
    setupPinOut( m_XAxis.m_dirGpio );

    encoderX.Setup( 23, 4, -10, 10 );
    encoderY.Setup( 18, 19, -10, 10 );

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
    pTask->encoderX.Run();
    pTask->encoderY.Run();


    while(1)
    {
        if( !pTask->m_movementList.empty() && !isPark )
        {
            for( auto movementIterator = pTask->m_movementList.begin(); movementIterator != pTask->m_movementList.end(); ++movementIterator )
            {
                pTask->m_XAxis = std::get<0>( *movementIterator );
                pTask->m_YAxis = std::get<1>( *movementIterator );

                ESP_LOGI( pTask->m_XAxis.m_log.c_str(), "Moving %llu steps, direction %s", pTask->m_XAxis.m_steps, pTask->stepDirection( pTask->m_XAxis.m_direction ).c_str() );
                ESP_LOGI( pTask->m_YAxis.m_log.c_str(), "Moving %llu steps, direction %s", pTask->m_YAxis.m_steps, pTask->stepDirection( pTask->m_YAxis.m_direction ).c_str() );

                std::unique_ptr< std::thread > threadX{nullptr};
                std::unique_ptr< std::thread > threadY{nullptr};

                gpio_set_level( static_cast<gpio_num_t>( ENABLE_PIN ), 0 );
                if( pTask->m_XAxis.m_steps > 0 )
                {
                    gpio_set_level( pTask->m_XAxis.m_dirGpio, pTask->m_XAxis.m_direction );
                    threadX = std::make_unique< std::thread > ( StepMotor::slewing, pTask->m_XAxis, pTask->encoderX );
                }

                if( pTask->m_YAxis.m_steps > 0 )
                {
                    gpio_set_level( pTask->m_YAxis.m_dirGpio, pTask->m_YAxis.m_direction );
                    threadY = std::make_unique< std::thread > ( StepMotor::slewing, pTask->m_YAxis, pTask->encoderY );
                }

                if( threadX )
                {
                    threadX.get()->join();
                    threadX.reset(nullptr);
                }
                if( threadY )
                {
                    threadY.get()->join();
                    threadY.reset(nullptr);
                }
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

void StepMotor::slewing( motorPasso axis, Encoder encoder )
{
    int pulse_count{ 0 };
    while( axis.m_steps-- && !stop )
    {
        gpio_set_level( axis.m_stepGpio, 1 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        gpio_set_level( axis.m_stepGpio, 0 );
        std::this_thread::sleep_for( std::chrono::microseconds( axis.m_speed ));
        // ESP_ERROR_CHECK( pcnt_unit_get_count( encoder.pcnt_unit, &pulse_count ) );
        // if( pulse_count == 0)
        // {
        //     //ESP_LOGI( axis.m_log.c_str(), "Step Loss, adding more: %llu steps left", axis.m_steps );
        //     axis.m_steps++;
        // }
        // else
        // {
        //     ESP_ERROR_CHECK( pcnt_unit_clear_count( encoder.pcnt_unit ) );
        // }
    }
}

void StepMotor::parkSlewing( motorPasso axis )
{
    int tries = 0;
    bool changedOnce = false;
    while( axis.m_steps )
    {
        if( axis.m_dirGpio == static_cast<gpio_num_t> (X_DIR) )
        {
            if( !gpio_get_level( static_cast<gpio_num_t> ( AZ_BACK ) ) )
            {
                if( !gpio_get_level( static_cast<gpio_num_t> ( AZ_FRONT ) ) )
                {
                    ESP_LOGI( "PARK", "STOP FRONT" );
                    break;
                }
            }

            if( !gpio_get_level( static_cast<gpio_num_t> ( AZ_FRONT ) ) && !changedOnce )
            {
                if( tries > 5 ) 
                {
                    axis.m_direction = COUNTERCLOCKWISE;
                    axis.m_steps = ANGLE_180;
                    gpio_set_level( axis.m_dirGpio, axis.m_direction );
                    ESP_LOGI( "PARK", "STOP BACK" );
                    changedOnce = true;
                }
                ESP_LOGI( "PARK", "STOP AZ_BACK" );

                tries++;
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

    //ESP_LOGI( "Teste", "Moving %llu steps", lessesSpeed );

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

std::string StepMotor::stepDirection( std::uint8_t direction )
{
    if( direction == CLOCKWISE )
    {
        return std::string("CLOCKWISE");
    }
    else
    {
        return std::string("COUNTERCLOCKWISE");
    }
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