#include "mqtt_manager.h"
#include <Arduino.h>
#include "wifi_manager.h"
#include "Config.h"

static PubSubClient mqttClient(getWifiClient());

PubSubClient& getMqttClient() {
  return mqttClient;
}

void configureMqtt()
{
  mqttClient.setServer(mqtt_server, mqtt_server_port);
}

void connectToMqtt(HomeAssistantBridge& bridge)
{
  connectToWifi();
  
  if (WiFi.status() != WL_CONNECTED) {
    return; // Wait for WiFi before attempting MQTT
  }

  digitalWrite(LED_WIFI, HIGH);

  if(!mqttClient.connected()) {
    digitalWrite(LED_MQTT, LOW);

    static unsigned long lastMqttAttempt = 0;
    if (millis() - lastMqttAttempt < 1000) {
      return;
    }
    lastMqttAttempt = millis();

    Serial.print("Attempting MQTT connection...");

    if(mqttClient.connect(deviceId, mqttUser, mqttPassword)) {
      Serial.println("connected");
      digitalWrite(LED_MQTT, HIGH);
    }
    else {
      Serial.println("failed, rc=" + String(mqttClient.state()) + " will try again in 1 second");
      bridge.notifyMqttDisconnected();
    }
  }
}
