#include "NTP_functions.h"

bool syncWithNTP()
{
  bool timeClientUpdateSuccess = 0;
  timeClientUpdateSuccess = timeClient.update(); //This is the actual get Internet time

  if (timeClientUpdateSuccess)
    {
      localTime = (timeClient.getHours()*3600)+(timeClient.getMinutes()*60)+timeClient.getSeconds();
      localTimeMs = localTime * 1000;
      get24HMaxMillis();
      syncTime = gul_max24hMillis/1000;
    }

  return timeClientUpdateSuccess;
}

void getDateFromNTP(String &date)
{
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;
    date = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
}

void getDateFromNTPToStruct(clock_and_date_type &timestruct)
{
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;
    int weekDay = ptm->tm_wday;
    int currentHour = ptm->tm_hour;
    int currentMinute = ptm->tm_min;
    int currentSecond = ptm->tm_sec;
    timestruct.d = monthDay;
    timestruct.mo = currentMonth;
    timestruct.y = currentYear;
    timestruct.h = currentHour;
    timestruct.m = currentMinute;
    timestruct.s = currentSecond;
    timestruct.wd = weekDay;
}
