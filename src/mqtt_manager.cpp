#include "mqtt_manager.h"
#include <Arduino.h>
#include "wifi_manager.h"
#include "Config.h"
#include <Preferences.h>

static PubSubClient mqttClient(getWifiClient());

static String savedMqttServer;
static String savedDeviceId;
static String savedMqttUser;
static String savedMqttPass;

PubSubClient& getMqttClient() {
  return mqttClient;
}

const String& getSavedDeviceId() {
  return savedDeviceId;
}

void configureMqtt()
{
  Preferences prefs;
  prefs.begin("wifi", true);
  savedMqttServer = prefs.isKey("mqtt_server") ? prefs.getString("mqtt_server") : String(mqtt_server);
  uint16_t savedMqttPort = prefs.isKey("mqtt_port") ? prefs.getUShort("mqtt_port") : mqtt_server_port;
  savedDeviceId = prefs.isKey("device_id") ? prefs.getString("device_id") : String(deviceId);
  savedMqttUser = prefs.isKey("mqtt_user") ? prefs.getString("mqtt_user") : String(mqttUser);
  savedMqttPass = prefs.isKey("mqtt_pass") ? prefs.getString("mqtt_pass") : String(mqttPassword);
  prefs.end();

  mqttClient.setServer(savedMqttServer.c_str(), savedMqttPort);
  mqttClient.setBufferSize(1024);
}

void connectToMqtt(CustomHomeAssistantBridge& bridge)
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

    if(mqttClient.connect(savedDeviceId.c_str(), savedMqttUser.c_str(), savedMqttPass.c_str())) {
      Serial.println("connected");
      digitalWrite(LED_MQTT, HIGH);

      String discoveryTopic = String("homeassistant/sensor/") + savedDeviceId + "/config";
      String payload = String("{\"name\": \"") + savedDeviceId + String("\", \"state_topic\": \"geappliances/") + savedDeviceId + String("/status\", \"unique_id\": \"") + savedDeviceId + String("\", \"device\": {\"identifiers\": [\"") + savedDeviceId + String("\"], \"name\": \"") + savedDeviceId + String("\"}}");
      mqttClient.publish(discoveryTopic.c_str(), payload.c_str(), true);
      mqttClient.publish((String("geappliances/") + savedDeviceId + "/status").c_str(), "online", true);
    }
    else {
      Serial.println("failed, rc=" + String(mqttClient.state()) + " will try again in 1 second");
      bridge.notifyMqttDisconnected();
    }
  }
}
