# DMX_CTRL
A DMX512-Combatible Light Control Platfrom for medium-complex projects.

There are currently standard solutions for light control that fill specific needs:
- DMX512 Compatible stage lighting ($13 - $ 300) usually with Full RGB control and maybe XYZ Control
- "Specialty" Solutions for more elaborate light installations using WS1812 Strips (Fadecandy and others)
- Progammable Light Controllers that can play back preprogrammed patterns that can get triggered by DMX512

What is missing is a controller that combines the simple, low cost wiring and control of DMX512 with the flexibility of a customizable firmware and control options.

## Usage Case
The follwing is an outline of the current usage case for an Art Car and why a specific controller is needed. The art car has multiple light-elements thzat need central coordination/sequencing but also reacts to other stimulus:
- "Hoover Field" -> WS2812 Light Strips around the base that display a moving pattern syncronized with the speed of the car.
- "Engines" -> Simpole RGB Strips that are playting a preprpogrammed startup/shutdown seuqence and a animation while moving
- "Internal Lights" -> Lantern with red LED's that are supposed to flicker and occasionally fail
- Mood Lighting" -> White LED Strips that can be controlled in brightness

These are all usage cases that are not "Simple" DMX RGB-Light cases but also do not warrant complex solutions like FadeCandy that need a lot of programming, USB wiring and complex Control Computers.

## Feature List
-	Can act as DMX Master or Slave
-	Up to 2 WS8212 LED strips with up to 1024 Pixel 
-	2x RGB LED/Strip and 1xRGBW LED/Strip support with up to 3A/12V per LED (Or optionally drive PWM controlled servos)
-	Configurable via USB/Serial Terminal
-	Up to 2 Analog Inputs (Can be used to control effects or act as Triggers)
-	8 Address Lines (Optional LCD via i2C and Encoder via GPIOs instead)
-	2 Mode Jumpers to hard-select options (Master/Slave and WS8212 Support on/off)
-	Test Button to go into Setup or Test Mode
-	External UART or option to connect ESP8266 WiFi Board
-	USB to use as a Computer-> Master Bridge
-	Total Cost per Board ~$16 (Most of that (8$) are MOSFETs so it depends on how many LED channels are needed)
-	Board size 10x5 cm

![Front](/DOC/front.jpeg)
![Back](/DOC/back.jpeg)

## Current Status
- DMX512 Control of PWM Channels -> Working
- DMX Address setting via Jumpers -> Working
- Serial Shell to control/Save settings -> Working
- Load defaults on startup -> Working
- Clear settings (Hold button on power-up) -> Working
- Test Mode (Press button after power up, cycles through all LED channels) -> Working
- Triggers -> Partially working

## Next Steps
- Plugin Mananger to simplify FX Plugin Development
- Fix ADC IRQ for Trigger
- Get at least one WS2812 Channel via DMA-Driven SPI Working
- Add support for Serial/USB/Wifi DMX

## BOM Cost
The BOM cost for the simplest implementation is 
- $0.50 for the board
- $2.00 for the Bluepill STM32F103 Board
- $0.50 for the RS485 Module
- $4.00 for Misc (Resistors, pin headers, switch)
- $0.80 per Mosfet for controllable Channels
So somewhere between $7 to $15.

## Notes

### DMX Control Registers
For reference refer to the DMX_CTRL.xlsx spreadsheet.
The numbers of registers depends on the Mode (With/Without WS2812 Control)

### DMX Control Wires (A/B)
With my controller the follwing pinout works (Looking at a male connector):
Pin 1 = GND
Pin 2 = A
Pin 3 = B
Soem controllers allow to swap A/B with a switch so your milage might vary....

### DMX Termination
DMX needs to be terminated at the end of the chain with a 160 Ohm Resistor.

### Power Supply
Board has been tested with 9 PWM channels with 216 RGB LED's with a total power consumption of 3.8A @ 12V.
If only 12V is supplied make sure to add J5 so the microcontroller is powered.

### Serial Shell
Serial shell is enabled at 115 kBaud on the serial port. (Note that Rx and Tx are swapped at the moment....).
There is a "Help" command to see all the supported commands.
For parameters just type in the command.

### Triggers
(Currently Not working)
The two trigger inputs  can be set up in different ways:

#### Direct Control
The analog value (0...3.3V) is directly mapped onto one of the control registers.
(For example an external inpout can be linked to the brightness of a channel or set the input parameter for an effect)

#### Threshold
A Specific value can be written to a specific control register wen crossing a threshold from low to high or from high to low.
(For example a specific effect can be tiurned on/off via a switch)

### Plugin Development
TBD

### Wifi/USB/Serial DMX Control
TBD

### Master Control
TBD

### What's that strange connector footprint for the WS2812 connection?
The idea is to solder on wires and then use a zip-tie through the holes and hot-glue to secure the wires.
Past projects had big problems with wires breaking off or shortening out... Maybe this will work.....

## References
- DMX Pinout
- Fadecandy Webpage
- Ultra-Cheap DMX Lights

## Known Issues (REV 1.0)
### Schematic:
- RX/TX Swapped on Serial Connector (TBD if they are right or wrong on ESP-01 connector...)
- GND not connected on RS485 Module

### Layout:
- Make RS485 module base 2.54 mm longer (Or not.. apprently they come in two sizes)
- Add silkscreen for Pin 1 to connectors
- Move RS232 Connector so it can be used in parallel with ESP-01 for Debug
- Bluepill collides with mounting hole in corner
- Remove solder mask from high-current paths for tinning
- Battery collides with power connector
- Move labels for power supply to other side of connector or back
- Make holes for TO220 Larger so they fit flush
- RS485 module is rotated 180 Deg
- Create more room for programming connector on bluepill board by moving 5V Regulator
