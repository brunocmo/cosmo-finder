#ifndef STEPMOTOR_H
#define STEPMOTOR_H

#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

void init();
void testOutput();


#endif