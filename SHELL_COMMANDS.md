# Shell Commands
Shell commands are available via the UART and via USB as default. They can be used to set up the system and test lights and inputs. After setting the parameters they can be stored to flash and will be reloaded at startup. (If you don't use the savesettings command all settings will be lost...)
Serial shell is enabled at 115 kBaud on the serial port. (Note that Rx and Tx are swapped at the moment....).
(If you need to program multiple devices it might jusat be easier to modify the defaults in settings.c...)
The UARTT and USB port share one shell instance so typing on both at the same time might yield unpredictable results. (But why would you do that????)
Defaults can be restored by pressing the button at startup. (This is also nessassary if the soruce code is modified to include new settings parameters as the structure in memeory will change and most likely lead to a crash on the next start when the defaults are loaded from flash-memeory...)

## Shell Commands

### help
Lists all the commands currently installed in the system.

### test [msg]
Loopback test to test shell. the string in msg will be printed to the shell

### getaddr
Retuns the current DMX Address configured by the ADDR jumpers

### getmode
Retuns the current mode configured by the MODE jumpers

### dumpadc
Print ADC values for channel A0/A1

### dumpregs
Print out all DMX Registers

### dumpsettings
Print out all Settings

### savesettings
Save Settings to Flash

### setpwm [id (0..2)] [ch (0..3)] [duty (0..65535)]
Set an individual PWM output to a specific value. 
(Direct HW access, can also be controlled by setting DMX Register)

### setbrightness [0..255]
Set the global brightness
(Direct HW access, can also be controlled by setting DMX Register)

### setoffset [ch (0..2)] [offset (-32766...32766)]
Sets an offset for either the Red, Green or Blue channels for the PWM outputs.
Value is an absolute offset for the PWM input.
(Used to calibrate the black level for a color channel)

### setgain [ch (0..2)] [gain (-32766...32766)]
Sets a gain multiplier for either the Red, Green or Blue channels for the PWM outputs.
Value is input / 100 so 100 = 1.0
(Used to calibrate the maximum output for a color channel)

### setgamma [ch (0..3)] [gamma (-32766...32766)]
Sets the gamma either for R,G,B or the strip. 
The gamma tables are stored in FLASH and recreated every time the command is executed.
Gamma is the value/100 so 220 = Gamma of 2.2 (Which is also the default value.)
For the strips gamma is not automatically applied and needs to be handled in the plugin code!

### setreg [reg] [val]
Set a DMX Register to a specific value

### storedefaultregs
Store the current settings of the DMX registers as defaults. These will be loaded and used even if no DMX input signal is present.
This does not store the values to FLASH but copies them to the settings. They will only be permanent AFTER issuing a 'savesettings' command.

### settriggerconfig  [trigger 0|1] [mode(0=None, 1=mapping, 2=switch)] [register (0..25] [level (0..255)]
Configure a trigger to a specific mode and define the DMX register that will be controlled by that trigger.
If the switch-mode is used 'level' defines the threshold level where we switch between the two control settingss

### settriggerctrl  [trigger 0|1] [Reg HiLo (0..25)] [Val HiLo (0..255)] [Reg LoHi (0..25)] [Val LoHi (0..255)]
Sets the control settings for 'switch' trigger mode.
It defines which DMX register will be set to which value when crossing the level set in the 'settriggerconfig' command both when crossing from low to high value and in the other direction.

### setfxmultiplier  [val (1,2,4)]
Sets the fx register multiplier. Currently the number of effects is limiited to 64 so this setup makes it possible to apply a multiplier to the fx register.
With a setting of 1 register value 0 corresponds to fx 0, value 1 to fx 1 and so on.
With a setting of 2 register value 0,1 corresponds to fx 0, value 2,3 to fx 1 and so on.
With a setting of 4 register value 0,1,2,3 corresponds to fx 0, value 4,5,6,7 to fx 1 and so on.
This is useful when using a manual controller with sliders to set the FX as it makes it easier to select one specific effect as it is available on up to 4 codes. (The sliders sometimes don't allow to set all possible values...)

### setstriplength [ch (0..2)] [length (0..1024)]
Sets the length of the WS2812B strips in number of pixels.
A restart is required after changing this setting and storing all settings as the memory is only allocated once during startup.

### setuartmode [0 = Serial, 1 = USB] [0 = Shell, 1 = Serial DMX]
Set the UART mode for the selected UART. storing the settings and a restart are required for these settings to become effective.
On the Serial UART we will still dump out debug messages but only respond to incoming serial DMX messages when selecting Serial DMX.

### switchuartmode [0 = Serial, 1 = USB]
Hard-switch the selected UART to Serial DMX Mode.