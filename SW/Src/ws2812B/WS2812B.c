#include "WS2812B/WS2812B.h"
#include <stdlib.h>

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;

// Constructor when n is the number of LEDs in the strip
uint8_t WS2812B_init(t_stripchannel ch,uint16_t number_of_leds)
{
	//init vars
	stripchannel[ch].brightness = 0;
	stripchannel[ch].pixels = NULL;
	stripchannel[ch].doubleBuffer = NULL;
	stripchannel[ch].time = 0;

	if (ch == CH1)
	{
		stripchannel[ch].phspi = &hspi1;
		stripchannel[ch].phdma_spi_tx = &hdma_spi1_tx;
	}
	else
	{
		stripchannel[ch].phspi = &hspi2;
		stripchannel[ch].phdma_spi_tx = &hdma_spi2_tx;
	}




  uint8_t buffer0[2] = { 0, 0 };

  stripchannel[ch].numBytes = (number_of_leds<<3) + number_of_leds + 2; // 9 encoded bytes per pixel. 1 byte empty peamble to fix issue with SPI MOSI and on byte at the end to clear down MOSI
							// Note. (n<<3) +n is a fast way of doing n*9

  if((stripchannel[ch].doubleBuffer = (uint8_t *)malloc(stripchannel[ch].numBytes*2)))
  {
	stripchannel[ch].numLEDs = number_of_leds;
	stripchannel[ch].pixels = stripchannel[ch].doubleBuffer;
    // Only need to init the part of the double buffer which will be interacted with by the API e.g. setPixelColor
    *stripchannel[ch].pixels=0;//clear the preamble byte
    *(stripchannel[ch].pixels+stripchannel[ch].numBytes-1)=0;// clear the post send cleardown byte.
    WS2812B_clear(ch);// Set the encoded data to all encoded zeros
  }
  else
  {
	//Malloc Failed....
	  stripchannel[ch].numLEDs = stripchannel[ch].numBytes = 0;
    return 0;
  }

  /* SPI DMA Init */
  /* SPI_TX Init */
  if (ch == CH1)
  {
	  stripchannel[ch].phdma_spi_tx->Instance = DMA1_Channel3;
  }
  else
  {
	  stripchannel[ch].phdma_spi_tx->Instance = DMA1_Channel4;
  }
  stripchannel[ch].phdma_spi_tx->Init.Direction = DMA_MEMORY_TO_PERIPH;
  stripchannel[ch].phdma_spi_tx->Init.PeriphInc = DMA_PINC_DISABLE;
  stripchannel[ch].phdma_spi_tx->Init.MemInc = DMA_MINC_ENABLE;
  stripchannel[ch].phdma_spi_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  stripchannel[ch].phdma_spi_tx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  stripchannel[ch].phdma_spi_tx->Init.Mode = DMA_NORMAL;
  stripchannel[ch].phdma_spi_tx->Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(stripchannel[ch].phdma_spi_tx) != HAL_OK)
  {
		//Init Failed....
	    stripchannel[ch].numLEDs = stripchannel[ch].numBytes = 0;
	    return 0;
  }

  //Create DMA Link
  __HAL_LINKDMA(stripchannel[ch].phspi,hdmatx,*stripchannel[ch].phdma_spi_tx);


   //initialize MOSI pin to LOW.  Without this, first time transmit for first LED might be wrong.
   HAL_SPI_Transmit(stripchannel[ch].phspi, buffer0, 1, 100 );

   return 1;
}


