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
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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

/* USER CODE BEGIN PV */

uint8_t buf[1024];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
	return ch;
}

int fgetc(FILE *f)
{
	int ch = -1;
	HAL_UART_Receive(&huart2,(uint8_t *)&ch, sizeof(ch), 1000);
	return ch;
}

void Can_Send_Msg(uint8_t *msg, uint8_t len)
{
	CAN_TxHeaderTypeDef CAN_Header;
	uint16_t SendTimes, SendCNT=0;
	uint8_t FreeTxNum=0;
	
	CAN_Header.StdId = 0x12;
	CAN_Header.ExtId = 0x12;
	CAN_Header.IDE = CAN_ID_STD;
	CAN_Header.RTR = CAN_RTR_DATA;
	CAN_Header.DLC = 8;
	
	SendTimes=len/8+(len%8?1:0);
	FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
	
	while(SendTimes--)
	{
		if(0 == SendTimes)
		{
			if(len%8)
				CAN_Header.DLC=len%8;
		}
		while(0==FreeTxNum)
		{
			FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
		}
		HAL_Delay(1);
		if(HAL_CAN_AddTxMessage(&hcan1, &CAN_Header, msg, (uint32_t *)CAN_TX_MAILBOX0) != HAL_OK)
		{
			Error_Handler();
		}
		SendCNT+=8;
	}
}
void Can_Send_8BitMsg()
{
	CAN_TxHeaderTypeDef TxHeader;
	uint8_t TxData[8] = {0x23, 0x81, 0x60, 0x00, 0x55, 0x55, 0x08, 0x00};
	uint32_t TxMailbox; 
	uint32_t std_id = 0x601;  

	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;            
	TxHeader.StdId=std_id;
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.DLC = 8;
					
	if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		 /* Transmission request Error */
		 Error_Handler();
	}
	
}

void CAN_Filters_Init(void)
{
	CAN_FilterTypeDef CAN_FilterInit1;
	
	CAN_FilterInit1.FilterBank = 14;
	CAN_FilterInit1.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterInit1.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterInit1.FilterIdHigh = 0x0000;
	CAN_FilterInit1.FilterIdLow = 0x0000;
	CAN_FilterInit1.FilterMaskIdHigh = 0x0000;
	CAN_FilterInit1.FilterMaskIdLow = 0x0000;
	CAN_FilterInit1.FilterFIFOAssignment = CAN_RX_FIFO0;
	CAN_FilterInit1.FilterActivation = CAN_FILTER_ENABLE;
	
	CAN_FilterInit1.SlaveStartFilterBank=0;
	if(HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInit1) != HAL_OK)
	{
    Error_Handler();
	}
	
	if(HAL_CAN_Start(&hcan1) != HAL_OK)
	{
    Error_Handler();
	}
	
	if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
    Error_Handler();
	}
	
	
	CAN_FilterTypeDef CAN_FilterInit2;
	
	CAN_FilterInit2.FilterBank = 14;
	CAN_FilterInit2.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterInit2.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterInit2.FilterIdHigh = 0x0000;
	CAN_FilterInit2.FilterIdLow = 0x0000;
	CAN_FilterInit2.FilterMaskIdHigh = 0x0000;
	CAN_FilterInit2.FilterMaskIdLow = 0x0000;
	CAN_FilterInit2.FilterFIFOAssignment = CAN_RX_FIFO1;
	CAN_FilterInit2.FilterActivation = CAN_FILTER_ENABLE;
	
	CAN_FilterInit2.SlaveStartFilterBank=14;
	if(HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInit2) != HAL_OK)
	{
    Error_Handler();
	}
	
	if(HAL_CAN_Start(&hcan2) != HAL_OK)
	{
    Error_Handler();
	}
	
	if(HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
	{
    Error_Handler();
	}
	
}

uint8_t Can_Receive_Msg(uint8_t *buf)
{
	CAN_RxHeaderTypeDef CAN_Header={0};
	
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN_Header, buf) != HAL_OK)
	{
		Error_Handler();
	}
	
	return CAN_Header.DLC;
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
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	
	printf("----------------send------------------\r\n");
	CAN_Filters_Init();
//	uint8_t strSend[]="HelloWorld!!!";
	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//printf("Hello World!!!\n");
		Can_Send_8BitMsg();
		//Can_Send_Msg(strSend, sizeof(strSend));
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		HAL_Delay(200);
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
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
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
  /** Configure the Systick interrupt time 
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	printf("xxxxxx===>HAL_CAN_RxFifo0MsgPendingCallback\n");
}
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	
	printf("xxxxxx===>HAL_CAN_RxFifo0FullCallback\n");
}
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{
	
	printf("xxxxxx===>HAL_CAN_RxFifo1FullCallback\n");
}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	
	printf("xxxxxx===>HAL_CAN_RxFifo1MsgPendingCallback\n");
	if(hcan->Instance == hcan2.Instance)
	{
		CAN_RxHeaderTypeDef CAN_Header={0};
		uint8_t rData[8];
		HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &CAN_Header, rData);
		printf("RxFifo:%s\n", rData);
		
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(200);
	//	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
	}
}
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
