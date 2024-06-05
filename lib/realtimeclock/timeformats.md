# Formats

## GPS time
- seconds since 6 jan 1980 00:00
- UTC without leap seconds

## Linux / Unix time
- seconds since 1 jan 1900 00:00
- no leap seconds

## Struct tm
tm_sec	int	seconds after the minute	0-61*
tm_min	int	minutes after the hour	0-59
tm_hour	int	hours since midnight	0-23
tm_mday	int	day of the month	1-31
tm_mon	int	months since January	0-11
tm_year	int	years since 1900	
tm_wday	int	days since Sunday	0-6
tm_yday	int	days since January 1	0-365
tm_isdst	int	Daylight Saving Time flag	




## STM32 RTC time
 - a set of HW registers
   - RTC_DR
     - Bits 23:20 YT[3:0]: Year tens in BCD format
     - Bits 19:16 YU[3:0]: Year units in BCD format
     - Bits 15:13 WDU[2:0]: Week day units
        000: forbidden
        001: Monday
        ...
        111: Sunday
     - Bit 12 MT: Month tens in BCD format
     - Bits 11:8 MU[3:0]: Month units in BCD format
     - Bits 7:6 Reserved, must be kept at reset value.
     - Bits 5:4 DT[1:0]: Date tens in BCD format
     - Bits 3:0 DU[3:0]: Date units in BCD format
   - RTC_TR
     - Bit 22 PM: AM/PM notation
        0: AM or 24-hour format
        1: PM
     - Bits 21:20 HT[1:0]: Hour tens in BCD format
     - Bits 19:16 HU[3:0]: Hour units in BCD format
     - Bit 15 Reserved, must be kept at reset value.
     - Bits 14:12 MNT[2:0]: Minute tens in BCD format
     - Bits 11:8 MNU[3:0]: Minute units in BCD format
     - Bit 7 Reserved, must be kept at reset value.
     - Bits 6:4 ST[2:0]: Second tens in BCD format
     - Bits 3:0 SU[3:0]: Second units in BCD format


## STM32 HAL time
  - RTC_DateTypeDef
    - uint8_t WeekDay : monday = 1, sunday = 7
    - uint8_t Month : 0x01 to 0x12 in BCD format !!!
    - uint8_t Date : 1-31
    - uint8_t Year : 00-99 -> starting from 2000

  - RTC_TimeTypeDef
    - uint8_t Hours : 0-23
    - uint8_t Minutes : 0-59
    - uint8_t Seconds : 0-59
    - uint8_t TimeFormat : 0=12 hour format, 1=24 hour format
    - uint32_t SubSeconds : do not use
    - uint32_t SecondFraction : do not use
    - uint32_t DayLightSaving : do not use
    - uint32_t StoreOperation : do not use


# Tools
- https://timetoolsltd.com/gps/what-is-gps-time/
- handy tool for testing this is https://www.epochconverter.com/
- https://cplusplus.com/reference/ctime/tm/