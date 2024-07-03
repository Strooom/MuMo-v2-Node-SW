#include <realtimeclock.hpp>
#include <buildinfo.hpp>
#include <cstring>
#include <logging.hpp>

#ifndef generic
#include "main.h"
extern RTC_HandleTypeDef hrtc;
#else
#include <ctime>
time_t mockRealTimeClock;
#endif

union realTimeClock::convert realTimeClock::convertor;
uint32_t realTimeClock::tickCounter{0};

time_t realTimeClock::unixTimeFromGpsTime(uint32_t gpsTime) {
    static constexpr uint32_t unixToGpsOffset{315964800};
    static constexpr uint32_t leapSecondsOffset{18};        // TODO : get this from nvs setting, so we can update it when needed
    return (gpsTime + unixToGpsOffset - leapSecondsOffset);
}

void realTimeClock::initialize() {
    time_t localTime = get();
    if (localTime < buildInfo::buildEpoch) {
        set();
        logging::snprintf("RTC initialized to buildTime\n");
    }
    localTime = get();
    logging::snprintf("UTC = %s", ctime(&localTime));
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

void realTimeClock::set(tm brokenDownTime) {
#ifndef generic
    RTC_TimeTypeDef stm32Time;
    RTC_DateTypeDef stm32Date;

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

uint8_t* realTimeClock::bytesFromTime_t(const time_t input) {
    convertor.asUint32 = static_cast<uint32_t>(input);
    return convertor.asBytes;
}

time_t realTimeClock::time_tFromBytes(const uint8_t* input) {
    convertor.asBytes[0] = input[0];
    convertor.asBytes[1] = input[1];
    convertor.asBytes[2] = input[2];
    convertor.asBytes[3] = input[3];
    return static_cast<time_t>(convertor.asUint32);
}

bool realTimeClock::needsSync() {
    return ((tickCounter % rtcSyncPeriodInTicks) == 0);
}