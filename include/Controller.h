#pragma once

#include "StepMotor.h"
#include "IndiDriverProtocol.h"
#include "Comms.h"
#include "HD44780.h"
#include "Gps6mv2.h"

#include <string>
#include <sstream>
#include <tuple>

class Controller
{
public:
    Controller();
    ~Controller() = default;

    void Init();
    void Run();
    void machineState();

    void printLCD( char* upRow, char* downRow );

    TaskHandle_t m_gpsHandle;
    TaskHandle_t m_commsHandle;
    TaskHandle_t m_motorHandle;
    Comms m_communication;
    IndiDriverProtocol m_protocol;
    StepMotor m_motorPasso;
    GPS::Gps6mv2 m_gps;

    bool m_gpsHasStopped;
};