#ifndef LANDING_HTML_H
#define LANDING_HTML_H

const char LANDING_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>GE Home Assistant Adapter Setup</title>
    <style>
        body { font-family: 'Inter', system-ui, sans-serif; background: #1e1e2e; color: #cdd6f4; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; min-height: 100vh; box-sizing: border-box; }
        .card { background: #181825; border-radius: 12px; padding: 30px; width: 100%; max-width: 400px; box-shadow: 0 8px 30px rgba(0,0,0,0.3); border: 1px solid #313244; }
        h2 { color: #f5c2e7; margin-top: 0; margin-bottom: 20px; font-weight: 600; text-align: center; }
        .btn { display: flex; align-items: center; justify-content: center; text-decoration: none; width: 100%; padding: 12px; border-radius: 6px; color: #11111b; font-size: 16px; font-weight: bold; cursor: pointer; transition: background 0.2s; margin-bottom: 15px; border: none; box-sizing: border-box; }
        .btn-primary { background: #cba6f7; }
        .btn-primary:hover { background: #f5c2e7; }
        .btn-secondary { background: #89b4fa; }
        .btn-secondary:hover { background: #b4befe; }
        .btn-danger { background: #f38ba8; color: #11111b; }
        .btn-danger:hover { background: #eba0ac; }
        .footer { margin-top: 25px; margin-bottom: 0; font-size: 13px; color: #6c7086; text-align: center; }
    </style>
</head>
<body>
    <div class="card">
        <h2 style="margin-bottom: 5px;">GE Home Assistant Adapter</h2>
        <p style="text-align: center; color: #a6adc8; margin-top: 0; margin-bottom: 20px; font-size: 14px;">Version v0.1.1</p>
        
        <a href="/wifi" class="btn btn-primary">Wi-Fi Connection</a>
        <a href="/settings" class="btn btn-secondary">Settings</a>
        <a href="/update" class="btn btn-secondary">OTA Update</a>
        <a href="/restart" class="btn btn-danger">Restart Device</a>

        <p class="footer"><a href="https://github.com/nicholaswilde/ge-home-assistant-adapter" target="_blank" style="color: #89b4fa; text-decoration: none;">GitHub</a></p>
    </div>
</body>
</html>
)=====";

#endif // LANDING_HTML_H
