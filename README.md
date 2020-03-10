# DMX_CTRL
DMX512 Controller for light installations based on Bluepill STM32F103 Board

Known Issues:
-----------------
Logical:

Schematic:
- RX/TX Swapped on Serial Connector (TBD if they are right or wrong on ESP-01 connector...)
- GND not connected on RS485 Module

Layout:
- Make RS485 Module base 2.54 mm longer (Or not.. apprently thery come in two sizes)
- Add Silkscreen for Pin 1 to connectors
- Move RS232 Connector so it can be used in Parallel with ESP-01 for Debug
- Bluepill collides with mounting hole in corner
- Remove solder Mask from High-Current Paths for tinning
- Battery collides with power connector
- Move Labels for Power Supply to other side of connector or back
- Make holes for TO220 Larger so thery fit flush
- RS485 Module is Rotated 180 Deg
- Create more room for Programming connector by moving 5V Regulator
