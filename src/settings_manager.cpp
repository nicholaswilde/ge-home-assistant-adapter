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
    String html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>OTA Update</title><style>";
    html += "body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }";
    html += ".card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; }";
    html += "h2 { color: #f5c2e7; margin-top: 0; margin-bottom: 20px; font-weight: 600; text-align: center; }";
    html += "input[type='file'] { width: 100%; padding: 12px; margin-bottom: 20px; border-radius: 6px; border: 1px solid #45475a; background: #313244; color: #cdd6f4; font-size: 16px; box-sizing: border-box; }";
    html += "button, .btn { display: flex; align-items: center; justify-content: center; text-decoration: none; width: 100%; padding: 12px; background: #cba6f7; border: none; border-radius: 6px; color: #11111b; font-size: 16px; font-weight: bold; cursor: pointer; transition: background 0.2s; margin-bottom: 15px; box-sizing: border-box; }";
    html += "button:hover { background: #f5c2e7; }";
    html += ".btn-secondary { background: #89b4fa; }";
    html += ".btn-secondary:hover { background: #b4befe; }";
    html += ".section-title { color: #89b4fa; font-size: 18px; margin-top: 20px; margin-bottom: 15px; border-bottom: 1px solid #313244; padding-bottom: 5px; }";
    html += ".footer { margin-top: 25px; margin-bottom: 0; font-size: 13px; color: #6c7086; text-align: center; }";
    html += "</style></head><body><div class='card'>";
    html += "<h2 style='margin-bottom: 5px;'>Home Assistant Adapter</h2>";
    html += "<p style='text-align: center; color: #a6adc8; margin-top: 0; margin-bottom: 20px; font-size: 14px;'>Version 1.0.0</p>";
    html += "<div class='section-title' style='margin-top: 0;'>Firmware Update</div>";
    html += "<form method='POST' action='/update' enctype='multipart/form-data'>";
    html += "<input type='file' name='update' accept='.bin' required>";
    html += "<button type='submit'>Install Update</button>";
    html += "</form>";
    html += "<a href='/' class='btn btn-secondary' style='margin-bottom: 0;'>Back to Dashboard</a>";
    html += "<p class='footer'><a href='https://github.com/nicholaswilde/home-assistant-adapter' target='_blank' style='color: #89b4fa; text-decoration: none;'>GitHub</a></p>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/update", HTTP_POST, [&server]() {
#ifdef OTA_PASSWORD
    if (!server.authenticate("admin", OTA_PASSWORD)) {
      return server.requestAuthentication();
    }
#endif
    server.sendHeader("Connection", "close");
    String resHtml = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Update Status</title><style>";
    resHtml += "body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }";
    resHtml += ".card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; text-align: center; }";
    resHtml += "h2 { color: #f5c2e7; margin-top: 0; }";
    resHtml += "p { color: #a6adc8; }";
    resHtml += "</style></head><body><div class='card'>";
    if (Update.hasError()) {
        resHtml += "<h2>Update Failed</h2><p>Something went wrong.</p>";
    } else {
        resHtml += "<h2>Update Successful</h2><p>Device is rebooting...</p>";
    }
    resHtml += "</div></body></html>";
    server.send(200, "text/html", resHtml);
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
