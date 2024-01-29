#include "time_client_functions.h"

void performTimeClientSetup()
{
    // Initialize a NTPClient to get time
    timeClient.begin();
    delay(100);
    // Set offset time in seconds to adjust for your timezone, for example:
    // GMT +1 = 3600
    // GMT +8 = 28800
    // GMT -1 = -3600
    // GMT 0 = 0
    //timeClient.setTimeOffset(10800); // SUMMER TIME
    timeClient.setTimeOffset(7200); // WINTER TIME

    bool syncSuccess = 0;
    syncSuccess = syncWithNTP();

    if (syncSuccess)
    {
      Serial.println("performTimeClientSetup: timeClientUpdateSuccess");
      formattedStartupTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
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
