#pragma once
#include <Arduino.h>
#include <Client.h>

extern bool mock_pubsub_connected;
extern bool mock_pubsub_connect_result;
extern int mock_pubsub_state;

class PubSubClient {
public:
    PubSubClient() {}
    PubSubClient(Client&) {}
    PubSubClient& setServer(const char*, uint16_t) { return *this; }
    PubSubClient& setCallback(void (*)(char*, uint8_t*, unsigned int)) { return *this; }
    PubSubClient& setClient(void*) { return *this; }
    bool connect(const char*, const char* = NULL, const char* = NULL) { return mock_pubsub_connect_result; }
    void disconnect() {}
    bool publish(const char*, const char*, bool = false) { return true; }
    bool subscribe(const char*) { return true; }
    bool loop() { return true; }
    bool connected() { return mock_pubsub_connected; }
    int state() { return mock_pubsub_state; }
};
