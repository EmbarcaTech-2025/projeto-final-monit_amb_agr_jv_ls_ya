#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "aht10.h"
#include "bmp280.h"
#include "bh1750.h"
#include "display.h"

#define I2C_PORT_SENSORS i2c0
static const uint I2C_SDA_SENSORS_PIN = 0;
static const uint I2C_SCL_SENSORS_PIN = 1;

#define I2C_PORT_DISPLAY i2c1
static const uint I2C_SDA_DISPLAY_PIN = 14;
static const uint I2C_SCL_DISPLAY_PIN = 15;
static const uint8_t I2C_OLED_ADDR = 0x3C;

#define BTN_A_PIN 5
#define BTN_B_PIN 6
#define BTN_C_PIN 22

typedef enum {
    MENU_MEASUREMENTS = 0,
    MENU_WIFI,
    MENU_SD,
    MENU_TIMING,
    MENU_COUNT
} MenuId;

typedef struct {
    float aht_temp, humidity;
    float bmp_temp, pressure;
    float lux;
    bool  aht_ok, bmp_ok, lux_ok;
} SensorData;

typedef struct {
    char  ssid[33];
    bool  wifi_connected;
} WifiStatus;

typedef struct {
    bool mqtt_connected;
} MqttStatus;

typedef struct {
    bool mounted;
    uint64_t total_kb;
    uint64_t used_kb;
    int last_errno;
} SdStatus;

typedef struct {
    uint32_t period_ms;
    absolute_time_t started_at;
    uint32_t last_loop_ms;
} TimingInfo;

typedef struct {
    MenuId menu;
    SensorData sens;
    WifiStatus wifi;
    MqttStatus mqtt;
    SdStatus sd;
    TimingInfo time;
} AppState;

typedef struct {
    uint pin;
    bool last;
} DebBtn;

static AppState app;

static inline void btn_init(DebBtn* b, uint pin) {
    b->pin = pin;
    gpio_init(pin);
    gpio_pull_up(pin);
    gpio_set_dir(pin, false);
    b->last = gpio_get(pin);
}

static inline bool btn_pressed(DebBtn* b) {
    bool now = gpio_get(b->pin);
    bool edge = (b->last == 1 && now == 0);
    b->last = now;
    return edge;
}

static void wifi_query_status(WifiStatus* ws) {
    strncpy(ws->ssid, "MinhaRede2G", sizeof(ws->ssid));
    ws->ssid[sizeof(ws->ssid)-1] = '\0';
    ws->wifi_connected = true;
}

static void mqtt_query_status(MqttStatus* ms) {
    ms->mqtt_connected = true;
}

static void sd_query_status(SdStatus* sd) {
    sd->mounted = true;
    sd->total_kb = 4096ULL * 1024ULL;
    sd->used_kb  =  512ULL * 1024ULL;
    sd->last_errno = 0;
}

static void read_sensors(SensorData* s) {
    s->aht_ok = aht10_read_data(&s->aht_temp, &s->humidity);
    s->bmp_ok = bmp280_read_data(&s->bmp_temp, &s->pressure);
    s->lux_ok = bh1750_read_lux(&s->lux);
}

static void setup_hardware(void) {
    stdio_init_all();
    sleep_ms(400);

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
}

int main(void) {
    setup_hardware();

    app.menu = MENU_MEASUREMENTS;
    app.time.period_ms = 2000;
    app.time.started_at = get_absolute_time();

    DebBtn bA, bB, bC;
    btn_init(&bA, BTN_A_PIN);
    btn_init(&bB, BTN_B_PIN);
    btn_init(&bC, BTN_C_PIN);

    absolute_time_t tSens = make_timeout_time_ms(app.time.period_ms);
    absolute_time_t tUI   = make_timeout_time_ms(200);

    while (true) {
        absolute_time_t t0 = get_absolute_time();

        if (btn_pressed(&bA)) app.menu = (MenuId)((app.menu + MENU_COUNT - 1) % MENU_COUNT);
        if (btn_pressed(&bB)) app.menu = (MenuId)((app.menu + 1) % MENU_COUNT);
        if (btn_pressed(&bC)) { }

        if (absolute_time_diff_us(get_absolute_time(), tSens) <= 0) {
            read_sensors(&app.sens);
            tSens = delayed_by_ms(tSens, app.time.period_ms);
        }

        static uint32_t slow = 0;
        if ((slow++ % 5) == 0) {
            wifi_query_status(&app.wifi);
            mqtt_query_status(&app.mqtt);
            sd_query_status(&app.sd);
        }

        if (absolute_time_diff_us(get_absolute_time(), tUI) <= 0) {
            switch (app.menu) {
                case MENU_MEASUREMENTS: {
                    float temp = app.sens.aht_ok ? app.sens.aht_temp : (app.sens.bmp_ok ? app.sens.bmp_temp : NAN);
                    float hum  = app.sens.aht_ok ? app.sens.humidity : NAN;
                    float pres = app.sens.bmp_ok ? app.sens.pressure : NAN;
                    float lux  = app.sens.lux_ok ? app.sens.lux : NAN;
                    display_update(temp, hum, pres, app.sens.bmp_ok, lux, app.sens.lux_ok);
                } break;
                case MENU_WIFI:
                    display_render_wifi_status(app.wifi.ssid, app.wifi.wifi_connected, app.mqtt.mqtt_connected);
                    break;
                case MENU_SD:
                    display_render_sd_status(app.sd.mounted, app.sd.total_kb, app.sd.used_kb, app.sd.last_errno);
                    break;
                case MENU_TIMING: {
                    uint64_t up_ms = absolute_time_diff_us(app.time.started_at, get_absolute_time())/1000ULL;
                    display_render_timing(app.time.period_ms, up_ms, app.time.last_loop_ms);
                } break;
                default: break;
            }
            tUI = delayed_by_ms(tUI, 200);
        }

        app.time.last_loop_ms = (uint32_t)(absolute_time_diff_us(t0, get_absolute_time())/1000ULL);
        tight_loop_contents();
    }
    return 0;
}
