#pragma once
#include <PubSubClient.h>
#include <cstdint>
#include "CustomMqttClientAdapter.h"

extern "C" {
#include "mqtt_bridge.h"
#include "tiny_gea3_erd_client.h"
#include "tiny_gea3_interface.h"
#include "tiny_timer.h"
#include "uptime_monitor.h"
}
#include "tiny_uart_adapter.hpp"

class CustomHomeAssistantBridge {
 public:
  static constexpr unsigned long baud = 230400;

  void begin(PubSubClient& client, Stream& uart, const char* deviceId, const String& applianceId, uint8_t clientAddress = 0xE4);
  void loop();
  void notifyMqttDisconnected();

 private:
  PubSubClient* pubSubClient;
  tiny_timer_group_t timer_group;
  tiny_uart_adapter_t uart_adapter;
  CustomMqttClientAdapter client_adapter;
  tiny_gea3_interface_t gea3_interface;
  uint8_t receive_buffer[255];
  uint8_t send_queue_buffer[1000];
  tiny_gea3_erd_client_t erd_client;
  uint8_t client_queue_buffer[1024];
  mqtt_bridge_t mqtt_bridge;
  uptime_monitor_t uptime_monitor;
};
