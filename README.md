# :shield: Home Assistant Adapter (C++) :robot:

[![task](https://img.shields.io/badge/Task-Enabled-brightgreen?style=for-the-badge&logo=task&logoColor=white)](https://taskfile.dev/#/)
[![ci](https://img.shields.io/github/actions/workflow/status/nicholaswilde/ge-home-assistant-adapter/ci.yml?label=ci&style=for-the-badge&branch=main&logo=github-actions)](https://github.com/nicholaswilde/ge-home-assistant-adapter/actions/workflows/ci.yml)

Firmware for the ESP32C3-based adapter [available from FirstBuild](https://firstbuild.com/inventions/home-assistant-adapter/) to connect GE appliances locally to Home Assistant via MQTT.

## :package: Hardware

The Home Assistant adapter consists of a
[Xiao ESP32C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/) and
[carrier board](doc/schematic-v1.0.pdf) that breaks out the serial interface of
the Xiao to an RJ45 jack.

## :hammer_and_wrench: Setup

1. Install [PlatformIO](https://platformio.org/) and [go-task](https://taskfile.dev).
2. Initialize the project:
   ```bash
   task init
   ```
   This will copy the sample configuration files to `config/Config.h` and `config/Certificate.h` (without overwriting existing custom configs) and configure Git hooks to prevent credentials from being committed.
3. Edit `config/Config.h` with your WiFi credentials, MQTT configuration, and device ID.
4. Edit `config/Certificate.h` to add your certificate (if any).

In-depth instructions can be found in the [Getting Started](doc/getting-started.md) guide.

## :globe_with_meridians: Web Interface & Portal

The firmware provides a Catppuccin-themed web interface for configuration and maintenance:
- **Captive Portal / Wi-Fi Setup (`/wifi`)**: Connect to the device AP (e.g. `ha-adapter-XXXX`) to scan networks and configure Wi-Fi credentials.
- **Device & MQTT Settings (`/settings`)**: Configure the MQTT Device ID, broker IP/port, username, masked password (with show/hide toggle), and base topic path. Values persist in NVS and fall back to `Config.h` defaults.
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

## :file_folder: Example Home Assistant Configuration

Sample yaml can be found in
[home-assistant-examples](https://github.com/geappliances/home-assistant-examples).

## :balance_scale: License

​[BSD 3-Clause License](LICENSE)

## :writing_hand: Author

​This project was started in 2026 by [Nicholas Wilde][2].

[2]: <https://github.com/nicholaswilde/>
