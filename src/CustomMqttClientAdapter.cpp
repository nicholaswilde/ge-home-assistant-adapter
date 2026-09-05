#include "CustomMqttClientAdapter.h"
#include "generated/ErdParser.h"
#include <Arduino.h>

static uint8_t ascii_hex_to_nybble(char ascii_hex) {
  if('A' <= ascii_hex && ascii_hex <= 'F') return ascii_hex - 'A' + 10;
  else if('a' <= ascii_hex && ascii_hex <= 'f') return ascii_hex - 'a' + 10;
  else return ascii_hex - '0';
}

static CustomMqttClientAdapter* mqtt_callback_self;

static void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  auto self = mqtt_callback_self;
  auto topic_string = String(topic);

  // We still allow writing via raw hex for now
  if(topic_string.startsWith("geappliances/") && topic_string.endsWith("/write")) {
    int erd_idx = topic_string.lastIndexOf("/erd_");
    if (erd_idx == -1) return;
    String erdString = topic_string.substring(erd_idx + 5, topic_string.lastIndexOf("/write"));
    
    tiny_erd_t erd = strtol(erdString.c_str(), NULL, 16);

    for(unsigned i = 0; i < length / 2; i++) {
      auto high = payload[2 * i];
      auto low = payload[2 * i + 1];
      payload[i] = (ascii_hex_to_nybble(high) << 4) | ascii_hex_to_nybble(low);
    }

    mqtt_client_on_write_request_args_t args = {
      .erd = erd,
      .size = static_cast<uint8_t>(length / 2),
      .value = payload,
    };
    tiny_event_publish(&self->write_request, &args);
  }
}

static void register_erd(i_mqtt_client_t* _self, tiny_erd_t erd) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  
  // Publish Home Assistant discovery payload
  publishHomeAssistantDiscovery(*self->client, erd, self->device_id, self->appliance_id);
  
  char buf[16];
  snprintf(buf, sizeof(buf), "%04x", erd);
  auto topic = String("geappliances/") + self->device_id + "/erd_" + String(buf) + "/write";
  self->client->subscribe(topic.c_str());
}

static void update_erd(i_mqtt_client_t* _self, tiny_erd_t erd, const void* _value, uint8_t size) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  
  String jsonPayload = decodeErdToJson(erd, reinterpret_cast<const uint8_t*>(_value), size);
  
  char buf[16];
  snprintf(buf, sizeof(buf), "%04x", erd);
  auto topic = String("geappliances/") + self->device_id + "/erd/0x" + String(buf) + "/state";
  Serial.printf("[MQTT] Publish: %s -> %s\n", topic.c_str(), jsonPayload.c_str());
  self->client->publish(topic.c_str(), jsonPayload.c_str(), true);
}

static void update_erd_write_result(i_mqtt_client_t* _self, tiny_erd_t erd, bool success, tiny_gea3_erd_client_write_failure_reason_t failure_reason) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  
  char buf[16];
  snprintf(buf, sizeof(buf), "%04x", erd);
  auto topic = String("geappliances/") + self->device_id + "/erd_" + String(buf) + "/write_result";
  
  if(success) {
    Serial.printf("[MQTT] Publish: %s -> success\n", topic.c_str());
    self->client->publish(topic.c_str(), "success", true);
  } else {
    Serial.printf("[MQTT] Publish: %s -> failed (reason %d)\n", topic.c_str(), failure_reason);
    self->client->publish(topic.c_str(), "failed", true);
  }
}

static void publish_sub_topic(i_mqtt_client_t* _self, const char* sub_topic, const char* payload) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  auto topic = String("geappliances/") + self->device_id + "/" + String(sub_topic);
  Serial.printf("[MQTT] Publish: %s -> %s\n", topic.c_str(), payload);
  self->client->publish(topic.c_str(), payload);
}

static i_tiny_event_t* on_write_request(i_mqtt_client_t* _self) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  return &self->write_request.interface;
}

static i_tiny_event_t* on_mqtt_disconnect(i_mqtt_client_t* _self) {
  auto self = reinterpret_cast<CustomMqttClientAdapter*>(_self);
  return &self->mqtt_disconnect.interface;
}

static const i_mqtt_client_api_t api = {
  register_erd,
  update_erd,
  update_erd_write_result,
  publish_sub_topic,
  on_write_request,
  on_mqtt_disconnect
};

void custom_mqtt_client_adapter_init(CustomMqttClientAdapter* self, PubSubClient* client, const char* deviceId, const String& applianceId) {
  self->interface.api = &api;
  self->client = client;
  self->device_id = deviceId;
  self->appliance_id = applianceId;

  mqtt_callback_self = self;
  client->setCallback(mqtt_callback);

  tiny_event_init(&self->write_request);
  tiny_event_init(&self->mqtt_disconnect);
}

void custom_mqtt_client_adapter_notify_mqtt_disconnected(CustomMqttClientAdapter* self) {
  tiny_event_publish(&self->mqtt_disconnect, nullptr);
}
