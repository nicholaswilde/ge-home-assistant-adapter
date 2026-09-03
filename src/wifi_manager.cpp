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
    
    _server.on("/wifi/scan", HTTP_GET, [this]() {
        WiFi.scanNetworks(true, false, false, 150);
        String html = "<!DOCTYPE html><html><head>";
        html += "<meta http-equiv='refresh' content='3;url=/wifi'>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
        html += "<title>Scanning...</title>";
        html += "<style>";
        html += "body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; }";
        html += ".card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; text-align: center; }";
        html += "h2 { color: #f5c2e7; margin-top: 0; }";
        html += "p { color: #a6adc8; }";
        html += "</style></head><body>";
        html += "<div class='card'><h2>Scanning for Wi-Fi...</h2><p>Please wait while we refresh the network list.</p></div>";
        html += "</body></html>";
        _server.send(200, "text/html", html);
    });
    
    _server.on("/wifi", HTTP_POST, [this]() {
        String newSsid = _server.arg("ssid");
        String newPass = _server.arg("password");
        if (newSsid.length() > 0) {
            _preferences.putString("ssid", newSsid);
            _preferences.putString("password", newPass);
            String resHtml = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Saved</title><style>";
            resHtml += "body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }";
            resHtml += ".card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; text-align: center; }";
            resHtml += "h2 { color: #a6e3a1; margin-top: 0; margin-bottom: 20px; }";
            resHtml += "p { color: #cdd6f4; }";
            resHtml += "</style></head><body><div class='card'>";
            resHtml += "<h2>Configuration Saved</h2><p>Device is rebooting to apply changes...</p>";
            resHtml += "</div></body></html>";
            _server.send(200, "text/html", resHtml);
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

    _server.on("/restart", HTTP_GET, [this]() {
        String resHtml = "<!DOCTYPE html><html lang='en'><head>";
        resHtml += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        resHtml += "<title>Restarting...</title><style>";
        resHtml += "body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }";
        resHtml += ".card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; text-align: center; }";
        resHtml += "h2 { color: #f38ba8; margin-top: 0; margin-bottom: 20px; }";
        resHtml += "p { color: #cdd6f4; }";
        resHtml += "</style></head><body><div class='card'>";
        resHtml += "<h2>Restarting</h2><p>Device is rebooting...</p>";
        resHtml += "</div></body></html>";
        _server.send(200, "text/html", resHtml);
        delay(1000);
        ESP.restart();
    });

    // Catch all for Captive Portal
    _server.onNotFound([this]() { handleNotFound(); });
}

void WifiManager::handleRoot() {
    _server.send(200, "text/html", LANDING_HTML);
}

void WifiManager::handleWifi() {
    int16_t scanStatus = WiFi.scanComplete();
    if (scanStatus >= 0) {
        _cachedNetworksHTML = "";
        for (int i = 0; i < scanStatus; ++i) {
            String ssidName = WiFi.SSID(i);
            int32_t rssi = WiFi.RSSI(i);
            _cachedNetworksHTML += "<div class='net-item' onclick='selectSSID(\"" + ssidName + "\")'>";
            _cachedNetworksHTML += "<span>" + ssidName + "</span>";
            _cachedNetworksHTML += "<span style='color: #a6adc8; font-size: 12px;'>" + String(rssi) + " dBm</span>";
            _cachedNetworksHTML += "</div>";
        }
        WiFi.scanDelete();
    } else if (scanStatus == WIFI_SCAN_FAILED) {
        WiFi.scanNetworks(true, false, false, 150);
        if (_cachedNetworksHTML.length() == 0 || _cachedNetworksHTML.indexOf("Scanning in progress") != -1) {
            _cachedNetworksHTML = "<div class='net-item' style='color: #a6adc8;'>Scanning in progress... Please refresh.</div>";
        }
    }

    String html = String(WIFI_HTML);
    if (scanStatus == WIFI_SCAN_RUNNING || scanStatus == WIFI_SCAN_FAILED) {
        html.replace("%META_REFRESH%", "<meta http-equiv='refresh' content='3'>");
    } else {
        html.replace("%META_REFRESH%", "");
    }
    
    html.replace("%NETWORK_LIST%", _cachedNetworksHTML);
    _server.send(200, "text/html", html);
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
