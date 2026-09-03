#pragma once
#include <Arduino.h>

extern bool mock_wifi_connected;
#define WIFI_STA 1
#define WIFI_AP 2
#define WL_CONNECTED 3
#define WL_DISCONNECTED 4

class WiFiClass {
public:
    String macAddress() { return "00:11:22:33:44:55"; }
    void mode(int) {}
    void begin(const char*, const char*) {}
    void disconnect() {}
    void persistent(bool) {}
    void setAutoReconnect(bool) {}
    int status() { return mock_wifi_connected ? WL_CONNECTED : WL_DISCONNECTED; }
    String SSID(int) { return "test-ssid"; }
    int scanNetworks() { return 1; }
    void softAP(const char*, const char* = NULL, int = 1, int = 0, int = 4) {}
};

extern WiFiClass WiFi;
