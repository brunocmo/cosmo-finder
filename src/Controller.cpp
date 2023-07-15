#include "Controller.h"

Controller::Controller() :
    m_communication(),
    m_protocol(),
    m_motorPasso()
{

}

void Controller::Init()
{
    m_motorPasso.Init();
    m_communication.Init();
}

void Controller::Run()
{
    xTaskCreate( &StepMotor::Run, "Run Motor", 4096, &m_motorPasso, tskIDLE_PRIORITY, NULL );
    xTaskCreate( &Comms::RunTCPServer, "Run TCP/IP Comms", 4096, &m_communication, tskIDLE_PRIORITY, NULL );
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
        case IndiDriverProtocol::Park:
            m_motorPasso.Park();
            m_communication.sendCommand( "OK" );
            break;
        case IndiDriverProtocol::GetLocation:
            // TODO
            break;
        case IndiDriverProtocol::Stop:
            m_motorPasso.FullStop();
            m_communication.sendCommand( "OK" );
            break;
        default:
            break;
        }

        m_communication.sizeOfBuffer = 0;
        memset(&m_communication.receiveBuffer, 0, 255);

        vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
}