#pragma once
#include <Arduino.h>
#include <PubSubClient.h>

class HomeAssistantBridge {
public:
    HomeAssistantBridge() {}
    void loop() {}
    void begin(PubSubClient&, const char*) {}
    void notifyMqttDisconnected() {}
};
