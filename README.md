xDot with Loriot
----------------

This is an adaptation of the original manual ABP (Activation by personalization) example from Multitech [official mbed repository](https://developer.mbed.org/teams/MultiTech/code/Dot-Examples/).

# Loriot

1. Create an account and register your gateway.
1. Generate a new device.
1. Take the device's `DevAddr`, `NwkSKey`, `AppSKey` (use big endian).
1. Use `utils/hex_to_bytes_array.py '11223344'` to obtain byte arrays.
1. Copy `auth/loriot_demo.h` to `auth/loriot.h`.
1. Replace the dev address, network session key and app session key accordingly.

# Notes

1. Be sure to use the correct mbed-os library version, that is tested and supported by the libxdot-mbed5 library. https://developer.mbed.org/teams/MultiTech/code/libxDot-mbed5/
