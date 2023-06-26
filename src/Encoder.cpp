#include "Encoder.h"

Encoder::Encoder() :
    watchPoints(),
    cbs(),
    queue(),
    pcnt_unit()
{

}

void Encoder::Setup()
{
    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = { EXAMPLE_PCNT_LOW_LIMIT, EXAMPLE_PCNT_HIGH_LIMIT };
    pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    ESP_LOGI(TAG, "set glitch filter");
    pcnt_glitch_filter_config_t filter_config = { 1000 };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = EXAMPLE_EC11_GPIO_A,
        .level_gpio_num = EXAMPLE_EC11_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));
    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = EXAMPLE_EC11_GPIO_B,
        .level_gpio_num = EXAMPLE_EC11_GPIO_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    ESP_LOGI(TAG, "set edge and level actions for pcnt channels");
    ESP_ERROR_CHECK( pcnt_channel_set_edge_action( pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE ) );
    ESP_ERROR_CHECK( pcnt_channel_set_level_action( pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE ) );
    ESP_ERROR_CHECK( pcnt_channel_set_edge_action( pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE ) );
    ESP_ERROR_CHECK( pcnt_channel_set_level_action( pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE ) );

    ESP_LOGI(TAG, "add watch points and register callbacks");

    watchPoints.push_back( EXAMPLE_PCNT_LOW_LIMIT );
    watchPoints.push_back( -50 );
    watchPoints.push_back( 0 );
    watchPoints.push_back( 50 );
    watchPoints.push_back( EXAMPLE_PCNT_HIGH_LIMIT );

    for ( auto watchPoint : watchPoints )
    {
        ESP_ERROR_CHECK( pcnt_unit_add_watch_point( pcnt_unit, watchPoint ) );
    }
}

bool Encoder::interruptExample(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    // send event data to queue, from this interrupt callback
    xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}

void Encoder::Run()
{
    cbs = { interruptExample };
    queue = xQueueCreate( 10, sizeof(int) );
    ESP_ERROR_CHECK( pcnt_unit_register_event_callbacks ( pcnt_unit, &cbs, queue ) );

    ESP_LOGI(TAG, "enable pcnt unit");
    ESP_ERROR_CHECK( pcnt_unit_enable(pcnt_unit) );
    ESP_LOGI(TAG, "clear pcnt unit");
    ESP_ERROR_CHECK( pcnt_unit_clear_count(pcnt_unit) );
    ESP_LOGI(TAG, "start pcnt unit");
    ESP_ERROR_CHECK( pcnt_unit_start(pcnt_unit) );

    // Report counter value
    int pulse_count = 0;
    int event_count = 0;
    while (1) {
        if (xQueueReceive(queue, &event_count, pdMS_TO_TICKS(1000))) {
            ESP_LOGI(TAG, "Watch point event, count: %d", event_count);
        } else {
            ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));
            ESP_LOGI(TAG, "Pulse count: %d", pulse_count);
        }
    }

}


