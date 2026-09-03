#pragma once
#include <Arduino.h>

class DNSServer {
public:
    void start(int, const char*, void*) {}
    void processNextRequest() {}
};
