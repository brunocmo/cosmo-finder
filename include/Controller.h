#pragma once

#include "StepMotor.h"
#include "IndiDriverProtocol.h"
#include "Comms.h"

class Controller
{
public:
    Controller();
    ~Controller() = default;

    void Init();
    void Run();
    void machineState();

    Comms m_communication;
    IndiDriverProtocol m_protocol;
    StepMotor m_motorPasso;
};