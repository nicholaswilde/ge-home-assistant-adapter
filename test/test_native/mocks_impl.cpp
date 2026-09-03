#include "WiFi.h"
#include "Update.h"
#include "wifi_manager.h"
#include <ArduinoFake.h>

WiFiClass WiFi;
UpdateClass Update;

bool mock_wifi_connected = true;
bool mock_pubsub_connected = true;
bool mock_pubsub_connect_result = true;
int mock_pubsub_state = 0;

class DummyClient : public Client {
public:
    size_t write(uint8_t) override { return 1; }
    size_t write(const uint8_t*, size_t size) override { return size; }
    int available() override { return 0; }
    int read() override { return -1; }
    int read(uint8_t*, size_t) override { return -1; }
    int peek() override { return -1; }
    void flush() override {}
    void stop() override {}
    uint8_t connected() override { return 1; }
    operator bool() override { return true; }
    int connect(IPAddress, uint16_t) override { return 1; }
    int connect(const char*, uint16_t) override { return 1; }
};

Client& getWifiClient() {
    static DummyClient client;
    return client;
}

void connectToWifi() {}

