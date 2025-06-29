#include <realtimeclock.hpp>
#include <cstring>

#ifndef generic
#include "main.h"
extern RTC_HandleTypeDef hrtc;
#else
#include <ctime>
time_t realTimeClock::mockRealTimeClock{0};
#endif

uint8_t realTimeClock::asBytes[4];
uint32_t realTimeClock::tickCounter{0};

time_t realTimeClock::unixTimeFromGpsTime(const uint32_t gpsTime) {
    static constexpr uint32_t unixToGpsOffset{315964800};
    static constexpr uint32_t leapSecondsOffset{18};
    return (gpsTime + unixToGpsOffset - leapSecondsOffset);
}

void realTimeClock::initialize() {
    time_t localTime = get();
    if (localTime < buildInfo::buildEpoch) {
        set();
    }
}

void realTimeClock::set(time_t unixTime) {
    if (unixTime >= buildInfo::buildEpoch) {
#ifndef generic
        tm brokenDownTime;
        (void)gmtime_r(&unixTime, &brokenDownTime);
        set(brokenDownTime);
#else
        mockRealTimeClock = unixTime;
#endif
    }
}

void realTimeClock::set(const tm& brokenDownTime) {
#ifndef generic
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;
    stm32Time.Hours          = static_cast<uint8_t>(brokenDownTime.tm_hour);
    stm32Time.Minutes        = static_cast<uint8_t>(brokenDownTime.tm_min);
    stm32Time.Seconds        = static_cast<uint8_t>(brokenDownTime.tm_sec);
    stm32Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    stm32Time.StoreOperation = RTC_STOREOPERATION_RESET;
    stm32Date.Year           = static_cast<uint8_t>(brokenDownTime.tm_year - 100);        // tm_year is years since 1900, STM32 expects years since 2000
    stm32Date.Month          = static_cast<uint8_t>(brokenDownTime.tm_mon + 1);           // tm_mon is 0..11.  HAL expects months in BCD format, from reviewing the code it seems it also handles binary. See stm32wlxx_hal_rtc.c line 924
    stm32Date.Date           = static_cast<uint8_t>(brokenDownTime.tm_mday);
    if (brokenDownTime.tm_wday == 0) {
        stm32Date.WeekDay = 7U;
    } else {
        stm32Date.WeekDay = static_cast<uint8_t>(brokenDownTime.tm_wday);
    }
    HAL_RTC_SetTime(&hrtc, &stm32Time, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &stm32Date, RTC_FORMAT_BIN);
#endif
}

time_t realTimeClock::get() {
#ifndef generic
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
#else
    return mockRealTimeClock;
#endif
}

const uint8_t* realTimeClock::bytesFromTime_t(const time_t input) {
    auto asUint32 = static_cast<uint32_t>(input);
    std::memcpy(asBytes, &asUint32, 4);
    return asBytes;
}

time_t realTimeClock::time_tFromBytes(const uint8_t* input) {
    uint32_t asUint32;
    std::memcpy(asBytes, input, 4);
    std::memcpy(&asUint32, asBytes, 4);
    return static_cast<time_t>(asUint32);
}

bool realTimeClock::needsSync() {
    return ((tickCounter % rtcSyncPeriodInTicks) == 0);
}