# DMX_CTRL
A DMX512-combatible light control platfrom for medium-complex projects.

There are currently standard solutions for light control that fill specific needs:
- DMX512 compatible stage lighting ($13 - $ 300) usually with Full RGB control and maybe XYZ Control
- Custom solutions for complex installations using WS1812 Strips (Fadecandy and others). This for Example: http://www.deniznicoleart.com/
- Progammable light controllers that can play back preprogrammed patterns that can get triggered by DMX512 (Chinese closed source products)

What is missing is a controller that combines the simple, low cost wiring and control of DMX512 with the flexibility of a customizable firmware and control options.

## Usage Case
The follwing is an outline of the current usage case for an art car and why a specific controller is needed. The art car has multiple light-elements that need central coordination/sequencing but also reacts to other stimulus:
- "Hoover Field" -> WS2812 Light Strips around the base that display a moving pattern syncronized with the speed of the car.
- "Engines" -> Simple RGB Strips that are playing a preprpogrammed startup/shutdown seuqence and a animation while moving
- "Internal Lights" -> Lantern with red LED's that are supposed to flicker and occasionally fail
- "Mood Lighting" -> White LED Strips that can be controlled in brightness
- "Centralized Control Panel" -> Touchscreen control to trigger and sequence effects (Either Cheap Windows Tablet or Raspberry Pi)
These are all usage cases that are not "Simple" DMX RGB-Light cases but also do not warrant complex solutions like FadeCandy that need a lot of programming, USB wiring and complex control computers.
While these effects could be achived with a handful of Arduinos it makes central control more compleicated as there is no common control bus easily available and the processing power of Arduinos is limited. 

## Feature List
-	Can act as DMX Master or Slave
-	Up to 2 WS8212 LED strips with up to 512 Pixel 
-	2x RGB LED/Strip and 1xRGBW LED/Strip support with up to 3A/12V per LED (Or optionally drive PWM controlled servos)
-	Configurable via USB/Serial Terminal
-	Up to 2 Analog Inputs (Can be used to control effects or act as Triggers)
-	8 Address Lines (Optional LCD via i2C and Encoder via GPIOs instead)
-	2 Mode Jumpers to hard-select options (Simple Mode (PWM Only) / Extended Mode (PWM+Strips+FX) and USB Enable/Disable)
-	Button to restore settings on startup or Test Mode
-	External UART or option to connect ESP8266 WiFi Board
-	USB to use as a Computer -> Master Bridge
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
- Triggers -> Working
- Plugin Manager -> Working

- WS2812 Support -> Pending
- Serial Control (Wifi/Serial/USB) -> Pending

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

## Development Enironment
The software is developed with ST Microelectronics STM32CubeIDE 1.3.0
The software framework was generated with STM's ST CubeMX Solution (Config file included in DOC folder) and uses ST's HAL framework.
(Not my favorite but that's what they want you to use...)
I use a ST-Link V2.0 Clone to upload/Debug software. (Eventually there might be a USB DFU Firmware Upload Client....)

WARNING:
There are currently a lot of bluepill STM32F103 board clones with fake STM32F103 circulating on Amazon.
These can be used with a special driver for the ST-Link (Because they show up with a different device ID and the original ST driver blocks those clones) but won't work with STM32CubeIDE out of the box... 

## Serial Shell
[List of Serial Shell Commands](SHELL_COMMANDS.md)

## FX Plugin API
[FX Plugin Manual](FX_API.md)

## Notes

### Jumper Options

#### ADDRESS
The 8 Address Jumpers set the DMX Start address for the Registers.
As there are 512 possible DMX Addresses the Address set via jumpers will be multiplied by 2.

#### MODE
Jumper 1
Disabled = DMX Register Mode 1 (Only PWM Control, not FX)
Enabled  = DMX Register Mode 2 (PWM + WS2812B Strips, FX enabled

In DMX Mode 1 only the following Registers will be available:
#define MAX_BRIGHTNESS		0
#define CH1_RED				1
#define CH1_GREEN			2
#define CH1_BLUE			3
#define CH2_RED				4
#define CH2_GREEN			5
#define CH2_BLUE			6
#define CH3_RED				7
#define CH3_GREEN			8
#define CH3_BLUE			9
#define CH3_WHITE			10

In DMX Mode 2 additionally the following Registers will be available:
#define FX_SELECT			11
#define	STRIP1_PATTERN		12
#define STRIP1_SPEED		13
#define STRIP1_SIZE			14
#define STRIP1_COMPLEXITY	15
#define STRIP1_V1			16
#define STRIP1_V2			17
#define STRIP1_V3			18
#define	STRIP2_PATTERN		19
#define STRIP2_SPEED		20
#define STRIP2_SIZE			21
#define STRIP2_COMPLEXITY	22
#define STRIP2_V1			23
#define STRIP2_V2			24
#define STRIP2_V3			25

Jumper 2
Disabled = No USB Support (Saves a lot of memory)
Enabled  = Eanbles Serial Port via USB

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

### Restoring Defaults
Pressing the button while powering up will erease all settings and restore the defaults.

### Test Mode
Pressing the button after powering up will turn on test mode where the module cycles through all lights to test wiring

### Triggers
The two trigger inputs  can be set up in different ways:

#### Direct Control
The analog value (0...3.3V) is directly mapped onto one of the control registers.
(For example an external inpout can be linked to the brightness of a channel or set the input parameter for an effect)

#### Threshold
A Specific value can be written to a specific control register wen crossing a threshold from low to high or from high to low.
(For example a specific effect can be tiurned on/off via a switch)

### Wifi/USB/Serial DMX Control

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
- Add Button for ESP8266 Button to trigger events on startup

### Layout:
- Make RS485 module base 2.54 mm longer (Or not.. apprently they come in two sizes.. Maybe add both sizes by adding another line of holes)
- Add silkscreen for Pin 1 to connectors
- Move RS232 Connector so it can be used in parallel with ESP-01 for Debug (Move further in so angled connector can be used....)
- Bluepill collides with mounting hole in corner
- Remove solder mask from high-current paths for tinning
- Battery collides with power connector
- Move labels for power supply to other side of connector or back
- Make holes for TO220 Larger so they fit flush
- RS485 module is rotated 180 Deg
- Create more room for programming connector on bluepill board by moving 5V Regulator
- Move Compyright Notice on Bottom to allow for Tinning
- For WS2812B Cable Connector, less extrame angle, 0.6mm smaller holes, Shink total size

## Known Issues (REV 2.0)
### Schematic:

### Layout:
- Bluepill collides with mounting hole in corner
- Panelize Board


