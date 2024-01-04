#include "measurement.h"
#include <ctime>

#ifndef generic

#include "main.h"

extern RTC_HandleTypeDef hrtc;

uint32_t measurement::getTimeStamp() {
    RTC_TimeTypeDef currTime;
    RTC_DateTypeDef currDate;

    HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BIN);

    struct tm t;
    time_t t_of_day;

    // Note : handy tool for testing this is https://www.epochconverter.com/

    t.tm_year  = currDate.Year + (2000 - 1900);        // currDate.Year contains 23 io 2023
    t.tm_mon   = currDate.Month - 1;                   // currDat.Month returns 1..12 but mktime expects zero based month
    t.tm_mday  = currDate.Date;                        // Day of the month
    t.tm_hour  = currTime.Hours;
    t.tm_min   = currTime.Minutes;
    t.tm_sec   = currTime.Seconds;
    t.tm_isdst = 1;        // TODO : find out how to deal with this..
    t_of_day   = mktime(&t);

    return static_cast<uint32_t>(t_of_day);
}

#else

uint32_t measurement::getTimeStamp() {
    return 0;
}

#endif




