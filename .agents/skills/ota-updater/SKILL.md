---
name: ota-updater
description: Skill to automate building and wirelessly flashing firmware (OTA) to the ESP32C3 adapter device.
---

# OTA Updater Skill

This skill provides an automated bash script to compile the latest firmware via PlatformIO and automatically upload it to the ESP32C3 adapter device over the local network (Over-The-Air).

## Prerequisites

1. The device must be powered on and connected to the same local network as your build machine.
2. The `.env` file at the root of the project must contain the correct IP address in the `DEVICE_IP` variable.
3. (Optional) If OTA is password-protected, set `OTA_PASSWORD` in the `.env` file.
4. The currently flashed firmware on the device must have the OTA web routes registered (i.e., `/update` endpoint).

## Usage

To run the OTA update, execute the script from the repository root:

```bash
# Flash the default environment (xiao_c3)
bash .agents/skills/ota-updater/ota_update.sh

# Flash a specific environment
bash .agents/skills/ota-updater/ota_update.sh xiao_c3
```

### What it does

1. Parses the `.env` file for the `DEVICE_IP` (and optional `OTA_PASSWORD`).
2. Compiles the firmware for the requested PlatformIO environment using `pio run -e <env>`.
3. Verifies the device is reachable over the network.
4. Uses `curl` to submit the compiled `firmware.bin` as a multipart form data upload to `http://<IP>/update`.
5. Prints success and waits for the device to automatically reboot.

## Agent Guidelines

- When prompted to update the device, flash the firmware, or perform an OTA update, execute this script using `ctx_execute` or `run_command`.
- **CRITICAL:** Before running the OTA updater, verify the `.env` file exists and has `DEVICE_IP` configured. If the user hasn't set it, prompt them to add it.
- Pass the environment variable as the argument to the `ota_update.sh` script (e.g., `bash .agents/skills/ota-updater/ota_update.sh xiao_c3`), or leave it blank to use the default `xiao_c3`.
