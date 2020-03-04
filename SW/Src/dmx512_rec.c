/*
 * dmx512_rec.c
 *
 *  Created on: 27.1.2013
 *      Author: honza
 */

#include "dmx512_rec.h"
#include "main.h"
#include <stm32f103xb.h>
#include <stm32f1xx.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_uart.h>
#include <string.h>
#include <sys/_stdint.h>
#include <system_stm32f1xx.h>
#include "dmx512_config.h"

extern UART_HandleTypeDef huart1;
static volatile uint8_t dmx_error = 1;
static volatile uint8_t start_flag = 0;
static enum packet_type packet_type;
static uint8_t buffer;
static volatile uint16_t byte_count = 0;
static volatile uint8_t led_status = GPIO_PIN_RESET;
static volatile uint32_t start_addr = 0;
static volatile uint8_t msg_cnt = 0;
//#define USE _TIMER


//DMX512 implementation
//---------------------

//We start with the simplest of implementation:
//1) We have a UART that triggers IRQ's on every incoming character
//2) We have a timer that resets on every incoming Character (By the UarT IRQ)
//3) If the timer expires after 90ms without receiving a character we interpret that as a start condition
//4) The next 512 bytes after a start condition on the UART are interpreted as a valid DMX512 packet
//5) The first byte is interpreted as the Start Code (Packet identifier)
//6) Every UART Failure resets the start condition
//7) Data is not double buffered and no indication is given that data has changed. (Assumes constant 44 FPS Packet-Stream)


//For more details regarding DMX512 Timing see
//http://www.erwinrol.com/page/articles/dmx512/

//Missing Features
//Event at end of data
//DMX512 Master Support



//On Timer Interrupt
#ifdef USE_TIMER
void TIM1_CC_IRQHandler(void)
{
	uint32_t break_time, mab_time;

	//Process Timer events
	HAL_TIM_IRQHandler(&tim1);

	//Clear IRQ
	TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);

	//Break Time is the time that has expired since starting the counter
	break_time = TIM_GetCapture2(TIM1);

	//Mark after Break (MAB) Time is the time after the timer started minus the break time
	mab_time = TIM_GetCapture1(TIM1) - break_time;

	// Find Start Condition
	// use some tolerance for times
	if (break_time > 80 && break_time < 10e3 && mab_time > 8)
	{
		//set Start Flag for UART IRQ Routine
		start_flag = 1;
		//Disable Timer
		TIM_ITConfig(TIM1, TIM_IT_CC1, DISABLE);
	}
}
//END TODO
#endif

/* UART1 Interrupt Service Routine */
void USART1_IRQHandler(void)
{
	//Get the received byte and clear Frame Error IRQ flag
	//On Framing Error restart Timer to search for a start condition
	if (__HAL_UART_GET_FLAG(&huart1,UART_FLAG_FE))
	{
		__HAL_UART_CLEAR_FEFLAG(&huart1);
#ifdef USE_TIMER
		TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
#else
		start_flag = 1;
#endif
		HAL_UART_IRQHandler(&huart1);
		return;
	}
	HAL_UART_IRQHandler(&huart1);
}

//UART IRQ Triggered by incoming characters
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	volatile uint8_t rx_byte;

	if (huart->Instance == USART1)
    {
		rx_byte = buffer;

		//Timer has signaled a start flag.
		//We can start to capture data.
		if (start_flag)
		{
			//On every incoming start packet we invert the LED to signal incoming data
			msg_cnt++;
			if (msg_cnt == 44)
			{
				msg_cnt = 0;
				led_status =!led_status;
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, led_status);
			}

			//Reset Variables
			dmx_error = 0;
			byte_count = 0;
			start_flag = 0;

			/* first byte determines packet type */
			switch (rx_byte)
			{
			case 0x0:
				packet_type = DATA_PACKET;
				break;
			case 0x17:
				packet_type = TEST_PACKET;
				break;
			default:
				//Anything else we declare an error....
				dmx_error = 1;
				//Restart Timer to search for a start condition
	#ifdef USE_TIMER
				TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);
	#endif
				//Get more data
				HAL_UART_Receive_IT(&huart1, &buffer, 1);
				return;
			}
		}

		//Determine if the data is for us and we need to store it
		if (packet_type == DATA_PACKET)
		{
			if (byte_count >= start_addr && byte_count < start_addr + get_reg_length())
				set_reg(byte_count - start_addr-1, rx_byte);
		}
		//Increase Counter
		byte_count++;

		//Get more data
		HAL_UART_Receive_IT(&huart1, &buffer, 1);
    }
}



/*
 * configures timer for detecting BREAK and MAB
 * UART for dmx512 data
 */
void dmx512_rec_init()
{
#ifdef USE_TIMER
	TIM_HandleTypeDef htim1;

	htim1.Instance =TIM1;
	htim1.Init.Period = 0xFFFF;
	htim1.Init.Prescaler = SystemCoreClock/1e6 - 1; /* timer tics are in us */
	htim1.Init.ClockDivision = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim1);


	TIM_IC_InitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	TIM_ICInitStructure.ICFilter = 0x0;
	TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    HAL_TIM_IC_ConfigChannel(&htim1, &TIM_ICInitStructure,TIM_CHANNEL_1);


    TIM_SlaveConfigTypeDef slaveConfig;

    slaveConfig.InputTrigger = TIM_TS_TI1FP1;
	slaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
	HAL_TIM_SlaveConfigSynchro(&htim1, &slaveConfig);

	//Start Timer
	HAL_TIM_Base_Start(&htim1);
#endif
	dmx512_rec_enable(1);
}



void dmx512_setRx(void)
{
	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);
    HAL_HalfDuplex_EnableReceiver(&huart1);
}

void dmx512_setTx(void)
{
	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	HAL_HalfDuplex_EnableTransmitter(&huart1);
}

void dmx512_rec_enable(uint8_t on)
{
	if (on)
	{
		//Determine Start Addr
		start_addr = get_addr();

		//Set Receiver to input
		dmx512_setRx();
		//Enable UART IRQ
    	HAL_UART_Receive_IT(&huart1, &buffer, 1);
		//Enable Timer
#ifdef USE_TIMER
		TIM_ITConfig(dmx512_config.tim, TIM_IT_CC1, ENABLE);
#endif
	}
	else
	{
		//Disable UART
		__HAL_UART_DISABLE(&huart1);
		//Disable Timer
#ifdef USE_TIMER
		TIM_ITConfig(dmx512_config.tim, TIM_IT_CC1, DISABLE);
#endif
	}
}
