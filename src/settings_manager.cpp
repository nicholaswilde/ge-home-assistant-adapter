#include "settings_manager.h"
#include <Arduino.h>
#include <WebServer.h>
#include <Update.h>
#include "Config.h"

void configureSettings(WebServer& server)
{
  server.on("/update", HTTP_GET, [&server]() {
#ifdef OTA_PASSWORD
    if (!server.authenticate("admin", OTA_PASSWORD)) {
      return server.requestAuthentication();
    }
#endif
    server.sendHeader("Connection", "close");
    server.send(200, "text/html",
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='update'>"
      "<input type='submit' value='Update'>"
      "</form>"
    );
  });

  server.on("/update", HTTP_POST, [&server]() {
#ifdef OTA_PASSWORD
    if (!server.authenticate("admin", OTA_PASSWORD)) {
      return server.requestAuthentication();
    }
#endif
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(1000);
    ESP.restart();
  }, [&server]() {
#ifdef OTA_PASSWORD
    if (!server.authenticate("admin", OTA_PASSWORD)) {
      return;
    }
#endif
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
      static bool ledState = false;
      ledState = !ledState;
      digitalWrite(LED_WIFI, ledState ? HIGH : LOW);
      digitalWrite(LED_MQTT, ledState ? HIGH : LOW);
      digitalWrite(LED_HEARTBEAT, ledState ? LOW : HIGH);
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  Serial.println("HTTP update server routes registered");
}

void loopSettings()
{
  // Deprecated, handled by wifiManager.update()
}
