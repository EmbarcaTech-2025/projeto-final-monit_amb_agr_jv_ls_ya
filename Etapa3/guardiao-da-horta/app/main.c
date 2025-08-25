#include <stdio.h>
#include "pico/stdlib.h"

#include "aht10.h"
#include "bmp280.h"

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

    printf("\nIniciando leituras...\n");
}

int main() {
    setup_hardware();

    while (1) {
        float aht_temp, humidity;
        float bmp_temp, pressure;

        printf("------------------------------------\n");
        if (aht10_read_data(&aht_temp, &humidity)) {
            printf("AHT10 -> Temp: %.2f C | Umidade: %.2f %%RH\n", aht_temp, humidity);
        } else {
            printf("AHT10 -> Falha na leitura.\n");
        }

        if (bmp280_read_data(&bmp_temp, &pressure)) {
            printf("BMP280 -> Temp: %.2f C | Pressao: %.2f hPa\n", bmp_temp, pressure);
        } else {
            printf("BMP280 -> Falha na leitura.\n");
        }
        
        sleep_ms(2000);
    }

    return 0;
}