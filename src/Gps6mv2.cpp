#include "Gps6mv2.h"

#include <regex>

using namespace GPS;

Gps6mv2::Gps6mv2()
{
    m_hasValue = false;
    uart_buffer_size = (1024);

    m_latitude = 0;
    m_longitude = 0;
    m_altitude = 0;
}

Gps6mv2::~Gps6mv2() = default;

void Gps6mv2::initUART2()
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

void Gps6mv2::receiveData( void* pTaskInstance )
{
    Gps6mv2* pTask = (Gps6mv2* ) pTaskInstance;
    static const char *RX_TASK_TAG = "RX_TASK";

    std::regex wordRegex;
    std::smatch matchRegex;
    std::string dataString;
    std::string stringMatch;

    wordRegex = R"(\$GPGGA,[\d\.]+,(\d+.\d+),(\w),(\d+.\d+)+,(\w),\d*,\d*,[\d\.]*,(\d+.\d+)*,.+)";

    char* data = (char*)malloc(pTask->uart_buffer_size+1);
    int rxBytes = 0;

    while( !pTask->m_hasValue )
    {
        ESP_LOGI(RX_TASK_TAG, "LI");
        rxBytes = uart_read_bytes(UART_NUM_2, data, pTask->uart_buffer_size, 5000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;

            dataString = data;

            if( std::regex_search( dataString, matchRegex, wordRegex ) )
            {
                stringMatch = matchRegex[0];
                ESP_LOGI(RX_TASK_TAG, "Read '%s'", stringMatch.c_str());

                pTask->m_latitude = std::stod( matchRegex[1] );
                pTask->m_latitude /= 100;
                if( matchRegex[2] == 'S' ) pTask->m_latitude *= -1;
                pTask->m_longitude = std::stod( matchRegex[3] );
                pTask->m_longitude /= 100;
                if( matchRegex[4] == 'W' ) pTask->m_longitude *= -1;
                pTask->m_altitude = std::stod( matchRegex[5] );
                pTask->m_hasValue = true;
            }
        }
    }

    free(data);

    while( 1 )
    {
        vTaskDelay( 5000/portTICK_PERIOD_MS );
    }

}


bool Gps6mv2::HasGPSLocation()
{
    return m_hasValue;
}

std::tuple< double, double, double > Gps6mv2::GpsInformation()
{
    if( m_hasValue )
    {
        return { m_latitude, m_longitude, m_altitude };
    }
    return { 0, 0, 0 };
}