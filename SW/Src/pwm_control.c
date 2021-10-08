/*
 * pwm_control.c
 *
 *  Created on: Feb 14, 2020
 *      Author: Michael Hartmann
 */

#include <stm32f1xx.h>
#include "pwm_control.h"
#include <stm32f1xx_hal_tim.h>
#include <string.h>
#include "shell.h"


TIM_HandleTypeDef timerinfo[3]; //(CH1 = 0 = htim2,CH2 = 1 = htim3,CH3 = 2 = htim1)

//Channels CH1,CH2,CH3 for RGBW...
const uint32_t channelinfo[3][4] = {{TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4,255},
									{TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,255},
		                            {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4}};
		
//1076

#define MAX_PWM 60000
const uint32_t pwm = MAX_PWM;  //1067
const uint32_t div = TIM_CLOCKDIVISION_DIV1;//TIM_CLOCKDIVISION_DIV1
const uint32_t pre = 2;//1
uint32_t maxPWM = MAX_PWM;

void init_timers(void)
{
	//Start Clocks to timer BLock
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();

	//Init Timers as PWM
	PWM_Timer_Init(PWM_CH1,pre,pwm,div,0);
	PWM_Timer_Init(PWM_CH2,pre,pwm,div,0);
	PWM_Timer_Init(PWM_CH3,pre,pwm,div,0);
}

void PWM_Timer_Init(pwmtimerid_t ID,uint32_t prescaler,uint32_t pwm_max, uint32_t clockdiv, uint32_t reinit)
{

	  TIM_MasterConfigTypeDef sMasterConfig;
	  TIM_OC_InitTypeDef sConfigOC;
	  TIM_ClockConfigTypeDef sClockSourceConfig;

	  memset(&sConfigOC, 0, sizeof(sConfigOC));
	  memset(&sMasterConfig, 0, sizeof(sMasterConfig));
	  memset(&sClockSourceConfig, 0, sizeof(sClockSourceConfig));

	  maxPWM = pwm_max;

	  switch(ID)
	  {
	  	  case PWM_CH1:
	  		timerinfo[ID].Instance = TIM2;
		  break;
	  	  case PWM_CH2:
	  		timerinfo[ID].Instance = TIM1;
		  break;
	  	  case PWM_CH3:
	  		timerinfo[ID].Instance = TIM3;
		  break;
	  }

	  timerinfo[(uint8_t)ID].Init.Prescaler = prescaler;
	  timerinfo[(uint8_t)ID].Init.CounterMode = TIM_COUNTERMODE_UP;
	  timerinfo[(uint8_t)ID].Init.Period = maxPWM-1;
	  timerinfo[(uint8_t)ID].Init.ClockDivision = clockdiv;
	  timerinfo[(uint8_t)ID].Init.RepetitionCounter = 0;
	  if (HAL_TIM_Base_Init(&timerinfo[(uint8_t)ID]) != HAL_OK)/* to use the Timer to generate a simple time base for TIM1 */
	  {
		  print("Timer Init Error 1");
	  }

	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;//the default clock is the internal clock from the APBx, using this function
	  if (HAL_TIM_ConfigClockSource(&timerinfo[(uint8_t)ID], &sClockSourceConfig) != HAL_OK)//Initializes the TIM PWM Time Base according to the specified
	 //parameters in the TIM_HandleTypeDef and create the associated handle.
	   {
		  print("Timer Init Error 2");
	   }


	  if (HAL_TIM_PWM_Init(&timerinfo[(uint8_t)ID]) != HAL_OK)
	  {
		  print("Timer Init Error 3");
	  }

	  if (reinit != 0)
		  return;

	  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	  sConfigOC.Pulse = maxPWM/2; /* 50% duty cycle is 538, set to 0 initially*///
	  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	  for (int ii=0;ii<4;ii++)
	  {
		  if (channelinfo[(uint8_t) ID][ii] != 255)
		  {
			  if (HAL_OK != HAL_TIM_PWM_ConfigChannel(&timerinfo[(uint8_t)ID], &sConfigOC, channelinfo[(int8_t)ID][ii]))
			  {
				  print("Timer Init Error 4");
			  }

		  }
	  }

	 // HAL_TIM_MspPostInit(&timerinfo[(uint8_t)ID]);//output pin assignment
	  HAL_TIM_Base_Start(&timerinfo[(uint8_t)ID]); //Starts the TIM Base generation

	  for (int ii=0;ii<4;ii++)
	  {
		  if (channelinfo[(uint8_t) ID][ii] != 255)
		  {
			  if (HAL_OK != HAL_TIM_PWM_Start(&timerinfo[(uint8_t)ID], channelinfo[(int8_t)ID][ii]))
			  {
				  print("Timer Init Error 5");
			  }

		  }
	  }
}



//Setup PWM
void configPWM(pwmtimerid_t ID,pwmchid_t channel, uint16_t duty)
{
	  if (channelinfo[(uint8_t) ID][channel] != 255)
	  {
	  __HAL_TIM_SET_COMPARE(&timerinfo[ID],  channelinfo[(uint8_t)ID][(uint8_t)channel], duty);
	  }
}


void setPWMdirect(uint8_t id, uint8_t ch, uint16_t val)
{
	configPWM((pwmtimerid_t)id,(pwmchid_t)ch, (uint16_t)val);
}


uint32_t getMaxPWM(void)
{
	return maxPWM;
}
