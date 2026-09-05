# :shield: GE Home Assistant Adapter (C++) :robot:

[![task](https://img.shields.io/badge/Task-Enabled-brightgreen?style=for-the-badge&logo=task&logoColor=white)](https://taskfile.dev/#/)
[![ci](https://img.shields.io/github/actions/workflow/status/nicholaswilde/ge-home-assistant-adapter/ci.yml?label=ci&style=for-the-badge&branch=main&logo=github-actions)](https://github.com/nicholaswilde/ge-home-assistant-adapter/actions/workflows/ci.yml)
[![Coveralls](https://img.shields.io/badge/dynamic/xml?url=https%3A%2F%2Fcoveralls.io%2Frepos%2Fgithub%2Fnicholaswilde%2Fge-home-assistant-adapter%2Fbadge.svg%3Fbranch%3Dmain&query=%2F%2F*%5Blocal-name()%3D'text'%5D%5Blast()%5D&label=Coveralls&style=for-the-badge&logo=coveralls)](https://coveralls.io/github/nicholaswilde/ge-home-assistant-adapter?branch=main)

Firmware for the ESP32C3-based adapter [available from FirstBuild](https://firstbuild.com/inventions/home-assistant-adapter/) to connect GE appliances locally to Home Assistant via MQTT.

> [!WARNING]
> This project is currently in a `v0.X.X` development stage. Features and configurations are subject to change, and breaking changes may be introduced at any time.

## :package: Hardware

The Home Assistant adapter consists of a
[Xiao ESP32C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/) and
[carrier board](docs/schematic-v1.0.pdf) that breaks out the serial interface of
the Xiao to an RJ45 jack.

See the [Compatibility List](docs/compatibility-list) for supported GE appliance models (cooking, dishwashers, dryers, washers, and water heaters).

## :zap: Installation

### Quick Install (Pre-compiled Binaries)

You can flash the latest pre-compiled firmware directly from your terminal using the provided flash script. Connect your ESP32C3 via USB and run the command below. Replace `/dev/ttyACM0` with your actual serial port if different.

```bash
# Flash the adapter
bash -c "$(curl -fsSL https://raw.githubusercontent.com/nicholaswilde/ge-home-assistant-adapter/main/scripts/flash.sh)" _ xiao_c3 /dev/ttyACM0
```

> [!WARNING]
> Running a script directly from the internet with `bash -c "$(curl...)"` is a potential security risk. Always review the script's source code before executing it to ensure it is safe. You can view the script [here](https://github.com/nicholaswilde/ge-home-assistant-adapter/blob/main/scripts/flash.sh).

## :hammer_and_wrench: Development Setup

1. Install [PlatformIO](https://platformio.org/) and [go-task](https://taskfile.dev).
2. Initialize the project:
   ```bash
   task init
   ```
   This will copy the sample configuration files to `config/Config.h` and `config/Certificate.h` (without overwriting existing custom configs) and configure Git hooks to prevent credentials from being committed.
3. Edit `config/Config.h` with your WiFi credentials, MQTT configuration, and device ID.
4. Edit `config/Certificate.h` to add your certificate (if any).

In-depth instructions can be found in the [Getting Started](docs/getting-started.md) guide.

## :globe_with_meridians: Web Interface & Configuration

When unconfigured or unable to connect to a saved Wi-Fi network, the adapter broadcasts a Wi-Fi Access Point (AP) named `ge-ha-adapter-XXXX` (where `XXXX` represents the last 4 characters of the device's MAC address).

### Initial Wi-Fi & MQTT Setup

1. **Connect to the Access Point:**
   - On your phone or computer, scan for available Wi-Fi networks and connect to `ge-ha-adapter-XXXX`.
   - The network is open and requires no password.
   - A captive portal popup should open automatically. If not, open a web browser and navigate to `http://192.168.4.1`.

2. **Configure Wi-Fi Credentials (`/wifi`):**
   - Click **Wi-Fi Connection** on the dashboard (or visit `http://192.168.4.1/wifi`).
   - Select your Wi-Fi network from the scanned list (or click **Refresh List** / enter the SSID manually).
   - Enter your Wi-Fi password. Use the eye icon to toggle visibility and confirm correctness.
   - Click **Save & Connect**. The adapter will store your network credentials in non-volatile storage (NVS) and restart.

3. **Configure MQTT & Device Settings (`/settings`):**
   - Once connected to your local network, browse to the adapter's assigned local IP (or navigate to `http://192.168.4.1/settings` while still connected to the AP).
   - Configure your parameters:
     - **MQTT Device ID**: Identifier used in topics and Home Assistant discovery (defaults to device ID in `Config.h`).
     - **MQTT Server**: IP address or hostname of your MQTT broker (e.g. Home Assistant Mosquitto broker).
     - **MQTT Port**: Port of your MQTT broker (default: `1883`).
     - **MQTT Username**: Username for MQTT authentication.
     - **MQTT Password**: Password for MQTT authentication (toggle visibility with the eye icon).
   - Click **Save Settings & Reboot** to save the values to NVS and restart the device.

### Web Interface Features

The onboard Catppuccin-themed web interface provides:
- **Captive Portal / Wi-Fi Setup (`/wifi`)**: Connect to the device AP to scan networks and configure Wi-Fi credentials.
- **Device & MQTT Settings (`/settings`)**: Configure MQTT broker, port, credentials, and device ID. Values persist in NVS and fall back to `Config.h` defaults.
- **OTA Firmware Update (`/update`)**: Modern drag-and-drop file upload zone for `.bin` firmware flashing.
- **Device Reboot (`/restart`)**: Restart the adapter with automatic redirect back to dashboard upon reconnecting.

## :sparkles: Differences from Upstream

This fork includes several enhancements over the upstream repository ([`geappliances/home-assistant-adapter`](https://github.com/geappliances/home-assistant-adapter)):

- **Web Configuration Portal**: Added an onboard Catppuccin-themed web interface with a captive portal for network scanning, Wi-Fi configuration, and device reboot.
- **Web-Based MQTT Configuration**: Dynamic configuration for MQTT connection details (broker, port, credentials, topic path, device ID) saved to non-volatile storage (NVS via `Preferences`) without requiring recompilation.
- **Password Masking & Visibility**: Password fields in the web interface feature show/hide toggle controls.
- **Web OTA Firmware Updates**: Integrated browser-based OTA update server with a modern drag-and-drop file upload area.
- **Auto-Discovery**: Automatic Home Assistant MQTT sensor discovery configuration published on connection.
- **Non-Blocking Connectivity**: Improved asynchronous connection handling for Wi-Fi and MQTT to prevent watchdog lockups during network loss.
- **Modern Tooling & CI**: Automated builds and checks with [Taskfile](https://taskfile.dev/), native Unity unit test suites, upstream regression tests, and automated GitHub release pipelines.

## :rocket: Usage

### Build

Builds the firmware into `.pio/build/xiao_c3/firmware.bin`.

```bash
task build
```

### Test

Runs native unit tests and python regression tests.

```bash
task test
```

### Clean

Deletes all build artifacts.

```bash
task clean
```

### Upload

Uploads/flashes the firmware to the ESP32-C3.

```bash
task upload
```

> [!NOTE]
> The board may need to be reset into bootloader mode by holding the B (boot)
> button and pressing the R (reset) button.

### (Serial) Monitor

Opens the PlatformIO serial monitor to view a connected ESP32-C3's serial output.

```bash
task monitor
```

### Hardware Serial Test (FTDI to RJ45)

Verify GEA3 serial communication between the carrier board's RJ45 connector and a USB-to-TTL (FTDI) serial adapter plugged into `/dev/ttyUSB0` at 230400 baud.

Wiring:
- RJ45 Pin 4 (Board RX) &rarr; FTDI TX
- RJ45 Pin 5 (Board TX) &rarr; FTDI RX
- RJ45 Pin 8 (GND) &rarr; FTDI GND

Run the test script:

```bash
uv run python scripts/test_serial.py
```

Pass `--port` or `--baud` if using different parameters:

```bash
uv run python scripts/test_serial.py --port /dev/ttyUSB0 --baud 230400
```

### Mock Appliance

Simulate a GEA3 appliance to test serial communication and verify Home Assistant MQTT auto-discovery without physical hardware:

```bash
task mock
```

Pass arguments (e.g. `--port`) using Task's `--` separator:

```bash
task mock -- --port /dev/ttyUSB0
```


## :file_folder: Example Home Assistant Configuration

Sample yaml can be found in
[home-assistant-examples](https://github.com/geappliances/home-assistant-examples).

## :electric_plug: FTDI Wiring

A 3.3V USB-to-TTL serial adapter (such as an FTDI FT232RL) can be connected to the carrier board's RJ45 jack to verify GEA3 communication using `scripts/test_serial.py` or simulate an appliance with `task mock`.

### Pinout Connections

- **FTDI TXD** &rarr; **RJ45 Pin 4** (Board RX)
- **FTDI RXD** &rarr; **RJ45 Pin 5** (Board TX)
- **FTDI GND** &rarr; **RJ45 Pin 8** (Board GND)

> [!NOTE]
> Leave VCC, RTS, CTS, and remaining RJ45 pins disconnected. The ESP32 is powered independently via its USB-C port.

### Wiring Diagram

```text
     +--------------------+
     |                    |
     |                 +----------+
     |                 |  RJ45    |
     |                 |     1[ ] |
+---------+            |     2[ ] |    +--------+
|         |            |     3[ ] |----| [ ]    |
| ESP32C3 |            |  RX 4[X] |----| [X]TX  |
|         |            |  TX 5[X] |----| [X]RX  |
+---------+            |     6[ ] |    | [ ]    |
     |                 |     7[ ] |    | [ ]    |
     |                 | GND 8[X] |----| [X]GND |
     |                 +----------+    +--------+
     |                    |               FTDI
     +--------------------+
         GE HA Adapter
```

## :balance_scale: License

​[BSD 3-Clause License](LICENSE)

## :writing_hand: Author

​This project was started in 2026 by [Nicholas Wilde][2].

[2]: <https://github.com/nicholaswilde/>
