#include "settings_manager.h"
#include <Arduino.h>
#include <WebServer.h>
#include <Update.h>
#include <Preferences.h>
#include "settings_html.h"

String settings_mqtt_server;
uint16_t settings_mqtt_port;
String settings_mqtt_user;
String settings_mqtt_password;
String settings_device_id;

static WebServer server(80);
static Preferences prefs;

static void loadSettings() {
  prefs.begin("settings", false);
  settings_mqtt_server = prefs.getString("mqtt_srv", "homeassistant.local");
  settings_mqtt_port = prefs.getUShort("mqtt_prt", 1883);
  settings_mqtt_user = prefs.getString("mqtt_usr", "homeassistant");
  settings_mqtt_password = prefs.getString("mqtt_pwd", "password");
  settings_device_id = prefs.getString("device_id", "some_device");
}

static void saveSettings() {
  prefs.putString("mqtt_srv", settings_mqtt_server);
  prefs.putUShort("mqtt_prt", settings_mqtt_port);
  prefs.putString("mqtt_usr", settings_mqtt_user);
  prefs.putString("mqtt_pwd", settings_mqtt_password);
  prefs.putString("device_id", settings_device_id);
}

void configureSettings()
{
  loadSettings();

  server.on("/", HTTP_GET, []() {
    String html = String(SETTINGS_HTML);
    html.replace("%DEVICE_ID%", settings_device_id);
    html.replace("%MQTT_SERVER%", settings_mqtt_server);
    html.replace("%MQTT_PORT%", String(settings_mqtt_port));
    html.replace("%MQTT_USER%", settings_mqtt_user);
    html.replace("%MQTT_PASSWORD%", settings_mqtt_password);
    server.send(200, "text/html", html);
  });

  server.on("/settings", HTTP_POST, []() {
    if (server.hasArg("device_id")) settings_device_id = server.arg("device_id");
    if (server.hasArg("mqtt_server")) settings_mqtt_server = server.arg("mqtt_server");
    if (server.hasArg("mqtt_port")) settings_mqtt_port = server.arg("mqtt_port").toInt();
    if (server.hasArg("mqtt_user")) settings_mqtt_user = server.arg("mqtt_user");
    if (server.hasArg("mqtt_password")) settings_mqtt_password = server.arg("mqtt_password");
    
    saveSettings();
    
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "Settings saved. Rebooting...");
    delay(1000);
    ESP.restart();
  });

  server.on("/update", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html",
      "<form method='POST' action='/update' enctype='multipart/form-data'>"
      "<input type='file' name='update'>"
      "<input type='submit' value='Update'>"
      "</form>"
      "<br><a href='/'>Settings</a>"
    );
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(1000);
    ESP.restart();
  }, []() {
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
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.begin();
  Serial.println("HTTP update server started");
}

void loopSettings()
{
  server.handleClient();
}
