import os
import re
import unittest
import urllib.request

UPSTREAM_REPO = os.environ.get("UPSTREAM_REPO", "https://github.com/geappliances/home-assistant-adapter")
LOCAL_REPO = os.environ.get("LOCAL_REPO", os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

class TestUpstreamRegression(unittest.TestCase):
    def setUp(self):
        upstream = UPSTREAM_REPO
        if upstream.startswith("http"):
            cache_dir = os.path.join(LOCAL_REPO, ".cache")
            cache_file = os.path.join(cache_dir, "upstream_main.cpp")
            
            # Use cached file if it exists
            if os.path.exists(cache_file):
                with open(cache_file, 'r') as f:
                    self.upstream_main = f.read()
            else:
                # Convert standard github repo URL to raw file URL
                if "github.com" in upstream and "raw.githubusercontent.com" not in upstream:
                    upstream = upstream.replace("github.com", "raw.githubusercontent.com").rstrip('/')
                    upstream += "/main"
                
                url = upstream.rstrip('/') + "/src/main.cpp"
                with urllib.request.urlopen(url) as response:
                    self.upstream_main = response.read().decode('utf-8')
                
                # Save to cache
                os.makedirs(cache_dir, exist_ok=True)
                with open(cache_file, 'w') as f:
                    f.write(self.upstream_main)
        else:
            with open(os.path.join(upstream, "src/main.cpp"), 'r') as f:
                self.upstream_main = f.read()
        
        local_code = ""
        src_dir = os.path.join(LOCAL_REPO, "src")
        for file in os.listdir(src_dir):
            if file.endswith(".cpp") or file.endswith(".h"):
                with open(os.path.join(src_dir, file), 'r') as f:
                    local_code += f.read() + "\n"
        self.local_main = local_code

    def test_essential_includes(self):
        """Ensure core includes are still present."""
        includes = [
            "PubSubClient.h",
            "WiFi.h",
            "Config.h",
            "HomeAssistantBridge.h"
        ]
        for inc in includes:
            self.assertTrue(re.search(r'#include\s+[<"]' + inc + r'[>"]', self.local_main), f"Missing include: {inc}")

    def test_bridge_initialization(self):
        """Ensure the bridge is initialized with mqtt and serial."""
        self.assertTrue(re.search(r'HomeAssistantBridge\s+bridge;', self.local_main), "Missing bridge instance")
        self.assertTrue(re.search(r'bridge\.begin\s*\(\s*(mqttClient|getMqttClient\(\))\s*,\s*Serial1\s*,\s*deviceId\s*\)', self.local_main), "Missing bridge.begin")

    def test_bridge_loop(self):
        """Ensure bridge.loop() is called."""
        self.assertTrue(re.search(r'bridge\.loop\s*\(\s*\)', self.local_main), "Missing bridge.loop()")

    def test_mqtt_configuration(self):
        """Ensure MQTT is configured and connected."""
        self.assertTrue(re.search(r'mqttClient\.setServer\s*\(', self.local_main), "Missing mqttClient.setServer")
        self.assertTrue(re.search(r'mqttClient\.connect\s*\(', self.local_main), "Missing mqttClient.connect")
        self.assertTrue(re.search(r'bridge\.notifyMqttDisconnected\s*\(\s*\)', self.local_main), "Missing notifyMqttDisconnected")

    def test_serial_initialization(self):
        """Ensure serial ports are initialized."""
        self.assertTrue(re.search(r'Serial\.begin\s*\(\s*115200\s*\)', self.local_main), "Missing Serial.begin")
        self.assertTrue(re.search(r'Serial1\.begin\s*\(\s*HomeAssistantBridge::baud\s*,\s*SERIAL_8N1\s*,\s*D7\s*,\s*D6\s*\)', self.local_main), "Missing Serial1.begin")

if __name__ == '__main__':
    unittest.main()
