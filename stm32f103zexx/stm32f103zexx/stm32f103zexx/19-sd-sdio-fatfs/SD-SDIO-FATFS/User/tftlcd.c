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
//#include "user_sys_lcdtype.h"

///////////// include files///////////////////////////
#include "font.h"
#include "tftlcd.h"

#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
///////////////////////////////////////////////////////////////

#define LCD_Change_A	(10)
#define LCD_FSMC			(0x6C000000)

#define LCD_Addr			((1<<LCD_Change_A)<<1)
#define LCD_BASE 			((uint32_t)(LCD_FSMC | ((~LCD_Addr) & 0x3fffffe)))
#define LCD_Reg      	((LCD_Reg_TypeDef *) LCD_BASE)
////////////////////////////////////////////////////////////////
static LCD_TypeDef LCD;
////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////
LCD_Device_TypeDef HAL_LCD_GetDevice(void)
{
	return LCD.Lcd_Dev;
}

//清屏函数
void HAL_LCD_Clear(void)
{
	HAL_LCD_FillRect(0, 0, LCD.Lcd_Dev.width, LCD.Lcd_Dev.height, LCD.bkColor);
}

void HAL_LCD_SetShowDir(uint8_t dir)
{
	if(!dir)
	{
		LCD.WriteCmd(0x0001);   
		LCD.WriteData(0x0100);
		LCD.WriteCmd(0x0003);   //设置彩屏显示方向的寄存器
		LCD.WriteData(0x1030);  
		LCD.Lcd_Dev.height=320;
		LCD.Lcd_Dev.width=240;	
	}
	else
	{
		LCD.WriteCmd(0x0001);   
		LCD.WriteData(0x0000);
		LCD.WriteCmd(0x0003);   //设置彩屏显示方向的寄存器
		LCD.WriteData(0x1038);  
		LCD.Lcd_Dev.height=240;
		LCD.Lcd_Dev.width=320;		
	}
	LCD.Lcd_Dev.dir = dir;
}

void HAL_LCD_SetBkColor(uint16_t color)
{
	LCD.bkColor = (LCD_ColorTyp)color;
}
void HAL_LCD_SetFontColor(uint16_t color)
{
	LCD.fontColor = (LCD_ColorTyp)color;
}

