#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Client.h>
#include <Preferences.h>

enum WifiState {
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_AP_MODE
};

class WifiManager {
public:
    WifiManager();
    void begin(const char* ssid, const char* password, const char* apName);
    void update();
    WifiState getState() const;
    WebServer& getServer() { return _server; }

private:
    WifiState _state;
    unsigned long _connectStartTime;
    const unsigned long _connectTimeoutMs = 15000;
    
    WebServer _server;
    DNSServer _dnsServer;
    Preferences _preferences;
    
    String _savedSsid;
    String _savedPassword;
    String _apName;

    void startAP();
    void setupRoutes();
    
    // Route handlers
    void handleRoot();
    void handleWifi();
    void handleSettings();
    void handleNotFound();
};

extern WifiManager wifiManager;

void configureWifi();
void connectToWifi();
Client& getWifiClient();

#endif // WIFI_MANAGER_H
