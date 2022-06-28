#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EEPROM.h>


/************* EEPROM MAP ***********
 *  0 - Reset counter (any reason)
 ***********************************/

#define ONE_DAY_IN_MILISECONDS  86400000

ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

const char* ssid = "Gorlitze";
const char* password = "A1b2C3d4";

//int LED1 = 14; //D7 ESP Board
//int LED2 = 12; //D6 ESP Board
//int LED3 = 5; //D1 ESP Board
//int LED4 = 4;  //D2 ESP Board
//int BTN1 = 5;
unsigned int REL_1 = 14;
unsigned int REL_2 = 12;
unsigned int REL_3 = 13;
//unsigned int REL_1 = 2;
//unsigned int REL_2 = 15;

int value1 = LOW;
int value2 = LOW;
int value3 = LOW;
//int value4 = LOW;

unsigned int cnt1 = 0;
unsigned int cnt2 = 0;
byte new_client = 0;
byte cmd_flashing_led1 = 0;
byte cmd_flashing_led2 = 0;
byte nbofresets = 0;
byte nbOfWifiConnectedTimes;

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

// Will keep board startup time
String formattedStartupTime;
String startupDate;
String connectedTime;
String connectedDate;

WiFiServer server(80);

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long localTime = 0;
unsigned long syncTime = 0;
unsigned long gul_max24hMillis = 0;
unsigned long wifiConnectedTimeBySystemTime;

bool gb_timeIsSynced = 0;
bool gb_timeHasToBeSynced = 1;

unsigned int gui_nbOfNTPsyncs = 0;

byte menuNumber = 0;
char *menuMessages[] = {"This is string 1", "This is string 2", "This is string 3",
                     "This is string 4", "This is string 5", "This is string 6"
                    };
char *menuLevel1Messages[] = {"1. Seteaza programe",
                              "2. blabla",
                              "3. blabla"
};

char *menuLevel2Messages[] = {"1.1 Aspersoare fata",
                              "1.2 Aspersoare spate",
                              "1.3 Irigatii legume"
};
char *menuLevel3Messages[] = {"Program nr [1]",
                              "Program nr [2]",
                              "Program nr [3]",
                              "Program nr [4]",
                              "Program nr [5]"
};

byte gu8_menuLevel1Position = 0;
byte gu8_menuLevel2Position = 0;
byte gu8_menuLevel3Position = 0;
byte gu8_menuCurrentLevel = 0;

typedef struct
{
  byte nbMaxPrograms;
  byte timeSetting[5][4];
  byte daysOfWeek;
} zoneSettings;

zoneSettings gs_zone1 = {0}, gs_zone2 = {0}, gs_zone3 = {0};

typedef struct
{
  unsigned int h;
  unsigned int m;
  unsigned int s;
} clock_type;

clock_type gs_localClock = {0};

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

bool syncWithNTP()
{
  bool timeClientUpdateSuccess = 0;
  timeClientUpdateSuccess = timeClient.update(); //This is the actual get Internet time

  if (timeClientUpdateSuccess)
    {
      localTime = (timeClient.getHours()*3600)+(timeClient.getMinutes()*60)+timeClient.getSeconds();
      get24HMaxMillis();
      syncTime = gul_max24hMillis/1000;
    }

  return timeClientUpdateSuccess;
}

void updateLocalTime()
{
  get24HMaxMillis();
  if ((gul_max24hMillis/1000) < syncTime) // after 86400 seconds
  {
    syncTime = gul_max24hMillis/1000;
    return;
  }
  localTime = localTime + (gul_max24hMillis/1000) - syncTime;
  syncTime = gul_max24hMillis/1000;
}



