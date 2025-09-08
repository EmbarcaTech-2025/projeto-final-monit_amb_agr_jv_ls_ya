#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include "pico/cyw43_arch.h"

/**
 * @brief Connects to a Wi-Fi network
 * 
 * @param ssid The SSID of the network to connect to
 * @param password The password for the network
 * 
 * This function initializes the Wi-Fi hardware and attempts to connect
 * to the specified network using WPA2 AES encryption.
 * 
 * Prints status messages to stdout (USB serial by default).
 */
void connect_to_wifi(const char *ssid, const char *password);

#endif // WIFI_CONN_H