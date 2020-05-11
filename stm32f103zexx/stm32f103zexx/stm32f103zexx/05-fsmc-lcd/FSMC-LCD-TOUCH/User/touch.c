#include "touch.h"
#include "i2c.h"
#include "spi.h"
#include "main.h"
#include "tftlcd.h"

#define TOUCH_ADJ_OK          'Y'              //��ʾ����У������׼����
#define TOUCH_ADJ_ADDR        200          //У��������24C02�е��׵�ַ200
/* ����У���������� */
#define LCD_ADJX_MIN (10)                      //��ȡ�ĸ������СXֵ
#define LCD_ADJX_MAX (HAL_LCD_GetDevice().width - LCD_ADJX_MIN) //��ȡ�ĸ�������Xֵ
#define LCD_ADJY_MIN (10)                      //��ȡ�ĸ������СYֵ
#define LCD_ADJY_MAX (HAL_LCD_GetDevice().height - LCD_ADJY_MIN) //��ȡ�ĸ�������Yֵ

#define LCD_ADJ_X (LCD_ADJX_MAX - LCD_ADJY_MIN)//��ȡ����Ŀ��
#define LCD_ADJ_Y (LCD_ADJY_MAX - LCD_ADJY_MIN)//��ȡ����ĸ߶�

#define TOUCH_READ_TIMES 40     //һ�ζ�ȡ����ֵ�Ĵ���

#define TOUCH_X_CMD      0xD0  //��ȡX������
#define TOUCH_Y_CMD      0x90  //��ȡY������
#define TOUCH_MAX        20    //Ԥ�ڲ�ֵ
#define TOUCH_X_MAX      4000  //X�����ֵ
#define TOUCH_X_MIN      100   //X����Сֵ
#define TOUCH_Y_MAX      4000  //Y�����ֵ
#define TOUCH_Y_MIN      100   //Y����Сֵ

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
	
	/* SPI���ٶȲ��˹��� */
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
	
	    /* �˲����� */
    /* ���ȴӴ�С���� */
    for(uint8_t i = 0; i < (TOUCH_READ_TIMES - 1); i++)
    {
        for(uint8_t j = i + 1; j < TOUCH_READ_TIMES; j++)
        {
            /* ����ֵ�Ӵ�С�������� */
            if(readValue[i] < readValue[j])
            {
                value = readValue[i];
								readValue[i] = readValue[j];
								readValue[j] = value;
            }
        }
    }
   
    /* ȥ�����ֵ��ȥ����Сֵ����ƽ��ֵ */
    totalValue = 0;
    for(uint8_t i = 1; i < TOUCH_READ_TIMES - 1; i++)     //��y��ȫ��ֵ
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
    
    /* �鿴������֮���ֻ����ֵ��� */
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
	
    /* �жϲ�����ֵ�Ƿ��ڿɿط�Χ�� */
		if((*xValue > TOUCH_MAX+0) || (*yValue > TOUCH_MAX+0))  
		{
			return 0xFF;
		}

    /* ��ƽ��ֵ */
    *xValue = (xValue1 + xValue2) / 2;
    *yValue = (yValue1 + yValue2) / 2;

    /* �жϵõ���ֵ���Ƿ���ȡֵ��Χ֮�� */
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

    /* ��ȡУ��������� */
		HAL_LCD_Clear();
    HAL_LCD_DrowSign(x, y, LCD_RED);
    i = 0;
    while(1)
    {
        if(!HAL_TOUCH_ReadXY(xValue, yValue))
        {
            i++;
            if(i > 10)         //��ʱһ�£��Զ�ȡ���ֵ
            {
                HAL_LCD_DrowSign(x, y, HAL_LCD_GetBkColor());
                return 0;
            }
        }
		timeCont++;
        /* ��ʱ�˳� */
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

    /* ��ȡ��һ���� */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MIN, &xPot[0], &yPot[0]))
    {
        return;
    }
    HAL_Delay(500);

    /* ��ȡ�ڶ����� */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MIN, LCD_ADJY_MAX, &xPot[1], &yPot[1]))
    {
        return;
    }
    HAL_Delay(500);

    /* ��ȡ�������� */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MIN, &xPot[2], &yPot[2]))
    {
        return;
    }
    HAL_Delay(500);

    /* ��ȡ���ĸ��� */
    if(HAL_TOUCH_ReadAdjust(LCD_ADJX_MAX, LCD_ADJY_MAX, &xPot[3], &yPot[3]))
    {
        return;
    }
    HAL_Delay(500);
    
    /* �����ȡ�����ĸ�������ݣ����ϳɶԽǵ������� */
    px[0] = (xPot[0] + xPot[1]) / 2;
    py[0] = (yPot[0] + yPot[2]) / 2;
    px[1] = (xPot[3] + xPot[2]) / 2;
    py[1] = (yPot[3] + yPot[1]) / 2;

    /* ����������� */
    xFactor = (float)LCD_ADJ_X / (px[1] - px[0]);
    yFactor = (float)LCD_ADJ_Y / (py[1] - py[0]);  
    
    /* ���ƫ���� */
    TouchAdj.xOffset = (int16_t)LCD_ADJX_MAX - ((float)px[1] * xFactor);
    TouchAdj.yOffset = (int16_t)LCD_ADJY_MAX - ((float)py[1] * yFactor);

    /* �����������������ݴ���Ȼ�󱣴� */
    TouchAdj.xFactor = xFactor ;
    TouchAdj.yFactor = yFactor ;
    
    TouchAdj.posState = TOUCH_ADJ_OK;
    Mem_WriteData(TOUCH_ADJ_ADDR, &TouchAdj.posState, sizeof(TouchAdj));            
}


uint8_t HAL_TOUCH_Scan(void)
{
    
    //if(PEN == 0)   //�鿴�Ƿ��д���
    {
        if(HAL_TOUCH_ReadXY(&TouchData.x, &TouchData.y)) //û�д���
        {
            return 0xFF;    
        }
        /* ������������ֵ���������������ֵ */
        TouchData.lcdx = TouchData.x * TouchAdj.xFactor + TouchAdj.xOffset;
        TouchData.lcdy = TouchData.y * TouchAdj.yFactor + TouchAdj.yOffset;
        
        /* �鿴��������ֵ�Ƿ񳬹�������С */
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






