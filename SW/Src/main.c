/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#ifdef SUPPORT_USB
#include "usb_device.h"
#endif
#include "dmx512_rec.h"
#include "dmx512_config.h"
#include "serial_tracer.h"
#include "shell.h"
#include "gpio_control.h"
#include "pwm_control.h"
#include "light_update.h"
#include "settings.h"
#include "triggers.h"
#include "testmode.h"
#include "fx_manager.h"
#include "fx_list.h"
#include "WS2812B/WS2812B.h"
#include "serial_dmx_parser.h"
#include "ring_buffer.h"
#include "gammactrl.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi2_tx;

/* USER CODE BEGIN PV */
uint8_t testmode = 0;

uint8_t UART_mode_SERIAL = 0;
uint8_t UART_mode_USB = 0;
uint8_t USB_Active = 0;

uint32_t frame_ms;

//UART Ringbuffer
#define RX_BUFF_SIZE 128
rb_att_t rx_buff_dmx;
rb_att_t rx_buff_shell;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_DMA_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t last_frame_ms;
  uint32_t current_frame_ms;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //Turn LED for Test
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  MX_ADC1_Init();
  MX_ADC2_Init();
#ifdef ENABLE_WATCHDOG
  MX_IWDG_Init();
#endif
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_DMA_Init();


  /* USER CODE BEGIN 2 */
  //Wait some time otherwise some boards screw up at startup and don't read back the config pins correctly...
  for (uint32_t w = 0;w<5000;w++)
      print_no_newline(" ");

  //Dump Startup info
  print("");
  print("-------------");
  print("%s V%d.%d",PROG_NAME,VERSION_MAJOR,VERSION_MINOR);
  print("-------------");
  print("");

 if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) == SET)
 {
	 print("Restart due to IWDG Reset");
  	  __HAL_RCC_CLEAR_RESET_FLAGS();
 }


 //Set Mode/Address from Address Pins
 dmx512_init(get_mode_from_pins(),get_addr_from_pins()*2);  //Address x2 so we can spread out the 8 Jumpers over 512 Addresses
 print("DMX512 Config complete");


#ifdef SUPPORT_USB
 if ((get_mode_from_pins() & 0x02) != 0)
 {
	 USB_Active = 1;
	 MX_USB_DEVICE_Init();
	 print("USB Enabled");
 }
 else
 {
	 print("USB Disabled");
 }
#else
 print("USB Hard Disabled");
#endif

 if(ring_buffer_init(&rx_buff_shell, RX_BUFF_SIZE) != RB_OK)
	 print("Shell Ring Buffer Init FAILED");
 else
	 print("Shell Ring Buffer Init Complete");

 if(ring_buffer_init(&rx_buff_dmx, RX_BUFF_SIZE) != RB_OK)
	 print("Serial DMX Ring Buffer Init FAILED");
 else
	 print("Serial DMX Ring Buffer Init Complete");

 //Setup UART1 (RS485)/DMX512 Receiver
 dmx512_rec_init();
 print("DMX512 Init complete");

  //Init/Setup PWM for Lights
  init_timers();
  print("Timer Init complete");
      
  //Init Defaults
  init_settings();
  print("Default Settings Initialized");

  // If Button is not pressed Load Defaults
  print("Loading Settings...");
  if (0 == check_button())
  {
	  if (false == load_settings())
	  {
		  //Set Default Gamma Table
		  init_settings();
		  recalcGamma();
		  save_settings();
		  print("Defaults created");
	  }

	  UART_mode_SERIAL = settings.UART_Mode_UART;
	  UART_mode_USB = settings.UART_Mode_USB;
	  print("Setting loaded");
  }
  else
  {
	  //Set Default Gamma Table
	  recalcGamma();
	  save_settings();
	  print("Defaults restored");
  }

  //Set Defaults
  apply_settings();

  print("Settings complete");

  if (DMX_MODE2 == get_mode())
  {
	  print("FX Installation Start");
	  init_fx_list();
	  install_fx();
	  print("FX Installation Finished");

     if (WS2812B_init(CH1,settings.strip1_length))
     {
	   print("WS2812B CH1 Init complete");
	   WS2812B_clear(CH1);
	   WS2812B_show(CH1);
     }
	 else
	 {
	   print("WS2812B CH1 Init FAILED");
	 }

     if (WS2812B_init(CH2,settings.strip1_length))
     {
	   print("WS2812B CH2 Init complete");
	   WS2812B_clear(CH2);
	   WS2812B_show(CH2);
     }
	 else
	 {
	   print("WS2812B CH2 Init FAILED");
	 }
     print("DMX Mode 2 Active");
  }
  else
  {
	  print("DMX Mode 1 Active");
  }

  init_update_lights();
  print("PWM Update Init complete");

  init_trigger();
  print("Trigger Init complete");

  print("Shell Active");

  //Shell is ready....
  print_no_newline("DBG>");

  //Get first frame time
  last_frame_ms = HAL_GetTick();

  //Turn LED off at completion of init
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	//Handle Test Shell and DMX Serial Process
	shell_process();
	dmx_serial_process();

	//Try to produce a constant framerate...
	current_frame_ms = HAL_GetTick();
	if (settings.frame_ms_target > 0)
	{
		//Loop until time for frame has elapsed...
		while ((current_frame_ms - last_frame_ms) < settings.frame_ms_target)
		{
			shell_process();
			dmx_serial_process();
			current_frame_ms = HAL_GetTick();
		}
	}

	//Calulate framerate
	frame_ms = current_frame_ms - last_frame_ms;
	last_frame_ms = current_frame_ms;

  //Check for Comm Timeout
  if ((COMM_TIMEOUT != 0) && (frame_ms > (last_packet_ms + COMM_TIMEOUT))) 
  {
    //Lets reset Registers to defaults....
    apply_settings();
  }

	if (testmode)
	{
		testmode = process_testmode();
		//Restore settings
		if (testmode == 0)
		{
			update_pwm_lights(1);
			if (DMX_MODE2 == get_mode())
			{
				WS2812B_clear(CH1);
				WS2812B_show(CH1);
				WS2812B_clear(CH2);
				WS2812B_show(CH2);
			}
		}
		else
		{
			if (DMX_MODE2 == get_mode())
			{
				WS2812B_test(CH1);
				WS2812B_show(CH1);
				WS2812B_test(CH2);
				WS2812B_show(CH2);
			}
		}
	}
	else
	{
		//Button pushed
		if (1 == check_button())
		{
			testmode = 1;
		}

		//	Read ADC and Handle Controls/Triggers
		process_trigger();
		//  Run Effect Programs if DMX Mode 2 is enabled
		if (DMX_MODE2 == get_mode())
		{
			run_fx();
			//Update WS2812 Data
			//TBD Should this be done from Here by
			//1) calling Show every Frame
			//Or
			//2) from inside the FX only when it is needed?
			//Going with option 1 for the moment. as the timing is more predictable...
			WS2812B_show(CH1);
			WS2812B_show(CH2);
		}

		//  Set PWM Lights
		update_pwm_lights(0);
		update_fx();
	}

	//  Reset Watchdog
