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
    String html = R"rawhtml(<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>OTA Update</title>
    <style>
        body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }
        .card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; text-align: center; }
        h2 { color: #f5c2e7; margin-top: 0; margin-bottom: 20px; font-weight: 600; text-align: center; }
        button, .btn { display: flex; align-items: center; justify-content: center; text-decoration: none; width: 100%; padding: 12px; background: #cba6f7; border: none; border-radius: 6px; color: #11111b; font-size: 16px; font-weight: bold; cursor: pointer; transition: background 0.2s; margin-bottom: 15px; box-sizing: border-box; }
        button:hover { background: #f5c2e7; }
        button:disabled { background: #45475a; color: #a6adc8; cursor: not-allowed; }
        .btn-secondary { background: #89b4fa; }
        .btn-secondary:hover { background: #b4befe; }
        .section-title { color: #89b4fa; font-size: 18px; margin-top: 20px; margin-bottom: 15px; border-bottom: 1px solid #313244; padding-bottom: 5px; }
        .footer { margin-top: 25px; margin-bottom: 0; font-size: 13px; color: #6c7086; text-align: center; }
        
        .drop-zone { border: 2px dashed #45475a; border-radius: 8px; padding: 30px 10px; margin-bottom: 20px; cursor: pointer; transition: border-color 0.2s, background-color 0.2s; }
        .drop-zone:hover, .drop-zone.dragover { border-color: #89b4fa; background: #313244; }
        .drop-zone p { margin: 0; color: #a6adc8; font-size: 14px; }
        input[type="file"] { display: none; }
    </style>
</head>
<body>
    <div class='card'>
        <h2 style='margin-bottom: 5px;'>GE Home Assistant Adapter</h2>
        <p style='text-align: center; color: #a6adc8; margin-top: 0; margin-bottom: 20px; font-size: 14px;'>Version 1.0.0</p>
        <div class='section-title' style='margin-top: 0;'>Firmware Update</div>
        
        <form method='POST' action='/update' enctype='multipart/form-data' id='upload-form'>
            <div class="drop-zone" id="drop-zone">
                <p id="file-name">Click or drag & drop binary file here</p>
                <input type="file" name="update" id="file-input" accept=".bin" required>
            </div>
            <button type='submit' id='upload-btn' disabled>Install Update</button>
        </form>
        
        <a href='/' class='btn btn-secondary' style='margin-bottom: 0;'>Back to Dashboard</a>
        <p class='footer'><a href='https://github.com/nicholaswilde/ge-home-assistant-adapter' target='_blank' style='color: #89b4fa; text-decoration: none;'>GitHub</a></p>
    </div>

    <script>
        const dropZone = document.getElementById('drop-zone');
        const fileInput = document.getElementById('file-input');
        const uploadBtn = document.getElementById('upload-btn');
        const fileName = document.getElementById('file-name');

        dropZone.addEventListener('click', () => fileInput.click());

        fileInput.addEventListener('change', (e) => {
            if (e.target.files.length > 0) handleFileSelect(e.target.files[0]);
        });

        dropZone.addEventListener('dragover', (e) => {
            e.preventDefault();
            dropZone.classList.add('dragover');
        });

        dropZone.addEventListener('dragleave', () => {
            dropZone.classList.remove('dragover');
        });

        dropZone.addEventListener('drop', (e) => {
            e.preventDefault();
            dropZone.classList.remove('dragover');
            if (e.dataTransfer.files.length > 0) {
                fileInput.files = e.dataTransfer.files;
                handleFileSelect(e.dataTransfer.files[0]);
            }
        });

        function handleFileSelect(file) {
            if (file.name.endsWith('.bin')) {
                fileName.innerText = file.name;
                uploadBtn.disabled = false;
            } else {
                alert('Please select a valid .bin file');
                uploadBtn.disabled = true;
                fileName.innerText = 'Click or drag & drop binary file here';
            }
        }
    </script>
</body>
</html>)rawhtml";
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
