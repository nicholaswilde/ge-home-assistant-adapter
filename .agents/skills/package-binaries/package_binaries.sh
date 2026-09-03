#!/bin/bash
set -e

ENVS=("xiao_c3")
if [ "$#" -gt 0 ]; then
    ENVS=("$@")
fi

OUTPUT_DIR="dist"
mkdir -p "$OUTPUT_DIR"

for ENV in "${ENVS[@]}"; do
    echo "Packaging for $ENV..."
    
    # Run the build
    pio run -e "$ENV"
    
    BUILD_DIR=".pio/build/$ENV"
    ZIP_NAME="$OUTPUT_DIR/$ENV.zip"
    
    # Remove old zip if exists
    rm -f "$ZIP_NAME"
    
    # Check if firmware exists
    if [ ! -f "$BUILD_DIR/firmware.bin" ]; then
        echo "Error: firmware.bin not found for $ENV!"
        exit 1
    fi
    
    # Copy flash.sh into build dir temporarily for packaging
    cp scripts/flash.sh "$BUILD_DIR/"
    
    # Create the zip with what exists
    FILES_TO_ZIP=("flash.sh")
    [ -f "$BUILD_DIR/firmware.bin" ] && FILES_TO_ZIP+=("firmware.bin")
    [ -f "$BUILD_DIR/partitions.bin" ] && FILES_TO_ZIP+=("partitions.bin")
    [ -f "$BUILD_DIR/bootloader.bin" ] && FILES_TO_ZIP+=("bootloader.bin")
    
    cd "$BUILD_DIR"
    zip "../../$ZIP_NAME" "${FILES_TO_ZIP[@]}"
    rm -f flash.sh
    cd ../../../
    
    echo "Created $ZIP_NAME"
done

echo "Packaging complete!"
