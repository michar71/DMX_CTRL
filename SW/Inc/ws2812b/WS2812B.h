#ifndef WS2812B_H
#define WS2812B_H

#include <stm32f1xx.h>
#include <stm32f1xx_hal_spi.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct{
	uint32_t time;
	uint16_t numLEDs;       // Number of RGB LEDs in strip
	uint16_t numBytes;      // Size of 'pixels' buffer
	uint8_t brightness;
	uint8_t *pixels;        // Holds the current LED color values, which the external API calls interact with 9 bytes per pixel + start + end empty bytes
	uint8_t *doubleBuffer;	// Holds the start of the double buffer (1 buffer for async DMA transfer and one for the API interaction.
	SPI_HandleTypeDef* phspi;
	DMA_HandleTypeDef* phdma_spi_tx;
}t_ws2812B;


typedef enum{
	CH1 = 0,
	CH2 = 1,
	CH_MAX = 2
}t_stripchannel;

t_ws2812B stripchannel[CH_MAX];

// New version uses one large LUT as its faster index into sequential bytes for the GRB pattern
static const uint8_t encoderLookup[256*3]={	0x92,0x49,0x24,0x92,0x49,0x26,0x92,0x49,0x34,0x92,0x49,0x36,0x92,0x49,0xA4,0x92,0x49,0xA6,0x92,0x49,0xB4,0x92,0x49,0xB6,0x92,0x4D,0x24,0x92,0x4D,0x26,0x92,0x4D,0x34,0x92,0x4D,0x36,0x92,0x4D,0xA4,0x92,0x4D,0xA6,0x92,0x4D,0xB4,0x92,0x4D,0xB6,0x92,0x69,0x24,0x92,0x69,0x26,0x92,0x69,0x34,0x92,0x69,0x36,0x92,0x69,0xA4,0x92,0x69,0xA6,0x92,0x69,0xB4,0x92,0x69,0xB6,0x92,0x6D,0x24,0x92,0x6D,0x26,0x92,0x6D,0x34,0x92,0x6D,0x36,0x92,0x6D,0xA4,0x92,0x6D,0xA6,0x92,0x6D,0xB4,0x92,0x6D,0xB6,0x93,0x49,0x24,0x93,0x49,0x26,0x93,0x49,0x34,0x93,0x49,0x36,0x93,0x49,0xA4,0x93,0x49,0xA6,0x93,0x49,0xB4,0x93,0x49,0xB6,0x93,0x4D,0x24,0x93,0x4D,0x26,0x93,0x4D,0x34,0x93,0x4D,0x36,0x93,0x4D,0xA4,0x93,0x4D,0xA6,0x93,0x4D,0xB4,0x93,0x4D,0xB6,0x93,0x69,0x24,0x93,0x69,0x26,0x93,0x69,0x34,0x93,0x69,0x36,0x93,0x69,0xA4,0x93,0x69,0xA6,0x93,0x69,0xB4,0x93,0x69,0xB6,0x93,0x6D,0x24,0x93,0x6D,0x26,0x93,0x6D,0x34,0x93,0x6D,0x36,0x93,0x6D,0xA4,0x93,0x6D,0xA6,0x93,0x6D,0xB4,0x93,0x6D,0xB6,0x9A,0x49,0x24,0x9A,0x49,0x26,0x9A,0x49,0x34,0x9A,0x49,0x36,0x9A,0x49,0xA4,0x9A,0x49,0xA6,0x9A,0x49,0xB4,0x9A,0x49,0xB6,0x9A,0x4D,0x24,0x9A,0x4D,0x26,0x9A,0x4D,0x34,0x9A,0x4D,0x36,0x9A,0x4D,0xA4,0x9A,0x4D,0xA6,0x9A,0x4D,0xB4,0x9A,0x4D,0xB6,0x9A,0x69,0x24,0x9A,0x69,0x26,0x9A,0x69,0x34,0x9A,0x69,0x36,0x9A,0x69,0xA4,0x9A,0x69,\
											0xA6,0x9A,0x69,0xB4,0x9A,0x69,0xB6,0x9A,0x6D,0x24,0x9A,0x6D,0x26,0x9A,0x6D,0x34,0x9A,0x6D,0x36,0x9A,0x6D,0xA4,0x9A,0x6D,0xA6,0x9A,0x6D,0xB4,0x9A,0x6D,0xB6,0x9B,0x49,0x24,0x9B,0x49,0x26,0x9B,0x49,0x34,0x9B,0x49,0x36,0x9B,0x49,0xA4,0x9B,0x49,0xA6,0x9B,0x49,0xB4,0x9B,0x49,0xB6,0x9B,0x4D,0x24,0x9B,0x4D,0x26,0x9B,0x4D,0x34,0x9B,0x4D,0x36,0x9B,0x4D,0xA4,0x9B,0x4D,0xA6,0x9B,0x4D,0xB4,0x9B,0x4D,0xB6,0x9B,0x69,0x24,0x9B,0x69,0x26,0x9B,0x69,0x34,0x9B,0x69,0x36,0x9B,0x69,0xA4,0x9B,0x69,0xA6,0x9B,0x69,0xB4,0x9B,0x69,0xB6,0x9B,0x6D,0x24,0x9B,0x6D,0x26,0x9B,0x6D,0x34,0x9B,0x6D,0x36,0x9B,0x6D,0xA4,0x9B,0x6D,0xA6,0x9B,0x6D,0xB4,0x9B,0x6D,0xB6,0xD2,0x49,0x24,0xD2,0x49,0x26,0xD2,0x49,0x34,0xD2,0x49,0x36,0xD2,0x49,0xA4,0xD2,0x49,0xA6,0xD2,0x49,0xB4,0xD2,0x49,0xB6,0xD2,0x4D,0x24,0xD2,0x4D,0x26,0xD2,0x4D,0x34,0xD2,0x4D,0x36,0xD2,0x4D,0xA4,0xD2,0x4D,0xA6,0xD2,0x4D,0xB4,0xD2,0x4D,0xB6,0xD2,0x69,0x24,0xD2,0x69,0x26,0xD2,0x69,0x34,0xD2,0x69,0x36,0xD2,0x69,0xA4,0xD2,0x69,0xA6,0xD2,0x69,0xB4,0xD2,0x69,0xB6,0xD2,0x6D,0x24,0xD2,0x6D,0x26,0xD2,0x6D,0x34,0xD2,0x6D,0x36,0xD2,0x6D,0xA4,0xD2,0x6D,0xA6,0xD2,0x6D,0xB4,0xD2,0x6D,0xB6,0xD3,0x49,0x24,0xD3,0x49,0x26,0xD3,0x49,0x34,0xD3,0x49,0x36,0xD3,0x49,0xA4,0xD3,0x49,0xA6,0xD3,0x49,0xB4,0xD3,0x49,0xB6,0xD3,0x4D,0x24,0xD3,0x4D,0x26,0xD3,0x4D,0x34,0xD3,\
											0x4D,0x36,0xD3,0x4D,0xA4,0xD3,0x4D,0xA6,0xD3,0x4D,0xB4,0xD3,0x4D,0xB6,0xD3,0x69,0x24,0xD3,0x69,0x26,0xD3,0x69,0x34,0xD3,0x69,0x36,0xD3,0x69,0xA4,0xD3,0x69,0xA6,0xD3,0x69,0xB4,0xD3,0x69,0xB6,0xD3,0x6D,0x24,0xD3,0x6D,0x26,0xD3,0x6D,0x34,0xD3,0x6D,0x36,0xD3,0x6D,0xA4,0xD3,0x6D,0xA6,0xD3,0x6D,0xB4,0xD3,0x6D,0xB6,0xDA,0x49,0x24,0xDA,0x49,0x26,0xDA,0x49,0x34,0xDA,0x49,0x36,0xDA,0x49,0xA4,0xDA,0x49,0xA6,0xDA,0x49,0xB4,0xDA,0x49,0xB6,0xDA,0x4D,0x24,0xDA,0x4D,0x26,0xDA,0x4D,0x34,0xDA,0x4D,0x36,0xDA,0x4D,0xA4,0xDA,0x4D,0xA6,0xDA,0x4D,0xB4,0xDA,0x4D,0xB6,0xDA,0x69,0x24,0xDA,0x69,0x26,0xDA,0x69,0x34,0xDA,0x69,0x36,0xDA,0x69,0xA4,0xDA,0x69,0xA6,0xDA,0x69,0xB4,0xDA,0x69,0xB6,0xDA,0x6D,0x24,0xDA,0x6D,0x26,0xDA,0x6D,0x34,0xDA,0x6D,0x36,0xDA,0x6D,0xA4,0xDA,0x6D,0xA6,0xDA,0x6D,0xB4,0xDA,0x6D,0xB6,0xDB,0x49,0x24,0xDB,0x49,0x26,0xDB,0x49,0x34,0xDB,0x49,0x36,0xDB,0x49,0xA4,0xDB,0x49,0xA6,0xDB,0x49,0xB4,0xDB,0x49,0xB6,0xDB,0x4D,0x24,0xDB,0x4D,0x26,0xDB,0x4D,0x34,0xDB,0x4D,0x36,0xDB,0x4D,0xA4,0xDB,0x4D,0xA6,0xDB,0x4D,0xB4,0xDB,0x4D,0xB6,0xDB,0x69,0x24,0xDB,0x69,0x26,0xDB,0x69,0x34,0xDB,0x69,0x36,0xDB,0x69,0xA4,0xDB,0x69,0xA6,0xDB,0x69,0xB4,0xDB,0x69,0xB6,0xDB,0x6D,0x24,0xDB,0x6D,0x26,0xDB,0x6D,0x34,0xDB,0x6D,0x36,0xDB,0x6D,0xA4,0xDB,0x6D,0xA6,0xDB,0x6D,0xB4,0xDB,0x6D,0xB6};


uint8_t WS2812B_init(t_stripchannel ch,uint16_t number_of_leds);
void WS2812B_show(t_stripchannel ch);
void WS2812B_setPixelColor(t_stripchannel ch,uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void WS2812B_setPixelColorDirect(t_stripchannel ch,uint16_t n, uint32_t c);
void WS2812B_setBrightness(t_stripchannel ch,uint8_t brightness);
void WS2812B_clear(t_stripchannel ch);
uint8_t WS2812B_getBrightness(t_stripchannel ch);
uint16_t WS2812B_numPixels(t_stripchannel ch);
uint32_t WS2812B_Color(uint8_t r, uint8_t g, uint8_t b);
void WS2812B_setStripColor(t_stripchannel ch,uint8_t r, uint8_t g, uint8_t b);
uint32_t WS2812B_colorWheel(uint8_t WheelPos);
void WS2812B_rainbow(t_stripchannel ch,uint8_t start, uint8_t count, uint8_t startval,  uint8_t stepsize);

void WS2812B_test(t_stripchannel ch);


#endif // WS2812B_H