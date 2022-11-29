#include "stepMotor.h"

void init()
{
    gpio_pad_select_gpio(STEPX);
    gpio_pad_select_gpio(DIRX);

    gpio_set_direction(STEPX, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIRX, GPIO_MODE_OUTPUT);


    gpio_pad_select_gpio(STEPY);
    gpio_pad_select_gpio(DIRY);

    gpio_set_direction(STEPY, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIRY, GPIO_MODE_OUTPUT);
}

void testOutput()
{

    printf("Um lado e depois \n");
    gpio_set_level( DIRX, 1 );
    gpio_set_level( DIRY, 0 );
    for(int x = 0; x < 400 ; ++x)
    {
        gpio_set_level( STEPX, 1 );
        gpio_set_level( STEPY, 0 );
        vTaskDelay(50 / portTICK_RATE_MS);
        gpio_set_level( STEPX, 0 );
        gpio_set_level( STEPY, 1 );            
        vTaskDelay(50 / portTICK_RATE_MS);

        printf(" %d fff \n", x);         

    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Outra lado e depois\n");
    gpio_set_level( DIRX, 0 );
    gpio_set_level( DIRY, 1 );
    for(int x = 0; x < 400 ; ++x)
    {
        gpio_set_level( STEPX, 1 );
        gpio_set_level( STEPY, 0 );
        vTaskDelay(50 / portTICK_RATE_MS);
        gpio_set_level( STEPX, 0 );
        gpio_set_level( STEPY, 1 );
        vTaskDelay(50 / portTICK_RATE_MS);

        printf(" %d ddd \n", x);   
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

}