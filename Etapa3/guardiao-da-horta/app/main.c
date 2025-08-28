#include <stdio.h>
#include "pico/stdlib.h"

#include "aht10.h"
#include "bmp280.h"
#include "bh1750.h"
#include "display.h"

#include "sdcard_manager.h"

#define I2C_PORT_SENSORS i2c0
const uint I2C_SDA_SENSORS_PIN = 0;
const uint I2C_SCL_SENSORS_PIN = 1;

#define I2C_PORT_DISPLAY i2c1
const uint I2C_SDA_DISPLAY_PIN = 14;
const uint I2C_SCL_DISPLAY_PIN = 15;
const uint8_t I2C_OLED_ADDR = 0x3C;

void setup_hardware();
void read_print_sensors_data();


int main() {
    setup_hardware();

    // sd_write_data("Temp:707,pressao:1bar,umidade:1,Luz:1.5\n","2025-08-27","/");//escreve texto em arquivo de nome '2025-08-27(ano-mes-dia)' e armazena na pasta raiz do SDcard
    char string_teste[2000]={};
    sd_read_data(string_teste,2000,"filename.txt","/");

    printf("Escrevendo conteúdo lido no arquivo: \n%s\n",string_teste);


    while (1) {

        read_print_sensors_data();
        
        sleep_ms(2000);
    }
    return 0;
}


void setup_hardware() {
    stdio_init_all();
    sleep_ms(2000);//necessário para o monitor serial funcionar com SDcard por SPI ligado
    printf("Hardware inicializado.\n");

    i2c_init(I2C_PORT_SENSORS, 100 * 1000);
    gpio_set_function(I2C_SDA_SENSORS_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_SENSORS_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_SENSORS_PIN);
    gpio_pull_up(I2C_SCL_SENSORS_PIN);

    i2c_init(I2C_PORT_DISPLAY, 400 * 1000);
    gpio_set_function(I2C_SDA_DISPLAY_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISPLAY_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISPLAY_PIN);
    gpio_pull_up(I2C_SCL_DISPLAY_PIN);
    
    display_init(I2C_PORT_DISPLAY, I2C_OLED_ADDR);
    aht10_init(I2C_PORT_SENSORS);
    bmp280_init(I2C_PORT_SENSORS);
    bh1750_init(I2C_PORT_SENSORS);
    printf("Sensores e display inicializados.\n\nIniciando leituras...\n");
}

void read_print_sensors_data(){
    float aht_temp, humidity, bmp_temp, pressure, lux;
    bool bmp_status, bh1750_status;
    
    aht10_read_data(&aht_temp, &humidity);
    bmp_status = bmp280_read_data(&bmp_temp, &pressure);
    bh1750_status = bh1750_read_lux(&lux);

    printf("------------------------------------\n");
    printf("AHT10: T=%.1fC, H=%.1f%% | BMP280: T=%.1fC, P=%.1f hPa | BH1750: L=%.1f lux\n", 
            aht_temp, humidity, bmp_temp, pressure, lux);
            
    display_update(aht_temp, humidity, pressure, bmp_status, lux, bh1750_status);
}
