#include "wifi_manager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#ifdef MQTT_TLS
#include <WiFiClientSecure.h>
#endif
#include "Config.h"

#ifdef MQTT_TLS
static WiFiClientSecure wifiClient;
#else
static WiFiClient wifiClient;
#endif

Client& getWifiClient() {
  return wifiClient;
}

void connectToWifi()
{
  if(WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("Connecting to WiFi...");

  unsigned retries = 0;
  while(WiFi.status() != WL_CONNECTED) {
    if(retries++ > 100) {
      Serial.println("WiFi connection failed, restarting...");
      ESP.restart();
    }

    digitalWrite(LED_WIFI, LOW);
    delay(100);
    Serial.print(".");
  }
}

void configureWifi()
{
  WiFiManager wm;

  // Set WiFi status LED low (indicating connecting)
  digitalWrite(LED_WIFI, LOW);

  // Set SSID for Access Point to ha-adapter-<mac>
  String mac = WiFi.macAddress();
  String cleanMac = "";
  for (size_t i = 0; i < mac.length(); i++) {
    if (mac[i] != ':') {
      cleanMac += mac[i];
    }
  }
  String last4 = cleanMac.substring(cleanMac.length() - 4);
  last4.toLowerCase();
  String apName = "ha-adapter-" + last4;

  Serial.println("Starting WiFiManager...");

  // If the user provided credentials in Config.h, try them first
  if (String(ssid) != "Your SSID" && String(ssid) != "") {
    Serial.print("Attempting to connect to hardcoded SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    // Wait up to 10 seconds for connection
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }

  // Automatically connect using saved credentials,
  // or start the captive portal Access Point if connection fails
  if (WiFi.status() != WL_CONNECTED) {
    if (!wm.autoConnect(apName.c_str())) {
      Serial.println("Failed to connect and hit timeout. Restarting...");
      delay(3000);
      ESP.restart();
    }
  }

  // Once connected, set WiFi status LED high
  digitalWrite(LED_WIFI, HIGH);

#ifdef MQTT_TLS
#ifdef MQTT_TLS_VERIFY
  X509List* cert = new X509List(CERT);
  wifiClient.setTrustAnchors(cert);
#else
  wifiClient.setInsecure();
#endif
#endif

  Serial.println("WiFi connected");
}
