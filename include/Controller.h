#pragma once

#include "StepMotor.h"
#include "IndiDriverProtocol.h"
#include "Comms.h"
#include "HD44780.h"
#include <string>

class Controller
{
public:
    Controller();
    ~Controller() = default;

    void Init();
    void Run();
    void machineState();

    void printLCD( char* upRow, char* downRow );

    Comms m_communication;
    IndiDriverProtocol m_protocol;
    StepMotor m_motorPasso;
};