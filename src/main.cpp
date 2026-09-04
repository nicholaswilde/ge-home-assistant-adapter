#include <Arduino.h>
#include "CustomHomeAssistantBridge.h"
#include "Config.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "settings_manager.h"
#include <Preferences.h>

static CustomHomeAssistantBridge bridge;
WifiManager wifiManager;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(LED_HEARTBEAT, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_MQTT, OUTPUT);

  // Initialize WiFi Manager (Captive Portal)
  wifiManager.begin(ssid, password, "ESP32_Config_AP");

  // Legacy WiFi config could still be called if desired
  // configureWifi();
  
  configureMqtt();
  configureSettings(wifiManager.getServer());

  Serial1.begin(CustomHomeAssistantBridge::baud, SERIAL_8N1, D7, D6);
  Preferences prefs;
  prefs.begin("wifi", true);
  String applianceId = prefs.isKey("appliance_type") ? prefs.getString("appliance_type") : String(defaultApplianceType);
  prefs.end();
  bridge.begin(getMqttClient(), Serial1, getSavedDeviceId().c_str(), applianceId);
}

void loop()
{
  wifiManager.update();
  
  connectToMqtt(bridge);
  bridge.loop();
  loopSettings();
  digitalWrite(LED_HEARTBEAT, millis() % 1000 < 500);
}
