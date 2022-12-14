#include "stepMotor.h"

void init()
{
    gpio_pad_select_gpio(GPIO_NUM_26); // STEPX
    gpio_pad_select_gpio(GPIO_NUM_16); // DIRX

    gpio_set_direction(GPIO_NUM_26, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_16, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO_NUM_25); // STEPY
    gpio_pad_select_gpio(GPIO_NUM_27); // DIRY

    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);

    return;
}

void testOutput()
{
    std::string esquerdo = "ANTI H";
    std::string direito = "HORARIO";

    printf("Um lado e depois \n");
    gpio_set_level( GPIO_NUM_16, 1 );
    gpio_set_level( GPIO_NUM_27, 0 );
    for(int x = 0; x < 400 ; ++x)
    {
        gpio_set_level( GPIO_NUM_26, 1 );
        gpio_set_level( GPIO_NUM_25, 0 );
        vTaskDelay(50 / portTICK_RATE_MS);
        gpio_set_level( GPIO_NUM_26, 0 );
        gpio_set_level( GPIO_NUM_25, 1 );            
        vTaskDelay(50 / portTICK_RATE_MS);

        ESP_LOGI( esquerdo.c_str(), "valor: %d  fff", x);      

    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Outra lado e depois\n");
    gpio_set_level( GPIO_NUM_16, 0 );
    gpio_set_level( GPIO_NUM_27, 1 );
    for(int x = 0; x < 400 ; ++x)
    {
        gpio_set_level( GPIO_NUM_26, 1 );
        gpio_set_level( GPIO_NUM_25, 0 );
        vTaskDelay(50 / portTICK_RATE_MS);
        gpio_set_level( GPIO_NUM_26, 0 );
        gpio_set_level( GPIO_NUM_25, 1 );
        vTaskDelay(50 / portTICK_RATE_MS);

        ESP_LOGI(direito.c_str(), "valor: %d  ddd", x);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

}