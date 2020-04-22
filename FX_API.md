# FX Plugin Development
DMX CTRL support custom Effects plugin development both for the PWM controls and the WS2812B strip control. (It's actually the only way to add more/new effects..)
The following document outlines the general process of developing a plugin. 
As plugins are just source code they can pretty much do whatever they want but following these guidelines probably means they will also work.
As plugins are only source code they share the same memeory as the genral code so care needs to be taken to reduce memeory usage and evaluate if code might overflow the stack.
As all Plugin Code is instantiated at startup it should rely as little as possible on statically allocated RAM. 
To simplify development an API is provided to access system ressources. It is not nessassary to use this API but it simplifies things....
No comprehensive framework for LED Graphic effects is provided as it really depends on what a plugin will do. It's easier to copy from other code bases and reuse in an FX then try to make those libraries work with this codebase specifically as this codebase is pure C and a lot of arduino routines/libraries are C++.
Plugins that control the PWM Light override the settings programmed via DMX. (They should actually use the FX_API functions to set the DMX Registers directly as those values are copied to the lights after the plugin RUN call has been completed.)

## Plugin Framework
Plugins relay on two function calls:
- A Register Function call to register the plugin
- A Run Function call that is called periodically

No guarantee is given that the Run Function will be called at specic or even time intervals. (It depends on incoming DMX data, interrupts, the length of the WS2812B strips and other factors.) but the code is arranged in a way to keep calsl as regular as possible. For most effects that will be sufficient.
If a more accurate time base is needed (For example to calculate speed and velocity of LED lights in a LED strip) a function is Provided in the FX_API to get the time in milliseconds and calulate time intervals based on that.

## Plugin Installation
### Registering Plugins
Plugins need to be registered so the firmware can keep track of them. Registration is static you cannot add plugins during runtime.
To register a plugin it's header file needs to be included in 'fx_install.h" and it's registration routine added to the 'fx_install' function call.

#### Plugin ID
The Plugin ID is the number that is used to activate it in the DMX FX_SELECT register. When plugins are registered they can either be registered with an absolute ID or ID = 0 which will add them to the next available spot.
If a plugin is registered with the same ID as an existing plugin it will overwrite that plugin.
Plugin ID 0 is special as it holds a "NULL" plugin that will not do anything.
(If plugins want to turn of the lights at the end of execution they either need to set all content to off/0 on termination or call another plugin that disables the lights)

#### Plugin init structure
In the plugin intialisation routine the plugin needs to pass over an init structure to the underlying plugin list manager.

The structure is declasred as this:
'''
//Effect Parameters
typedef struct{
	t_fx_type type;
	t_fx_mode mode;
	uint8_t	  next_fx;	//Next effect if duration != 0. If set to 0 turns off effects
	char      fxname[9];
	uint32_t  duration;   //Duration in Frames. If 0 runs continuously otherwise advances to next effect after x frames
	fx_run_t fx_run_pointer;
}s_fx_param;
'''

The structure should be declared as const so no RAM is used.
'''
static const s_fx_param param = {PWM_FX,MODE_CONTINOUS,0,"PPULSE",0,fx_pwm_pulsing_light_run};

void fx_pwm_pulsing_light(uint8_t fx_num)
{
	//Register effect
	register_fx(&param,fx_num);
}
'''

#### Plugin Type

#### Plugin Mode

#### Plugin next_fx
The 'next_fx' Plugin parameter determines what plugin will be activated either after the 'duration' parameter expires or the plugin self-terminates.

#### Plugin fxname
The anme field can store up to 8 characters for the name of the plugin. (Mostly used for debugging)

#### Plugin Duration
The 'duration' field holds the duration the plugin is supposed to run in number of frames. At the end of the duration the plugin will terminate and either fall back to the last plugin or the plugin defined in 'next_fx'.

#### Plugin Run Function

### Plugin Error Codes
### Self-Terminating Plugins


## Plugin API
## Plugin Example

## Notes
- Plugins can access the Analog Trigger Values from A0/A1 inputs through a FX_API function call. 