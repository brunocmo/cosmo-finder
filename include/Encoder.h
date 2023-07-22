#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_sleep.h"

#include <vector>
#include <string>

class Encoder
{
    public:
    Encoder();
    ~Encoder() = default;

    void Setup( int gpioA, int gpioB, int lowLimit, int highLimit );
    static bool interruptExample( pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx );
    void Run();

    std::vector<int> watchPoints;
    pcnt_event_callbacks_t cbs;
    QueueHandle_t queue;
    pcnt_unit_handle_t pcnt_unit;
    std::string encoderLog;
};