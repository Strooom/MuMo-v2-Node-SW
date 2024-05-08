#include <realtimeclock.hpp>
#include <cstring>

#ifndef generic
#include "main.h"
extern RTC_HandleTypeDef hrtc;
#endif

time_t realTimeClock::unixTimeFromGpsTime(uint32_t gpsTime) {
    static constexpr uint32_t unixToGpsOffset{315964800};
    static constexpr uint32_t leapSecondsOffset{18};
    return (gpsTime + unixToGpsOffset - leapSecondsOffset);
}

void realTimeClock::set() {
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;

    char timeString[16]      = __TIME__;        // eg "10:20:30"
    stm32Time.Hours          = (timeString[0] - '0') * 10 + (timeString[1] - '0');
    stm32Time.Minutes        = (timeString[3] - '0') * 10 + (timeString[4] - '0');
    stm32Time.Seconds        = (timeString[6] - '0') * 10 + (timeString[7] - '0');
    stm32Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stm32Time.StoreOperation = RTC_STOREOPERATION_RESET;

    char dateString[16] = __DATE__;        // eg "Feb 12 2024"
    stm32Date.Date      = (dateString[4] - '0') * 10 + (dateString[5] - '0');

    if (strncmp("Jan", dateString, 3) == 0) {
        stm32Date.Month = 1;
    } else if (strncmp("Feb", dateString, 3) == 0) {
        stm32Date.Month = 2;
    } else if (strncmp("Mar", dateString, 3) == 0) {
        stm32Date.Month = 3;
    } else if (strncmp("Apr", dateString, 3) == 0) {
        stm32Date.Month = 4;
    } else if (strncmp("May", dateString, 3) == 0) {
        stm32Date.Month = 5;
    } else if (strncmp("Jun", dateString, 3) == 0) {
        stm32Date.Month = 6;
    } else if (strncmp("Jul", dateString, 3) == 0) {
        stm32Date.Month = 7;
    } else if (strncmp("Aug", dateString, 3) == 0) {
        stm32Date.Month = 8;
    } else if (strncmp("Sep", dateString, 3) == 0) {
        stm32Date.Month = 9;
    } else if (strncmp("Oct", dateString, 3) == 0) {
        stm32Date.Month = 10;
    } else if (strncmp("Nov", dateString, 3) == 0) {
        stm32Date.Month = 11;
    } else if (strncmp("Dec", dateString, 3) == 0) {
        stm32Date.Month = 12;
    }
    stm32Date.Year    = (dateString[9] - '0') * 10 + (dateString[10] - '0');
    stm32Date.WeekDay = 1;        // I don't know how to get the day of the week from the date string

    HAL_PWR_EnableBkUpAccess();
    HAL_RTC_SetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);
    HAL_PWR_DisableBkUpAccess();
}

void realTimeClock::set(time_t unixTime) {
    tm brokenDownTime;
    (void)gmtime_r(&unixTime, &brokenDownTime);
    set(brokenDownTime);
}

void realTimeClock::set(tm brokenDownTime) {
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;

    // TODO : I could check if the members of brokenDownTime are within the correct ranges for the STM32WLE5

    stm32Time.Hours          = brokenDownTime.tm_hour;
    stm32Time.Minutes        = brokenDownTime.tm_min;
    stm32Time.Seconds        = brokenDownTime.tm_sec;
    stm32Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stm32Time.StoreOperation = RTC_STOREOPERATION_RESET;
    stm32Date.Year  = brokenDownTime.tm_year - 100;        // tm_year is years since 1900, STM32 expects years since 2000
    stm32Date.Month = brokenDownTime.tm_mon + 1;           // tm_mon is 0..11 TODO : check if this is correct as it seems HAL uses BCD months...
    stm32Date.Date  = brokenDownTime.tm_mday;
    if (brokenDownTime.tm_wday == 0) {
        stm32Date.WeekDay = 7;
    } else {
        stm32Date.WeekDay = brokenDownTime.tm_wday;
    }

    HAL_PWR_EnableBkUpAccess();
    HAL_RTC_SetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);
    HAL_PWR_DisableBkUpAccess();
}

time_t realTimeClock::get() {
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;

    HAL_RTC_GetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);        // You must call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values in the higher-order calendar shadow registers to ensure consistency between the time and date values. Reading RTC current time locks the values in calendar shadow registers until Current date is read to ensure consistency between the time and date values

    struct tm brokenDownTime;
    time_t now;

    brokenDownTime.tm_year  = stm32Date.Year + (2000 - 1900);        // currDate.Year contains 23 io 2023
    brokenDownTime.tm_mon   = stm32Date.Month - 1;                   // currDat.Month returns 1..12 but mktime expects zero based month
    brokenDownTime.tm_mday  = stm32Date.Date;                        // Day of the month
    brokenDownTime.tm_hour  = stm32Time.Hours;
    brokenDownTime.tm_min   = stm32Time.Minutes;
    brokenDownTime.tm_sec   = stm32Time.Seconds;
    now                     = mktime(&brokenDownTime);

    return now;
}
