#ifndef SETTINGS_HTML_H
#define SETTINGS_HTML_H

const char SETTINGS_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Device Settings</title>
    <style>
        body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: flex-start; min-height: 100vh; box-sizing: border-box; }
        .card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; }
        h2 { color: #f5c2e7; margin-top: 0; margin-bottom: 20px; font-weight: 600; text-align: center; }
        label { display: block; margin-bottom: 8px; color: #a6adc8; font-size: 14px; }
        select, input[type='text'], input[type='password'], input[type='number'] { width: 100%; padding: 12px; margin-bottom: 20px; border-radius: 6px; border: 1px solid #45475a; background: #313244; color: #cdd6f4; font-size: 16px; box-sizing: border-box; }
        select:focus, input:focus { outline: none; border-color: #f5c2e7; }
        button, .btn { display: flex; align-items: center; justify-content: center; text-decoration: none; width: 100%; padding: 12px; background: #cba6f7; border: none; border-radius: 6px; color: #11111b; font-size: 16px; font-weight: bold; cursor: pointer; transition: background 0.2s; margin-bottom: 15px; box-sizing: border-box; }
        button:hover, .btn-primary:hover { background: #f5c2e7; }
        .btn-secondary { background: #89b4fa; }
        .btn-secondary:hover { background: #b4befe; }
        .section-title { color: #89b4fa; font-size: 18px; margin-top: 20px; margin-bottom: 15px; border-bottom: 1px solid #313244; padding-bottom: 5px; }
        .footer { margin-top: 25px; margin-bottom: 0; font-size: 13px; color: #6c7086; text-align: center; }
        .password-wrapper { position: relative; display: block; margin-bottom: 20px; }
        .password-wrapper input[type='password'], .password-wrapper input[type='text'] { padding-right: 40px; margin-bottom: 0; }
        .password-wrapper .toggle-password { position: absolute; right: 12px; top: 12px; cursor: pointer; color: #a6adc8; user-select: none; display: flex; align-items: center; justify-content: center; height: 20px; width: 20px; transition: color 0.2s; }
        .password-wrapper .toggle-password:hover { color: #cdd6f4; }
    </style>
</head>
<body>
    <div class="card">
        <h2 style="margin-bottom: 5px;">GE Home Assistant Adapter</h2>
        <p style="text-align: center; color: #a6adc8; margin-top: 0; margin-bottom: 20px; font-size: 14px;">Version v0.1.1</p>
        
        <form action="/settings" method="POST">
            <div class='section-title' style='margin-top: 0;'>Device Settings</div>
            
            <label for="deviceId">MQTT Device ID</label>
            <input type="text" id="deviceId" name="deviceId" value="%DEVICE_ID%">
            
            <div class='section-title'>MQTT Settings</div>
            <label for="mqtt_server">MQTT Server</label>
            <input type="text" id="mqtt_server" name="mqtt_server" value="%MQTT_SERVER%">
            
            <label for="mqtt_port">MQTT Port</label>
            <input type="number" id="mqtt_port" name="mqtt_port" value="%MQTT_PORT%">
            
            <label for="mqtt_user">MQTT Username</label>
            <input type="text" id="mqtt_user" name="mqtt_user" value="%MQTT_USER%">
            
            <label for="mqtt_password">MQTT Password</label>
            <div class='password-wrapper'>
                <input type='password' id='mqtt_password' name='mqtt_password' value='%MQTT_PASSWORD%'>
                <span class='toggle-password' onclick='togglePwd("mqtt_password", this)'><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path><circle cx="12" cy="12" r="3"></circle></svg></span>
            </div>
            
            <button type="submit">Save Settings & Reboot</button>
        </form>
        <a href="/" class="btn btn-secondary" style="margin-bottom: 0;">Back to Dashboard</a>
        <p class="footer"><a href="https://github.com/nicholaswilde/ge-home-assistant-adapter" target="_blank" style="color: #89b4fa; text-decoration: none;">GitHub</a></p>
    </div>
    
    <script>
    function togglePwd(id, el) {
        var eyeSvg = '<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"></path><circle cx="12" cy="12" r="3"></circle></svg>';
        var eyeOffSvg = '<svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19m-6.72-1.07a3 3 0 1 1-4.24-4.24"></path><line x1="1" y1="1" x2="23" y2="23"></line></svg>';
        var input = document.getElementById(id);
        if (input.type === "password") {
            input.type = "text";
            el.innerHTML = eyeOffSvg;
        } else {
            input.type = "password";
            el.innerHTML = eyeSvg;
        }
    }
    </script>
</body>
</html>
)=====";

#endif // SETTINGS_HTML_H
