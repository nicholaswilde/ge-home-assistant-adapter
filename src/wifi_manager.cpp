#include "wifi_manager.h"
#include "landing_html.h"
#include "wifi_html.h"
#include "settings_html.h"

const byte DNS_PORT = 53;

WifiManager::WifiManager() : _state(WIFI_STATE_DISCONNECTED), _server(80) {}

void WifiManager::begin(const char* ssid, const char* password, const char* apName) {
    _preferences.begin("wifi", false);
    _savedSsid = _preferences.getString("ssid", ssid ? ssid : "");
    _savedPassword = _preferences.getString("password", password ? password : "");
    
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    _apName = "ha-adapter-" + mac.substring(mac.length() - 4);
    
    _state = WIFI_STATE_CONNECTING;
    _connectStartTime = millis();
    
    if (_savedSsid.length() > 0) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_savedSsid.c_str(), _savedPassword.c_str());
    } else {
        startAP();
    }
}

void WifiManager::update() {
    switch (_state) {
        case WIFI_STATE_CONNECTING:
            if (WiFi.status() == WL_CONNECTED) {
                _state = WIFI_STATE_CONNECTED;
                // Once connected, start the web server to handle OTA and other routes
                setupRoutes();
                _server.begin();
            } else if (millis() - _connectStartTime > _connectTimeoutMs) {
                startAP();
            }
            break;
            
        case WIFI_STATE_AP_MODE:
            _dnsServer.processNextRequest();
            _server.handleClient();
            break;
            
        case WIFI_STATE_CONNECTED:
            // Handle connected state tasks (OTA server etc)
            _server.handleClient();
            break;
            
        case WIFI_STATE_DISCONNECTED:
            break;
    }
}

WifiState WifiManager::getState() const {
    return _state;
}

void WifiManager::startAP() {
    _state = WIFI_STATE_AP_MODE;
    
    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);
    WiFi.disconnect();
    delay(200);
    
    WiFi.mode(WIFI_AP);
    
    IPAddress apIP(192, 168, 4, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    
    WiFi.softAP(_apName.c_str());
    delay(100);
    
    // Route all DNS requests to AP IP (Captive Portal)
    _dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer.start(DNS_PORT, "*", apIP);
    
    setupRoutes();
    _server.begin();
}

void WifiManager::setupRoutes() {
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/wifi", HTTP_GET, [this]() { handleWifi(); });
    _server.on("/settings", HTTP_GET, [this]() { handleSettings(); });
    
    _server.on("/wifi", HTTP_POST, [this]() {
        String newSsid = _server.arg("ssid");
        String newPass = _server.arg("password");
        if (newSsid.length() > 0) {
            _preferences.putString("ssid", newSsid);
            _preferences.putString("password", newPass);
            _server.send(200, "text/html", "<html><body><h1>Saved! Rebooting...</h1></body></html>");
            delay(1000);
            ESP.restart();
        } else {
            _server.sendHeader("Location", "/wifi", true);
            _server.send(302, "text/plain", "");
        }
    });
    
    _server.on("/settings", HTTP_POST, [this]() {
        // Handle saving Settings logic here
        _server.sendHeader("Location", "/", true);
        _server.send(302, "text/plain", "");
    });

    // Catch all for Captive Portal
    _server.onNotFound([this]() { handleNotFound(); });
}

void WifiManager::handleRoot() {
    _server.send(200, "text/html", LANDING_HTML);
}

void WifiManager::handleWifi() {
    _server.send(200, "text/html", WIFI_HTML);
}

void WifiManager::handleSettings() {
    _server.send(200, "text/html", SETTINGS_HTML);
}

void WifiManager::handleNotFound() {
    // If not AP mode, return a normal 404
    if (_state != WIFI_STATE_AP_MODE) {
        _server.send(404, "text/plain", "Not found");
        return;
    }
    
    // Check if the request is to our IP, to avoid infinite redirect loops
    String host = _server.hostHeader();
    if (host != "192.168.4.1") {
        // Captive portal behavior: redirect all unknown to root via our IP
        _server.sendHeader("Location", "http://192.168.4.1/", true);
        _server.send(302, "text/plain", "");
    } else {
        // Already on our IP but not found, send 404
        _server.send(404, "text/plain", "Not found");
    }
}

#include "Config.h"
#ifdef MQTT_TLS
#include <WiFiClientSecure.h>
static WiFiClientSecure globalWifiClient;
#else
static WiFiClient globalWifiClient;
#endif

Client& getWifiClient() {
    return globalWifiClient;
}

void connectToWifi() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    
    static unsigned long lastWifiPrint = 0;
    if (millis() - lastWifiPrint > 2000) {
        Serial.println("Connecting to WiFi...");
        lastWifiPrint = millis();
    }
}

void configureWifi() {
    // Legacy support, nothing needed if we already called wifiManager.begin()
}
