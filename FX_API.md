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

### Plugin ID
The Plugin ID is the number that is used to activate it in the DMX FX_SELECT register. When plugins are registered they can either be registered with an absolute ID or ID = 0 which will add them to the next available spot.
If a plugin is registered with the same ID as an existing plugin it will overwrite that plugin.
Plugin ID 0 is special as it holds a "NULL" plugin that will not do anything.
(If plugins want to turn of the lights at the end of execution they either need to set all content to off/0 on termination or call another plugin that disables the lights)

### Plugin init structure
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

### Plugin Type
Plugins can be one of 3 different type:
- PWM Control Omly
- Strip Control Only
- Both

'''
//Types of Effects
typedef enum{
	PWM_FX,			//An effect that controls the PWM outputs
	STRIP_FX,		//An Effect that controls the LED Strips
	PWM_STRIP_FX	//An Effect that controls both
}t_fx_type;
'''

### Plugin Mode
Plugins can be one of three different modes:
- Continous -> The Plugin will run forever unless the user selects a different effect
- Single Shot -> The plugin will run once for the frames defined in 'duration' and then extecute the 'next_fx' plugin or fall back to the last one
- Loop Mode -> The plugin will run for the frames defined in 'duration' but then start again after running through the INIT sequence. (Careful with allocating memeory in the INIT sequence so memeory does't get allcoated every time...)

'''
//Type of Modes
typedef enum{
	MODE_CONTINOUS,  	//Effect runs until some other effect is selected
	MODE_SINGLE_SHOT,	//Effect Runs once for duration and returns to the last effect
	MODE_LOOP 			//Effect Runs for duration and then INIT State is called again once
}t_fx_mode;
'''

### Plugin next_fx
The 'next_fx' Plugin parameter determines what plugin will be activated either after the 'duration' parameter expires or the plugin self-terminates.

### Plugin fxname
The anme field can store up to 8 characters for the name of the plugin. (Mostly used for debugging)

### Plugin Duration
The 'duration' field holds the duration the plugin is supposed to run in number of frames. At the end of the duration the plugin will terminate and either fall back to the last plugin or the plugin defined in 'next_fx'.

### Plugin Run Function
The Run function will be called one at the start of the plugin, for every succesive frame and once at the end.

The function call looks like this:
'''
t_fx_result fx_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
'''
And the function is part of the plugin defnition structure.

The state is one of the following states:

'''
//State for the current function call. It is guaranteed that
//FX_INIT will be executed once on start and FX_END once effect ends.
typedef enum{
	FX_INIT,
	FX_RUN,
	FX_END,
	FX_DONE
}t_fx_state;
'''

### Plugin Error Codes
The plugin run routine needs to return one of the following erro codes:
'''
//Type of Return Values
typedef enum{
	FX_OK,
	FX_COMPLETED,
	FX_RUNNING,
	FX_ERROR
}t_fx_result;
'''
If a plugin wants to self-terminate it has to return a FX_COMPLETED errorcode.

### Self-Terminating Plugins
Plugins can end operation either because the user selects a different plugin, the duration has expired in single-shot mode or the plugin self-termines by returning the FX_COMPLETED errorcode.
After that code is returned the plugin will be called once more with the FX_END state.

## Plugin API
To simplify development all nessassary header files and helper routines are accesible via the fx_api.h header file.

### Access to DMX Registers
DMX Register values should be accessed through the 'get_DMX_variable' function.
Register definitions are provided in the fx_api.h file.
To optimize code register mappings as arrays have been provided to more efficiently access registers, for example:
'''
    //Loop through both channels
    for (uint8_t ii = 0;ii<CH_MAX;ii++)
    {
        step[ii] = get_DMX_variable(DMX_CH_REG[ii][DMX_STRIP_SIZE]);
    }
'''
If the PWM DMX registers are modified while the plugin is running those values might overwrite any changes made by the Plugin! If that is an issue it might be better to set the value of the DMX PWM registers directly through the '' function call.

### Access to PWM Controls
PWM Channels can either be accessed as individual channels or RGB channels with the following functions:
'''
void set_pwm_light(uint8_t index, uint8_t val);
void set_pwm_timer_channel(uint8_t ch, uint8_t r, uint8_t g, uint8_t b);
'''
Additionally PWM parameters can also be changed 

### Access to WS2812B Strips
Access to the WS2812B strips is provided through the following functions:
'''
void WS2812B_setPixelColor(t_stripchannel ch,uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_setPixelColorDirect(t_stripchannel ch,uint16_t n, uint32_t c);
void WS2812B_setBrightness(t_stripchannel ch,uint8_t brightness);
void WS2812B_clear(t_stripchannel ch);
uint8_t WS2812B_getBrightness(t_stripchannel ch);
uint16_t WS2812B_numPixels(t_stripchannel ch);
uint32_t WS2812B_Color(uint8_t r, uint8_t g, uint8_t b);
'''
These are equivalent to the Adafruit Led Library function calls.

### Overall Brightness
Overall Brightness can be set independently for PWM and Strips through:
'''
//Set Overall Brightness
void set_pwm_brightness(uint8_t val);
void set_strip_brightness(uint8_t val);
'''
(If both PWM and Strips need to be linked to the MAX_BRIGHTNESS register it needs to be set in the plugin.)

### Access to System Hardware
- Plugins can access the Analog Trigger Values from A0/A1 inputs through the 'get_tigger_val' FX_API function call. 
- Plugins can access the System Timer through the 'millisec' FX_API function call. 

## Plugin Example
'''
//Include what's nessassary here
#include "fx_api/fx_api.h"
#include "fx/fx_test.h"

static const s_fx_param param = {PWM_FX,MODE_CONTINOUS,0,"PRUN",0,fx_run};

//This gets called form fx_install.h
void fx_install(uint8_t fx_num)
{
	//Register effect
	register_fx(&param1, fx_num);
}

//This is the main run routine for the plugin. Everything happenes here...
t_fx_result fx_run(t_fx_state state,uint32_t framecount,const uint32_t duration)
{
	switch(state)
	{
		case FX_INIT:
			//Plugin insitalisation like zeroing out variables, allocating memeory or setting up HW shoudl happen here
			return FX_OK;
		case FX_RUN:
			//Execiting stuff like controlling the lights should happen here.
			return FX_RUNNING;
		case FX_END:
			//Deallocate memeory and clean up here...
			return FX_COMPLETED;
		case FX_DONE:
            //This should never happen...
			break;
	}
	return FX_ERROR;
}
'''