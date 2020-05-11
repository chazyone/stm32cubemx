#ifndef __TFTLCD_H
#define __TFTLCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

//////////////////////////////////////////////////////////////////////////////////////////
///typedef
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
		LCD_BROWN 	=	0XBC40, //棕色
		LCD_BRRED 	=	0XFC07, //棕红色
		LCD_GRAY  	=	0X8430 //灰色
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

//////////////////////////////////////////////////////////////////////////////////////////
/////system
void HAL_LCD_SetShowDir(uint8_t dir);
uint16_t HAL_LCD_Init(void);
void HAL_LCD_SetBkColor(uint16_t color);
void HAL_LCD_SetFontColor(uint16_t color);
uint16_t HAL_LCD_GetBkColor(void);
uint16_t HAL_LCD_GetFontColor(void);

void HAL_LCD_Clear(void);	 												//清屏
int HAL_LCD_SetRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
LCD_Device_TypeDef HAL_LCD_GetDevice(void);

//////////////////////////////////////////////////////////////////////////////////////////
/////user
///////320*240 
void HAL_LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void HAL_LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
uint16_t HAL_LCD_ReadPoint(uint16_t x, uint16_t y);
void HAL_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void HAL_LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void HAL_LCD_PutChar(uint16_t x, uint16_t y, uint8_t c, uint8_t csize, uint8_t overlay);
void HAL_LCD_Puts(uint16_t x, uint16_t y, const char *str, uint8_t csize, uint8_t overlay);
void HAL_LCD_Print(uint16_t x, uint16_t y, uint8_t csize, uint8_t overlay, const char *format, ...);
//////////////////////////////other

void HAL_LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color);
void HAL_LCD_ShowSwitch(uint8_t mode);

//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif

