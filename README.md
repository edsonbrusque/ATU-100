# ATU-100 firmware for serial communication

This is a firmware for using the popular open source ATU-100 Antenna Tuner in remote operations.

This is very useful, for example, to have the tuner right at the feedpoint of the antenna.


## Build instructions

To compile it, just download and install Microchip MPLAB-X IDE and XC8 compiler:

https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide

https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers

Open the project, click on the hammer icon (Build Project) and that's it. It should  compile on first try without any errors on most systems.


## How to program

The compiled hex file is on dist\default\production folder.

Just load it on PICkit or any other tool you use for writing the microcontroller.


## Acknowledgements

This code is derived from:

WA1RCT/N7DDC-ATU-100-mini-and-extended-boards

Which is derived from:

https://github.com/Dfinitski/N7DDC-ATU-100-mini-and-extended-boards

