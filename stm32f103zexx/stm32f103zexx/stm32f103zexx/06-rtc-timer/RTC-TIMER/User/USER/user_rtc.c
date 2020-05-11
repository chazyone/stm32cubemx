#include "user_rtc.h"
#include "rtc.h"

void USER_RTC_Init(void)
{
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x02)
	{
		USER_RTC_SetTime(0, 0, 0);
		USER_RTC_SetDate(20, 3, 31, 1);//不可直接备份 可保存在备份区中
		USER_RTC_SetAlarm(0, 1, 0);//不可直接备份 可保存在备份区中
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x02);
		
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR11, (uint16_t)20);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR12, (uint16_t)3);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR13, (uint16_t)28);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR14, (uint16_t)6);
	}
	else
	{
		RTC_DateTypeDef mDate = {0};
		mDate.Year = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR11);
		mDate.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR12);
		mDate.Date = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR13);
		mDate.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR14);
		
		USER_RTC_SetDate(mDate.Year, mDate.Month, mDate.Date, mDate.WeekDay);
	}
}

void USER_RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_TimeTypeDef mTime = {0};
	mTime.Hours = hours;
	mTime.Minutes = minutes;
	mTime.Seconds = seconds;
		
	if (HAL_RTC_SetTime(&hrtc, &mTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

void USER_RTC_SetDate(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday)
{
	RTC_DateTypeDef mDate = {0};
	mDate.Year = year;
	mDate.Month = month;
	mDate.WeekDay = weekday;
	mDate.Date = date;
		
	if (HAL_RTC_SetDate(&hrtc, &mDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

void USER_RTC_SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_AlarmTypeDef mAlarm = {0};
	mAlarm.Alarm = RTC_ALARM_A;
	mAlarm.AlarmTime.Hours = hours;
	mAlarm.AlarmTime.Minutes = minutes;
	mAlarm.AlarmTime.Seconds = seconds;
		
	if (HAL_RTC_SetAlarm(&hrtc, &mAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
	
	
	HAL_RTC_SetAlarm_IT(&hrtc, &mAlarm, RTC_FORMAT_BIN);
}


