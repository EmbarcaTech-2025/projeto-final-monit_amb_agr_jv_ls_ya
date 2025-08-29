#include "display.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static ssd1306_t disp;

static inline void draw_title(const char *title) {
    char sep[22];
    memset(sep, '-', sizeof(sep));
    sep[21] = '\0';
    ssd1306_draw_string(&disp, 0, 0, 1, title);
    ssd1306_draw_string(&disp, 0, 10, 1, sep);
}


static inline void clear_and_frame(const char *title) {
    ssd1306_clear(&disp);
    draw_title(title);
}

void display_init(i2c_inst_t *i2c_port, uint8_t i2c_address) {
    ssd1306_init(&disp, 128, 64, i2c_address, i2c_port);
}

void display_update(float aht_temp, float humidity, float pressure, bool bmp_ok, float lux, bool bh1750_ok) {
    char line1[24], line2[24], line3[24], line4[24];

    clear_and_frame("Medicoes");

    snprintf(line1, sizeof(line1), "Temp: %.1f C", aht_temp);
    snprintf(line2, sizeof(line2), "Umid: %.0f %%RH", humidity);
    if (bmp_ok)  snprintf(line3, sizeof(line3), "Pres: %.0f hPa", pressure);
    else         snprintf(line3, sizeof(line3), "Pres: Falha");
    snprintf(line4, sizeof(line4), "Luminosidade: %.0f Lux", lux);

    ssd1306_draw_string(&disp, 0, 20, 1, line1);
    ssd1306_draw_string(&disp, 0, 32, 1, line2);
    ssd1306_draw_string(&disp, 0, 44, 1, line3);
    ssd1306_draw_string(&disp, 0, 56, 1, line4);

    ssd1306_show(&disp);
}

void display_render_wifi_status(const char* ssid, bool wifi_ok, bool mqtt_ok) {
    char l1[24], l2[24], l3[24];

    clear_and_frame("WiFi / MQTT");

    snprintf(l1, sizeof(l1), "SSID: %.16s", ssid ? ssid : "(n/d)");
    snprintf(l2, sizeof(l2), "WiFi: %s", wifi_ok ? "Conectado" : "Descon.");
    snprintf(l3, sizeof(l3), "MQTT: %s", mqtt_ok ? "Conectado" : "Descon.");

    ssd1306_draw_string(&disp, 0, 20, 1, l1);
    ssd1306_draw_string(&disp, 0, 32, 1, l2);
    ssd1306_draw_string(&disp, 0, 44, 1, l3);

    ssd1306_show(&disp);
}

void display_render_sd_status(bool mounted, uint64_t total_kb, uint64_t used_kb, int last_errno) {
    char l1[24], l2[24], l3[24], l4[24],prefixo;
    uint64_t aux_value_print;
    float mem={0};

    clear_and_frame("Cartao SD");

    aux_value_print=total_kb;

    snprintf(l1, sizeof(l1), "SD: %s", mounted ? "OK" : "NOK");
    mem=check_size_MKb(&prefixo,aux_value_print);
    snprintf(l2, sizeof(l2), "Tot: %.1f %cB", mem,prefixo);
    aux_value_print=used_kb;
    mem=check_size_MKb(&prefixo,aux_value_print);
    snprintf(l3, sizeof(l3), "Uso: %.1f %cB", mem,prefixo);
    snprintf(l4, sizeof(l4), "Err: %d", last_errno);

    ssd1306_draw_string(&disp, 0, 20, 1, l1);
    ssd1306_draw_string(&disp, 0, 32, 1, l2);
    ssd1306_draw_string(&disp, 0, 44, 1, l3);

    ssd1306_show(&disp);
}

void display_render_timing(uint32_t timer_read, uint64_t timer_mqtt, uint32_t timer_sd) {
    char l1[24], l2[24], l3[24];

    clear_and_frame("Timing");

    snprintf(l1, sizeof(l1), "Leituras: %ums", timer_read);
    snprintf(l2, sizeof(l2), "Envio MQTT: %ums", timer_mqtt);
    snprintf(l3, sizeof(l3), "Gravacao SD: %ums", timer_sd);

    ssd1306_draw_string(&disp, 0, 20, 1, l1);
    ssd1306_draw_string(&disp, 0, 32, 1, l2);
    ssd1306_draw_string(&disp, 0, 44, 1, l3);

    ssd1306_show(&disp);
}

//recebe valor total de bytes e retorna o float e altera o prefixo char que acompanha B
float check_size_MKb(char *prefixo,uint64_t data_size){
    uint64_t aux=data_size;
    float faux=data_size;
    if(aux>=(1024*1024)){
        faux/=(float)(1024*1024);
        *prefixo='G';
    }else if(aux>=(1024)){
        faux/=(float)(1024);
        *prefixo='M';
    } else{
        *prefixo='K';
    }
    return faux;
}