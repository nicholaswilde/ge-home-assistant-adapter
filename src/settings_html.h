#pragma once

const char* const SETTINGS_HTML = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Settings</title>
<style>
body { font-family: Arial, sans-serif; margin: 20px; }
label { display: block; margin-top: 10px; }
input { width: 100%; padding: 8px; margin-top: 5px; box-sizing: border-box; }
input[type=submit] { background-color: #4CAF50; color: white; border: none; cursor: pointer; margin-top: 20px; }
</style>
</head>
<body>
<h2>Device Settings</h2>
<form action="/settings" method="POST">
  <label>Device ID:</label>
  <input type="text" name="device_id" value="%DEVICE_ID%">
  
  <label>MQTT Server:</label>
  <input type="text" name="mqtt_server" value="%MQTT_SERVER%">
  
  <label>MQTT Port:</label>
  <input type="number" name="mqtt_port" value="%MQTT_PORT%">
  
  <label>MQTT User:</label>
  <input type="text" name="mqtt_user" value="%MQTT_USER%">
  
  <label>MQTT Password:</label>
  <input type="password" name="mqtt_password" value="%MQTT_PASSWORD%">
  
  <input type="submit" value="Save & Reboot">
</form>
<br>
<a href="/update">Firmware Update</a>
</body>
</html>
)=====";
