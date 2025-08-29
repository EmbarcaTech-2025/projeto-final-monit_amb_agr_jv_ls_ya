#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

void display_init(i2c_inst_t *i2c_port, uint8_t i2c_address);
void display_update(float aht_temp, float humidity, float pressure, bool bmp_ok, float lux, bool bh1750_ok);
void display_render_wifi_status(const char* ssid, bool wifi_ok, bool mqtt_ok);
void display_render_sd_status(bool mounted, uint64_t total_kb, uint64_t used_kb, int last_errno);
void display_render_timing(uint32_t period_ms, uint64_t uptime_ms, uint32_t last_loop_ms);

#endif
