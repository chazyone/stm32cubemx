#include "spi_flash.h"
#include "spi.h"

#define SPI_FLASH_WriteEnable							0x06
#define SPI_FLASH_WriteDisable						0x04
#define SPI_FLASH_ReadStatusRegister_1		0x05
#define SPI_FLASH_ReadStatusRegister_2		0x35
#define SPI_FLASH_WriteStatusRegister			0x01
#define SPI_FLASH_PageProgram							0x02
#define SPI_FLASH_QuadPageProgram					0x32
#define SPI_FLASH_BlockErase_64						0xd8
#define SPI_FLASH_BlockErase_32						0x52
#define SPI_FLASH_SectorErase							0x20
#define SPI_FLASH_ChipErase								0xc7//C7h/60h
#define SPI_FLASH_EraseSuspend						0x75
#define SPI_FLASH_EraseResume							0x7a
#define SPI_FLASH_Power_down							0xb9
#define SPI_FLASH_HighPerformanceMode			0xa3
#define SPI_FLASH_ContinuousReadModeReset 0xff
#define SPI_FLASH_HPMDeviceID							0xab
#define SPI_FLASH_ManufacDeviceID					0x90
#define SPI_FLASH_ReadUniqueID						0x4b
#define SPI_FLASH_JEDECID									0x9f
//////////////////////////////////////////////////////////
#define SPI_FLASH_ReadData								0x03
#define SPI_FLASH_FastRead								0x0b
#define SPI_FLASH_CS(__RET__)							(__RET__? HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET)\
																						:HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET))


HAL_StatusTypeDef Flash_SPI_CmdTransmit(uint8_t Addr)
{
	return HAL_SPI_Transmit(&hspi2, &Addr, 1, 1000);
}
HAL_StatusTypeDef Flash_SPI_DataTransmit(uint8_t *pData, uint16_t Size)
{
	return HAL_SPI_Transmit(&hspi2, pData, Size, 1000);
}
HAL_StatusTypeDef Flash_SPI_DataReceive(uint8_t *pData, uint16_t Size)
{
	return HAL_SPI_Receive(&hspi2, pData, Size, 1000);
}

HAL_StatusTypeDef Flash_SPI_DataTransmitReceive(uint8_t *pData, uint16_t Size)
{
	return HAL_SPI_TransmitReceive(&hspi2, pData, pData, Size, 1000);
}
//------------------------------------------------------------
void Flash_SPI_WriteEnable(void)
{
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_WriteEnable);
	SPI_FLASH_CS(0);
}
void Flash_SPI_WriteDisable(void)
{
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_WriteDisable);
	SPI_FLASH_CS(0);
}

//------------------------------------------------------------

uint8_t Flash_SPI_ReadSReg1(void)
{
	uint8_t rByte = 0;
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ReadStatusRegister_1);
	Flash_SPI_DataReceive(&rByte, 1);
	SPI_FLASH_CS(0);
	
	return rByte;
}

uint8_t Flash_SPI_ReadSReg2(void)
{
	uint8_t rByte = 0;
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ReadStatusRegister_2);
	Flash_SPI_DataReceive(&rByte, 1);
	SPI_FLASH_CS(0);
	
	return rByte;
}

void FLASH_SPI_WaitBusy(void)
{
	while((Flash_SPI_ReadSReg1()&0x01)==0x01);
}

uint8_t Flash_SPI_WriteSReg(void)
{
	uint16_t Byte2 = 0;
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_WriteStatusRegister);
	Flash_SPI_DataReceive((uint8_t *)&Byte2,2);
	SPI_FLASH_CS(0);
	
	return Byte2;
}

void Flash_SPI_SectorErase(uint32_t SecAddr)
{
	
	uint32_t sAddr=SecAddr;
	SecAddr /= 4096;//只能擦除扇区，应该传输被擦除扇区的首地址 一个扇区4k=4096字节
	sAddr = ((sAddr & 0x000000ff) << 16) | ((sAddr & 0x0000ff00) << 0) | ((sAddr & 0x00ff0000) >> 16);
	
	Flash_SPI_WriteEnable();
	FLASH_SPI_WaitBusy();
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_SectorErase);
	Flash_SPI_DataTransmit((uint8_t *)&SecAddr, 3);
	SPI_FLASH_CS(0);
	FLASH_SPI_WaitBusy();
	Flash_SPI_WriteDisable();
	
}

uint16_t Flash_SPI_ManufacDeviceID(void)
{
	uint16_t deviceid = 0;
	uint8_t dummy[3]={0};
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ManufacDeviceID);
	Flash_SPI_DataTransmit(dummy, 3);
	Flash_SPI_DataReceive((uint8_t *)&deviceid, 2);
	SPI_FLASH_CS(0);
	
	return deviceid;
}

void Flash_SPI_ReadData(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size)
{
	startAddr = ((startAddr & 0x000000ff) << 16) | ((startAddr & 0x0000ff00) << 0) | ((startAddr & 0x00ff0000) >> 16);
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ReadData);
	Flash_SPI_DataTransmit((uint8_t *)&startAddr ,3);
	Flash_SPI_DataReceive(pBuffer, Size);
	SPI_FLASH_CS(0);
}

void Flash_SPI_FastRead(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size)
{
	uint8_t dummy=0xff;
	startAddr = ((startAddr & 0x000000ff) << 16) | ((startAddr & 0x0000ff00) << 0) | ((startAddr & 0x00ff0000) >> 16);
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_FastRead);
	Flash_SPI_DataTransmit((uint8_t *)&startAddr ,3);
	Flash_SPI_DataTransmit(&dummy, 1);
	Flash_SPI_DataReceive(pBuffer, Size);
	SPI_FLASH_CS(0);
}

void Flash_SPI_WritePage(uint32_t startAddr, uint8_t *pBuffer,uint8_t Size)
{
	startAddr = ((startAddr & 0x000000ff) << 16) | ((startAddr & 0x0000ff00) << 0) | ((startAddr & 0x00ff0000) >> 16);
	
	Flash_SPI_WriteEnable();
	FLASH_SPI_WaitBusy();
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_PageProgram);
	Flash_SPI_DataTransmit((uint8_t *)&startAddr ,3);
	Flash_SPI_DataReceive(pBuffer, Size);
	SPI_FLASH_CS(0);
	FLASH_SPI_WaitBusy();
	Flash_SPI_WriteDisable();
}

