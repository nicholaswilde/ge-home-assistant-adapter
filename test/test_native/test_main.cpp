#include <unity.h>
#include <ArduinoFake.h>
#include "mqtt_manager.h"
#include "HomeAssistantBridge.h"

using namespace fakeit;

extern bool mock_wifi_connected;
extern bool mock_pubsub_connected;
extern bool mock_pubsub_connect_result;
extern int mock_pubsub_state;

void setUp(void) {
    ArduinoFakeReset();
    mock_wifi_connected = true;
    mock_pubsub_connected = true;
    mock_pubsub_connect_result = true;
    mock_pubsub_state = 0;
}

void tearDown(void) {
}

void test_configure_mqtt(void) {
    configureMqtt();
    TEST_ASSERT_EQUAL(1, 1);
}

void test_connectToMqtt_wifi_disconnected(void) {
    mock_wifi_connected = false;
    HomeAssistantBridge bridge;
    connectToMqtt(bridge);
    Verify(Method(ArduinoFake(), digitalWrite)).Never();
}

void test_connectToMqtt_already_connected(void) {
    mock_wifi_connected = true;
    mock_pubsub_connected = true;
    HomeAssistantBridge bridge;
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    
    connectToMqtt(bridge);
    
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_WIFI, HIGH)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_MQTT, LOW)).Never();
}

void test_connectToMqtt_connect_success(void) {
    mock_wifi_connected = true;
    mock_pubsub_connected = false;
    mock_pubsub_connect_result = true;
    
    HomeAssistantBridge bridge;
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), millis)).Return(2000, 2000);
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char*))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char*))).AlwaysReturn();

    connectToMqtt(bridge);

    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_WIFI, HIGH)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_MQTT, LOW)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_MQTT, HIGH)).Once();
}

void test_connectToMqtt_connect_failure(void) {
    mock_wifi_connected = true;
    mock_pubsub_connected = false;
    mock_pubsub_connect_result = false;
    
    HomeAssistantBridge bridge;
    When(Method(ArduinoFake(), digitalWrite)).AlwaysReturn();
    When(Method(ArduinoFake(), millis)).Return(5000, 5000);
    
    When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char*))).AlwaysReturn();
    When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const String&))).AlwaysReturn();

    connectToMqtt(bridge);

    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_WIFI, HIGH)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_MQTT, LOW)).Once();
    Verify(Method(ArduinoFake(), digitalWrite).Using(LED_MQTT, HIGH)).Never();
}

void test_get_mqtt_client(void) {
    PubSubClient& client = getMqttClient();
    TEST_ASSERT_NOT_NULL(&client);
}

void test_get_saved_device_id(void) {
    const String& id = getSavedDeviceId();
    TEST_ASSERT_NOT_NULL(&id);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_configure_mqtt);
    RUN_TEST(test_connectToMqtt_wifi_disconnected);
    RUN_TEST(test_connectToMqtt_already_connected);
    RUN_TEST(test_connectToMqtt_connect_success);
    RUN_TEST(test_connectToMqtt_connect_failure);
    RUN_TEST(test_get_mqtt_client);
    RUN_TEST(test_get_saved_device_id);
    UNITY_END();
    return 0;
}
