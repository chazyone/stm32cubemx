#include "touch.h"
#include "i2c.h"
#include "spi.h"
#include "main.h"
#include "tftlcd.h"

#define TOUCH_ADJ_OK          'Y'              //表示触摸校正参数准备好
#define TOUCH_ADJ_ADDR        200          //校正参数在24C02中的首地址200
/* 触摸校正因数设置 */
#define LCD_ADJX_MIN (10)                      //读取四个点的最小X值
#define LCD_ADJX_MAX (HAL_LCD_GetDevice().width - LCD_ADJX_MIN) //读取四个点的最大X值
#define LCD_ADJY_MIN (10)                      //读取四个点的最小Y值
#define LCD_ADJY_MAX (HAL_LCD_GetDevice().height - LCD_ADJY_MIN) //读取四个点的最大Y值

#define LCD_ADJ_X (LCD_ADJX_MAX - LCD_ADJY_MIN)//读取方框的宽度
#define LCD_ADJ_Y (LCD_ADJY_MAX - LCD_ADJY_MIN)//读取方框的高度

#define TOUCH_READ_TIMES 40     //一次读取触摸值的次数

#define TOUCH_X_CMD      0xD0  //读取X轴命令
#define TOUCH_Y_CMD      0x90  //读取Y轴命令
#define TOUCH_MAX        20    //预期差值
#define TOUCH_X_MAX      4000  //X轴最大值
#define TOUCH_X_MIN      100   //X轴最小值
#define TOUCH_Y_MAX      4000  //Y轴最大值
#define TOUCH_Y_MIN      100   //Y轴最小值

#define TOUCH_TCS(__VALUE__) (__VALUE__?HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET):\
												HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET))

#define TOUCH_PEN ((uint8_t )HAL_GPIO_ReadPin(TOUCH_PEN_GPIO_Port, TOUCH_PEN_Pin))		


static TouchTypeDef TouchData;
static PosTypeDef TouchAdj;

void HAL_TOUCH_Init(void)
{
	Mem_ReadData(TOUCH_ADJ_ADDR, (uint8_t *)&TouchAdj, sizeof(TouchAdj));
	if(TouchAdj.posState != TOUCH_ADJ_OK)
	{
		HAL_TOUCH_Adjust();
	}
}

uint16_t HAL_TOUCH_ReadData(uint8_t cmd)
{
	uint16_t readValue[TOUCH_READ_TIMES], value;
	uint32_t totalValue;
	
	/* SPI的速度不宜过快 */
  HAL_SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_16);
	
	for(uint8_t i = 0; i < TOUCH_READ_TIMES; i++)
	{
		
		TOUCH_TCS(0);
		HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
		HAL_SPI_Receive(&hspi1, (uint8_t *)&readValue[i], 2, 1000);
		//readValue[i] = (readValue[i] << 8) | ((readValue[i] & 0x00ff) >> 8);
		readValue[i] >>= 3;
		
		TOUCH_TCS(1);
	}
	
	    /* 滤波处理 */
    /* 首先从大到小排序 */
    for(uint8_t i = 0; i < (TOUCH_READ_TIMES - 1); i++)
    {
        for(uint8_t j = i + 1; j < TOUCH_READ_TIMES; j++)
        {
            /* 采样值从大到小排序排序 */
            if(readValue[i] < readValue[j])
            {
                value = readValue[i];
								readValue[i] = readValue[j];
								readValue[j] = value;
            }
        }
    }
   
    /* 去掉最大值，去掉最小值，求平均值 */
    totalValue = 0;
    for(uint8_t i = 1; i < TOUCH_READ_TIMES - 1; i++)     //求y的全部值
    {
        totalValue += readValue[i];
    }
    value = totalValue / (TOUCH_READ_TIMES - 2);
      
    return value;
}


uint8_t HAL_TOUCH_ReadXY(uint16_t *xValue, uint16_t *yValue)
{   
    uint16_t xValue1, yValue1, xValue2, yValue2;

    xValue1 = HAL_TOUCH_ReadData(TOUCH_X_CMD);
    yValue1 = HAL_TOUCH_ReadData(TOUCH_Y_CMD);
    xValue2 = HAL_TOUCH_ReadData(TOUCH_X_CMD);
    yValue2 = HAL_TOUCH_ReadData(TOUCH_Y_CMD);
    
    /* 查看两个点之间的只采样值差距 */
    if(xValue1 > xValue2)
    {
        *xValue = xValue1 - xValue2;
    }
    else
    {
        *xValue = xValue2 - xValue1;
    }

    if(yValue1 > yValue2)
    {
        *yValue = yValue1 - yValue2;
    }
    else
    {
        *yValue = yValue2 - yValue1;
    }
	
    /* 判断采样差值是否在可控范围内 */
		if((*xValue > TOUCH_MAX+0) || (*yValue > TOUCH_MAX+0))  
		{
			return 0xFF;
		}

    /* 求平均值 */
    *xValue = (xValue1 + xValue2) / 2;
    *yValue = (yValue1 + yValue2) / 2;

    /* 判断得到的值，是否在取值范围之内 */
    if((*xValue > TOUCH_X_MAX+0) || (*xValue < TOUCH_X_MIN) 
       || (*yValue > TOUCH_Y_MAX+0) || (*yValue < TOUCH_Y_MIN))
    {                   
        return 0xFF;
    }
 
    return 0; 
}


