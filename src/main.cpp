#include <Arduino.h>
#include "HomeAssistantBridge.h"
#include "Config.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "settings_manager.h"

static HomeAssistantBridge bridge;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(LED_HEARTBEAT, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_MQTT, OUTPUT);

  configureWifi();
  configureMqtt();
  configureSettings();

  Serial1.begin(HomeAssistantBridge::baud, SERIAL_8N1, D7, D6);
  bridge.begin(getMqttClient(), Serial1, deviceId);
}

void loop()
{
  connectToMqtt(bridge);
  bridge.loop();
  loopSettings();
  digitalWrite(LED_HEARTBEAT, millis() % 1000 < 500);
}