// Sends the current buffer to the leds
void WS2812B_show(t_stripchannel ch)
{
  uint32_t loopcnt = 0;

  //SPI.dmaSendAsync(pixels,numBytes);// Start the DMA transfer of the current pixel buffer to the LEDs and return immediately.

  //Wait for last transfer to finish
  while(__HAL_SPI_GET_FLAG(stripchannel[ch].phspi, SPI_FLAG_BSY ))
  {
	  loopcnt++;
  }

  while (HAL_GetTick()<stripchannel[ch].time)
  {
	  loopcnt++;
  }


  //we know how long the transfer takes..
  //(440ns per bit) * 8 bits * 3 colors * number of pixels...

  //Send Data via DMA
  HAL_SPI_Transmit_DMA(stripchannel[ch].phspi, stripchannel[ch].pixels, stripchannel[ch].numBytes );
  stripchannel[ch].time = HAL_GetTick() + 6;  //we need to wait at least XXX systicks for the colors to latch in after the last transfer.
  	  	  	  	  	  	  	 //The added 6 ms here are purely experimental...

  // Need to copy the last / current buffer to the other half of the double buffer as most API code does not rebuild the entire contents
  // from scratch. Often just a few pixels are changed e.g in a chaser effect

  if (stripchannel[ch].pixels==stripchannel[ch].doubleBuffer)
  {
	// pixels was using the first buffer
	  stripchannel[ch].pixels = stripchannel[ch].doubleBuffer+stripchannel[ch].numBytes;  // set pixels to second buffer
	memcpy(stripchannel[ch].pixels,stripchannel[ch].doubleBuffer,stripchannel[ch].numBytes);// copy first buffer to second buffer
  }
  else
  {
	// pixels was using the second buffer
	  stripchannel[ch].pixels	= stripchannel[ch].doubleBuffer;  // set pixels to first buffer
	memcpy(stripchannel[ch].pixels,stripchannel[ch].doubleBuffer+stripchannel[ch].numBytes,stripchannel[ch].numBytes);	 // copy second buffer to first buffer
  }
}

//Sets a specific pixel to a specific r,g,b colour
//Because the pixels buffer contains the encoded bitstream, which is in triplets
//the lookup table need to be used to find the correct pattern for each byte in the 3 byte sequence.
void WS2812B_setPixelColor(t_stripchannel ch,uint16_t n, uint8_t r, uint8_t g, uint8_t b)
 {
   uint8_t *bptr = stripchannel[ch].pixels + (n<<3) + n +1;
   uint8_t *tPtr = (uint8_t *)encoderLookup + g*2 + g;// need to index 3 x g into the lookup

   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;

   tPtr = (uint8_t *)encoderLookup + r*2 + r;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;

   tPtr = (uint8_t *)encoderLookup + b*2 + b;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
   *bptr++ = *tPtr++;
 }

