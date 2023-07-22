#include <string>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include <driver/i2c.h>
#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/queue.h"
#include <unistd.h>
#include <thread>

#include "Gps6mv2.h"
#include "Controller.h"

#include "Wifi.h"
#include "HD44780.h"
#include "Encoder.h"

#define LCD_ADDR 0x27
#define SDA_PIN  21
#define SCL_PIN  22
#define LCD_COLS 16
#define LCD_ROWS 2

#define WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD

WIFI::Wifi::state_e wifiState { WIFI::Wifi::state_e::NOT_INITIALIZED };
WIFI::Wifi Wifi;

void setupWifi()
{
    esp_event_loop_create_default();
    nvs_flash_init();

    Wifi.SetCredentials(WIFI_SSID, WIFI_PASS);
    Wifi.Init(); 
}

void runWifi( void* pTaskInstance )
{
    while( 1 )
    {
        wifiState = Wifi.GetState();

        switch (wifiState)
        {
        case WIFI::Wifi::state_e::READY_TO_CONNECT:
            std::cout << "Wifi Status: READY_TO_CONNECT\n";
            Wifi.Begin();
            break;
        case WIFI::Wifi::state_e::DISCONNECTED:
            std::cout << "Wifi Status: DISCONNECTED\n";
            Wifi.Begin();
            break;
        case WIFI::Wifi::state_e::CONNECTING:
            std::cout << "Wifi Status: CONNECTING\n";
            break;
        case WIFI::Wifi::state_e::WAITING_FOR_IP:
            std::cout << "Wifi Status: WAITING_FOR_IP\n";
            break;
        case WIFI::Wifi::state_e::ERROR:
            std::cout << "Wifi Status: ERROR\n";
            break;
        case WIFI::Wifi::state_e::CONNECTED:
            //std::cout << "Wifi Status: CONNECTED\n";
            // testServer();
            break;
        case WIFI::Wifi::state_e::NOT_INITIALIZED:
            std::cout << "Wifi Status: NOT_INITIALIZED\n";
            break;
        case WIFI::Wifi::state_e::INITIALIZED:
            std::cout << "Wifi Status: INITIALIZED\n";
            break;
        }
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}


extern "C" void app_main(void)
{
    // Shutting down stepMotor drivers
    ESP_ERROR_CHECK(gpio_reset_pin(GPIO_NUM_25));
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_25));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_25));
    gpio_set_level( GPIO_NUM_25, 1 );

    LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);

    LCD_home();
    LCD_clearScreen();
    LCD_writeStr("Starting");
    LCD_setCursor(0, 1);
    LCD_writeStr("Cosmo Finder...");

    setupWifi();
    xTaskCreate( &runWifi, "Run Wifi", 4096, NULL, tskIDLE_PRIORITY, NULL );

    while ( wifiState != WIFI::Wifi::state_e::CONNECTED )
    {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }

    std::cout << "Wifi Status: CONNECTED\n";

    LCD_home();
    LCD_clearScreen();
    LCD_writeStr("Connect to IP");
    LCD_setCursor(0, 1);
    LCD_writeStr(WIFI::myIp);

    vTaskDelay( 5000 / portTICK_PERIOD_MS );

    Controller controller;
    controller.Init();
    controller.Run();
    
    LCD_home();
    LCD_clearScreen();
    LCD_writeStr("Cosmo Finder");
    LCD_setCursor(0, 1);
    LCD_writeStr("Ready!");

    while( wifiState == WIFI::Wifi::state_e::CONNECTED )
    {
        controller.machineState();
    }
}