void setup()
{ Serial.begin(115200);
   delay(10);
   pinMode(REL_1, OUTPUT);
   pinMode(REL_2, OUTPUT);
   pinMode(REL_3, OUTPUT);

   digitalWrite(REL_1, LOW);
   digitalWrite(REL_2, LOW);
   digitalWrite(REL_3, LOW);


  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED)
  {    delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (WiFi.status() == WL_CONNECTED)
  {
    wifiConnectedTimeBySystemTime = millis()/1000;
    //Serial.println("timeClientUpdateSuccess");
    connectedTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
    Serial.print("Formatted WiFi connected Time: ");
    Serial.println(connectedTime);
    EEPROM.begin(2); //2 bytes used
    nbOfWifiConnectedTimes = EEPROM.read(1);
    nbOfWifiConnectedTimes++;
    EEPROM.write(1, nbOfWifiConnectedTimes);
    EEPROM.commit();
  }

   server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Start of OTA configuration
  ArduinoOTA.setHostname("ASP_v0.12");
  ArduinoOTA.setPassword("ototo");
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


  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(10800);

  EEPROM.begin(2); //2 bytes used
  nbofresets = EEPROM.read(0);
  //nbOfWifiConnectedTimes = EEPROM.read(1);
  nbofresets++;
  EEPROM.write(0, nbofresets);
  //nbOfWifiConnectedTimes = EEPROM.read(1);
  EEPROM.commit();
  delay(100);

  bool syncSuccess = 0;
  syncSuccess = syncWithNTP();

  if (syncSuccess)
  {
    Serial.println("timeClientUpdateSuccess");
    formattedStartupTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
    Serial.print("Formatted START-UP Time: ");
    Serial.println(formattedStartupTime);
  }

  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  startupDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);


} //end of setup()

