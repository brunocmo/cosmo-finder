#include "IndiDriverProtocol.h"

#include <regex>

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

void IndiDriverProtocol::translateMessage( std::string message )
{
    std::regex wordRegex;
    std::smatch matchRegex;
    m_commandId = static_cast< std::uint8_t >( message[0] );

    std::string data = message.substr( 1, message.size() -1 );
    switch ( m_commandId )
    {
    case GoTo:
        m_azimuthSteps = getFloatValues( data.substr( 0, sizeof( float ) ) );
        m_altitudeSteps = getFloatValues( data.substr( 5, sizeof( float ) ) );
        break;
    case Park:
        m_needToPark = true;
        break;
    case Track:
        m_azimuthSteps = getFloatValues( data.substr( 0, sizeof( float ) ) );
        m_altitudeSteps = getFloatValues( data.substr( 5, sizeof( float ) ) );
        break;
    case GetLocation:
        m_latitude = getDoubleValues( data.substr( 0, sizeof( double ) ) );
        m_longitude = getDoubleValues( data.substr( 8, sizeof( double ) ) );
        m_elevation = getFloatValues( data.substr( 16, sizeof( float ) ) );
        break;
    case Stop:
        m_needToStop = true;
        break;
    
    default:
        break;
    }
}

float IndiDriverProtocol::getFloatValues( std::string toTransform )
{
    float convertedValue;
    memcpy( &convertedValue, toTransform.c_str(), sizeof( float ) );
    return convertedValue;
}

double IndiDriverProtocol::getDoubleValues( std::string toTransform )
{
    double convertedValue;
    memcpy( &convertedValue, toTransform.c_str(), sizeof( double ) );
    return convertedValue;
}

        // wordRegex = R"(\d+)";
        // if( std::regex_search( data, matchRegex, wordRegex ) )
        // {
        //     m_azimuthSteps = matchRegex[1];
        // }