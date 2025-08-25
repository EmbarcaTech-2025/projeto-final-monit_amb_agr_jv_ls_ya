#include <stdio.h>
#include "pico/stdlib.h"

#include "aht10.h"
#include "bmp280.h"
#include "bh1750.h"

#define I2C_PORT i2c0
const uint I2C_SDA_PIN = 0;
const uint I2C_SCL_PIN = 1;

void setup_hardware() {

    stdio_init_all();
    sleep_ms(2000);
    printf("Hardware inicializado.\n");

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    aht10_init(I2C_PORT);
    printf("Sensor AHT10 inicializado.\n");
    
    bmp280_init(I2C_PORT);
    printf("Sensor BMP280 inicializado.\n");

    bh1750_init(I2C_PORT);
    printf("Sensor BH1750 inicializado.\n");

    printf("\nIniciando leituras...\n");
}

int main() {
    setup_hardware();

    while (1) {
        float aht_temp, humidity;
        float bmp_temp, pressure;
        float lux;
        bool bmp_status, bh1750_status;

        aht10_read_data(&aht_temp, &humidity);
        bmp_status = bmp280_read_data(&bmp_temp, &pressure);
        bh1750_status = bh1750_read_lux(&lux);

        printf("------------------------------------\n");
        printf("AHT10 -> Temp: %.2f C | Umidade: %.2f %%RH\n", aht_temp, humidity);
        
        if (bmp_status) {
            printf("BMP280 -> Temp: %.2f C | Pressao: %.2f hPa\n", bmp_temp, pressure);
        } else {
            printf("BMP280 -> Falha na leitura.\n");
        }

        if (bh1750_status) {
            printf("BH1750 -> Luz: %.2f lux\n", lux);
        } else {
            printf("BH1750 -> Falha na leitura.\n");
        }
        
        sleep_ms(2000);
    }

    return 0;
}