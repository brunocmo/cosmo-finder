
#include <stdio.h>
#include <driver/gpio.h>
// Include FreeRTOS for delay
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED 2 // LED connected to GPIO2

int app_main() {
    // Configure pin
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    // Main loop
    while(true) {
        printf("0 - Tô em zero\n");
        gpio_set_level(LED, 0);
        vTaskDelay(500 / portTICK_RATE_MS);
        printf("1 - Tô em um\n");
        gpio_set_level(LED, 1);
        vTaskDelay(500 / portTICK_RATE_MS);
        fflush(stdout);
    }
}