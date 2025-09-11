#ifndef MQTT_MANAGER
#define MQTT_MANAGER

#include "wifi_conn.h"
#include "mqtt_comm.h"

#include <math.h>

// #define SSID "CLARO_5G5D1167"
// #define PASSWD "385D1167"

#define SSID "Trojan"
#define PASSWD "11282931"

void mqtt_conect_init();

void mqtt_get_and_publish(bool wifi_connected,bool mqtt_connected, bool aht_ok,bool bmp_ok,bool lux_ok,
    float aht_temp, float bmp_temp,float humidity,float pressure,float lux_val);

void mqtt_get_and_publish2(bool wifi_connected,bool mqtt_connected,char *str);

bool wifi_check();
bool mqtt_check();


#endif