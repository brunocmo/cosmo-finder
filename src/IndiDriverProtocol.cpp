#include "IndiDriverProtocol.h"

#include <regex>
#include <iostream>

IndiDriverProtocol::IndiDriverProtocol() :
    m_commandId( 0 ),
    m_azimuthSteps( 0 ),
    m_altitudeSteps( 0 ),
    m_latitude( 0 ),
    m_longitude( 0 ),
    m_elevation( 0 ),
    m_needToPark( false ),
    m_needToStop( false ),
    m_crc( 0 )
{
}

void IndiDriverProtocol::translateMessage( char* message )
{
    m_commandId = static_cast< std::uint8_t >( message[0] );

    switch ( m_commandId )
    {
    case GoTo:
    case Track:
        m_azimuthSteps = getIntValues( &message[1] );
        m_altitudeSteps = getIntValues( &message[6] );
        break;
    case Park:
        m_needToPark = true;
        break;
    case GetLocation:
        break;
    case Stop:
        m_needToStop = true;
        break;
    
    default:
        break;
    }
}

std::uint8_t IndiDriverProtocol::GetCommand()
{
    return m_commandId;
}

int IndiDriverProtocol::GetAzimuthSteps()
{
    return m_azimuthSteps;
}

int IndiDriverProtocol::GetAltitudeSteps()
{
    return m_altitudeSteps;
}

int IndiDriverProtocol::getIntValues( char* toTransform )
{
    int convertedValue;
    memcpy( &convertedValue, toTransform, sizeof( int ) );
    return convertedValue;
}

float IndiDriverProtocol::getFloatValues( char* toTransform )
{
    float convertedValue;
    memcpy( &convertedValue, toTransform, sizeof( float ) );
    return convertedValue;
}

double IndiDriverProtocol::getDoubleValues( char* toTransform )
{
    double convertedValue;
    memcpy( &convertedValue, toTransform, sizeof( double ) );
    return convertedValue;
}
