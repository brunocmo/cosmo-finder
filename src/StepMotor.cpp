#include "StepMotor.h"

#include <chrono>
#include <iostream>
#include <thread>

static const char* tag = "StepMotor";
int step = 0;
gptimer_handle_t gptimer;
gptimer_alarm_config_t alarm_config;

bool IRAM_ATTR example_timer_on_alarm_cb_v1(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_data);


StepMotor::StepMotor()
{
    this->init();
    this->gptimer_init();
}

void StepMotor::setupPin( gpio_num_t pinSet )
{
    ESP_ERROR_CHECK(gpio_reset_pin(pinSet));
    ESP_ERROR_CHECK(gpio_set_direction(pinSet, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_pulldown_dis(pinSet));
    ESP_ERROR_CHECK(gpio_pullup_dis(pinSet));
}

void StepMotor::init()
{
    // X Axis Step and Dir
    setupPin( GPIO_NUM_12 );
    setupPin( GPIO_NUM_14 );
    // Y Axis Step and Dir
    setupPin( GPIO_NUM_27 );
    setupPin( GPIO_NUM_26 );
}

void StepMotor::testOutput( void * params )
{
    int direcaoX = 1;
    int direcaoY = 0;

    printf("Um lado e depois \n");
    gpio_set_level( GPIO_NUM_14, direcaoX );
    gpio_set_level( GPIO_NUM_26, direcaoY );

    while(1)
    {

        gptimer_stop(gptimer);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        gptimer_set_alarm_action(gptimer, &alarm_config);
        gptimer_start(gptimer);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        // auto start = std::chrono::high_resolution_clock::now();
        // for(int x = 0; x < 1000 ; ++x)
        // {
        //     gpio_set_level( GPIO_NUM_12, 1 );
        //     gpio_set_level( GPIO_NUM_27, 0 );
        //     std::this_thread::sleep_for( std::chrono::microseconds( 700 ));
        //     gpio_set_level( GPIO_NUM_12, 0 );
        //     gpio_set_level( GPIO_NUM_27, 1 );            
        //     std::this_thread::sleep_for( std::chrono::microseconds( 700 ));
        // }
        // auto end = std::chrono::high_resolution_clock::now();

        // // Calculate the duration of the loop in milliseconds
        // double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // // Print the execution time
        // std::cout << "Loop execution time: " << duration << " milliseconds" << std::endl;

        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // printf("Outra lado e depois\n");

        // direcaoX = 0;
        // direcaoY = 1;

        // gpio_set_level( GPIO_NUM_14, direcaoX );
        // gpio_set_level( GPIO_NUM_26, direcaoY );
        // for(int x = 0; x < 1000 ; ++x)
        // {
        //     gpio_set_level( GPIO_NUM_12, 1 );
        //     gpio_set_level( GPIO_NUM_27, 0 );
        //     vTaskDelay(5 / portTICK_PERIOD_MS);
        //     gpio_set_level( GPIO_NUM_12, 0 );
        //     gpio_set_level( GPIO_NUM_27, 1 );
        //     vTaskDelay(5 / portTICK_PERIOD_MS);
        // }

        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (counter % 2 == 0) {
        // gptimer_stop(gptimer);
        // gptimer_set_alarm_action(gptimer, &alarm_config2);
        // gptimer_start(gptimer);
        // } else {
        //     gptimer_stop(gptimer);
        //     gptimer_set_alarm_action(gptimer, &alarm_config1);
        //     gptimer_start(gptimer);
        // }
    }
}

void StepMotor::gptimer_init()
{
    QueueHandle_t queue = xQueueCreate(10, sizeof(example_queue_element_t));
    if (!queue) {
        ESP_LOGE(tag, "Creating queue failed");
        return;
    }

    ESP_LOGI(tag, "Create timer handle");
    gptimer                       = NULL;
    gptimer_config_t timer_config = {
        .clk_src       = GPTIMER_CLK_SRC_DEFAULT,
        .direction     = GPTIMER_COUNT_UP,
        .resolution_hz = 10000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb_v1,
    };
    // ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, this));

    ESP_LOGI(tag, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    ESP_LOGI(tag, "Start timer, stop it at alarm event");
    alarm_config.reload_count               = 0;
    alarm_config.alarm_count                = 6;
    alarm_config.flags.auto_reload_on_alarm = true;

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

bool IRAM_ATTR example_timer_on_alarm_cb_v1(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_data) {
    BaseType_t high_task_awoken = pdFALSE;
    // stop timer immediately
    // gptimer_stop(timer);
    // Retrieve count value and send to queue
    // motor->step();;
    // ESP_LOGI("TIMER", "step state %d", motor->step_state);
    step = !step;
    gpio_set_level(GPIO_NUM_12, step);

    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}