uint8_t HAL_TOUCH_ReadAdjust(uint16_t x, uint16_t y, uint16_t *xValue, uint16_t *yValue)
{
    uint8_t i;
    uint32_t timeCont;

    /* 读取校正点的坐标 */
		HAL_LCD_Clear();
    HAL_LCD_DrowSign(x, y, LCD_RED);
    i = 0;
    while(1)
    {
        if(!HAL_TOUCH_ReadXY(xValue, yValue))
        {
            i++;
            if(i > 10)         //延时一下，以读取最佳值
            {
                HAL_LCD_DrowSign(x, y, HAL_LCD_GetBkColor());
                return 0;
            }
        }
		timeCont++;
        /* 超时退出 */
        if(timeCont > 0xFFFFFFFE)
        {   
                HAL_LCD_DrowSign(x, y, HAL_LCD_GetBkColor()); 
                return 0xFF;
        } 
    }       
}

void HAL_TOUCH_Adjust(void)
{
    uint16_t px[2], py[2], xPot[4], yPot[4];
    float xFactor, yFactor;

    /* 读取第一个点 */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MIN, &xPot[0], &yPot[0]))
    {
        return;
    }
    HAL_Delay(500);

    /* 读取第二个点 */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MAX, &xPot[1], &yPot[1]))
    {
        return;
    }
    HAL_Delay(500);

    /* 读取第三个点 */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MIN, &xPot[2], &yPot[2]))
    {
        return;
    }
    HAL_Delay(500);

    /* 读取第四个点 */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MAX, &xPot[3], &yPot[3]))
    {
        return;
    }
    HAL_Delay(500);
    
    /* 处理读取到的四个点的数据，整合成对角的两个点 */
    px[0] = (xPot[0] + xPot[1]) / 2;
    py[0] = (yPot[0] + yPot[2]) / 2;
    px[1] = (xPot[3] + xPot[2]) / 2;
    py[1] = (yPot[3] + yPot[1]) / 2;

    /* 求出比例因数 */
    xFactor = (float)LCD_ADJ_X / (px[1] - px[0]);
    yFactor = (float)LCD_ADJ_Y / (py[1] - py[0]);  
    
    /* 求出偏移量 */
    TouchAdj.xOffset = (int16_t)LCD_ADJX_MAX - ((float)px[1] * xFactor);
    TouchAdj.yOffset = (int16_t)LCD_ADJY_MAX - ((float)py[1] * yFactor);

    /* 将比例因数进行数据处理，然后保存 */
    TouchAdj.xFactor = xFactor ;
    TouchAdj.yFactor = yFactor ;
    
    TouchAdj.posState = TOUCH_ADJ_OK;
    Mem_WriteData(TOUCH_ADJ_ADDR, &TouchAdj.posState, sizeof(TouchAdj));            
}


uint8_t HAL_TOUCH_Scan(void)
{
    
    //if(PEN == 0)   //查看是否有触摸
    {
        if(HAL_TOUCH_ReadXY(&TouchData.x, &TouchData.y)) //没有触摸
        {
            return 0xFF;    
        }
        /* 根据物理坐标值，计算出彩屏坐标值 */
        TouchData.lcdx = TouchData.x * TouchAdj.xFactor + TouchAdj.xOffset;
        TouchData.lcdy = TouchData.y * TouchAdj.yFactor + TouchAdj.yOffset;
        
        /* 查看彩屏坐标值是否超过彩屏大小 */
        if(TouchData.lcdx > HAL_LCD_GetDevice().width)
        {
            TouchData.lcdx = HAL_LCD_GetDevice().width;
        }
        if(TouchData.lcdy > HAL_LCD_GetDevice().height)
        {
            TouchData.lcdy = HAL_LCD_GetDevice().height;
        }
        return 0; 
    }
 //   return 0xFF;       
}

TouchTypeDef HAL_TOUCH_GetTouchData(void)
{
	return TouchData;
}






