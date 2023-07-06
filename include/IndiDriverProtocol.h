#pragma once

#include <cstdint>
#include <string>
#include <memory>


class IndiDriverProtocol
{
public:
    IndiDriverProtocol();
    ~IndiDriverProtocol() = default;

    enum Command : unsigned char
    {
        GoTo = 0x01,
        Park = 0x02,
        Track = 0x03,
        GetLocation = 0x04,
        Stop = 0x05
    };

    

private:
    void translateMessage( std::string message );

    float getFloatValues( std::string toTransform );
    double getDoubleValues( std::string toTransform );

    std::uint8_t m_commandId;

    float m_azimuthSteps;
    float m_altitudeSteps;

    double m_latitude;
    double m_longitude;
    float m_elevation;
    bool m_needToPark;
    bool m_needToStop;

    short m_crc;
};