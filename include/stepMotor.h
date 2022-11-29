#ifndef STEPMOTOR_H
#define STEPMOTOR_H

#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define STEPX 26
#define DIRX 16

#define STEPY 25
#define DIRY 27

void init();
void testOutput();


#endif