#pragma once
#include <Arduino.h>

#define UPLOAD_FILE_START 1
#define UPLOAD_FILE_WRITE 2
#define UPLOAD_FILE_END 3
#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

struct HTTPUpload {
    int status;
    String filename;
    size_t totalSize;
};

class WebServer {
public:
    WebServer(int = 80) {}
    void on(const char*, void (*)()) {}
    void begin() {}
    void handleClient() {}
    void send(int, const char*, const String&) {}
    void sendHeader(const char*, const char*, bool = false) {}
    String arg(const char*) { return ""; }
    bool hasArg(const char*) { return false; }
    HTTPUpload& upload() { static HTTPUpload u; return u; }
};
