/**
  ******************************************************************************
  * File Name          : user_lcdbase.h
  * Description        : This file contains all the functions prototypes for 
  *                      the user_lcdbase  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_SYS_LCDTYPE_H
#define __USER_SYS_LCDTYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"

typedef struct
{
    volatile uint16_t Cmd;
    volatile uint16_t Data;
}LCD_Reg_TypeDef;

typedef struct
{
	volatile uint16_t width;
	volatile uint16_t height;
	volatile uint16_t id;
	volatile uint8_t 	dir;
}LCD_Device_TypeDef;

//pen color
typedef enum
{
		LCD_WHITE		=	0xFFFF,
		LCD_BLACK		=	0x0000,	  
		LCD_BLUE   	=	0x001F,
		LCD_BRED   	=	0XF81F,
		LCD_GRED 		=	0XFFE0,
		LCD_GBLUE		=	0X07FF,
		LCD_RED    	=	0xF800,
		LCD_MAGENTA	=	0xF81F,
		LCD_GREEN  	=	0x07E0,
		LCD_CYAN   	=	0x7FFF,
		LCD_YELLOW	=	0xFFE0,
		LCD_BROWN 	=	0XBC40, //×ØÉ«
		LCD_BRRED 	=	0XFC07, //×ØºìÉ«
		LCD_GRAY  	=	0X8430 //»ÒÉ«
}LCD_ColorTyp;

typedef struct
{
	LCD_Reg_TypeDef *Lcd_Reg;
	LCD_Device_TypeDef Lcd_Dev;
	LCD_ColorTyp bkColor;
	LCD_ColorTyp fontColor;
	void (*WriteCmd)(uint16_t);
	void (*WriteData)(uint16_t);
	uint16_t (*ReadData)(void);
	void (*WriteValue)(uint16_t, uint16_t );
	uint16_t (*ReadValue)(uint16_t);
	void (*WriteColor)(uint16_t);
}LCD_TypeDef;

#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
