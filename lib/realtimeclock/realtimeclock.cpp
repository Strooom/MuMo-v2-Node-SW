#include <realtimeclock.hpp>
#include <buildinfo.hpp>
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

    stm32Time.Hours          = buildInfo::buildHour;
    stm32Time.Minutes        = buildInfo::buildMinute;
    stm32Time.Seconds        = buildInfo::buildSecond;
    stm32Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stm32Time.StoreOperation = RTC_STOREOPERATION_RESET;
    stm32Date.Date           = buildInfo::buildDay;
    stm32Date.Month          = buildInfo::buildMonth;
    stm32Date.Year           = buildInfo::buildYear - 2000;
    if (buildInfo::buildDayOfWeek == 0) {
        stm32Date.WeekDay = 7;
    } else {
        stm32Date.WeekDay = (buildInfo::buildDayOfWeek);
    }

    HAL_RTC_SetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);
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
    stm32Date.Year           = brokenDownTime.tm_year - 100;        // tm_year is years since 1900, STM32 expects years since 2000
    stm32Date.Month          = brokenDownTime.tm_mon + 1;           // tm_mon is 0..11 TODO : check if this is correct as it seems HAL uses BCD months...
    stm32Date.Date           = brokenDownTime.tm_mday;
    if (brokenDownTime.tm_wday == 0) {
        stm32Date.WeekDay = 7;
    } else {
        stm32Date.WeekDay = brokenDownTime.tm_wday;
    }

    // HAL_PWR_EnableBkUpAccess(); This created a bug where the RTC would be set but not running - for further investigation
    HAL_RTC_SetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);
    // HAL_PWR_DisableBkUpAccess(); This created a bug where the RTC would be set but not running - for further investigation
}

time_t realTimeClock::get() {
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;

    HAL_RTC_GetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);        // You must call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values in the higher-order calendar shadow registers to ensure consistency between the time and date values. Reading RTC current time locks the values in calendar shadow registers until Current date is read to ensure consistency between the time and date values

    struct tm brokenDownTime;
    time_t now;

    brokenDownTime.tm_year = stm32Date.Year + (2000 - 1900);        // currDate.Year contains 23 io 2023
    brokenDownTime.tm_mon  = stm32Date.Month - 1;                   // currDat.Month returns 1..12 but mktime expects zero based month
    brokenDownTime.tm_mday = stm32Date.Date;                        // Day of the month
    brokenDownTime.tm_hour = stm32Time.Hours;
    brokenDownTime.tm_min  = stm32Time.Minutes;
    brokenDownTime.tm_sec  = stm32Time.Seconds;
    now                    = mktime(&brokenDownTime);

    return now;
}
