#pragma once
#include <Arduino.h>

extern String settings_mqtt_server;
extern uint16_t settings_mqtt_port;
extern String settings_mqtt_user;
extern String settings_mqtt_password;
extern String settings_device_id;

void configureSettings();
void loopSettings();
