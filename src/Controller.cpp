#include "Controller.h"

Controller::Controller() :
    m_gpsHandle(),
    m_commsHandle(),
    m_motorHandle(),
    m_communication(),
    m_protocol(),
    m_motorPasso(),
    m_gps(),
    m_gpsHasStopped( false )
{

}

void Controller::Init()
{
    m_motorPasso.Init();
    m_communication.Init();
    m_gps.initUART2();
}

void Controller::Run()
{
    xTaskCreate( &StepMotor::Run, "Run Motor", 4096, &m_motorPasso, tskIDLE_PRIORITY, &m_motorHandle );
    xTaskCreate( &Comms::RunTCPServer, "Run TCP/IP Comms", 4096, &m_communication, tskIDLE_PRIORITY, &m_commsHandle );
    xTaskCreate( &GPS::Gps6mv2::receiveData, "Run GPS", 32000, &m_gps, tskIDLE_PRIORITY, &m_gpsHandle);
}

void Controller::machineState()
{
    while( 1 )
    {
        m_protocol.translateMessage( m_communication.receiveCommand() );

        switch ( m_protocol.GetCommand() )
        {
            case IndiDriverProtocol::GoTo:
            case IndiDriverProtocol::Track:
            {
                if( m_protocol.GetCommand() == IndiDriverProtocol::GoTo )
                {
                    printLCD( "Status:", "Slewing...");
                }
                else
                {
                    printLCD( "Status:", "Tracking...");
                }

                uint8_t stepDirectionAltitude;
                uint8_t stepDirectionAzimuth;
                stepDirectionAltitude = ( m_protocol.GetAltitudeSteps() ) >= 0 ? StepMotor::CLOCKWISE : StepMotor::COUNTERCLOCKWISE;
                stepDirectionAzimuth = ( m_protocol.GetAzimuthSteps() ) >= 0 ? StepMotor::CLOCKWISE : StepMotor::COUNTERCLOCKWISE;

                m_motorPasso.Movement( stepDirectionAzimuth, std::abs( m_protocol.GetAzimuthSteps() ),
                                        stepDirectionAltitude, std::abs( m_protocol.GetAltitudeSteps() ) );

                std::cout << stepDirectionAltitude << " , " << m_protocol.GetAltitudeSteps() << '\n';
                std::cout << stepDirectionAzimuth << " , " << m_protocol.GetAzimuthSteps() << '\n';

                m_communication.sendCommand( "OK" );
                return;
                break;
            }
            case IndiDriverProtocol::Park:
            {
                printLCD( "Status:", "PARKING...");
                m_motorPasso.Park();
                m_communication.sendCommand( "OK" );
                break;
            }
            case IndiDriverProtocol::GetLocation:
            {
                std::cout << "Entrei getLocation " << '\n';
                std::tuple< double, double, double > gpsInfo{ m_gps.GpsInformation() };

                double longitude{ std::get<0>( gpsInfo ) };
                double latitude{ std::get<1>( gpsInfo ) };
                double altitude{ std::get<2>( gpsInfo ) };

                char stream[28] = {0};
                char* streamptr = stream;
                memcpy(streamptr, &longitude, sizeof( double ));
                streamptr += sizeof(double);
                *streamptr++ = ',';
                memcpy(streamptr, &latitude, sizeof( double ));
                streamptr += sizeof(double);
                *streamptr++ = ',';
                memcpy(streamptr, &altitude, sizeof( double ));
                streamptr += sizeof(double);
                *streamptr++ = '\0';

                for( int i{0}; i<28; i++)
                {
                    std::cout << std::hex << (int)stream[i] << " ";
                }
                std::cout << '\n';

                std::cout << "travar? " << '\n';

                m_communication.sendCommand( stream );
                break;
            }
            case IndiDriverProtocol::Stop:
            {
                printLCD( "Status:", "FULL STOP");
                m_motorPasso.FullStop();
                m_communication.sendCommand( "OK" );
                break;
            }
            default:
                break;
        }

        m_communication.sizeOfBuffer = 0;
        memset(&m_communication.receiveBuffer, 0, 255);

        if( !m_gpsHasStopped && m_gps.HasGPSLocation() )
        {
            std::tuple< double, double, double > gpsInfo;
            std::cout << "Pegar GPS: " << std::get<0>( gpsInfo ) << " " << std::get<1>( gpsInfo ) << " " << std::get<2>( gpsInfo ) << '\n';
            vTaskDelete( m_gpsHandle );
            m_gpsHasStopped = true;
        }

        vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
}

void Controller::printLCD( std::string upRow, std::string downRow )
{
    LCD_home();
    LCD_clearScreen();
    LCD_writeStr(upRow.c_str());
    LCD_setCursor(0, 1);
    LCD_writeStr(downRow.c_str());
}