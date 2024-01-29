#include "helper_functions.h"

void blinkAllLeds(byte nbOfTimes, byte period)
{
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
}

void blinkOneLed(byte load, byte nbOfTimes, byte period)
{
	for (int i=0; i<nbOfTimes; i++)
	{
		digitalWrite(load, HIGH);
		delay(period/2);
		digitalWrite(load, LOW);
		delay(period/2);
	}
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
