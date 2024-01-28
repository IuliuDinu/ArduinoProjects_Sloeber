#include <NTPClient.h>
#include "NTP_functions.h"

extern NTPClient timeClient;
extern String formattedStartupTime;
extern String startupDate;
extern String connectedTime;
extern unsigned long wifiConnectedTimeBySystemTime;


void performTimeClientSetup();
