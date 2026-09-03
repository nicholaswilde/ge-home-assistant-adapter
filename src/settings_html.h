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
        body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }
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
    </style>
</head>
<body>
    <div class="card">
        <h2 style="margin-bottom: 5px;">Home Assistant Adapter</h2>
        <p style="text-align: center; color: #a6adc8; margin-top: 0; margin-bottom: 20px; font-size: 14px;">Version v0.1.0</p>
        
        <div class='section-title' style='margin-top: 0;'>Device Settings</div>
        
        <form action="/settings" method="POST">
            <label for="deviceName">Device Name</label>
            <input type="text" id="deviceName" name="deviceName" placeholder="My Device">
            
            <button type="submit">Save Settings</button>
        </form>
        <a href="/" class="btn btn-secondary" style="margin-bottom: 0;">Back to Dashboard</a>
        <p class="footer"><a href="https://github.com/nicholaswilde/home-assistant-adapter" target="_blank" style="color: #89b4fa; text-decoration: none;">GitHub</a></p>
    </div>
</body>
</html>
)=====";

#endif // SETTINGS_HTML_H
