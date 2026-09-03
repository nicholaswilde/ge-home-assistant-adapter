#ifndef WIFI_HTML_H
#define WIFI_HTML_H

const char WIFI_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Configuration</title>
    %META_REFRESH%
    <style>
        body {
            font-family: 'Inter', system-ui, sans-serif;
            background: #1e1e2e; /* Mocha Base */
            color: #cdd6f4;      /* Mocha Text */
            margin: 0; padding: 20px;
            display: flex; justify-content: center; align-items: flex-start;
            min-height: 100vh; box-sizing: border-box;
        }
        .card {
            background: #181825; /* Mocha Mantle */
            border-radius: 12px;
            padding: 30px; width: 100%; max-width: 500px;
            box-shadow: 0 8px 30px rgba(0,0,0,0.3);
            border: 1px solid #313244; /* Mocha Surface0 */
        }
        h1, h2 { color: #f5c2e7; margin-top: 0; font-weight: 600; text-align: center; }
        p.subtitle { color: #a6adc8; text-align: center; font-size: 15px; margin-bottom: 30px; }
        label { display: block; margin-bottom: 8px; color: #a6adc8; font-size: 14px; }
        select, input[type='text'], input[type='password'], input[type='number'] {
            width: 100%; padding: 12px; margin-bottom: 20px; border-radius: 6px;
            border: 1px solid #45475a; background: #313244;
            color: #cdd6f4; font-size: 16px; box-sizing: border-box;
        }
        select:focus, input:focus { outline: none; border-color: #f5c2e7; }
        .btn {
            display: flex; align-items: center; justify-content: center; text-decoration: none;
            width: 100%; padding: 14px; border-radius: 8px; color: #11111b;
            font-size: 16px; font-weight: bold; cursor: pointer; transition: transform 0.2s, background 0.2s;
            margin-bottom: 15px; border: none; box-sizing: border-box;
        }
        .btn:active { transform: scale(0.98); }
        .btn-primary { background: #89b4fa; }
        .btn-primary:hover { background: #b4befe; }
        .btn-secondary { background: #a6e3a1; }
        .btn-secondary:hover { background: #94e2d5; }
        .section-title { color: #89b4fa; font-size: 18px; margin-top: 20px; margin-bottom: 15px; border-bottom: 1px solid #313244; padding-bottom: 5px; }
        .net-list { margin-bottom: 20px; max-height: 150px; overflow-y: auto; border: 1px solid #313244; border-radius: 6px; padding: 10px; background: #11111b; }
        .net-item { display: flex; justify-content: space-between; padding: 8px; cursor: pointer; border-bottom: 1px solid #1e1e2e; }
        .net-item:last-child { border-bottom: none; }
        .net-item:hover { background: #313244; color: #f5c2e7; }
    </style>
    <script>
    function selectSSID(ssid) { document.getElementById('ssid').value = ssid; }
    </script>
</head>
<body>
    <div class="card">
        <h1>WiFi Settings</h1>
        <p class="subtitle">Connect to network</p>
        
        <div style='display: flex; justify-content: space-between; align-items: center;'>
            <label style='margin-bottom: 0;'>Select Network</label>
            <a href='/wifi/scan' style='color: #cba6f7; font-size: 12px; text-decoration: none;'>&#x21bb; Refresh List</a>
        </div>
        <div style='height: 8px;'></div>
        <div class='net-list'>
            %NETWORK_LIST%
        </div>

        <form action="/wifi" method="POST">
            <label for="ssid">SSID</label>
            <input type="text" id="ssid" name="ssid" required placeholder="Network Name">
            
            <label for="password">Password</label>
            <input type="password" id="password" name="password" placeholder="Leave blank if open">
            
            <button type="submit" class="btn btn-primary">Save & Connect</button>
        </form>
        <a href="/" class="btn btn-secondary">Back</a>
    </div>
</body>
</html>
)=====";

#endif // WIFI_HTML_H
