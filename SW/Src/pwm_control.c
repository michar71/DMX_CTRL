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
const uint32_t channelinfo[3][4] = {{TIM_CHANNEL_2,TIM_CHANNEL_4,TIM_CHANNEL_3,255},
		                            {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4},
									{TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,255}};




void init_timers(void)
{
	//Start Clocks to timer BLock
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();

	//Init Timers as PWM
	PWM_Timer_Init(PWM_CH1);
	PWM_Timer_Init(PWM_CH2);
	PWM_Timer_Init(PWM_CH3);
}

void PWM_Timer_Init(pwmtimerid_t ID)
{

	  TIM_MasterConfigTypeDef sMasterConfig;
	  TIM_OC_InitTypeDef sConfigOC;
	  TIM_ClockConfigTypeDef sClockSourceConfig;

	  memset(&sConfigOC, 0, sizeof(sConfigOC));
	  memset(&sMasterConfig, 0, sizeof(sMasterConfig));
	  memset(&sClockSourceConfig, 0, sizeof(sClockSourceConfig));

	  switch(ID)
	  {
	  	  case PWM_CH1:
	  		timerinfo[ID].Instance = TIM2;
		  break;
	  	  case PWM_CH2:
	  		timerinfo[ID].Instance = TIM3;
		  break;
	  	  case PWM_CH3:
	  		timerinfo[ID].Instance = TIM1;
		  break;
	  }

	  timerinfo[(uint8_t)ID].Init.Prescaler = 0;
	  timerinfo[(uint8_t)ID].Init.CounterMode = TIM_COUNTERMODE_UP;
	  timerinfo[(uint8_t)ID].Init.Period = 1066; //TBD....
	  timerinfo[(uint8_t)ID].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
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


	  sConfigOC.OCMode = TIM_OCMODE_PWM1;
	  sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
	  sConfigOC.Pulse = MAX_PWM/2; /* 50% duty cycle is 538, set to 0 initially*///
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





//Init GPIO
/*
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_pwm->Instance==TIM3)
  {
    // Peripheral clock enable
    __TIM3_CLK_ENABLE();

    //TIM3 GPIO Configuration
    //PC9     ------> TIM3_CH4
    //PC8     ------> TIM3_CH3
    //PC7     ------> TIM3_CH2
    //PC6     ------> TIM3_CH1

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  }

}
*/


//Setup PWM
void configPWM(pwmtimerid_t ID,pwmchid_t channel, uint16_t duty)
{
	  if (channelinfo[(uint8_t) ID][channel] != 255)
	  {
	  __HAL_TIM_SET_COMPARE(&timerinfo[ID],  channelinfo[(uint8_t)ID][(uint8_t)channel], duty);
	  }
}

void configRGB(uint8_t ch,uint8_t r, uint8_t g, uint8_t b)
{

}
