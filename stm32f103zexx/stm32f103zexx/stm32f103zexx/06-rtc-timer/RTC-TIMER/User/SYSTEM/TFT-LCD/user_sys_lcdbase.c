/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : lcdbase.c
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Includes ------------------------------------------------------------------*/
#include "user_sys_lcdtype.h"

#define LCD_Change_A	(10)
#define LCD_FSMC			(0x6C000000)

#define LCD_Addr			((1<<LCD_Change_A)<<1)
#define LCD_BASE 			((uint32_t)(LCD_FSMC | ((~LCD_Addr) & 0x3fffffe)))
#define LCD_Reg      	((LCD_Reg_TypeDef *) LCD_BASE)

extern LCD_TypeDef LCD;
void LCD_Delayus(uint32_t us)
{
	uint32_t i=us*72;
	while(i--);
}

void LCD_WriteCmd(uint16_t cmd)
{
	LCD_Reg->Cmd  = cmd;
}

void LCD_WriteData(uint16_t data)
{
	LCD_Reg->Data = data;
}

uint16_t LCD_ReadData(void)
{
	volatile uint16_t data;
	data = LCD_Reg->Data;
	return data;
}

void LCD_WriteValue(uint16_t cmd, uint16_t value)
{
	LCD_Reg->Cmd = cmd;
	LCD_Reg->Data = value;
}

uint16_t LCD_ReadValue(uint16_t cmd)
{
	LCD_WriteCmd(cmd);
	//LCD_Delayus(5);//待优化 5us不精确
	return LCD_ReadData();
}

//void HAL_LCD_Reg_WriteRAM_Init(void)
//{
//	LCD->Cmd = lcddev.wramcmd;
//}

void LCD_WriteColor(uint16_t rgb)
{
	LCD_Reg->Data = rgb;
}

//void LCD_WriteCmd(uint16_t cmd);
//void LCD_WriteData(uint16_t data);
//uint16_t LCD_ReadData(void);
//void LCD_WriteValue(uint16_t cmd, uint16_t value);
//uint16_t LCD_ReadValue(uint16_t cmd);
//void LCD_WriteColor(uint16_t rgb);

extern void LCD_TypeDef_Init(void)
{
	LCD.Lcd_Reg = LCD_Reg;
	LCD.WriteCmd = LCD_WriteCmd;
	LCD.WriteData = LCD_WriteData;
	LCD.ReadData = LCD_ReadData;
	LCD.WriteValue = LCD_WriteValue;
	LCD.ReadValue = LCD_ReadValue;
	LCD.WriteColor = LCD_WriteColor;
}

///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
