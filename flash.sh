#!/usr/bin/env bash

REPO_NAME=${PWD##*/}
DEVICE_NAME="XDOT"

# Skip compiling and flash the existing binary. Use "previous" argument.
# ./flash.sh previous
if [ "$1" != "previous" ]; then

    # Remove the previous build if any.
    if [ -d ./BUILD ]; then
        rm -r ./BUILD/*
        echo "[INFO] Removed the previous build."
    fi

    # Compile it for xdot platform with ARM gcc.
    mbed compile -m xdot_l151cc -t GCC_ARM

fi

if [ ! -f ./BUILD/xdot_l151cc/GCC_ARM/${REPO_NAME}.bin ]; then
    echo "[ERROR] No binary file to flash. Are there any compiling errors?"
    exit 1
fi

if [ ! -d /Volumes/${DEVICE_NAME} ]; then
    echo "[ERROR] The XDOT device is not connected."
    exit 2;
fi

# Flash it on your connected device.
echo "[INFO] Flashing XDOT device ..."
echo "[INFO] Check the blinking led on the device, when it's finished press the reset button, on the device."
cp ./BUILD/xdot_l151cc/GCC_ARM/${REPO_NAME}.bin /Volumes/${DEVICE_NAME}
