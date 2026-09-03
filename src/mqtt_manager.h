#pragma once
#include <PubSubClient.h>
#include "HomeAssistantBridge.h"

void configureMqtt();
void connectToMqtt(HomeAssistantBridge& bridge);
PubSubClient& getMqttClient();
const String& getSavedDeviceId();
