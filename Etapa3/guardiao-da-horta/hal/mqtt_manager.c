#include "mqtt_manager.h"




void mqtt_conect_init(){
    // const char *ssid = "CLARO_5G5D1167";
    // const char *password = "385D1167";
    
    connect_to_wifi(SSID, PASSWD);

    // mqtt_setup("bitdog1", "192.168.1.76");
    mqtt_setup("bitdog1", "192.168.0.165");
}

void mqtt_get_and_publish(bool wifi_connected,bool mqtt_connected, bool aht_ok,bool bmp_ok,bool lux_ok,
    float aht_temp, float bmp_temp,float humidity,float pressure,float lux_val){
    // 2. Prepara os dados para o JSON (lógica similar à do seu display)
    float temp = aht_ok ? aht_temp : (bmp_ok ? bmp_temp : NAN);
    float hum  = aht_ok ? humidity : NAN;
    float pres = bmp_ok ? pressure : NAN;
    float lux  = lux_ok ? lux_val : NAN;

    // 3. Cria a string JSON
    char json_payload[256];
    sprintf(json_payload,
            "{\"temperatura\":%.2f, \"umidade\":%.2f, \"pressao\":%.2f, \"luminosidade\":%.1f}",
            temp,
            hum,
            pres / 100.0f, // Converte de Pa para hPa (opcional, mas comum)
            lux);

    // 4. Publica a mensagem via MQTT
    //    Verifica se o Wi-Fi e o MQTT estão conectados antes de enviar
    if (wifi_connected && mqtt_connected) {
        mqtt_comm_publish("bitdoglab/data", (const uint8_t *)json_payload, strlen(json_payload));
    }
}