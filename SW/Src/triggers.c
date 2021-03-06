/*
 * triggers.c
 *
 *  Created on: Mar 5, 2020
 *      Author: Michael Hartmann
 */

#include "triggers.h"
#include "settings.h"
#include "dmx512_config.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "shell.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;


#define ADC_BUF_SIZE 5
typedef struct{
	uint16_t adc_val;
	uint16_t last_adc_val;
	uint16_t buf[ADC_BUF_SIZE];
	uint8_t bufcnt;
}adcdata_s;

adcdata_s adcdata[2];
ADC_HandleTypeDef* currentADCHandler = &hadc1;

void ADC1_2_IRQHandler()
{
	//need to figure out where IRQ comes from here....
	HAL_ADC_IRQHandler(currentADCHandler);

    //Alternatively we could call the handler for both sources and let the handler routine sort it out....
    //HAL_ADC_IRQHandler(&hadc1);
    //HAL_ADC_IRQHandler(&hadc2);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	uint8_t adc = 0;
	uint32_t sum = 0;
	uint32_t adcval = 0;

	if (hadc->Instance == ADC2)
	{
		adc = 1;
	}

	adcval = HAL_ADC_GetValue(hadc);
	adcval = adcval / 16; //Convert to 8-bit...
	adcdata[adc].buf[adcdata[adc].bufcnt] = (uint16_t)adcval;
	adcdata[adc].bufcnt++;

	//Calculate average over x samples to reduce noise
	if (ADC_BUF_SIZE == adcdata[adc].bufcnt)
	{
		do
		{
			adcdata[adc].bufcnt--;
			sum = sum + adcdata[adc].buf[adcdata[adc].bufcnt];
		}
		while(adcdata[adc].bufcnt > 0);
		adcdata[adc].adc_val = (uint16_t)(sum / ADC_BUF_SIZE);
	}

	//swap ADC
	if (hadc->Instance == ADC1)
		currentADCHandler = &hadc2;
	else
		currentADCHandler = &hadc1;
	//Start ADC Again
    HAL_ADC_Start_IT(currentADCHandler);
}


void init_trigger(void)
{
	//Clear data
	for (uint8_t ii=0;ii<2;ii++)
		memset(&adcdata[ii], 0, sizeof(adcdata_s));

	//Eanble IRQ's
    HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	//Setup IRQ's
	HAL_ADC_Start_IT(currentADCHandler);
}

void process_trigger(void)
{
	//check if ADC Value has changed
	if ((settings.trigger_mode_A0 != TRIGGERMODE_NONE) && (adcdata[0].adc_val != adcdata[0].last_adc_val))
	{
		//Based on Settings manipulate Registers
		switch(settings.trigger_mode_A0)
		{
			case TRIGGERMODE_MAPPING:
				set_reg(settings.trigger_reg_A0, adcdata[0].adc_val);
				break;

			case TRIGGERMODE_SWITCH:
				//Low->Hi
				if ((adcdata[0].adc_val > settings.trigger_level_A0) && (adcdata[0].last_adc_val < settings.trigger_level_A0))
				{
					set_reg(settings.trigger_reg_lohi_A0, settings.trigger_val_lohi_A0);
				}
				else if ((adcdata[0].adc_val <settings. trigger_level_A0) && (adcdata[0].last_adc_val > settings.trigger_level_A0))
				{
					set_reg(settings.trigger_reg_hilo_A0, settings.trigger_val_hilo_A0);
				}

				break;
		}
		//Store Last Value
		adcdata[0].last_adc_val = adcdata[0].adc_val;
	}

	//check if ADC Value has changed
	if ((settings.trigger_mode_A1 != TRIGGERMODE_NONE) && (adcdata[1].adc_val != adcdata[1].last_adc_val))
	{
		//Based on Settings manipulate Registers
		switch(settings.trigger_mode_A1)
		{
			case TRIGGERMODE_MAPPING:
				set_reg(settings.trigger_reg_A1, adcdata[1].adc_val);
				break;

			case TRIGGERMODE_SWITCH:
				//Low->Hi
				if ((adcdata[1].adc_val > settings.trigger_level_A1) && (adcdata[1].last_adc_val < settings.trigger_level_A1))
				{
					set_reg(settings.trigger_reg_lohi_A1, settings.trigger_val_lohi_A1);
				}
				else if ((adcdata[1].adc_val < settings.trigger_level_A1) && (adcdata[1].last_adc_val > settings.trigger_level_A1))
				{
					set_reg(settings.trigger_reg_hilo_A1, settings.trigger_val_hilo_A1);
				}

				break;
		}
		//Store Last Value
		adcdata[1].last_adc_val = adcdata[1].adc_val;
	}
}


uint8_t get_trigger(uint8_t ch)
{
	if ((ch == 0) || (ch == 1))
		return adcdata[ch].adc_val;
	else
		return 0;
}

void print_adc_data(void)
{
	for (int ii=0;ii<2;ii++)
	{
		print("ADC %d",ii+1);
		print("-------");
		print("ADC VAL: %d",adcdata[ii].adc_val);
		print("ADC LAST VAL: %d",adcdata[ii].last_adc_val);
		print("ADC BUFCNT: %d",adcdata[ii].bufcnt);
		print("");
	}

}
