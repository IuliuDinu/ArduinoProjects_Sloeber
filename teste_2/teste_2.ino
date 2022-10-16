/*
  Rui Santos
  Complete project details at our blog.
    - ESP32: https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
    - ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based in the RTDB Basic Example by Firebase-ESP-Client library by mobizt
  https://github.com/mobizt/Firebase-ESP-Client/blob/main/examples/RTDB/Basic/Basic.ino
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#include <NTPClient.h>
#include <ArduinoOTA.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
//#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
//#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

//const char* ssid = "Gorlitze";
//const char* password = "A1b2C3d4";
const char* ssid = "Gorlitze_etaj";
const char* password = "A1b2C3d4";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

// Insert Firebase project API Key
#define API_KEY "AIzaSyCRgAcqnRgAv7ZxruTVZaOaOtZ26ZvAHUk"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp8266-firebase-demo-1-default-rtdb.europe-west1.firebasedatabase.app/"

#define USER_EMAIL "inginer.a.dinu@gmail.com"
#define USER_PASSWORD "tacapaca"

#define DEVBABY1

#define EEPROM_TOTAL_NB_OF_DEFINED_BYTES       8

#define ONE_DAY_IN_MILISECONDS  86400000
#define MAX_WIFI_DISC_LOOP_COUNTER 60

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int intValue = 0;
int gpio1 = 0;
String text1 = "";

unsigned int REL_1 = 14;
unsigned int REL_2 = 12;
unsigned int REL_3 = 13;

String formattedStartupTime;
String startupDate;
String connectedTime;
String connectedDate;
u32_t connectedIP;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long localTime = 0;
unsigned long syncTime = 0;
unsigned long gul_max24hMillis = 0;
unsigned long wifiConnectedTimeBySystemTime;

bool gb_timeIsSynced = 0;
bool gb_timeHasToBeSynced = 1;

unsigned int gui_nbOfNTPsyncs = 0;
unsigned int wifiWaitCounter = 0;
unsigned int wifiDisconnectedLoopCounter = 0;
unsigned int wifiDisconnectedCounter = 0;
//unsigned int wifiConnectionAttempts = 0;

bool syncWithNTP()
{
  bool timeClientUpdateSuccess = 0;
  timeClientUpdateSuccess = timeClient.update(); //This is the actual get Internet time

  // if (timeClientUpdateSuccess)
  //   {
  //     localTime = (timeClient.getHours()*3600)+(timeClient.getMinutes()*60)+timeClient.getSeconds();
  //     get24HMaxMillis();
  //     syncTime = gul_max24hMillis/1000;
  //   }

  return timeClientUpdateSuccess;
}

void setup(){


    Serial.begin(115200);
   delay(100);
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
  Serial.println(" ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {    delay(5000);
      wifiWaitCounter++;
      Serial.print(wifiWaitCounter);
      Serial.print(", ");
      if (wifiWaitCounter > 100)
      {
          ESP.restart();
      }
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (WiFi.status() == WL_CONNECTED)
  {

        digitalWrite(REL_1, HIGH);
        digitalWrite(REL_2, HIGH);
        digitalWrite(REL_3, HIGH);

        // Initialize a NTPClient to get time
        timeClient.begin();
        delay(100);
        // Set offset time in seconds to adjust for your timezone, for example:
        // GMT +1 = 3600
        // GMT +8 = 28800
        // GMT -1 = -3600
        // GMT 0 = 0
        timeClient.setTimeOffset(10800);

        bool syncSuccess = 0;
        syncSuccess = syncWithNTP();

        if (syncSuccess)
        {
          Serial.println("Setup: timeClientUpdateSuccess");
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


      wifiConnectedTimeBySystemTime = millis()/1000;
      //Serial.println("timeClientUpdateSuccess");
      connectedTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
      Serial.print("Formatted WiFi connected Time: ");
      Serial.println(connectedTime);
      // EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used
      // delay(100);
      // nbOfWifiConnectedTimes = EEPROM.read(EEPROM_ADDR_WIFI_CONN_COUNTER);
      // nbOfWifiConnectedTimes++;
      // delay(100);
      // EEPROM.write(EEPROM_ADDR_WIFI_CONN_COUNTER, nbOfWifiConnectedTimes);
      // delay(100);
      // if (EEPROM.commit())
      // {
      //   Serial.println("EEPROM has been updated.");
      // }
      // else
      // {
      //   Serial.println("EEPROM commit failed.");
      // }
      //   delay(100);
    }
    else
    {
      Serial.println("WIFI != CONNECTED, pula mea");
    }

    Serial.println(WiFi.localIP());
  connectedIP = WiFi.localIP();
  Serial.print("int connectedIP = ");
  Serial.println(connectedIP);
  Serial.println(IPAddress(connectedIP));

#ifdef DEVBABY1
 /* if (connectedIP != 1023518912) // which means 192.168.1.61
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }*/
#endif

#ifdef DEVBABY1
  ArduinoOTA.setHostname("DEVBABY1");
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

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);



    /* Sign up */
  /*if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("signup ok");
    signupOK = true;
  }
  else{
    Serial.println("signup NOT ok");
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }*/

  if (Firebase.ready())
    Serial.println("Firebase.ready() - YES");
  else
    Serial.println("Firebase.ready() - NO");

    // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

}

void loop(){

if (!WiFi.isConnected())
  {
    Serial.println("WiFi was disconnected");
    if (wifiDisconnectedLoopCounter > MAX_WIFI_DISC_LOOP_COUNTER)
      {
        //save EEPROM REL values - neah, already done when set on/off
        ESP.restart();
      }
    wifiDisconnectedLoopCounter++;
  }
  else
  {
    if (wifiDisconnectedLoopCounter)
      {
        wifiDisconnectedLoopCounter = 0;
        wifiDisconnectedCounter++;
      }
  }

    ArduinoOTA.handle(); // OTA usage

  signupOK = 1;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    //Serial.println("Firebase ready");
    //Serial.print("signupOK= ");
    //Serial.println(signupOK);
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      digitalWrite(REL_2, !digitalRead(REL_2));
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      digitalWrite(REL_3, !digitalRead(REL_3));
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }


    // Read an Int number from the database path test/gigel
    if (Firebase.RTDB.getInt(&fbdo, "/test/gigel")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        Serial.print("gigel= ");
        Serial.println(intValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }

    // Read an Int number from the database path bambam/gpio1
    if (Firebase.RTDB.getInt(&fbdo, "/bambam/gpio1")) {
      if (fbdo.dataType() == "int") {
        gpio1 = fbdo.intData();
        Serial.print("gpio1= ");
        Serial.println(gpio1);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }

    // Write an Int number on the database path bambam/gpio1
    if (Firebase.RTDB.setInt(&fbdo, "bambam/gpio1", gpio1+3)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Read a String variable from the database path bambam/text1
    if (Firebase.RTDB.getInt(&fbdo, "/bambam/text1")) {
      if (fbdo.dataType() == "string") {
        text1 = fbdo.stringData();
        Serial.print("text1= ");
        Serial.println(text1);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }


  }

  else
  {
    //Serial.println("Firebase NOT ready OR signupOK=FALSE");
  }
}