void loop()
{
  //currentMillis = millis();
  //Serial.println(currentMillis);

/*  if ((millis()/ONE_DAY_IN_MILISECONDS) > 30)
  {
    byte nbofresets;
    nbofresets = EEPROM.read(0);
    nbofresets++;
    EEPROM.write(0, nbofresets);
    EEPROM.commit();
    ESP.restart();
  }*/

  updateLocalTime();
  convertFromSecToStructHMS(localTime, &gs_localClock);
  //if ((gs_localClock.h == 20) && (gs_localClock.m == 47) && (gs_localClock.s > 0) && (gs_localClock.s < 10))
 /* if (((localTime > 0)&&(localTime < 10)) || ((localTime > 32400)&&(localTime < 32410)) || ((localTime > 43200)&&(localTime < 43210)) || ((localTime > 54000)&&(localTime < 54010)) || ((localTime > 64800)&&(localTime < 64810)) || ((localTime > 75600)&&(localTime < 75610)))
  {
    gb_timeIsSynced = 0;
    delay(15000);
  }

  if (!gb_timeIsSynced)
  {
    syncWithNTP();
    updateLocalTime();
    gb_timeIsSynced = 1;
    gui_nbOfNTPsyncs++;
  }*/

  ArduinoOTA.handle(); // OTA usage

  WiFiClient client = server.available();

  if (!client)
  {
    Serial.print("!client ");
    Serial.println(cnt1);
    cnt1++;
    delay(500);
    //return;
  }
  else
  {
    new_client = 1;
    Serial.println("new client");
    cnt1=0;
  }


  if (new_client != 0)
  {
    while(!client.available())
    {
        //delay(1);
        Serial.print("!client.available() ");
        Serial.println(cnt2);
        cnt2++;
        delay(500);
        if (cnt2 > 60)
        {
          cnt2=0;
          break;
        }
    }


       Serial.println("---> client.available()");
       cnt2=0;

        String request = client.readStringUntil('\r');
        Serial.println(request);
        client.flush();
        //client.println("HTTP/1.1 200 OK");
        //client.println("Content-Type: text/html");
        //client.println("");

        if (request.indexOf("rel1on") != -1)
        {       digitalWrite(REL_1, HIGH);
                value1 = HIGH;
                client.println("RELAY_1 is ON");
                client.println("ASPERSOR SPATE PORNIT");
                client.println("");
        }

        if (request.indexOf("rel1off") != -1)
        {       digitalWrite(REL_1, LOW);
                value1 = LOW;
                client.println("RELAY_1 is OFF");
                client.println("ASPERSOR SPATE OPRIT");
                client.println("");
        }

        if (request.indexOf("rel2on") != -1)
        {       digitalWrite(REL_2, HIGH);
                value2 = HIGH;
                client.println("RELAY_2 is ON");
                client.println("PICURATOR LEGUME PORNIT");
                client.println("");
        }

        if (request.indexOf("rel2off") != -1)
        {       digitalWrite(REL_2, LOW);
                value2 = LOW;
                client.println("RELAY_2 is OFF");
                client.println("PICURATOR LEGUME OPRIT");
                client.println("");
        }

        if (request.indexOf("rel3on") != -1)
        {       digitalWrite(REL_3, HIGH);
                value2 = HIGH;
                client.println("RELAY_3 is ON");
                client.println("ASPERSOR FATA PORNIT");
                client.println("");
        }

        if (request.indexOf("rel3off") != -1)
        {       digitalWrite(REL_3, LOW);
                value2 = LOW;
                client.println("RELAY_3 is OFF");
                client.println("ASPERSOR FATA OPRIT");
                client.println("");
        }



        if (request.indexOf("checktime") != -1)
        {
              bool syncSuccess = 0;
              syncSuccess = syncWithNTP();

              if (syncSuccess)
              {
                Serial.println("timeClientUpdateSuccess");
                String formattedTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
                client.print("Formatted Time: ");
                client.println(formattedTime);
                client.print("localTime (s) from NTP: ");
                client.println(localTime);
                Serial.print("Formatted Time: ");
                Serial.println(formattedTime);
              }

              else
              {
                Serial.println("timeClientUpdate NO SUCCESS");
              }

        }

        if (request.indexOf("boardTime") != -1)
        {
          client.print("Board SystemTime (s): ");
          client.println(millis()/1000);
          client.print("Board WiFi connected Time (s): ");
          client.println(wifiConnectedTimeBySystemTime);

        }

        if (request.indexOf("ProgVersion") != -1)
        {
          client.println("*********************");
          client.println("aspersoare_v0.10_devb");
          client.println("Comenzi disponibile:");
          client.println("rel1on - aspersor spate");
          client.println("rel1off");
          client.println("rel2on - picurator legume");
          client.println("rel2off");
          client.println("rel3on - aspersor fata");
          client.println("rel3off");
          client.println("checktime - apeleaza NTP");
          client.println("boardTime - [sec] timp dupa reset");
          client.println("localtime - experimental");
          client.println("SystemRestart - Reset sistem");
          client.println("****************************");

        }

        if (request.indexOf("localtime") != -1)
        {
          //get24HMaxMillis();
          //localTime = localTime + (gul_max24hMillis/1000) - syncTime;
          client.print("localTime (s): ");
          client.println(localTime);
          client.print("localTime (hms): ");
          client.print(gs_localClock.h);
          client.print(":");
          client.print(gs_localClock.m);
          client.print(":");
          client.println(gs_localClock.s);
          client.println("DEBUG: ");
          client.print("gul_max24hMillis/1000: ");
          client.println(gul_max24hMillis/1000);
          client.print("syncTime: ");
          client.println(syncTime);
          client.print("millis()/1000: ");
          client.println(millis()/1000);
          client.print("gui_nbOfNTPsyncs: ");
          client.println(gui_nbOfNTPsyncs);
          client.println("END OF DEBUG: ");


          //de facut calcul si comparatie daca millis trece de 1 zi - done, se face in get24HMaxMillis()

        }

        if (request.indexOf("SystemRestart") != -1)
        {
          ESP.restart();
        }

        if (request.indexOf("BoardStartupTime") != -1)
        {
           client.print("Board START-UP Time: ");
           client.println(formattedStartupTime);
           client.print("Board START-UP Date: ");
           client.println(startupDate);
           client.print("Stored number of resets: ");
           client.println(nbofresets);
        }

        if (request.indexOf("btn_ok") != -1)
        {

          switch(gu8_menuCurrentLevel)
          {
            case 0:
              gu8_menuCurrentLevel = 1;
              client.println(menuLevel1Messages[gu8_menuLevel1Position]);
              break;
            case 1:
              gu8_menuCurrentLevel = 2;
              client.println(menuLevel2Messages[gu8_menuLevel2Position]);
              break;
            case 2:
              gu8_menuCurrentLevel = 3;
              client.println(menuLevel3Messages[gu8_menuLevel3Position]);
              break;
          }


        }

        if (request.indexOf("btn_minus") != -1)
        {
          if (gu8_menuCurrentLevel == 1)
          {
            if (gu8_menuLevel1Position > 0)
              {
                gu8_menuLevel1Position--;
              }
            client.println(menuLevel1Messages[gu8_menuLevel1Position]);
          }
          if (gu8_menuCurrentLevel == 2)
          {
            if (gu8_menuLevel2Position > 0)
              {
                gu8_menuLevel2Position--;
              }
            client.println(menuLevel2Messages[gu8_menuLevel2Position]);
          }
          if (gu8_menuCurrentLevel == 3)
          {
            if (gu8_menuLevel3Position > 0)
              {
                gu8_menuLevel3Position--;
              }
            client.println(menuLevel3Messages[gu8_menuLevel3Position]);
          }
        }

        if (request.indexOf("btn_plus") != -1)
        {

        }

        if (request.indexOf("btn_prev") != -1)
        {

        }

        if (request.indexOf("btn_next") != -1)
        {

        }






         new_client = 0;
      }





  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
