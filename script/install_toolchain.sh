#!/usr/bin/env bash
set -e

TOOLCHAIN_DIR="$HOME/.platformio/packages/toolchain-riscv32-esp"
VERSION="8.4.0+2021r2-patch5"

if [ -d "$TOOLCHAIN_DIR" ] && [ -f "$TOOLCHAIN_DIR/package.json" ]; then
    echo "Toolchain already installed in $TOOLCHAIN_DIR."
    exit 0
fi

echo "Installing ESP32 RISC-V toolchain manually..."

OS=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m)

if [ "$OS" = "linux" ]; then
    if [ "$ARCH" = "x86_64" ]; then
        URL="https://github.com/espressif/crosstool-NG/releases/download/esp-2021r2-patch5/riscv32-esp-elf-gcc8_4_0-esp-2021r2-patch5-linux-amd64.tar.gz"
    elif [[ "$ARCH" == *"arm"* ]] || [[ "$ARCH" == *"aarch64"* ]]; then
        URL="https://github.com/espressif/crosstool-NG/releases/download/esp-2021r2-patch5/riscv32-esp-elf-gcc8_4_0-esp-2021r2-patch5-linux-armel.tar.gz"
    else
        echo "Unsupported Linux architecture: $ARCH"
        exit 1
    fi
elif [ "$OS" = "darwin" ]; then
    URL="https://github.com/espressif/crosstool-NG/releases/download/esp-2021r2-patch5/riscv32-esp-elf-gcc8_4_0-esp-2021r2-patch5-macos.tar.gz"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

TMP_FILE=$(mktemp)
echo "Downloading $URL..."
curl -sL "$URL" -o "$TMP_FILE"

mkdir -p "$TOOLCHAIN_DIR"
echo "Extracting toolchain..."
tar -xzf "$TMP_FILE" -C "$TOOLCHAIN_DIR" --strip-components=1

echo "Creating package.json..."
cat << EOF > "$TOOLCHAIN_DIR/package.json"
{
  "name": "toolchain-riscv32-esp",
  "version": "$VERSION",
  "description": "GCC Toolchain for Espressif 32-bit RISC-V based SoCs"
}
EOF

rm "$TMP_FILE"
echo "Toolchain installed successfully!"
