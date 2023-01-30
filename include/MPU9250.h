#pragma once

#include <driver/i2c.h>
#include <driver/gpio.h>



class MPU9250
{
public:
    enum {
        I2C_MASTER_SCL_IO = GPIO_NUM_22,
        I2C_MASTER_SDA_IO = GPIO_NUM_21,
        I2C_MASTER_NUM = 0,
        I2C_MASTER_FREQ_HZ = 400000,
        I2C_MASTER_TX_BUF_DISABLE = 0,
        I2C_MASTER_RX_BUF_DISABLE = 0,
        I2C_MASTER_TIMEOUT_MS = 1000,
        MPU9250_SENSOR_ADDR = 0X68,
        MPU9250_WHO_AM_I_REG_ADDR = 0X75,
        MPU9250_PWR_MGMT_1_REG_ADDR = 0X6B,
        MPU9250_RESET_BIT = 7

    };

    MPU9250();
    ~MPU9250();

    static esp_err_t mpu9250_register_read( u_int8_t reg_addr, u_int8_t *data, size_t len );
    static esp_err_t mpu9250_register_write( u_int8_t reg_addr, u_int8_t data );
    static esp_err_t i2c_master_init();
    

};