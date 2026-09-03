#pragma once
#include <Arduino.h>

class Preferences {
public:
    Preferences() {}
    bool begin(const char*, bool = false) { return true; }
    void end() {}
    String getString(const char*, const String& defaultValue = String()) { return defaultValue; }
    size_t putString(const char*, const String& value) { return value.length(); }
    void clear() {}
    bool isKey(const char*) { return false; }
    uint16_t getUShort(const char*) { return 0; }
};