uint16_t HAL_LCD_GetBkColor(void)
{
	return LCD.bkColor;
}
uint16_t HAL_LCD_GetFontColor(void)
{
	return LCD.fontColor;
}
uint16_t HAL_LCD_Init(void)
{
	uint16_t retID = 0;
	
	LCD.Lcd_Reg = LCD_Reg;
	LCD.WriteCmd = LCD_WriteCmd;
	LCD.WriteData = LCD_WriteData;
	LCD.ReadData = LCD_ReadData;
	LCD.WriteValue = LCD_WriteValue;
	LCD.ReadValue = LCD_ReadValue;
	LCD.WriteColor = LCD_WriteColor;
	
	//HAL_Delay(50);
	LCD.WriteCmd(0x00);
	retID = LCD.Lcd_Dev.id = LCD.ReadData();
	
	LCD.WriteValue(0x00E3,0x3008); // Set internal timing
	LCD.WriteValue(0x00E7,0x0012); // Set internal timing
	LCD.WriteValue(0x00EF,0x1231); // Set internal timing
	
	LCD.WriteValue(0x0001,0x0100); // set SS and SM bit
	LCD.WriteValue(0x0002,0x0700); // set 1 line inversion
	LCD.WriteValue(0x0003,0x1030); // set GRAM write direction and BGR=1.
	LCD.WriteValue(0x0004,0x0000); // Resize register
	LCD.WriteValue(0x0008,0x0207); // set the back porch and front porch
	LCD.WriteValue(0x0009,0x0000); // set non-display area refresh cycle ISC[3:0]
	LCD.WriteValue(0x000A,0x0000); // FMARK function
	LCD.WriteValue(0x000C,0x0000); // RGB interface setting
	LCD.WriteValue(0x000D,0x0000); // Frame marker Position
	LCD.WriteValue(0x000F,0x0000); // RGB interface polarity
	//*************Power On sequence ****************//
	LCD.WriteValue(0x0010,0x0000); // SAP);WriteData(BT[3:0]);WriteData(AP);WriteData(DSTB);WriteData(SLP);WriteData(STB
	LCD.WriteValue(0x0011,0x0007); // DC1[2:0]);WriteData(DC0[2:0]);WriteData(VC[2:0]
	LCD.WriteValue(0x0012,0x0000); // VREG1OUT voltage
	LCD.WriteValue(0x0013,0x0000); // VDV[4:0] for VCOM amplitude
	//HAL_Delay(20); // Dis-charge capacitor power voltage
	LCD.WriteValue(0x0010,0x1490); // SAP);WriteData(BT[3:0]);WriteData(AP);WriteData(DSTB);WriteData(SLP);WriteData(STB
	LCD.WriteValue(0x0011,0x0227); // DC1[2:0]);WriteData(DC0[2:0]);WriteData(VC[2:0]
	//HAL_Delay(50); // Delay 50ms
	LCD.WriteValue(0x0012,0x001C); // Internal reference voltage= Vci;
	//HAL_Delay(50); // Delay 50ms
	LCD.WriteValue(0x0013,0x1A00); // Set VDV[4:0] for VCOM amplitude
	LCD.WriteValue(0x0029,0x0025); // Set VCM[5:0] for VCOMH
	LCD.WriteValue(0x002B,0x000C); // Set Frame Rate
	//HAL_Delay(50); // Delay 50ms
	LCD.WriteValue(0x0020,0x0000); // GRAM horizontal Address
	LCD.WriteValue(0x0021,0x0000); // GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	LCD.WriteValue(0x0030,0x0000);
	LCD.WriteValue(0x0031,0x0506);
	LCD.WriteValue(0x0032,0x0104);
	LCD.WriteValue(0x0035,0x0207);
	LCD.WriteValue(0x0036,0x000F);
	LCD.WriteValue(0x0037,0x0306);
	LCD.WriteValue(0x0038,0x0102);
	LCD.WriteValue(0x0039,0x0707);
	LCD.WriteValue(0x003C,0x0702);
	LCD.WriteValue(0x003D,0x1604);
	//------------------ Set GRAM area ---------------//
	LCD.WriteValue(0x0050,0x0000); // Horizontal GRAM Start Address
	LCD.WriteValue(0x0051,0x00EF); // Horizontal GRAM End Address
	LCD.WriteValue(0x0052,0x0000); // Vertical GRAM Start Address
	LCD.WriteValue(0x0053,0x013F); // Vertical GRAM Start Address
	LCD.WriteValue(0x0060,0x2700); // Gate Scan Line
	LCD.WriteValue(0x0061,0x0001); // NDL,VLE);WriteData(REV
	LCD.WriteValue(0x006A,0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	LCD.WriteValue(0x0080,0x0000);
	LCD.WriteValue(0x0081,0x0000);
	LCD.WriteValue(0x0082,0x0000);
	LCD.WriteValue(0x0083,0x0000);
	LCD.WriteValue(0x0084,0x0000);
	LCD.WriteValue(0x0085,0x0000);
	//------------ Panel Control -------------------//
	LCD.WriteValue(0x0090,0x0010);
	LCD.WriteValue(0x0092,0x0600);
	LCD.WriteValue(0x0007,0x0133); // 262K color and display ON

	LCD.WriteValue(0x0010,0x12B0); // SAP);WriteData(BT[3:0]);WriteData(AP);WriteData(DSTB);WriteData(SLP
	LCD.WriteValue(0x0011,0x0007);
	LCD.WriteValue(0x0017,0x0001); // DC1[2:0]);WriteData(DC0[2:0]);WriteData(VC[2:0]
	LCD.WriteValue(0x0012,0x01BD); // VREG1OUT voltage
	LCD.WriteValue(0x0013,0x1700); // VDV[4:0] for VCOM amplitude
	//HAL_Delay(20); // Dis-charge capacitor power voltage
	LCD.WriteValue(0x0010,0x1490); // SAP);WriteData(BT[3:0]);WriteData(AP);WriteData(DSTB);WriteData(SLP);WriteData(STB
	LCD.WriteValue(0x0011,0x0227); // DC1[2:0]);WriteData(DC0[2:0]);WriteData(VC[2:0]
	//HAL_Delay(50); // Delay 50ms
	LCD.WriteValue(0x0012,0x001C); //Inernal reference voltage =Vci;
	//HAL_Delay(50); // Delay 50ms
	LCD.WriteValue(0x0013,0x1A00); // VDV[4:0] for VCOM amplitude
	LCD.WriteValue(0x0029,0x0025); // VCM[5:0] for VCOMH
	//HAL_Delay(50); // Delay 50ms
	//HAL_LCD_WriteCmd(0x0007);HAL_LCD_WriteData(0x0133); // 262K color and display ON

	LCD.WriteValue(0x0007,0x0133);
	//HAL_Delay(120);
	LCD.WriteCmd(0x0022);
	
	HAL_LCD_SetShowDir(0);		//0：竖屏  1：横屏  默认竖屏
	HAL_LCD_SetBkColor(LCD_WHITE);
	HAL_LCD_SetFontColor(LCD_BLACK);
	HAL_LCD_Clear();
	
	return retID;
}

void HAL_LCD_ShowSwitch(uint8_t mode)
{
	if(mode)
	{
		LCD.WriteValue(0x0007,0x0133); // 262K color and display ON
		//LCD.WriteCmd(0x0022);
	}
	else
	{
		LCD.WriteValue(0x0007,0x0130); // 262K color and display OFF
		//LCD.WriteCmd(0x0022);
	}
}
/**
  * @brief  LCD Set rectangle
  * @param  x1,y1 start addr
  * @param  x2,y2 end addr
  * @retval ERR
  */

int HAL_LCD_SetRect(uint16_t x1, uint16_t y1, uint16_t x2,uint16_t y2)
{
	if(x2 < x1 || y2 < y1)
		return -1;
	if(LCD.Lcd_Dev.dir==0)
	{
		LCD.WriteCmd(0x0050);   
		LCD.WriteData(x1);
		LCD.WriteCmd(0x0051);  
		LCD.WriteData(x2);
		LCD.WriteCmd(0x0052);   
		LCD.WriteData(y1);
		LCD.WriteCmd(0x0053);   
		LCD.WriteData(y2);
			 
		LCD.WriteCmd(0x0020);   
		LCD.WriteData(x1);
		LCD.WriteCmd(0x0021);   
		LCD.WriteData(y1);	
	}   	
	else
	{
		LCD.WriteCmd(0x0052);   
		LCD.WriteData(x1);
		LCD.WriteCmd(0x0053);  
		LCD.WriteData(x2);
		LCD.WriteCmd(0x0050);   
		LCD.WriteData(y1);
		LCD.WriteCmd(0x0051);   
		LCD.WriteData(y2);
			 
		LCD.WriteCmd(0x0021);   
		LCD.WriteData(x1);
		LCD.WriteCmd(0x0020);   
		LCD.WriteData(y1);	
	}
	LCD.WriteCmd(0x0022);
	
	return 0;
}

void HAL_LCD_FillRect(uint16_t x, uint16_t y, uint16_t width,uint16_t height,uint16_t color)
{
	HAL_LCD_SetRect(x, y, x + width - 1,y + height -1);
	uint16_t h = height;
	while(h--)
	{
		uint16_t w = width;
		while(w--)
		{
			LCD.WriteColor(color);
		}
	}
}
/**
  * @brief  LCD draw point
  * @param  x,y point
  * @retval NONE
  */

void HAL_LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	if((x <= LCD.Lcd_Dev.width) && (y <= LCD.Lcd_Dev.height))
	{
		HAL_LCD_SetRect(x, y, x, y);
		LCD.WriteColor(color);
	}
}

