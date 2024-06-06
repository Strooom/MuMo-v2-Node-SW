// ######################################################################################
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ###
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ###
// ######################################################################################

#include <time.hpp>

#ifndef generic
#include <main.h>
extern RTC_HandleTypeDef hrtc;
#endif

uint32_t applicationTime::rtcTicksSinceLastUpdate{rtcTicksBetweenUpdate + 1};

void applicationTime::tick() {
    rtcTicksSinceLastUpdate++;
}

bool applicationTime::needsUpdate() {
    return (rtcTicksSinceLastUpdate >= rtcTicksBetweenUpdate);
}

void applicationTime::update(time_t unixTime) {
    struct tm* timeInfo;
    timeInfo = gmtime(&unixTime);

#ifndef generic
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    sTime.Hours           = timeInfo->tm_hour;
    sTime.Minutes         = timeInfo->tm_min;
    sTime.Seconds         = timeInfo->tm_sec;
    sTime.DayLightSaving  = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation  = RTC_STOREOPERATION_RESET;
    if (timeInfo->tm_wday == 0) {
        timeInfo->tm_wday = 7;
    }        // the chip goes mon tue wed thu fri sat sun
    sDate.WeekDay = timeInfo->tm_wday;
    sDate.Month   = timeInfo->tm_mon + 1;
    sDate.Date    = timeInfo->tm_mday;
    sDate.Year    = timeInfo->tm_year - 100;        // time.h is years since 1900, STM32-RTC is years since 2000

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        Error_Handler();
    }
#endif
}

void updateRTC(time_t now) {
#ifndef generic
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    struct tm time_tm;        // https://cplusplus.com/reference/ctime/tm/
    time_tm = *(localtime(&now));

    sTime.Hours   = (uint8_t)time_tm.tm_hour;
    sTime.Minutes = (uint8_t)time_tm.tm_min;
    sTime.Seconds = (uint8_t)time_tm.tm_sec;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    // time_tm.tm_wday is 0..6 for sunday to saturday
    // rtc.WeekDay is 1..7 for monday to sunday

    if (time_tm.tm_wday == 0) {
        time_tm.tm_wday = 7;
    }
    sDate.WeekDay = (uint8_t)time_tm.tm_wday;
    sDate.Month   = (uint8_t)time_tm.tm_mon + 1;                      // tm_mon goes 0..11 for jan to dec  rtc.Month goes 1..12
    sDate.Date    = (uint8_t)time_tm.tm_mday;                         // tm_mday goes 1..31  rtc.Date goes 1..31
    sDate.Year    = (uint16_t)(time_tm.tm_year + 1900 - 2000);        // tm_year is years since 1900, rtc.Year is years since 2000

    /*
     * update the RTC
     */
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);        // lock it in with the backup registers
#endif
}

void getRTC() {
#ifndef generic
    RTC_DateTypeDef rtcDate;
    RTC_TimeTypeDef rtcTime;
    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);

    uint8_t hh         = rtcTime.Hours;
    uint8_t mm         = rtcTime.Minutes;
    uint8_t ss         = rtcTime.Seconds;
    uint8_t d          = rtcDate.Date;
    uint8_t m          = rtcDate.Month;
    uint16_t y         = rtcDate.Year;
    uint16_t yr        = (uint16_t)(y + 2000 - 1900);
    time_t currentTime = {0};
    struct tm tim      = {0};
    tim.tm_year        = yr;
    tim.tm_mon         = m - 1;
    tim.tm_mday        = d;
    tim.tm_hour        = hh;
    tim.tm_min         = mm;
    tim.tm_sec         = ss;
    currentTime        = mktime(&tim);
    struct tm printTm  = {0};
    printTm            = *(localtime(&currentTime));
#endif
}