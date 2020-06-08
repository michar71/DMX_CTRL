#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdbool.h>
#include "stm32f1xx_hal.h"

//#define   _EEPROM_F1_LOW_DESTINY
#define   _EEPROM_F1_MEDIUM_DESTINY
//#define   _EEPROM_F1_HIGH_DESTINY

#ifdef  _EEPROM_F1_LOW_DESTINY
#define		_EEPROM_FLASH_PAGE_SIZE								1024
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 1 Kbytes */
#define _EEPROM_FLASH_PAGE_ADDRESS    (ADDR_FLASH_PAGE_0|(_EEPROM_FLASH_PAGE_SIZE*_EEPROM_USE_FLASH_PAGE))
#if (_EEPROM_USE_FLASH_PAGE>31)
#error  "Please Enter correct value _EEPROM_USE_FLASH_PAGE  (0 to 31)"
#endif
#endif


#ifdef  _EEPROM_F1_MEDIUM_DESTINY
#define		_EEPROM_FLASH_PAGE_SIZE								1024
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 1 Kbytes */
#define _EEPROM_FLASH_PAGE_ADDRESS    (ADDR_FLASH_PAGE_0|(_EEPROM_FLASH_PAGE_SIZE*_EEPROM_USE_FLASH_PAGE))
#if (_EEPROM_USE_FLASH_PAGE>127)
#error  "Please Enter correct value _EEPROM_USE_FLASH_PAGE  (0 to 127)"
#endif
#endif


#ifdef  _EEPROM_F1_HIGH_DESTINY
#define		_EEPROM_FLASH_PAGE_SIZE								2048
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define _EEPROM_FLASH_PAGE_ADDRESS    (ADDR_FLASH_PAGE_0|(_EEPROM_FLASH_PAGE_SIZE*_EEPROM_USE_FLASH_PAGE))
#if (_EEPROM_USE_FLASH_PAGE>255)
#error  "Please Enter correct value _EEPROM_USE_FLASH_PAGE  (0 to 255)"
#endif
#endif

#define     _EEPROM_USE_FLASH_PAGE              127

//Support individual Address write. This requires one Page of Ram for temporary storage...
//#define INDIVIDUAL_ADDR_WRITE

//################################################################################################################
bool    EE_LL_Format(uint16_t startpage, uint16_t pages);
bool    EE_LL_Read(uint16_t startpage,uint16_t addr,uint16_t size, uint8_t* Data);
bool    EE_LL_Write(uint16_t startpage,uint16_t addr,uint16_t size, uint8_t* Data);

bool	EE_Format();
bool 	EE_Read(uint16_t VirtualAddress, uint8_t* Data);
#ifdef INDIVIDUAL_ADDR_WRITE
bool 	EE_Write(uint16_t VirtualAddress, uint8_t Data);
#endif

bool	EE_Reads(uint16_t VirtualAddress,uint16_t HowMuchToRead,uint32_t* Data);
bool 	EE_Writes(uint16_t VirtualAddress,uint16_t HowMuchToWrite,uint32_t* Data);

//################################################################################################################

#endif
