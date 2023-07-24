#include "Gps6mv2.h"

#include <regex>

using namespace GPS;

Gps6mv2::Gps6mv2()
{
    m_hasValue = false;
    uart_buffer_size = 1024;

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

    ESP_LOGI(RX_TASK_TAG, "TESTE");
    char wtf[1024] = {0};
    char* data = wtf;
    int rxBytes = 0;

    while( 1 )
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
                break;
            }
        }
    }

    while( 1 )
    {
        vTaskDelay( 5000/portTICK_PERIOD_MS );
        std::cout << "meucu" << '\n';
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

bool Gps6mv2::readGPSLatitudePersistence()
{
    ESP_ERROR_CHECK( nvs_flash_init_partition("nvsData") );

    nvs_handle_t partitionHandle;
    esp_err_t responseNVS = nvs_open_from_partition("nvsData", "gpsPersist", NVS_READONLY, &partitionHandle );

    int32_t latitude{0};

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsLatitude", "Namespace not found");
        nvs_close( partitionHandle );
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_get_i32(responseNVS, "latitude", &latitude);

        switch( responseStr )
        {
            case ESP_OK:
                std::cout << "Latitude value: " << latitude << '\n';
                //m_latitude = std::stod( latitude );
                break;
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE("gpsLatitude", "latitude not found");
                nvs_close( partitionHandle );
                return false;
            default:
                ESP_LOGE("gpsLatitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));
                nvs_close( partitionHandle );
                return false;
        }

        nvs_close( partitionHandle );
        return true;
    }
}

bool Gps6mv2::writeGPSLatitudePersistence( double latitude )
{
    ESP_ERROR_CHECK( nvs_flash_init_partition("nvsData") );

    nvs_handle_t partitionHandle;
    esp_err_t responseNVS = nvs_open_from_partition("nvsData", "gpsPersist", NVS_READWRITE, &partitionHandle );

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsLatitude", "Namespace not found");
        nvs_close( partitionHandle );
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_set_i32(responseNVS, "latitude", 13 );


        ESP_LOGE("gpsLatitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));


        nvs_commit( partitionHandle );
        nvs_close( partitionHandle );
        return true;
    }
}

bool Gps6mv2::readGPSLongitudePersistence()
{
    ESP_ERROR_CHECK( nvs_flash_init() );

    nvs_handle partitionHandle;
    esp_err_t responseNVS = nvs_open("gpsPersist", NVS_READONLY, &partitionHandle );

    size_t sizeLong = 12;
    char longitude[12] = {0};

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsLongitude", "Namespace not found");
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_get_str(responseNVS, "longitude", longitude, &sizeLong );

        switch( responseStr )
        {
            case ESP_OK:
                std::cout << "Longitude value: " << longitude << '\n';
                m_longitude = std::stod( longitude );
                break;
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE("gpsLongitude", "Longitude not found");
                return false;
            default:
                ESP_LOGE("gpsLongitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));
                return false;
        }

        nvs_close( partitionHandle );
        return true;
    }
}

bool Gps6mv2::writeGPSLongitudePersistence( double longitude )
{
    ESP_ERROR_CHECK( nvs_flash_init() );

    nvs_handle partitionHandle;
    esp_err_t responseNVS = nvs_open("gpsPersist", NVS_READWRITE, &partitionHandle );

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsLongitude", "Namespace not found");
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_set_str(responseNVS, "longitude", std::to_string(longitude).c_str() );

        if( responseStr == ESP_ERR_NVS_NOT_FOUND )
        {
            ESP_LOGE("gpsLongitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));
            return false;
        }

        nvs_close( partitionHandle );
        return true;
    }
}

bool Gps6mv2::readGPSAltitudePersistence()
{
    ESP_ERROR_CHECK( nvs_flash_init() );

    nvs_handle partitionHandle;
    esp_err_t responseNVS = nvs_open("gpsPersist", NVS_READONLY, &partitionHandle );

    size_t sizeLong = 9;
    char altitude[9] = {0};

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsAltitude", "Namespace not found");
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_get_str(responseNVS, "altitude", altitude, &sizeLong );

        switch( responseStr )
        {
            case ESP_OK:
                std::cout << "altitude value: " << altitude << '\n';
                m_altitude = std::stod( altitude );
                break;
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE("gpsAltitude", "altitude not found");
                return false;
            default:
                ESP_LOGE("gpsAltitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));
                return false;
        }

        nvs_close( partitionHandle );
        return true;
    }
}

bool Gps6mv2::writeGPSAltitudePersistence( double altitude )
{
    ESP_ERROR_CHECK( nvs_flash_init() );

    nvs_handle partitionHandle;
    esp_err_t responseNVS = nvs_open("gpsPersist", NVS_READWRITE, &partitionHandle );

    if( responseNVS == ESP_ERR_NVS_NOT_FOUND )
    {
        ESP_LOGE("gpsAltitude", "Namespace not found");
        return false;
    }
    else
    {
        esp_err_t responseStr = nvs_set_str(responseNVS, "altitude", std::to_string(altitude).c_str() );

        if( responseStr == ESP_ERR_NVS_NOT_FOUND )
        {
            ESP_LOGE("gpsAltitude", "Error to get NVS (%s)", esp_err_to_name(responseStr));
            return false;
        }

        nvs_close( partitionHandle );
        return true;
    }
}