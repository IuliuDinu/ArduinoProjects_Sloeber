#include "helper_functions.h"

void blinkAllLeds(byte nbOfTimes, byte period)
{
#ifdef LEDS_DEBUG_MODE
	for (int i=0; i<nbOfTimes; i++)
	{
		digitalWrite(REL_1, HIGH);
		delay(period/2);
		digitalWrite(REL_1, LOW);
		delay(period/2);
		digitalWrite(REL_2, HIGH);
		delay(period/2);
		digitalWrite(REL_2, LOW);
		delay(period/2);
		digitalWrite(REL_3, HIGH);
		delay(period/2);
		digitalWrite(REL_3, LOW);
		delay(period/2);
	}
#endif
}

void blinkOneLed(byte load, byte nbOfTimes, byte period)
{
#ifdef LEDS_DEBUG_MODE
	for (int i=0; i<nbOfTimes; i++)
	{
		digitalWrite(load, HIGH);
		delay(period/2);
		digitalWrite(load, LOW);
		delay(period/2);
	}
#endif
}

void resetAllLoads()
{
	rel1_status = LOW;
	rel2_status = LOW;
	rel3_status = LOW;
	digitalWrite(REL_1, rel1_status);
	digitalWrite(REL_2, rel2_status);
	digitalWrite(REL_3, rel3_status);
}

void convertFromSecToStructHMS(unsigned long ul_sec, clock_type *hms_var)
{
  hms_var->h = 0;
  hms_var->m = 0;
  hms_var->s = 0;

  if (ul_sec > 3600)
  {
    hms_var->h = ul_sec/3600;

    if (hms_var->h >= 24)
    {
      hms_var->h -=24;
    }

    ul_sec -= (hms_var->h*3600);
  }

  if (ul_sec > 60)
  {
    hms_var->m = ul_sec/60;
    ul_sec -= (hms_var->m*60);
  }

  hms_var->s = ul_sec;
}

void get24HMaxMillis()
{
  gul_max24hMillis = millis();
  if (gul_max24hMillis > ONE_DAY_IN_MILISECONDS)
  {
    unsigned long multiplier = gul_max24hMillis/ONE_DAY_IN_MILISECONDS;
    gul_max24hMillis -= (multiplier*ONE_DAY_IN_MILISECONDS);
  }
}

void serverBegin()
{
   server.begin();
   Serial.println("Server started");
   // Print the IP address
   Serial.print("Use this URL to connect: ");
   Serial.print("http://");
   Serial.println(WiFi.localIP());
   connectedIP = WiFi.localIP();
   Serial.print("int connectedIP = ");
   Serial.println(connectedIP);
   Serial.println(IPAddress(connectedIP));
}

void OTASetup()
{

  // Start of OTA configuration
#ifdef DEVBABY1
  ArduinoOTA.setHostname("DEVBABY1");
  ArduinoOTA.setPassword("ototo");
#endif
#ifdef ASP
  ArduinoOTA.setHostname("ASP");
  ArduinoOTA.setPassword("ototo");
#endif
#ifdef DEVBIG
  ArduinoOTA.setHostname("DEVBIG");
  ArduinoOTA.setPassword("ototo");
#endif
#ifdef ESPBOX1
  ArduinoOTA.setHostname("ESPBOX1");
  ArduinoOTA.setPassword("ototo");
#endif
#ifdef ESPBOX2
  ArduinoOTA.setHostname("ESPBOX2");
  ArduinoOTA.setPassword("ototo");
#endif

  ArduinoOTA.onStart([]()
  {
	Serial.println("OTA_Start");
  });

  ArduinoOTA.onEnd([]()
  {
	Serial.println("\nOTA_End");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
	  Serial.printf("OTA_Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
	Serial.printf("OTA_Error[%u]: ", error);
	if (error == OTA_AUTH_ERROR) Serial.println("OTA_Auth Failed");
	else if (error == OTA_BEGIN_ERROR) Serial.println("OTA_Begin Failed");
	else if (error == OTA_CONNECT_ERROR) Serial.println("OTA_Connect Failed");
	else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA_Receive Failed");
	else if (error == OTA_END_ERROR) Serial.println("OTA_End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA ready");
  // End of OTA configuration
}

void checkCorrectIPObtained()
{
#ifdef DEVBABY1x // we use multiple connection options for this devboard
  if (connectedIP != 1023518912) // which means 192.168.1.61 - for TPLink
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef DEVBABY1 // we use multiple connection options for this devboard
  if (connectedIP != 1023519168) // which means 192.169.1.61 - for Kaon
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef ASPx
  if (connectedIP != 1040296128) // which means 192.168.1.62
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef ASP
  if (connectedIP != 1040296384) // which means 192.169.1.62 - for Kaon
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef DEVBIG
  if (connectedIP != 1006741952) // which means 192.169.1.60 - for Kaon
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    //ESP.restart();												// TBD: to uncomment and move this entire section elsewhere
  }
#endif

#ifdef ESPBOX1x
  if (connectedIP != 1090627776) // which means 192.168.1.65
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef ESPBOX1
  if (connectedIP != 1090628032) // which means 192.169.1.65 - for Kaon
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef ESPBOX2x
  if (connectedIP != 1107404992) // which means 192.168.1.66
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

#ifdef ESPBOX2
  if (connectedIP != 1107404992) // which means 192.169.1.66 - for Kaon
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    //ESP.restart();
  }
#endif
}
