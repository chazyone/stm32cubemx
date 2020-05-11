#ifndef __USER_SYS_LCD_H
#define __USER_SYS_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "user_sys_lcdtype.h"


//////////////////////////////////////////////////////////////////////////////////////////
/////system
void HAL_LCD_SetShowDir(uint8_t dir);
uint16_t HAL_LCD_Init(void);
void HAL_LCD_SetBkColor(uint16_t color);
void HAL_LCD_SetFontColor(uint16_t color);
void HAL_LCD_Clear(void);	 												//«Â∆¡
int HAL_LCD_SetRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
LCD_Device_TypeDef HAL_LCD_GetDevice(void);
//////////////////////////////////////////////////////////////////////////////////////////
///////user


void HAL_LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void HAL_LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
uint16_t HAL_LCD_ReadPoint(uint16_t x, uint16_t y);
void HAL_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void HAL_LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void HAL_LCD_PutChar(uint16_t x, uint16_t y, uint8_t c, uint8_t csize, uint8_t overlay);
void HAL_LCD_Puts(uint16_t x, uint16_t y, const char *str, uint8_t csize, uint8_t overlay);
void HAL_LCD_Printf(uint16_t x, uint16_t y, uint8_t csize, uint8_t overlay, const char *format, ...);
//////////////////////////////other

void HAL_LCD_ShowSwitch(uint8_t mode);

//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif

