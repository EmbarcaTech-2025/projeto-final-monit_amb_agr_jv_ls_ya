#include "bmp280.h"

static const uint8_t SENSOR_ADDR = 0x76;
static i2c_inst_t* i2c_port;

static uint16_t dig_T1;
static int16_t  dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int32_t t_fine; 

static int32_t compensate_temp(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

static uint32_t compensate_pressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    return (uint32_t)p;
}

static void read_calibration_data() {
    uint8_t buffer[24];
    uint8_t reg = 0x88;
    i2c_write_blocking(i2c_port, SENSOR_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_port, SENSOR_ADDR, buffer, 24, false);

    dig_T1 = (buffer[1] << 8) | buffer[0];
    dig_T2 = (buffer[3] << 8) | buffer[2];
    dig_T3 = (buffer[5] << 8) | buffer[4];
    dig_P1 = (buffer[7] << 8) | buffer[6];
    dig_P2 = (buffer[9] << 8) | buffer[8];
    dig_P3 = (buffer[11] << 8) | buffer[10];
    dig_P4 = (buffer[13] << 8) | buffer[12];
    dig_P5 = (buffer[15] << 8) | buffer[14];
    dig_P6 = (buffer[17] << 8) | buffer[16];
    dig_P7 = (buffer[19] << 8) | buffer[18];
    dig_P8 = (buffer[21] << 8) | buffer[20];
    dig_P9 = (buffer[23] << 8) | buffer[22];
}

// Funções públicas
void bmp280_init(i2c_inst_t *i2c) {
    i2c_port = i2c;
    read_calibration_data();

    uint8_t config_data[] = {0xF4, 0x27};
    i2c_write_blocking(i2c_port, SENSOR_ADDR, config_data, 2, false);
}

bool bmp280_read_data(float *temp, float *pressure) {
    uint8_t buffer[6];
    uint8_t reg = 0xF7;
    i2c_write_blocking(i2c_port, SENSOR_ADDR, &reg, 1, true);
    if (i2c_read_blocking(i2c_port, SENSOR_ADDR, buffer, 6, false) != 6) {
        return false;
    }

    int32_t adc_P = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);
    int32_t adc_T = (buffer[3] << 12) | (buffer[4] << 4) | (buffer[5] >> 4);

    int32_t temp_c100 = compensate_temp(adc_T);
    uint32_t pressure_pa = compensate_pressure(adc_P);

    *temp = temp_c100 / 100.0f;
    *pressure = (pressure_pa / 256.0f) / 100.0f;

    return true;
}