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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *f)
{
	return ITM_SendChar(ch);
}


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
void Flash_SPI_WriteEnable()
{
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_WriteEnable);
	SPI_FLASH_CS(0);
}
void Flash_SPI_WriteDisable()
{
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_WriteDisable);
	SPI_FLASH_CS(0);
}

//------------------------------------------------------------

uint8_t Flash_SPI_ReadSReg1()
{
	uint8_t rByte = 0;
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ReadStatusRegister_1);
	Flash_SPI_DataReceive(&rByte, 1);
	SPI_FLASH_CS(0);
	
	return rByte;
}

uint8_t Flash_SPI_ReadSReg2()
{
	uint8_t rByte = 0;
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ReadStatusRegister_2);
	Flash_SPI_DataReceive(&rByte, 1);
	SPI_FLASH_CS(0);
	
	return rByte;
}
void FLASH_SPI_WaitBusy()
{
	while((Flash_SPI_ReadSReg1()&0x01)==0x01);
}
uint8_t Flash_SPI_WriteSReg()
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

uint16_t Flash_SPI_ManufacDeviceID()
{
	uint16_t deviceid = 0;
	uint8_t dummy[3]={0};
	
	SPI_FLASH_CS(1);
	Flash_SPI_CmdTransmit(SPI_FLASH_ManufacDeviceID);
	Flash_SPI_DataTransmit(dummy, 3);
	Flash_SPI_DataReceive((uint8_t *)&deviceid, 2);
	SPI_FLASH_CS(0);
	printf("-->DeviceId:%04x\n", deviceid);
	
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	uint8_t str[]="hello world";
	uint8_t len=sizeof(str);
	Flash_SPI_ManufacDeviceID();
	
	//printf("sizeof(str)%d:\n",sizeof(str));
	//写flash 之前必须擦除对应扇区
	Flash_SPI_SectorErase(0x01);
	Flash_SPI_WritePage(0x01,str, len);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		uint8_t str1[len];
		
		Flash_SPI_ReadData(0x01, str1, len);
		printf("%s\n", str1);
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
