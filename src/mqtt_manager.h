#pragma once
#include <PubSubClient.h>
#include "CustomHomeAssistantBridge.h"

void configureMqtt();
void connectToMqtt(CustomHomeAssistantBridge& bridge);
PubSubClient& getMqttClient();
const String& getSavedDeviceId();