uint16_t HAL_LCD_ReadPoint(uint16_t x, uint16_t y)
{
	uint16_t r = 0, g = 0, b = 0;
	if((x <= LCD.Lcd_Dev.width) && (y <= LCD.Lcd_Dev.height))
	{
		HAL_LCD_SetRect(x, y, x, y);
		LCD.WriteCmd(0x0022);
		r = LCD.ReadData();
		r = LCD.ReadData();
		g = r & 0x07e0;
		b = r & 0x001f;
		r = r & 0xf800;
		return (r >> 11) || (g) || (b << 11);
	}
	return 0;
}
void HAL_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	float kx = (x2 > x1)? 1.0f : ((x2 == x1)? 0.0f : -1.0f);
	float ky = (y2 > y1)? 1.0f : ((y2 == y1)? 0.0f : -1.0f);
	if(x2 != x1 && y2 != y1)
	{
		ky = ky * fabs((y2 - y1 + 1) / (float)(x2 - x1 +1));
	}		
	for(float x = x1, y = y1; ((x1 < x2) && (x <= x2)) || ((x1 > x2) && (x >= x2)) || ((y1 < y2)&&(y <= y2)) || ((y1 > y2)&&(y >= y2)); x += kx)
	{
		y += ky;
		HAL_LCD_DrawPoint(x, y, color);
	}
	return ;
}

