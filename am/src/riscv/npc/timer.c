#include <am.h>

extern volatile uint64_t _ptime_uptime;

static uint64_t global_time;

static int is_leap_year(int y) {
  // Gregorian leap year rule
  return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

void __am_timer_init() {
  global_time = _ptime_uptime;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = _ptime_uptime - global_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  uint64_t rtc_us = _ptime_uptime;
  uint64_t total_sec = rtc_us / 1000000ULL; // drop sub-second part

  rtc->second = (int)(total_sec % 60);
  total_sec /= 60;
  rtc->minute = (int)(total_sec % 60);
  total_sec /= 60;
  rtc->hour   = (int)(total_sec % 24);
  total_sec /= 24;                          // now it's total days since base

  uint32_t days = (uint32_t)total_sec;

  int year = 1970;
  while (1) {
    uint32_t diy = is_leap_year(year) ? 366u : 365u;
    if (days >= diy) { days -= diy; year++; }
    else break;
  }
  rtc->year = year;

  static const uint8_t mdays_norm[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  uint8_t month = 0;
  while (month < 12) {
    uint32_t dim = mdays_norm[month];
    if (month == 1 && is_leap_year(year)) dim = 29; // February
    if (days >= dim) { days -= dim; month++; }
    else break;
  }

  rtc->month = month + 1;   // 1..12
  rtc->day   = days + 1;    // 1..31
}
