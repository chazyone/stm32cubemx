#ifndef __TOUCH_H
#define __TOUCH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"


typedef struct 
{
	uint16_t x;
	uint16_t y;
  uint16_t lcdx;
  uint16_t lcdy;
} TouchTypeDef;

typedef struct{
    uint8_t posState;   
    int16_t xOffset;
    int16_t yOffset; 
    float xFactor;
    float yFactor;
} PosTypeDef;



void HAL_TOUCH_Init(void);
void HAL_TOUCH_Adjust(void);
uint8_t HAL_TOUCH_Scan(void);


TouchTypeDef HAL_TOUCH_GetTouchData(void);


#ifdef __cplusplus
}
#endif


#endif

