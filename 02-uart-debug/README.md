Project 2: UART Debug Output

**Hardware:** STM32F401RE Nucleo Board  
**Objective:** Serial communication for debugging and data output. Also for better understanding of register manipulation 

## Features
- Bare metal UART2 driver implementation
- Real time debug output via USB virtual COM port


## Build & Flash
make clean
make flash

## Open serial monitor (Mac)
screen /dev/cu.usbmodem* 9600

#
Author:Diego Klish 