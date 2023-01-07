#include "Gps6mv2.h"

using namespace GPS;

Gps6mv2::Gps6mv2()
{
    uart_buffer_size = (1024);
    initUART0();

}

Gps6mv2::~Gps6mv2() = default;

void Gps6mv2::initUART0()
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TXD_PIN2, RXD_PIN2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, 0, 0, NULL, 0));

}

void Gps6mv2::sendData()
{
    // Write data to UART.                                                                                                                                                        1   "";
    std::string marcao{"Ola mundo?"};
    uart_write_bytes(UART_NUM_2, (const char*)marcao.c_str(), marcao.size());

    // // Write data to UART, end with a break signal.
    // uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);
}

void Gps6mv2::receiveData()
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*)malloc(uart_buffer_size+1);

        const int rxBytes = uart_read_bytes(UART_NUM_2, data, uart_buffer_size, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            //ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
            
        }

    free(data);
}