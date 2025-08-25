#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdbool.h>
#include "hardware/i2c.h"

void display_init(i2c_inst_t *i2c_port, uint8_t i2c_address);

void display_update(float aht_temp, float humidity, float pressure, bool bmp_ok, float lux, bool bh1750_ok);

#endif