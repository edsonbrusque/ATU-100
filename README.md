# ATU-100 firmware for serial communication

This is a firmware for using the popular open source ATU-100 Antenna Tuner in remote operations.

This is very useful, for example, to have the tuner right at the feedpoint of the antenna.


## Build instructions

### Online Build (No Installation Required!) ⭐

The easiest way to build this firmware is using **GitHub Actions** - no software installation needed!

Simply push your code to GitHub and the firmware will be automatically compiled in the cloud. Download the `.hex` file from the workflow artifacts.

See **[BUILD.md](BUILD.md)** for complete instructions on:
- 🌐 **GitHub Actions** (recommended - zero setup)
- 🐳 **Docker** (local build without MPLAB X)
- 💻 **MPLAB X IDE** (traditional method)

### Traditional Build with MPLAB X

If you prefer to build locally with the IDE:

1. Download and install Microchip MPLAB-X IDE and XC8 compiler:
   - https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide
   - https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers

2. Open the project in MPLAB X (located in `ATU_100_EXT_board/FirmWare_PIC16F1938`)

3. Click on the hammer icon (Build Project)

The firmware should compile on first try without any errors on most systems.


## How to program

The compiled hex file is on dist\default\production folder.

Just load it on PICkit or any other tool you use for writing the microcontroller.


## How to interface

The serial routine runs at a baud-rate of 9600 bps.

To control the tuner you just need to send a character as if you were pressing a button on the original firmware. The firmware is case insensitive and the commands are:

**A** - toggles auto (automatic tuning)

**B** - toggles bypass

**R** - resets the tuner (makes C = 0 and L=0)

**T** - forces tuning

The tuner sends almost the same text that is send to the display on the original firmware whenever there's a change in status.


## Acknowledgements

This code is derived from:

https://github.com/WA1RCT/N7DDC-ATU-100-mini-and-extended-boards

Which is derived from:

https://github.com/Dfinitski/N7DDC-ATU-100-mini-and-extended-boards

