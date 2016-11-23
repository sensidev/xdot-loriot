xDot with Loriot
----------------

This is an adaptation of the original manual ABP (Activation by personalization) example from Multitech [official mbed repository](https://developer.mbed.org/teams/MultiTech/code/Dot-Examples/).

# Prerequisites

1. Use a Mac OS (>=10.11.6) PC.
1. Installed the mbed [toolchain](https://docs.mbed.com/docs/mbed-os-handbook/en/5.1/dev_tools/cli/).
1. [Install](https://gist.github.com/joegoggins/7763637) `gcc-arm-none-eabi` with homebrew.
1. Multitech [mLinux Conduit Gateway with LoRa mCard](http://www.multitech.com/brands/multiconnect-conduit) and [xDot-DK](http://www.multitech.com/brands/multiconnect-xdot).

# Loriot

1. Clone this repo and change dir into it.
1. Create a loriot free account and register your gateway.
1. Generate a new device.
1. Take the device's `DevAddr`, `NwkSKey`, `AppSKey` (use the big endian).
1. Use `utils/hex_to_bytes_array.py 11223344` to obtain byte arrays.
1. Copy `auth/loriot_demo.h` to `auth/loriot.h`.
1. Replace the dev address, network session key and app session key accordingly.
1. Connect your device to the PC.
1. Compile and flash it with `./flash.sh`.

# Notes

1. Be sure to use the correct mbed-os library version, that is tested and supported by the `libxdot-mbed5` [library](https://developer.mbed.org/teams/MultiTech/code/libxDot-mbed5/).
1. To see the debug logs coming from xDot you need to connect to the serial interface through USB. e.g. `screen /dev/cu.usbmodem14222 115200`
