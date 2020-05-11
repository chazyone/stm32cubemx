#ifndef __USER_RTC_H
#define __USER_RTC_H

#include "main.h"

void USER_RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void USER_RTC_SetDate(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday);
void USER_RTC_SetAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);

void USER_RTC_Init(void);

#endif

