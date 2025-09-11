#ifndef MQTT_MANAGER
#define MQTT_MANAGER

#include "wifi_conn.h"
#include "mqtt_comm.h"

#include <math.h>

#define SSID "CLARO_5G5D1167"
#define PASSWD "385D1167"


void mqtt_conect_init();

void mqtt_get_and_publish(bool wifi_connected,bool mqtt_connected, bool aht_ok,bool bmp_ok,bool lux_ok,
    float aht_temp, float bmp_temp,float humidity,float pressure,float lux_val);


#endif