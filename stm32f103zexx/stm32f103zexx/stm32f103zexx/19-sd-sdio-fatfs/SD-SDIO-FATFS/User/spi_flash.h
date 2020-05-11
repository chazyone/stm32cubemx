#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

uint8_t Flash_SPI_ReadSReg1(void);
uint8_t Flash_SPI_ReadSReg2(void);
uint8_t Flash_SPI_WriteSReg(void);
uint16_t Flash_SPI_ManufacDeviceID(void);

/*
example:

	//写flash 之前必须擦除对应扇区
	Flash_SPI_SectorErase(0x01);
	Flash_SPI_WritePage(0x01,str, len);
	
  while (1)
  {
		uint8_t str1[len];
		
		Flash_SPI_ReadData(0x01, str1, len);
		printf("%s\n", str1);
  }
*/

void Flash_SPI_SectorErase(uint32_t SecAddr);
void Flash_SPI_ReadData(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size);
void Flash_SPI_WritePage(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size);
void Flash_SPI_FastRead(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size);

#ifdef __cplusplus
}
#endif

#endif