#ifdef ENABLE_WATCHDOG
	HAL_IWDG_Refresh(&hiwdg);
#endif

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{
  /* Peripheral clock enable */
  __HAL_RCC_SPI1_CLK_ENABLE();

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
	  Error_Handler();
  }
}

/* SPI1 init function */
static void MX_SPI2_Init(void)
{
  /* Peripheral clock enable */
  __HAL_RCC_SPI2_CLK_ENABLE();

  /* SPI1 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
	  Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

//-----------------------------------
//RS485 UART for DMX512 communication
//-----------------------------------

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 250000;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_2;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  /* Peripheral interrupt init*/
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE END USART1_Init 2 */
}

//-------------------------------------------------
//TTL 3.3V UART for Debugging or connect to ESP8266
//-------------------------------------------------

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  GPIO_InitStruct.Pin = RS485_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_DIR_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin MODE_0_Pin MODE_1_Pin */
  GPIO_InitStruct.Pin = MODE_0_Pin|MODE_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CH1_R_Pin CH1_G_Pin CH1_B_Pin CH2_R_Pin 
                           CH2_G_Pin CH3_R_Pin CH3_G_Pin CH3_B_Pin */
  GPIO_InitStruct.Pin = CH1_R_Pin|CH1_G_Pin|CH1_B_Pin|CH2_R_Pin 
                          |CH2_G_Pin|CH3_R_Pin|CH3_G_Pin|CH3_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CH2_B_Pin CH2_W_Pin */
  GPIO_InitStruct.Pin = CH2_B_Pin|CH2_W_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_Pin ADDR_0_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin|ADDR_0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ADDR_5_Pin ADDR_6_Pin ADDR_7_Pin ADDR_1_Pin 
                           ADDR_2_Pin ADDR_3_Pin ADDR_4_Pin */
  GPIO_InitStruct.Pin = ADDR_5_Pin|ADDR_6_Pin|ADDR_7_Pin|ADDR_1_Pin 
                          |ADDR_2_Pin|ADDR_3_Pin|ADDR_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : WS8212_CH2_Pin WS8212_CH1_Pin */
  GPIO_InitStruct.Pin = WS8212_CH2_Pin|WS8212_CH1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_DIR_Pin */
  GPIO_InitStruct.Pin = RS232_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_DIR_GPIO_Port, &GPIO_InitStruct);

  /*Configure peripheral I/O remapping */
  __HAL_AFIO_REMAP_SPI1_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
