#pragma once

#include <PubSubClient.h>
#include <Preferences.h>
#include "i_mqtt_client.h"
#include "i_tiny_event.h"

extern "C" {
#include "tiny_event.h"
}

struct CustomMqttClientAdapter {
  i_mqtt_client_t interface;
  PubSubClient* client;
  const char* device_id;
  String appliance_id;
  tiny_event_t write_request;
  tiny_event_t mqtt_disconnect;
};

void custom_mqtt_client_adapter_init(CustomMqttClientAdapter* self, PubSubClient* client, const char* deviceId, const String& applianceId);
void custom_mqtt_client_adapter_notify_mqtt_disconnected(CustomMqttClientAdapter* self);
