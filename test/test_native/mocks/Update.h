#pragma once
#include <Arduino.h>

class UpdateClass {
public:
    bool begin(size_t) { return true; }
    size_t write(uint8_t*, size_t len) { return len; }
    bool end(bool) { return true; }
    bool hasError() { return false; }
    void printError(Stream&) {}
};

extern UpdateClass Update;
