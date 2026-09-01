#include "mqtt_manager.h"
#include <Arduino.h>
#include "wifi_manager.h"
#include "settings_manager.h"

static PubSubClient mqttClient(getWifiClient());

PubSubClient& getMqttClient() {
  return mqttClient;
}

void configureMqtt()
{
  mqttClient.setServer(settings_mqtt_server.c_str(), settings_mqtt_port);
}

void connectToMqtt(HomeAssistantBridge& bridge)
{
  connectToWifi();
  digitalWrite(LED_WIFI, HIGH);

  if(!mqttClient.connected()) {
    digitalWrite(LED_MQTT, LOW);

    unsigned retries = 0;
    while(!mqttClient.connected()) {
      if(retries++ > 10) {
        Serial.println("MQTT connection failed, restarting...");
        ESP.restart();
      }

      Serial.print("Attempting MQTT connection...");

      if(mqttClient.connect(settings_device_id.c_str(), settings_mqtt_user.c_str(), settings_mqtt_password.c_str())) {
        Serial.println("connected");
        digitalWrite(LED_MQTT, HIGH);
      }
      else {
        Serial.println("failed, rc=" + String(mqttClient.state()) + " will try again in 1 second");
        delay(1000);
      }
    }

    bridge.notifyMqttDisconnected();
  }
}