void HAL_LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	HAL_LCD_DrawLine(x1, y1, x2, y1, color);
	HAL_LCD_DrawLine(x2, y1, x2, y2 - 1, color);
	HAL_LCD_DrawLine(x2, y2, x1, y2, color);
	HAL_LCD_DrawLine(x1, y2, x1, y1 + 1, color);
}

void HAL_LCD_PutChar(uint16_t x,uint16_t y,uint8_t c,uint8_t csize, uint8_t overlay)
{
	uint8_t *p = NULL;
	if(csize == 12) p = (uint8_t *)ascii_1206[c - ' ']; 
	else if(csize == 16) p = (uint8_t *)ascii_1608[c - ' ']; 
	else if(csize == 24) p = (uint8_t *)ascii_2412[c - ' ']; 
	else return;
	
	uint8_t ty = y, tx = x;
	for(uint8_t i = 0; i < csize; i++)
	{
		uint8_t nch = p[i];
		for(uint8_t t = 0; t <8; t++)
		{
			if(nch&0x80) HAL_LCD_DrawPoint(tx, ty, LCD.fontColor);
			else if(overlay) HAL_LCD_DrawPoint(tx, ty, LCD.bkColor);
			
			nch <<= 1;
			ty++;
			if(ty - y >= 12)
			{
				ty = y;
				tx++;
				break;
			}
		}
	}
}

void HAL_LCD_Puts(uint16_t x,uint16_t y, const char *str, uint8_t csize, uint8_t overlay)
{
	char *p = (char *)str;
	while(*p != '\0')
	{
		HAL_LCD_PutChar(x, y, *p, csize, overlay);
		x += csize/2;
		p++;
	}
}


void HAL_LCD_Print(uint16_t x, uint16_t y, uint8_t csize, uint8_t overlay, const char *format, ...)
{
	va_list ap;
	char strBuf[256];
	va_start(ap, format);
	vsprintf(strBuf, format, ap);
	va_end(ap);
	HAL_LCD_Puts(x, y, strBuf, csize, overlay);
	
}



// 画一个十字的标记
// x：标记的X坐标
// y：标记的Y坐标
// color：标记的颜色
void HAL_LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color)
{
    /* 画点 */
		HAL_LCD_DrawRect(x - 1, y - 1, x + 1, y + 1, color);

    /* 画竖 */
		HAL_LCD_DrawLine(x - 4, y, x + 4, y, color);
	
    /* 画横 */
		HAL_LCD_DrawLine(x, y - 4, x, y + 4, color);
}


///************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
