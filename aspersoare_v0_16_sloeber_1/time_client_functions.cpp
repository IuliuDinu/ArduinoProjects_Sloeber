#include "time_client_functions.h"
#include "NTP_functions.h"

void performTimeClientSetup()
{
    // Initialize a NTPClient to get time
	int timeOffset = 0;
    timeClient.begin();
    delay(100);
    currentDaylightSavingStatus = getEeprom_DaylightSavingStatus();
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    //timeClient.setTimeOffset(10800); // SUMMER TIME
    //timeClient.setTimeOffset(7200); // WINTER TIME
    if (currentDaylightSavingStatus == TRUE)
    {
    	timeOffset = 7200;	// winter time
    }
    else
    {
    	timeOffset = 10800;	// summer time
    }
    timeClient.setTimeOffset(timeOffset);

    bool syncSuccess = 0;
    syncSuccess = syncWithNTP();

    if (syncSuccess)
    {
      Serial.println("performTimeClientSetup: timeClientUpdateSuccess");
      formattedStartupTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
      getDateFromNTPToStruct(gs_current_time_and_date);
      prevMonth = gs_current_time_and_date.mo;
      prevDay = gs_current_time_and_date.d;
      Refresh_sunsetTime(&todayTimeForLampStart);
      Serial.print("Formatted START-UP Time: ");
      Serial.println(formattedStartupTime);
    }

    getDateFromNTP(startupDate);


  wifiConnectedTimeBySystemTime = millis()/1000;
  //Serial.println("timeClientUpdateSuccess");
  connectedTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
  Serial.print("Formatted WiFi connected Time: ");
  Serial.println(connectedTime);
}