void WS2812B_setPixelColorDirect(t_stripchannel ch,uint16_t n, uint32_t c)
{
  uint8_t r,g,b;

  if(stripchannel[ch].brightness)
  {
	r = ((int)((uint8_t)(c >> 16)) * (int)stripchannel[ch].brightness) >> 8;
	g = ((int)((uint8_t)(c >>  8)) * (int)stripchannel[ch].brightness) >> 8;
	b = ((int)((uint8_t)c) * (int)stripchannel[ch].brightness) >> 8;
  }
  else
  {
	r = (uint8_t)(c >> 16),
	g = (uint8_t)(c >>  8),
	b = (uint8_t)c;
  }
  WS2812B_setPixelColor(ch,n,r,g,b);
}

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t WS2812B_Color(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

//Get the number of pixels
uint16_t WS2812B_numPixels(t_stripchannel ch)
{
  return stripchannel[ch].numLEDs;
}

// Adjust output brightness; 0=darkest (off), 255=brightest.  This does
// NOT immediately affect what's currently displayed on the LEDs.  The
// next call to show() will refresh the LEDs at this level.  However,
// this process is potentially "lossy," especially when increasing
// brightness.  The tight timing in the WS2811/WS2812 code means there
// aren't enough free cycles to perform this scaling on the fly as data
// is issued.  So we make a pass through the existing color data in RAM
// and scale it (subsequent graphics commands also work at this
// brightness level).  If there's a significant step up in brightness,
// the limited number of steps (quantization) in the old data will be
// quite visible in the re-scaled version.  For a non-destructive
// change, you'll need to re-render the full strip data.  C'est la vie.
void WS2812B_setBrightness(t_stripchannel ch,uint8_t b)
{
  // Stored brightness value is different than what's passed.
  // This simplifies the actual scaling math later, allowing a fast
  // 8x8-bit multiply and taking the MSB.  'brightness' is a uint8_t,
  // adding 1 here may (intentionally) roll over...so 0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  uint8_t newBrightness = b + 1;
  uint8_t c;
  uint8_t *ptr = stripchannel[ch].pixels;
  uint8_t oldBrightness = stripchannel[ch].brightness - 1; // De-wrap old brightness value
  uint16_t scale;

  if(newBrightness != stripchannel[ch].brightness) // Compare against prior value
  {
    // Brightness has changed -- re-scale existing data in RAM
    if(oldBrightness == 0)
      scale = 0; // Avoid /0
    else if(b == 255)
      scale = 65535 / oldBrightness;
    else
      scale = (((uint16_t)newBrightness << 8) - 1) / oldBrightness;

    for(uint16_t i=0; i<stripchannel[ch].numBytes; i++)
	  {
      c = *ptr;
      *ptr++ = (c * scale) >> 8;
    }
    stripchannel[ch].brightness = newBrightness;
  }
}

//Return the brightness value
uint8_t WS2812B_getBrightness(t_stripchannel ch)
{
  return stripchannel[ch].brightness - 1;
}

//Sets the encoded pixel data to turn all the LEDs off.
void WS2812B_clear(t_stripchannel ch)
{
	uint8_t * bptr= stripchannel[ch].pixels+1;// Note first byte in the buffer is a preable and is always zero. hence the +1
	uint8_t *tPtr;

	for(int i=0;i< (stripchannel[ch].numLEDs *3);i++)
	{
    tPtr = (uint8_t *)encoderLookup;
    *bptr++ = *tPtr++;
    *bptr++ = *tPtr++;
    *bptr++ = *tPtr++;
	}
}

//Set the whole strip to the same color
void WS2812B_setStripColor(t_stripchannel ch,uint8_t r, uint8_t g, uint8_t b)
{
  for(uint16_t i=0; i<WS2812B_numPixels(ch); i++)
  {
	  WS2812B_setPixelColor(ch,i, r,g,b);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t WS2812B_colorWheel(uint8_t WheelPos)
{
  if(WheelPos < 85)
  {
    return WS2812B_Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else
  {
    if(WheelPos < 170)
    {
     WheelPos -= 85;
     return WS2812B_Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
     WheelPos -= 170;
     return WS2812B_Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
  }
}


//show rainbow colorsa starting at pixel start for count pixels with a given startvalue with a defined stepsize
void WS2812B_rainbow(t_stripchannel ch,uint8_t start, uint8_t count, uint8_t startval,  uint8_t stepsize)
{
  uint16_t i, j;

	j =startval;
	for(i=start; i<=(start+count); i++)
	{
		WS2812B_setPixelColorDirect(ch,i, WS2812B_colorWheel((j) & 255));
		j = j + stepsize;
		if (j>255)
			j = j - 255;
	}
}

//Running light pattern
void WS2812B_test(t_stripchannel ch)
{
	static uint16_t cnt = 0;
	static uint16_t delay = 0;

	if (delay != 100)
	{
		delay ++;
		return;
	}
	delay = 0;

	//Clear All
	WS2812B_clear(ch);

	WS2812B_setPixelColor(ch,0, 255, 0, 0);
	WS2812B_setPixelColor(ch,1, 0, 255, 0);
	WS2812B_setPixelColor(ch,2, 0, 0, 255);
	WS2812B_setPixelColor(ch,3, 255, 0, 255);
	WS2812B_setPixelColor(ch,4, 0, 255, 255);
	WS2812B_setPixelColor(ch,5, 255, 255, 0);
	WS2812B_setPixelColor(ch,6, 255, 255, 255);
	//Set Pixel
	//WS2812B_setPixelColor(cnt, 255, 0, 0);

	//Increase Counter
	cnt++;
	if (cnt == WS2812B_numPixels(ch))
		cnt = 0;

}