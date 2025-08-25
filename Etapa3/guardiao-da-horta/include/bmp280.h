#ifndef BMP280_H
#define BMP280_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdbool.h>

void bmp280_init(i2c_inst_t *i2c);

bool bmp280_read_data(float *temp, float *pressure);

#endif