---
name: package-binaries
description: Skill to build and package firmware binary ZIP files (partitions.bin, firmware.bin) for the xiao_c3 environment.
---

# Package Binaries Skill

This skill builds and bundles PlatformIO binary outputs into individual ZIP files containing `partitions.bin` and `firmware.bin` for manual distribution and hardware testing.

## Usage

To generate zip archives for the default `xiao_c3` board:

```bash
bash .agents/skills/package-binaries/package_binaries.sh
```

To generate zip archives for specific environments:

```bash
bash .agents/skills/package-binaries/package_binaries.sh xiao_c3
```

### Outputs

The resulting zip files will be placed in the `dist/` folder (or `$OUTPUT_DIR` if overridden):
- `dist/xiao_c3.zip`

Each zip package contains:
- `flash.sh` (standalone flash script)
- `partitions.bin`
- `firmware.bin`
- Note: ESP32-C3 typically handles the bootloader differently or bundles it, but if `bootloader.bin` exists it will be included.

## Agent Guidelines

- Run this script using `run_command` whenever asked to create, prepare, or package test/release binary zip files for external testers or releases.
