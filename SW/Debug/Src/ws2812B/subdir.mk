################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ws2812B/WS2812B.c 

OBJS += \
./Src/ws2812B/WS2812B.o 

C_DEPS += \
./Src/ws2812B/WS2812B.d 


# Each subdirectory must supply rules for building sources it contributes
Src/ws2812B/WS2812B.o: ../Src/ws2812B/WS2812B.c
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/ws2812B/WS2812B.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

