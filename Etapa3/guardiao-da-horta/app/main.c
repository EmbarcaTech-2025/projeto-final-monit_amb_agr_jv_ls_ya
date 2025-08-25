#include <stdio.h>
#include "pico/stdlib.h"

#include "aht10.h"

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
    printf("Sensor AHT10 inicializado. Iniciando leituras.\n\n");
}

int main() {
    setup_hardware();

    while (1) {
        float temperatura, umidade;

        if (aht10_read_data(&temperatura, &umidade)) {
            printf("Temperatura: %.2f C\n", temperatura);
            printf("Umidade:     %.2f %%RH\n", umidade);
        } else {
            printf("Falha na leitura do sensor. Tentando novamente...\n");
        }

        printf("---------------------------\n");
        sleep_ms(2000);
    }

    return 0;
}