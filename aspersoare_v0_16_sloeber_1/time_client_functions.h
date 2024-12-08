#include <NTPClient.h>
#include "NTP_functions.h"

extern NTPClient timeClient;
extern String formattedStartupTime;
extern String startupDate;
extern String connectedTime;
extern unsigned long wifiConnectedTimeBySystemTime;
extern clock_and_date_type gs_current_time_and_date;
extern unsigned int prevMonth, prevDay;


void performTimeClientSetup();
