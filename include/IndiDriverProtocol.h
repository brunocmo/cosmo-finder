#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <cmath>


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

    std::uint8_t GetCommand();
    int GetAzimuthSteps();
    int GetAltitudeSteps();

    void translateMessage( char* message );

private:
    int getIntValues( char* toTransform );
    float getFloatValues( char* toTransform );
    double getDoubleValues( char* toTransform );

    std::uint8_t m_commandId;

    int m_azimuthSteps;
    int m_altitudeSteps;

    double m_latitude;
    double m_longitude;
    float m_elevation;
    bool m_needToPark;
    bool m_needToStop;

    short m_crc;
};