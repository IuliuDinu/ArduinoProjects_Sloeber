#include "Arduino.h"
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
//#include <WiFiUdp.h>
#include <EEPROM.h>


/************* EEPROM MAP ***********
 *  0 - Reset counter (any reason)
 *  1 - WiFi connection counter
 *  2 - REL_1 last state
 *  3 - REL_1 last state (redundancy)
 *  4 - REL_2 last state
 *  5 - REL_2 last state (redundancy)
 *  6 - REL_3 last state
 *  7 - REL_3 last state (redundancy)
 ***********************************/
#define EEPROM_ADDR_MAX									4095
#define EEPROM_ADDR_RST_COUNTER                			0
#define EEPROM_ADDR_WIFI_CONN_COUNTER          			1
#define EEPROM_ADDR_REL_1_LAST_STATE           			2
#define EEPROM_ADDR_REL_1_LAST_STATE_INV       			3
#define EEPROM_ADDR_REL_2_LAST_STATE           			4
#define EEPROM_ADDR_REL_2_LAST_STATE_INV       			5
#define EEPROM_ADDR_REL_3_LAST_STATE           			6
#define EEPROM_ADDR_REL_3_LAST_STATE_INV       			7
#define EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS	   			8
#define EEPROM_ADDR_MENU_NB_SCH_DAILY					9
#define EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS			10
#define EEPROM_ADDR_MENU_NB_SCH_ONETIME					11
#define EEPROM_ADDR_MENU_IN_PROGRESS		   			12
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED		13
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY	14
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR	15
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN	16
#define EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC	17

 #define EEPROM_TOTAL_NB_OF_DEFINED_BYTES       18

#define ONE_DAY_IN_MILISECONDS  86400000
#define MAX_WIFI_DISC_LOOP_COUNTER 60
//#define TIME_INTERVAL_TO_RECHECK_NTP	300000 // 300000ms = 5 min
#define TIME_INTERVAL_TO_RECHECK_NTP	900000 // 900000ms = 15 min
//#define TIME_INTERVAL_TO_RECHECK_NTP	60000 // 60000ms = 1 min
#define AVG_MEASURED_MAIN_LOOP_DURATION_MS	502

#define TRUE 1
#define FALSE 0

/************* MENU MAP *****************************************
 * "meniu_1" - O tura aspersor spate 15 min	[DONE]
 * "meniu_2" - O tura aspersor spate 20 min [DONE]
 * "meniu_3" - O tura aspersor spate 25 min [DONE]
 * "meniu_4" - O tura aspersor spate 30 min [DONE]
 * "meniu_5" - O tura aspersor fata 15 min [DONE]
 * "meniu_6" - O tura aspersor fata 20 min [DONE]
 * "meniu_7" - O tura aspersor fata 25 min [DONE]
 * "meniu_8" - O tura aspersor fata 30 min [DONE]
 * "meniu_9" - O tura picurator gradina 15 min [DONE]
 * "meniu_10" - O tura picurator gradina 20 min [DONE]
 * "meniu_11" - O tura picurator gradina 25 min [DONE]
 * "meniu_12" - O tura picurator gradina 30 min [DONE]
 * "meniu_13" - O tura aspersor spate, apoi fata, cate 15 min
 * "meniu_14" - O tura aspersor spate, apoi fata, cate 20 min
 * "meniu_15" - O tura aspersor spate, apoi fata, cate 25 min
 * "meniu_16" - O tura aspersor spate, apoi fata, cate 30 min
 * "meniu_17" - O tura aspersor fata, apoi spate, apoi picurator cate 15 min
 * "meniu_18" - O tura aspersor fata, apoi spate, apoi picurator cate 20 min
 * "meniu_19" - O tura aspersor fata, apoi spate, apoi picurator cate 25 min
 * "meniu_20" - O tura aspersor fata, apoi spate, apoi picurator cate 30 min
 * "meniu_21" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 15 min [DONE]
 * "meniu_22" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 20 min [DONE]
 * "meniu_23" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 25 min [DONE]
 * "meniu_24" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 30 min [DONE]
 * "meniu_25" - O tura programata la 05:00 aspersor spate, 30 min
 * "meniu_30" - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 15 min [DONE]
 * "meniu_31" - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 20 min [DONE]
 * "meniu_32" - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 25 min [DONE]
 * "meniu_33" - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 30 min [DONE]
 * "meniu_35" - O tura programata la XX whatever ora, aspersor spate, apoi picurator, apoi fata cate 5 min
 * "meniu_36" - O tura programata la XX:XX aspersor spate, 5 min
 * "meniu_26" - O tura programata la orele X pana Y, lampa SPATE (rel_2)
 * "meniu_30" - O tura programata de la ora X pana la ora Y, o singura data, pt LED_1 (rel_1)
****************************************************************/
#define MENIU_1_DURATION				900
#define MENIU_2_DURATION				1200
#define MENIU_3_DURATION				1500
#define MENIU_4_DURATION				1800
#define MENIU_5_DURATION				900
#define MENIU_6_DURATION				1200
#define MENIU_7_DURATION				1500
#define MENIU_8_DURATION				1800
#define MENIU_9_DURATION				900
#define MENIU_10_DURATION				1200
#define MENIU_11_DURATION				1500
#define MENIU_12_DURATION				1800
#define MENIU_21_LOCALTIME_START		18000
#define MENIU_21_DURATION				900
#define MENIU_22_LOCALTIME_START		18000
#define MENIU_22_DURATION				1200
#define MENIU_23_LOCALTIME_START		18000
#define MENIU_23_DURATION				1500
#define MENIU_24_LOCALTIME_START		18000
#define MENIU_24_DURATION				1800
#define MENIU_25_LOCALTIME_START		18000
#define MENIU_25_DURATION				1800
#define MENIU_30_LOCALTIME_START		18000
#define MENIU_30_DURATION				900
#define MENIU_31_LOCALTIME_START		18000
#define MENIU_31_DURATION				1200
#define MENIU_32_LOCALTIME_START		18000
#define MENIU_32_DURATION				1500
#define MENIU_33_LOCALTIME_START		18000
#define MENIU_33_DURATION				1800
#define MENIU_35_LOCALTIME_START		69600
#define MENIU_35_DURATION				300
#define MENIU_36_LOCALTIME_START		58800
#define MENIU_36_DURATION				300
#define MENIU_40_LOCALTIME_START		75600
#define MENIU_40_LOCALTIME_END			86340

//bool meniuAutomatInCurs = 0;
//bool meniuProgramatInCurs = 0;
unsigned int nrMeniuAutomat = 0;


#define DEVBABY1 // DEVBABY1 board
//#define ASP     // ASP board
//#define DEVBIG	// Big (first) DEV board
//#define ESPBOX1	// controller lumini spate
//#define ESPBOX2	// controller lumini fata

//ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");


#if ((defined ASP) || (defined DEVBABY1)) // we use multiple connection options for ASP
const char* ssid = "Gorlitze_etaj";
const char* password = "A1b2C3d4";
#endif



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

//int value4 = LOW;

unsigned int cnt1 = 0;
unsigned int cnt2 = 0;
byte new_client = 0;
byte cmd_flashing_led1 = 0;
byte cmd_flashing_led2 = 0;
byte nbofresets = 0;
byte nbOfWifiConnectedTimes;
byte nbOfWifiConnectionLosses = 0;
byte rel1_status = LOW;
byte rel2_status = LOW;
byte rel3_status = LOW;
bool timerInstantInProgress = FALSE; // refers to instant timer activation

bool timerScheduledOneTime = FALSE;	// refers to 1-time scheduled activation
bool timerScheduledDaily = FALSE;	// refers to daily scheduled activation
bool timerScheduledOneTimeOneLoadOnly = FALSE;	// refers to 1-time scheduled activation, time X to Y, only 1 load
bool timerScheduledDailyOneLoadOnly = FALSE;	// refers to daily scheduled activation, time X to Y, only 1 load

bool loadsScheduledOneTime[3] = {{FALSE}}; // refers to 1-time scheduled activation
bool loadsScheduledDaily[3] = {{FALSE}}; // refers to daily scheduled activation
bool loadsScheduledOneTimeOneLoadOnly[3] = {{FALSE}}; // refers to 1-time scheduled activation, time X to Y, only 1 load
bool loadsScheduledDailyOneLoadOnly[3] = {{FALSE}}; // refers to daily scheduled activation, time X to Y, only 1 load

bool timerScheduledOneTimeStarted = FALSE; // refers to 1-time scheduled activation
bool timerScheduledDailyStarted = FALSE; // refers to daily scheduled activation
bool timerScheduledOneTimeOneLoadOnlyStarted = FALSE; // refers to 1-time scheduled activation, only 1 load
bool timerScheduledDailyOneLoadOnlyStarted = FALSE; // refers to daily scheduled activation, only 1 load

bool timerScheduledDailyOneLoadOnlyLoadHasBeenSwitchedOn = FALSE;

byte menuNumberScheduledOneTime = 0;	// refers to 1-time scheduled activation
byte menuNumberScheduledOneTimeOneLoadOnly = 0;	// refers to 1-time scheduled activation, time X to Y, only 1 load
byte menuNumberScheduledDaily = 0;	// refers to 1-time scheduled activation, time X to Y, only 1 load
byte menuNumberScheduledDailyOneLoadOnly = 0;	// refers to daily scheduled activation, time X to Y, only 1 load; TO BE SWITCHED TO ARRAY

byte menuNumberInstantInProgress = 0; // refers to instant timer activation

bool loadsInProgress[3] = {{FALSE}}; // refers to instant timer activation
byte actualLoadInProgress = 0;	// refers to any type of program, used to switch to next load

byte menuInProgress = 0; // refers to any menu, when it's actually running
byte lastMenuSuccessfullyEnded = 0; // gets the value of the last menu


// Will keep board startup time
String formattedStartupTime;
String startupDate;
String connectedTime;
String connectedDate;
u32_t connectedIP;

WiFiServer server(80);

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long localTime = 0; // seconds
unsigned long localTimeMs = 0; // milliseconds
unsigned long boardTime = 0; // seconds
unsigned long boardTimeMs = 0; // milliseconds
unsigned long syncTime = 0;
unsigned long gul_max24hMillis = 0;
unsigned long wifiConnectedTimeBySystemTime;
unsigned long instantProgramStartTime = 0; // refers to instant timer activation
unsigned long timestampForNextNTPSync = 0;
unsigned char cnt = 0;
unsigned long timestampForNextLoadSwitch = 0;


bool gb_timeIsSynced = 0;
bool gb_timeHasToBeSynced = 1;

unsigned int gui_nbOfNTPsyncs = 0;
unsigned int wifiWaitCounter = 0;
unsigned int wifiDisconnectedLoopCounter = 0;
unsigned int wifiDisconnectedCounter = 0;
//unsigned int wifiConnectionAttempts = 0;


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

typedef struct
{
	unsigned int h;
	unsigned int m;
	unsigned int s;
	unsigned int d;
	unsigned int mo;
	unsigned int y;
} clock_and_date_type;

clock_type gs_localClock = {0};
clock_and_date_type gs_last_successful_menu_run = {0};

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
      localTimeMs = localTime * 1000;
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
    int currentHour = ptm->tm_hour;
    int currentMinute = ptm->tm_min;
    int currentSecond = ptm->tm_sec;
    timestruct.d = monthDay;
    timestruct.mo = currentMonth;
    timestruct.y = currentYear;
    timestruct.h = currentHour;
    timestruct.m = currentMinute;
    timestruct.s = currentSecond;
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


byte eepromInitParticularByte(byte address)
{
	byte status = 0;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	EEPROM.write(address, 0);
	delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte eepromInitParticularBytesFromTo(byte address_from, byte address_to)
{
	byte i;
	byte status = 0;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	for (i = address_from; i <= address_to; i++)
	{
		EEPROM.write(i, 0);
	}
	delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

void eepromEraseAllDefinedBytes()
{
	int i;
	EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //N bytes used
	delay(100);
	for (i=0; i<EEPROM_TOTAL_NB_OF_DEFINED_BYTES; i++)
	{
		EEPROM.write(i, 0);
		delay(100);
	}
    EEPROM.commit();
    delay(100);
}

void eepromEraseAddr01()
{
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(0, 0);
    delay(100);
    EEPROM.write(1, 0);
    delay(100);
    EEPROM.commit();
    delay(100);
}

void eepromEraseRelLastState()
{
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //6 bytes used now
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE, 0);
  delay(100);
  EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE_INV, 1);
  delay(100);
  EEPROM.commit();
  delay(100);
}

byte setEepromRel_1(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_1_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEepromRel_2(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_2_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEepromRel_3(byte state)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE, state);
    delay(100);
    EEPROM.write(EEPROM_ADDR_REL_3_LAST_STATE_INV, !(state));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_timerScheduledDaily(bool state) // save info in EEPROM: is daily timer scheduled or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 0
    {
    	temp = temp | (0b00000001);
    }
    else				// reset bit 0
    {
    	temp = temp & (0b11111110);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_0(bool state) // save info in EEPROM: is load 0 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 1
    {
    	temp = temp | (0b00000010);
    }
    else				// reset bit 1
    {
    	temp = temp & (0b11111101);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_1(bool state) // save info in EEPROM: is load 1 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 2
    {
    	temp = temp | (0b00000100);
    }
    else				// reset bit 2
    {
    	temp = temp & (0b11111011);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledDaily_2(bool state) // save info in EEPROM: is load 2 scheduled for daily run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
    if (state == TRUE) 	// set bit 3
    {
    	temp = temp | (0b00001000);
    }
    else				// reset bit 3
    {
    	temp = temp & (0b11110111);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuNumberScheduledDaily(byte val) // save info in EEPROM: the number of daily-type scheduled menu
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_NB_SCH_DAILY, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_timerScheduledOneTime(bool state) // save info in EEPROM: is 1-time timer scheduled or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 0
    {
    	temp = temp | (0b00000001);
    }
    else				// reset bit 0
    {
    	temp = temp & (0b11111110);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_0(bool state) // save info in EEPROM: is load 0 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 1
    {
    	temp = temp | (0b00000010);
    }
    else				// reset bit 1
    {
    	temp = temp & (0b11111101);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_1(bool state) // save info in EEPROM: is load 1 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 2
    {
    	temp = temp | (0b00000100);
    }
    else				// reset bit 2
    {
    	temp = temp & (0b11111011);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_loadsScheduledOneTime_2(bool state) // save info in EEPROM: is load 2 scheduled for 1-time run or not
{
    byte status = 0;
    byte temp = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
    if (state == TRUE) 	// set bit 3
    {
    	temp = temp | (0b00001000);
    }
    else				// reset bit 3
    {
    	temp = temp & (0b11110111);
    }
    EEPROM.write(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS, temp);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuNumberScheduledOneTime(byte val) // save info in EEPROM: the number of 1-time-type scheduled menu
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_NB_SCH_ONETIME, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_menuInProgress(byte val) // save info in EEPROM: the menu which is in progress (currently running)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_IN_PROGRESS, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded(byte val) // save info in EEPROM: the menu which successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Day(byte val) // save info in EEPROM: the day on which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Hour(byte val) // save info in EEPROM: the hour at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Min(byte val) // save info in EEPROM: the minute at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

byte setEeprom_lastMenuSuccessfullyEnded_Sec(byte val) // save info in EEPROM: the second at which the menu successfully ended last time
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC, byte(val));
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
}

void setEeprom_allParametersForScheduledOneTime()
{
	setEeprom_timerScheduledOneTime(timerScheduledOneTime);
	setEeprom_loadsScheduledOneTime_0(loadsScheduledOneTime[0]);
	setEeprom_loadsScheduledOneTime_1(loadsScheduledOneTime[1]);
	setEeprom_loadsScheduledOneTime_2(loadsScheduledOneTime[2]);
	setEeprom_menuNumberScheduledOneTime(menuNumberScheduledOneTime);
}

void setEeprom_allParametersForScheduledDaily()
{
	setEeprom_timerScheduledDaily(timerScheduledDaily);
	setEeprom_loadsScheduledDaily_0(loadsScheduledDaily[0]);
	setEeprom_loadsScheduledDaily_1(loadsScheduledDaily[1]);
	setEeprom_loadsScheduledDaily_2(loadsScheduledDaily[2]);
	setEeprom_menuNumberScheduledDaily(menuNumberScheduledDaily);
}


byte getEepromRel_1()
{
  byte state, state_inv;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_1_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_1_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_1_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_1_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return 0;
}

byte getEepromRel_2()
{
  byte state, state_inv;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_2_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_2_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_2_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_2_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return 0;
}

byte getEepromRel_3()
{
  byte state, state_inv;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //2 bytes used
  state = EEPROM.read(EEPROM_ADDR_REL_3_LAST_STATE);
  delay(100);
  state_inv = EEPROM.read(EEPROM_ADDR_REL_3_LAST_STATE_INV);
  delay(100);
  Serial.print("EEPROM_ADDR_REL_3_LAST_STATE: ");
  Serial.println(state);
  Serial.print("EEPROM_ADDR_REL_3_LAST_STATE_INV: ");
  Serial.println(state_inv);
  if (state == !(state_inv))
    return state;
  else
    return 0;
}

bool getEeprom_timerScheduledDaily()
{
  bool state;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  state = (bool)(temp & 0b00000001);
  return state;
}

bool getEeprom_loadsScheduledDaily_0()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000010)>>1);
  return val;
}

bool getEeprom_loadsScheduledDaily_1()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000100)>>2);
  return val;
}

bool getEeprom_loadsScheduledDaily_2()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00001000)>>3);
  return val;
}

byte getEeprom_menuNumberScheduledDaily()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_NB_SCH_DAILY);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_NB_SCH_DAILY: ");
  Serial.println(val);
  return val;
}

bool getEeprom_timerScheduledOneTime()
{
  bool state;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  state = (bool)(temp & 0b00000001);
  return state;
}

bool getEeprom_loadsScheduledOneTime_0()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000010)>>1);
  return val;
}

bool getEeprom_loadsScheduledOneTime_1()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00000100)>>2);
  return val;
}

bool getEeprom_loadsScheduledOneTime_2()
{
  bool val;
  byte temp = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  temp = EEPROM.read(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
  delay(100);
  Serial.print("EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS: ");
  Serial.println(temp);
  val = (bool)((temp & 0b00001000)>>3);
  return val;
}

byte getEeprom_menuNumberScheduledOneTime()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_NB_SCH_ONETIME);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_NB_SCH_ONETIME: ");
  Serial.println(val);
  return val;
}

byte getEeprom_menuInProgress()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_IN_PROGRESS);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_IN_PROGRESS: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Day()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_DAY: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Hour()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_HOUR: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Min()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_MIN: ");
  Serial.println(val);
  return val;
}

byte getEeprom_lastMenuSuccessfullyEnded_Sec()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC);
  delay(100);
  Serial.print("EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED_SEC: ");
  Serial.println(val);
  return val;
}

void eepromPrintAllVariables(WiFiClient client)
{
	client.println("*** Listing all EEPROM values **");
	client.print("EEPROM value for REL_1: ");
	client.println(getEepromRel_1());
	client.print("EEPROM value for REL_2: ");
	client.println(getEepromRel_2());
	client.print("EEPROM value for REL_3: ");
	client.println(getEepromRel_3());
	client.print("timerScheduledDaily: ");
	client.println(getEeprom_timerScheduledDaily());
	client.print("loadsScheduledDaily[0]: ");
	client.println(getEeprom_loadsScheduledDaily_0());
	client.print("loadsScheduledDaily[1]: ");
	client.println(getEeprom_loadsScheduledDaily_1());
	client.print("loadsScheduledDaily[2]: ");
	client.println(getEeprom_loadsScheduledDaily_2());
	client.print("menuNumberScheduledDaily: ");
	client.println(getEeprom_menuNumberScheduledDaily());
	client.print("timerScheduledOneTime: ");
	client.println(getEeprom_timerScheduledOneTime());
	client.print("loadsScheduledOneTime[0]: ");
	client.println(getEeprom_loadsScheduledOneTime_0());
	client.print("loadsScheduledOneTime[1]: ");
	client.println(getEeprom_loadsScheduledOneTime_1());
	client.print("loadsScheduledOneTime[2]: ");
	client.println(getEeprom_loadsScheduledOneTime_2());
	client.print("menuNumberScheduledOneTime: ");
	client.println(getEeprom_menuNumberScheduledOneTime());
	client.print("menuInProgress: ");
	client.println(getEeprom_menuInProgress());
	client.print("lastMenuSuccessfullyEnded: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded());
	client.print("lastMenuSuccessfullyEnded_Day: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Day());
	client.print("lastMenuSuccessfullyEnded_Hour: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Hour());
	client.print("lastMenuSuccessfullyEnded_Minute: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Min());
	client.print("lastMenuSuccessfullyEnded_Second: ");
	client.println(getEeprom_lastMenuSuccessfullyEnded_Sec());
	client.println("*** ************** **");
}

void loadTimersDataFromEEPROM() // load saved timers configuration (of all types) from EEPROM
{
	timerScheduledDaily = getEeprom_timerScheduledDaily();
	loadsScheduledDaily[0] = getEeprom_loadsScheduledDaily_0();
	loadsScheduledDaily[1] = getEeprom_loadsScheduledDaily_1();
	loadsScheduledDaily[2] = getEeprom_loadsScheduledDaily_2();
	menuNumberScheduledDaily = getEeprom_menuNumberScheduledDaily();
	timerScheduledOneTime = getEeprom_timerScheduledOneTime();
	loadsScheduledOneTime[0] = getEeprom_loadsScheduledOneTime_0();
	loadsScheduledOneTime[1] = getEeprom_loadsScheduledOneTime_1();
	loadsScheduledOneTime[2] = getEeprom_loadsScheduledOneTime_2();
	menuNumberScheduledOneTime = getEeprom_menuNumberScheduledOneTime();
	menuInProgress = getEeprom_menuInProgress();
	lastMenuSuccessfullyEnded = getEeprom_lastMenuSuccessfullyEnded();
}

void saveLastMenuSuccessfullyEnded_Parameters()
{
	setEeprom_lastMenuSuccessfullyEnded_Day(byte(gs_last_successful_menu_run.d));
	setEeprom_lastMenuSuccessfullyEnded_Hour(byte(gs_last_successful_menu_run.h));
	setEeprom_lastMenuSuccessfullyEnded_Min(byte(gs_last_successful_menu_run.m));
	setEeprom_lastMenuSuccessfullyEnded_Sec(byte(gs_last_successful_menu_run.s));
}

void updateLocalTimersInMainLoop()
{
	  currentMillis = millis();

	  // Updating localtime every 15 minutes
	  if ((currentMillis - timestampForNextNTPSync) > TIME_INTERVAL_TO_RECHECK_NTP)
	  {
		  if (syncWithNTP())
		  {
			  Serial.print("Local time (s): ");
			  Serial.println(localTime);
		  }
		  else
		  {
			  Serial.println("Failure, try again.");
		  }
		  timestampForNextNTPSync = millis();
	  }
	  else
	  {
		  localTimeMs += AVG_MEASURED_MAIN_LOOP_DURATION_MS;
		  if (localTimeMs > ONE_DAY_IN_MILISECONDS)
		  {
			  localTimeMs -= ONE_DAY_IN_MILISECONDS;
		  }
		  localTime = localTimeMs / 1000;
		  boardTimeMs = currentMillis;
		  boardTime = boardTimeMs / 1000;
	  }
}

void printThisMenuIsNotInProgress_1(WiFiClient client) // rejection reply for wrong stop attempt, applicable for instant programs only
{
	client.println("This program is not in progress.");
	client.print("This one is: meniu_");
	client.println(menuNumberInstantInProgress);
	client.println("");
}

void printThisProgramIsNotScheduled_1(WiFiClient client) // rejection reply for wrong stop attempt, applicable for 1-time programs only
{
	client.print("This program is not scheduled. This one is: meniu_");
	client.println(menuNumberScheduledOneTime);
}

void printThisProgramIsNotScheduled_2(WiFiClient client) // rejection reply for wrong stop attempt, applicable for daily programs only
{
	client.print("This program is not scheduled. This one is: meniu_");
	client.println(menuNumberScheduledDaily);
}

void printAnother1TimeProgramIsScheduled(WiFiClient client) // rejection reply in case another 1-time program was scheduled
{
	client.print("Another 1-time program is scheduled already: meniu_");
	client.print(menuNumberScheduledOneTime);
	client.println(". Cancel it first");
}

void printAnotherDailyProgramIsScheduled(WiFiClient client) // rejection reply in case another daily program was scheduled
{
	client.print("Another daily program is scheduled already: meniu_");
	client.print(menuNumberScheduledDaily);
	client.println(". Cancel it first");
}


void setup()
{
   //eepromEraseAddr01();
   //eepromEraseRelLastState();
	//eepromEraseAllDefinedBytes();
	//eepromInitParticularByte(EEPROM_ADDR_RST_COUNTER);
	//eepromInitParticularByte(EEPROM_ADDR_WIFI_CONN_COUNTER);
	//eepromInitParticularByte(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
	//eepromInitParticularByte(EEPROM_ADDR_MENU_NB_SCH_DAILY);
	//eepromInitParticularByte(EEPROM_ADDR_TIMER_SCH_ONETIME_PARAMS);
	//eepromInitParticularByte(EEPROM_ADDR_MENU_NB_SCH_ONETIME);
	//eepromInitParticularByte(EEPROM_ADDR_MENU_IN_PROGRESS);
	//eepromInitParticularByte(EEPROM_ADDR_LAST_MENU_SUCCESSFULLY_ENDED);
   wifiDisconnectedLoopCounter = 0;
   wifiDisconnectedCounter = 0;

   Serial.begin(115200);
   delay(100);
   pinMode(REL_1, OUTPUT);
   pinMode(REL_2, OUTPUT);
   pinMode(REL_3, OUTPUT);

   digitalWrite(REL_1, LOW);
   digitalWrite(REL_2, LOW);
   digitalWrite(REL_3, LOW);

#ifdef DEVBABY1
   digitalWrite(REL_1, HIGH);
   digitalWrite(REL_2, HIGH);
   digitalWrite(REL_3, HIGH);
#endif


  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println(" ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {    delay(20000);
      wifiWaitCounter++;
      Serial.print(wifiWaitCounter);
      Serial.print(", ");
      if (wifiWaitCounter > 10)
      {
          ESP.restart();
      }
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (WiFi.status() == WL_CONNECTED)
  {
#ifdef DEVBABY1
	   digitalWrite(REL_1, LOW);
	   digitalWrite(REL_2, HIGH);
	   digitalWrite(REL_3, LOW);
#endif
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

        getDateFromNTP(startupDate);


      wifiConnectedTimeBySystemTime = millis()/1000;
      //Serial.println("timeClientUpdateSuccess");
      connectedTime = timeClient.getFormattedTime(); // this retrieves the last updated values from the object timeClient;
      Serial.print("Formatted WiFi connected Time: ");
      Serial.println(connectedTime);
      EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used
      delay(100);
      nbOfWifiConnectedTimes = EEPROM.read(EEPROM_ADDR_WIFI_CONN_COUNTER);
      nbOfWifiConnectedTimes++;
      delay(100);
      EEPROM.write(EEPROM_ADDR_WIFI_CONN_COUNTER, nbOfWifiConnectedTimes);
      delay(100);
      if (EEPROM.commit())
      {
        Serial.println("EEPROM has been updated.");
      }
      else
      {
        Serial.println("EEPROM commit failed.");
      }
        delay(100);
    }
    else
    {
      Serial.println("WIFI != CONNECTED, pula mea");
    }

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
  if (connectedIP != 1006741952) // which means 192.169.1.60
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
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

#ifdef ESPBOX2
  if (connectedIP != 1107404992) // which means 192.168.1.66
  {
    Serial.println("Wrong IP, ESP will reset");
    Serial.println("!!!!!!!!!!!!!");
    ESP.restart();
  }
#endif

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




  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte
  delay(100);
  nbofresets = EEPROM.read(EEPROM_ADDR_RST_COUNTER);
  nbofresets++;
  delay(100);
  EEPROM.write(EEPROM_ADDR_RST_COUNTER, nbofresets);
  delay(100);
  if (EEPROM.commit())
  {
    Serial.println("EEPROM has been updated.");
  }
  else
  {
    Serial.println("EEPROM commit failed.");
  }
  delay(100);

  //load EEPROM REL values
#if defined (ESPBOX1) || defined (ESPBOX2)
  rel1_status = getEepromRel_1();
  rel2_status = getEepromRel_2();
  rel3_status = getEepromRel_3();
  digitalWrite(REL_1, rel1_status);
  digitalWrite(REL_2, rel2_status);
  digitalWrite(REL_3, rel3_status);

  Serial.print("rel1_status: ");
  Serial.println(rel1_status);
  Serial.print("rel2_status: ");
  Serial.println(rel2_status);
  Serial.print("rel3_status: ");
  Serial.println(rel3_status);
#endif

  //load EEPROM values related to timers scheduled daily or one-time
  loadTimersDataFromEEPROM();

  timestampForNextNTPSync = millis();

} //end of setup()

void loop()
{
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

  updateLocalTimersInMainLoop();

  // Handlers for 1-time scheduled program, one or multiple loads (but in order REL_1, REL_2, REL_3):
  // **********************************************************************************************
  if (timerScheduledOneTime != FALSE)
  {	Serial.println("timerScheduledOneTime != FALSE");

	if (menuNumberScheduledOneTime == 35)	// Handler for menu Nb 35 (TEST MENU)
	{	Serial.println("menuNumberScheduledOneTime == 35");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_35_LOCALTIME_START) && (localTime < MENIU_35_LOCALTIME_START + MENIU_35_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_35_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_35_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 35;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 35) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_35_LOCALTIME_START) && (localTime < (MENIU_35_LOCALTIME_START + (3*MENIU_35_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_35_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_35_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!
				else
				{
					if ((menuInProgress == 35) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}	// tested OK!
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_35_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 35;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 35

	if (menuNumberScheduledOneTime == 36)	// Handler for menu Nb 36 (TEST MENU)
	{	Serial.println("menuNumberScheduledOneTime == 36");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_36_LOCALTIME_START) && (localTime < MENIU_36_LOCALTIME_START + MENIU_36_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_36_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_36_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 36;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 36) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_36_LOCALTIME_START) && (localTime < (MENIU_36_LOCALTIME_START + (3*MENIU_36_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_36_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_36_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!
				else
				{
					if ((menuInProgress == 36) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}	// tested OK!
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_36_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 36;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 36

	if (menuNumberScheduledOneTime == 21)	// Handler for menu Nb 21
	{	Serial.println("menuNumberScheduledOneTime == 21");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_21_LOCALTIME_START) && (localTime < MENIU_21_LOCALTIME_START + MENIU_21_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_21_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_21_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 21;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}
			else
			{
				if ((menuInProgress == 21) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_21_LOCALTIME_START) && (localTime < (MENIU_21_LOCALTIME_START + (3*MENIU_21_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_21_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_21_DURATION;
					actualLoadInProgress = 0;
				}
				else
				{
					if ((menuInProgress == 21) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_21_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 21;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 21

	if (menuNumberScheduledOneTime == 22)	// Handler for menu Nb 22
	{	Serial.println("menuNumberScheduledOneTime == 22");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_22_LOCALTIME_START) && (localTime < MENIU_22_LOCALTIME_START + MENIU_22_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_22_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_22_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 22;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}
			else
			{
				if ((menuInProgress == 22) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_22_LOCALTIME_START) && (localTime < (MENIU_22_LOCALTIME_START + (3*MENIU_22_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_22_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_22_DURATION;
					actualLoadInProgress = 0;
				}
				else
				{
					if ((menuInProgress == 22) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_22_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 22;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 22

	if (menuNumberScheduledOneTime == 23)	// Handler for menu Nb 23
	{	Serial.println("menuNumberScheduledOneTime == 23");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_23_LOCALTIME_START) && (localTime < MENIU_23_LOCALTIME_START + MENIU_23_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_23_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_23_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 23;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}
			else
			{
				if ((menuInProgress == 23) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_23_LOCALTIME_START) && (localTime < (MENIU_23_LOCALTIME_START + (3*MENIU_23_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_23_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_23_DURATION;
					actualLoadInProgress = 0;
				}
				else
				{
					if ((menuInProgress == 23) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_23_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 23;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 23

	if (menuNumberScheduledOneTime == 24)	// Handler for menu Nb 24
	{	Serial.println("menuNumberScheduledOneTime == 24");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_24_LOCALTIME_START) && (localTime < MENIU_24_LOCALTIME_START + MENIU_24_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_24_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_24_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 24;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}
			else
			{
				if ((menuInProgress == 24) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_24_LOCALTIME_START) && (localTime < (MENIU_24_LOCALTIME_START + (3*MENIU_24_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_24_LOCALTIME_START !!!!!!!!");
					timerScheduledOneTimeStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_24_DURATION;
					actualLoadInProgress = 0;
				}
				else
				{
					if ((menuInProgress == 24) && (lastMenuSuccessfullyEnded == 0))
					{	// power resumed too late, we cancel this menu
						menuInProgress = 0;
						setEeprom_menuInProgress(menuInProgress);
						lastMenuSuccessfullyEnded = 0;
						setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
						timerScheduledOneTime = FALSE;
						menuNumberScheduledOneTime = 0;
						loadsScheduledOneTime[0] = FALSE;
						loadsScheduledOneTime[1] = FALSE;
						loadsScheduledOneTime[2] = FALSE;
						setEeprom_allParametersForScheduledOneTime();
					}
				}
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_24_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledOneTime[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledOneTime[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledOneTime[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				timerScheduledOneTime = FALSE;
				actualLoadInProgress = 0;
				menuNumberScheduledOneTime = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
            	setEeprom_allParametersForScheduledOneTime();
            	menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 24;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);


		}
	}		// End of handler for menu Nb 24


	//if (menuNumberScheduledOneTime == XX)	// Handler for menu Nb XX
  } 	// End of handlers for 1-time scheduled program
  	  	 // *******************************************************************************


  // Handlers for 1-time scheduled program from X to Y time, only one load programs:
/*  if (timerScheduledOneTimeOneLoadOnly != FALSE)
  {	Serial.println("timerScheduledOneTimeOneLoadOnly != FALSE");
	if (menuNumberScheduledOneLoadOnly == 30)	// Handler for menu Nb 30, only rel_2
	{	Serial.println("menuNumberScheduledOneLoadOnly == 30");
		if (timerScheduledOneTimeOneLoadOnlyStarted == FALSE)	// scheduled timer not yet started
		{
			if (localTime >= MENIU_30_LOCALTIME_START)
			{	Serial.println("!!!!! localTime >= MENIU_30_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeOneLoadOnlyStarted = TRUE;
				//timestampForNextLoadSwitch = localTime + 300; // TO DO: to handle with millis to avoid problem around 12 AM
			}
		}
		else	// programmed timer has already started
		{
			Serial.println("IN PROGRESS");
			if (localTime >= MENIU_30_LOCALTIME_END)
			{
				timerScheduledOneTimeOneLoadOnly = FALSE;
				menuNumberScheduledOneLoadOnly = 0;
				timerScheduledOneTimeOneLoadOnlyStarted = FALSE;
				loadsScheduledOneTimeOneLoadOnly[0] = FALSE;
				loadsScheduledOneTimeOneLoadOnly[1] = FALSE;
				loadsScheduledOneTimeOneLoadOnly[2] = FALSE;
			}

			rel1_status = loadsScheduledOneTimeOneLoadOnly[0];
			rel2_status = loadsScheduledOneTimeOneLoadOnly[1];
			rel3_status = loadsScheduledOneTimeOneLoadOnly[2];

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);

		}
	}		// End of handler for menu Nb 30
	//if (menuNumberScheduledOneLoadOnly == XX)	// Handler for menu Nb XX
  } 	// End handlers of for 1-time scheduled program from X to Y time, only one load programs:
*/



  // Handlers for daily scheduled program, one or multiple loads (but in order REL_1, REL_2, REL_3):
  // **********************************************************************************************
  if (timerScheduledDaily != FALSE)
  {	Serial.println("timerScheduledDaily != FALSE");

  	  if (menuNumberScheduledDaily == 30)	// Handler for menu Nb 30
  	  {
  		Serial.println("menuNumberScheduledOneTime == 30");
		if (timerScheduledDailyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_30_LOCALTIME_START) && (localTime < MENIU_30_LOCALTIME_START + MENIU_30_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_30_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_30_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 30;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 30) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_30_LOCALTIME_START) && (localTime < (MENIU_30_LOCALTIME_START + (3*MENIU_30_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_30_LOCALTIME_START !!!!!!!!");
					timerScheduledDailyStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_30_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!

			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_30_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledDaily[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledDaily[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledDaily[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				actualLoadInProgress = 0;
				timerScheduledDailyStarted = FALSE;
				rel1_status = LOW;
				rel2_status = LOW;
				rel3_status = LOW;
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 30;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);
		}
	}		// End of handler for menu Nb 30

  	  if (menuNumberScheduledDaily == 31)	// Handler for menu Nb 31
  	  {
  		Serial.println("menuNumberScheduledOneTime == 31");
		if (timerScheduledDailyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_31_LOCALTIME_START) && (localTime < MENIU_31_LOCALTIME_START + MENIU_31_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_31_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_31_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 31;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 31) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_31_LOCALTIME_START) && (localTime < (MENIU_31_LOCALTIME_START + (3*MENIU_31_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_31_LOCALTIME_START !!!!!!!!");
					timerScheduledDailyStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_31_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!

			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_31_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledDaily[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledDaily[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledDaily[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				actualLoadInProgress = 0;
				timerScheduledDailyStarted = FALSE;
				rel1_status = LOW;
				rel2_status = LOW;
				rel3_status = LOW;
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 31;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);
		}
	}		// End of handler for menu Nb 31

  	  if (menuNumberScheduledDaily == 32)	// Handler for menu Nb 32
  	  {
  		Serial.println("menuNumberScheduledOneTime == 32");
		if (timerScheduledDailyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_32_LOCALTIME_START) && (localTime < MENIU_32_LOCALTIME_START + MENIU_32_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_32_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_32_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 32;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 32) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_32_LOCALTIME_START) && (localTime < (MENIU_32_LOCALTIME_START + (3*MENIU_32_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_32_LOCALTIME_START !!!!!!!!");
					timerScheduledDailyStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_32_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!

			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_32_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledDaily[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledDaily[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledDaily[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				actualLoadInProgress = 0;
				timerScheduledDailyStarted = FALSE;
				rel1_status = LOW;
				rel2_status = LOW;
				rel3_status = LOW;
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 32;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);
		}
	}		// End of handler for menu Nb 32

  	  if (menuNumberScheduledDaily == 33)	// Handler for menu Nb 33
  	  {
  		Serial.println("menuNumberScheduledOneTime == 33");
		if (timerScheduledDailyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_33_LOCALTIME_START) && (localTime < MENIU_33_LOCALTIME_START + MENIU_30_DURATION) && (menuInProgress == 0)) // To fix here
			{	Serial.println("!!!!! localTime >= MENIU_33_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_33_DURATION;
				actualLoadInProgress = 0;
				menuInProgress = 33;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 0;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
			}	// tested OK!
			else
			{
				if ((menuInProgress == 33) && (lastMenuSuccessfullyEnded == 0) && (localTime >= MENIU_33_LOCALTIME_START) && (localTime < (MENIU_33_LOCALTIME_START + (3*MENIU_33_DURATION))))
				{	// power resumed within a maximum time inverval of 3*(menu duration per load)
					Serial.println("!!!!! POWER RESUMED, localTime >= MENIU_33_LOCALTIME_START !!!!!!!!");
					timerScheduledDailyStarted = TRUE;
					timestampForNextLoadSwitch = boardTime + MENIU_33_DURATION;
					actualLoadInProgress = 0;
				}	// tested OK!

			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + MENIU_33_DURATION;
				actualLoadInProgress++;
			}

			if (actualLoadInProgress == 0)
			{
				if (loadsScheduledDaily[0] == TRUE)
				{
					rel1_status = HIGH;
					rel2_status = LOW;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 1)
			{
				if (loadsScheduledDaily[1] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = HIGH;
					rel3_status = LOW;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 2)
			{
				if (loadsScheduledDaily[2] == TRUE)
				{
					rel1_status = LOW;
					rel2_status = LOW;
					rel3_status = HIGH;
				}
				else actualLoadInProgress++;
			}

			if (actualLoadInProgress == 3)
			{
				actualLoadInProgress = 0;
				timerScheduledDailyStarted = FALSE;
				rel1_status = LOW;
				rel2_status = LOW;
				rel3_status = LOW;
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
				lastMenuSuccessfullyEnded = 33;
				setEeprom_lastMenuSuccessfullyEnded(lastMenuSuccessfullyEnded);
				getDateFromNTPToStruct(gs_last_successful_menu_run);
				saveLastMenuSuccessfullyEnded_Parameters();
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);
		}
	}		// End of handler for menu Nb 33

  }	// End of handlers for daily scheduled programs
 	 // *******************************************************************************************

  // TBD - will potentially be abandoned since this can be also covered by daily multiple loads handler
  // Handlers for daily scheduled program from X to Y time, only one load programs:
  if (timerScheduledDailyOneLoadOnly != FALSE)
  {	//Serial.println("timerScheduledDailyOneLoadOnly != FALSE");
	if (menuNumberScheduledDailyOneLoadOnly == 40)	// Handler for menu Nb 40, only rel_2
	{	//Serial.println("menuNumberScheduledDailyOneLoadOnly == 40");
		if (timerScheduledDailyOneLoadOnlyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_40_LOCALTIME_START) && (localTime < MENIU_40_LOCALTIME_END))
			{	//Serial.println("!!!!! localTime >= MENIU_40_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyOneLoadOnlyStarted = TRUE;
				//timestampForNextLoadSwitch = localTime + 300; // TO DO: to handle with millis to avoid problem around 12 AM
			}
		}
		else	// programmed timer has already started
		{
			//Serial.println("IN PROGRESS");

			if (timerScheduledDailyOneLoadOnlyLoadHasBeenSwitchedOn == FALSE)
			{
				//rel1_status = loadsScheduledDailyOneLoadOnly[0];
				rel2_status = loadsScheduledDailyOneLoadOnly[1];
				//rel3_status = loadsScheduledDailyOneLoadOnly[2];

				//digitalWrite(REL_1, rel1_status);
				digitalWrite(REL_2, rel2_status);
				//digitalWrite(REL_3, rel3_status);
				timerScheduledDailyOneLoadOnlyLoadHasBeenSwitchedOn = TRUE;
			}

			if (localTime >= MENIU_40_LOCALTIME_END)
			{
				timerScheduledDailyOneLoadOnlyStarted = FALSE;
				//rel1_status = LOW;
				rel2_status = LOW;
				//rel3_status = LOW;
				//digitalWrite(REL_1, rel1_status);
				digitalWrite(REL_2, rel2_status);
				//digitalWrite(REL_3, rel3_status);
				timerScheduledDailyOneLoadOnlyLoadHasBeenSwitchedOn = FALSE;
			}

		}
	}		// End of handler for menu Nb 40
	//if (menuNumberScheduledDailyOneLoadOnly == XX)	// Handler for menu Nb XX
  } 	// End handlers of for daily scheduled program from X to Y time, only one load programs:



  // Handlers for instant timer activation: **************************************************************
  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 1))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_1_DURATION )
	  {
		  rel1_status = LOW;
		  digitalWrite(REL_1, rel1_status);
		  timerInstantInProgress = FALSE;
		  loadsInProgress[0] = FALSE;
		  menuNumberInstantInProgress = 0;
	  }
  }	// End of handler for "meniu_1"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 2))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_2_DURATION )
	  {
		  rel1_status = LOW;
		  digitalWrite(REL_1, rel1_status);
		  timerInstantInProgress = FALSE;
		  loadsInProgress[0] = FALSE;
		  menuNumberInstantInProgress = 0;
	  }
  }	// End of handler for "meniu_2"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 3))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_3_DURATION )
		  {
		  	  rel1_status = LOW;
		  	  digitalWrite(REL_1, rel1_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[0] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_3"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 4))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_4_DURATION )
		  {
		  	  rel1_status = LOW;
		  	  digitalWrite(REL_1, rel1_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[0] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_4"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 5))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_5_DURATION )
		  {
		  	  rel3_status = LOW;
		  	  digitalWrite(REL_3, rel3_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[2] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_5"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 6))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_6_DURATION )
		  {
		  	  rel3_status = LOW;
		  	  digitalWrite(REL_3, rel3_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[2] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_6"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 7))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_7_DURATION )
		  {
		  	  rel3_status = LOW;
		  	  digitalWrite(REL_3, rel3_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[2] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_7"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 8))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_8_DURATION )
		  {
		  	  rel3_status = LOW;
		  	  digitalWrite(REL_3, rel3_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[2] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_8"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 9))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_9_DURATION )
		  {
		  	  rel2_status = LOW;
		  	  digitalWrite(REL_2, rel2_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[1] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_9"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 10))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_10_DURATION )
		  {
		  	  rel2_status = LOW;
		  	  digitalWrite(REL_2, rel2_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[1] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_10"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 11))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_11_DURATION )
		  {
		  	  rel2_status = LOW;
		  	  digitalWrite(REL_2, rel2_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[1] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_11"

  if ((timerInstantInProgress != FALSE) && (menuNumberInstantInProgress == 12))
  {
	  if (((currentMillis/1000)-instantProgramStartTime) > MENIU_12_DURATION )
		  {
		  	  rel2_status = LOW;
		  	  digitalWrite(REL_2, rel2_status);
		  	  timerInstantInProgress = FALSE;
		  	  loadsInProgress[1] = FALSE;
		  	  menuNumberInstantInProgress = 0;
		  }
  }	// End of handler for "meniu_12"

  // End of handlers for instant timer activation ***********************************************************


  ArduinoOTA.handle(); // OTA usage

  WiFiClient client = server.available();



  if (!client)
  {
    //Serial.print("!client ");
    //Serial.println(cnt1);
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
        //Serial.print("!client.available() ");
        //Serial.println(cnt2);
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

        client.print("Request is: ");
        client.println(request);

        if (request.indexOf("rel1on") != -1)
        {
                rel1_status = HIGH;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_1(rel1_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_1, rel1_status);
#ifdef ESPBOX1
                client.println("RELAY_1 is ON");
                client.println("nimic");
#endif
#ifdef ESPBOX2
                client.println("RELAY_1 is ON");
                client.println("Lampa fara senzor pornita");
#endif
#ifdef ASP
                client.println("RELAY_1 is ON");
                client.println("Aspersoare SPATE pornite");
#endif
#ifdef DEVBABY1
                client.println("LED_1 is ON");
                //client.println("Aspersoare SPATE pornite");
#endif

                client.println("");
                //getEepromRel_1();
        }

        if (request.indexOf("rel1off") != -1)
        {
                rel1_status = LOW;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_1(rel1_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_1, rel1_status);
#ifdef ESPBOX1
                client.println("RELAY_1 is OFF");
                client.println("nimic");
#endif
#ifdef ESPBOX2
                client.println("RELAY_1 is OFF");
                client.println("Lampa fara senzor oprita");
#endif
#ifdef ASP
                client.println("RELAY_1 is OFF");
                client.println("Aspersoare SPATE oprite");
#endif
#ifdef DEVBABY1
                client.println("LED_1 is OFF");
                //client.println("Aspersoare SPATE oprite");
#endif
                client.println("");
                //getEepromRel_1();
        }

        if (request.indexOf("rel2on") != -1)
        {
                rel2_status = HIGH;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_2(rel2_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_2, rel2_status);
#ifdef ESPBOX1
                client.println("RELAY_2 is ON");
                client.println("LAMPA SPATE PORNITA");
#endif
#ifdef ESPBOX2
                client.println("RELAY_2 is ON");
                client.println("Priza backup pornita");
#endif
#ifdef ASP
                client.println("RELAY_2 is ON");
                client.println("Picuratoare gradina pornite");
#endif
#ifdef DEVBABY1
                client.println("LED_2 is ON");
                //client.println("Picuratoare gradina pornite");
#endif
                client.println("");
                //getEepromRel_2();
        }

        if (request.indexOf("rel2off") != -1)
        {
                rel2_status = LOW;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_2(rel2_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_2, rel2_status);
#ifdef ESPBOX1
                client.println("RELAY_2 is OFF");
                client.println("LAMPA SPATE OPRITA");
#endif
#ifdef ESPBOX2
                client.println("RELAY_2 is OFF");
                client.println("Priza backup oprita");
#endif
#ifdef ASP
                client.println("RELAY_2 is OFF");
                client.println("Picuratoare gradina oprite");
#endif
#ifdef DEVBABY1
                client.println("LED_2 is OFF");
                //client.println("Picuratoare gradina oprite");
#endif
                client.println("");
                //getEepromRel_2();
        }

        if (request.indexOf("rel3on") != -1)
        {
                rel3_status = HIGH;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_3(rel3_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_3, rel3_status);
#ifdef ESPBOX1
                client.println("RELAY_3 is ON");
                client.println("LAMPA LATERALA PORNITA");
#endif
#ifdef ESPBOX2
                client.println("RELAY_3 is ON");
                client.println("Lampa cu senzor pornita");
#endif
#ifdef ASP
                client.println("RELAY_3 is ON");
                client.println("Aspersoare FATA pornite");
#endif
#ifdef DEVBABY1
                client.println("LED_3 is ON");
                //client.println("Aspersoare FATA pornite");
#endif
                client.println("");
                //getEepromRel_3();
        }

        if (request.indexOf("rel3off") != -1)
        {
                rel3_status = LOW;
#if defined (ESPBOX1) || defined (ESPBOX2)
                if (setEepromRel_3(rel3_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
#endif
                digitalWrite(REL_3, rel3_status);
#ifdef ESPBOX1
                client.println("RELAY_3 is OFF");
                client.println("LAMPA LATERALA OPRITA");
#endif
#ifdef ESPBOX2
                client.println("RELAY_3 is OFF");
                client.println("Lampa cu senzor oprita");
#endif
#ifdef ASP
                client.println("RELAY_3 is OFF");
                client.println("Aspersoare FATA oprite");
#endif
#ifdef DEVBABY1
                client.println("LED_3 is OFF");
                //client.println("Aspersoare FATA oprite");
#endif
                client.println("");
                //getEepromRel_3();
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
                Serial.println("");
              }

              else
              {
                Serial.println("timeClientUpdate NO SUCCESS");
                Serial.println("");
              }

        }

        if (request.indexOf("boardTime") != -1)
        {
          client.print("Board SystemTime (s): ");
          client.println(millis()/1000);
          //client.print("Board WiFi connected Time (s): ");
          //client.println(wifiConnectedTimeBySystemTime);
          client.println(" ");

        }

        if (request.indexOf("ProgVersion") != -1)
        {
        	client.println("*********************");
        	client.println("Versiune: aspersoare_v0_15_sloeber_1");
#ifdef ESPBOX1
        	client.println("Cutie relee ESPBOX1 - CURTE SPATE");
        	client.println("Comenzi disponibile:");
        	client.println("rel1on (buton SP ON)- nimic");
        	client.println("rel1off (buton SP OFF) - nimic");
        	client.println("rel2on (buton LEG ON) - lampa spate ON");
        	client.println("rel2off (buton LEG OFF) - lampa spate OFF");
        	client.println("rel3on (buton FA ON) - lampa laterala ON");
        	client.println("rel3off (buton FA OFF) - lampa laterala OFF");
        	client.println("checktime - apeleaza NTP");
        	client.println("meniu_40 - Lampa spate pornita zilnic de la 21:00 la 23:59");
        	client.println("m40_stop - ANULARE PROGRAM: Lampa spate pornita zilnic de la 21:00 la 23:59");
        	client.println("boardTime - [sec] timp dupa reset");
        	client.println("localtime - experimental");
        	client.println("SystemRestart (buton RESET) - Reset sistem");

#endif
#ifdef ESPBOX2
        	client.println("Cutie relee ESPBOX2 - CURTE FATA");
        	client.println("Comenzi disponibile:");
        	client.println("rel1on (buton SP ON)- lampa fara senzor ON");
        	client.println("rel1off (buton SP OFF) - lampa fara senzor OFF");
        	client.println("rel2on (buton LEG ON) - priza backup ON");
        	client.println("rel2off (buton LEG OFF) - priza backup OFF");
        	client.println("rel3on (buton FA ON) - lampa cu senzor ON");
        	client.println("rel3off (buton FA OFF) - lampa cu senzor OFF");
        	client.println("checktime - apeleaza NTP");
        	client.println("boardTime - [sec] timp dupa reset");
        	client.println("localtime - experimental");
        	client.println("SystemRestart (buton RESET) - Reset sistem");

#endif
#ifdef ASP
        	client.println("Cutie relee ASPERSOARE");
        	client.println("Comenzi disponibile:");
        	client.println("rel1on (buton SP ON)- porneste asp spate");
        	client.println("rel1off (buton SP OFF) - opreste asp spate");
        	client.println("rel2on (buton LEG ON) - porneste picuratoare gradina");
        	client.println("rel2off (buton LEG OFF) - opreste picuratoare gradina");
        	client.println("rel3on (buton FA ON) - porneste asp fata");
        	client.println("rel3off (buton FA OFF) - opreste asp fata");
        	client.println("checktime - apeleaza NTP");
        	client.println("boardTime - [sec] timp dupa reset");
        	client.println("localtime - experimental");
        	client.println("SystemRestart (buton RESET) - Reset sistem");
        	client.println("meniu_1 - O tura aspersor spate 15 min, acum");
        	client.println("meniu_1_stop - Anuleaza meniu_1");
        	client.println("meniu_2 - O tura aspersor spate 20 min, acum");
        	client.println("meniu_2_stop - Anuleaza meniu_2");
        	client.println("meniu_3 - O tura aspersor spate 25 min, acum");
        	client.println("meniu_3_stop - Anuleaza meniu_3");
        	client.println("meniu_4 - O tura aspersor spate 30 min, acum");
        	client.println("meniu_4_stop - Anuleaza meniu_4");
        	client.println("meniu_5 - O tura aspersor fata 15 min, acum");
        	client.println("meniu_5_stop - Anuleaza meniu_5");
        	client.println("meniu_6 - O tura aspersor fata 20 min, acum");
        	client.println("meniu_6_stop - Anuleaza meniu_6");
        	client.println("meniu_7 - O tura aspersor fata 25 min, acum");
        	client.println("meniu_7_stop - Anuleaza meniu_7");
        	client.println("meniu_8 - O tura aspersor fata 30 min, acum");
        	client.println("meniu_8_stop - Anuleaza meniu_8");
        	client.println("meniu_9 - O tura picurator gradina 15 min, acum");
        	client.println("meniu_9_stop - Anuleaza meniu_9");
        	client.println("meniu_10 - O tura picurator gradina 20 min, acum");
        	client.println("meniu_10_stop - Anuleaza meniu_10");
        	client.println("meniu_11 - O tura picurator gradina 25 min, acum");
        	client.println("meniu_11_stop - Anuleaza meniu_11");
        	client.println("meniu_12 - O tura picurator gradina 30 min, acum");
        	client.println("meniu_12_stop - Anuleaza meniu_12");
        	client.println("meniu_21 - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 15 min");
        	client.println("meniu_21_stop - Anuleaza meniu_21");
        	client.println("meniu_22 - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 20 min");
        	client.println("meniu_22_stop - Anuleaza meniu_22");
        	client.println("meniu_23 - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 25 min");
        	client.println("meniu_23_stop - Anuleaza meniu_23");
        	client.println("meniu_24 - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 30 min");
        	client.println("meniu_24_stop - Anuleaza meniu_24");
        	client.println("meniu_30 - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 15 min");
			client.println("meniu_30_stop - Anuleaza meniu_30");
        	client.println("meniu_31 - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 20 min");
			client.println("meniu_31_stop - Anuleaza meniu_31");
        	client.println("meniu_32 - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 25 min");
			client.println("meniu_32_stop - Anuleaza meniu_32");
        	client.println("meniu_33 - Program zilnic de la ora 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 30 min");
			client.println("meniu_33_stop - Anuleaza meniu_33");
        	client.println("timers_status - Afiseaza status programe active");
        	client.println("print_eeprom - Afiseaza toate valorile din EEPROM");
#endif
#ifdef DEVBABY1
        	client.println("Placa devzoltare 'DEVBABY1'");
        	client.println("Comenzi disponibile:");
        	client.println("rel1on (buton SP ON)- porneste LED_1");
        	client.println("rel1off (buton SP OFF) - opreste LED_1");
        	client.println("rel2on (buton LEG ON) - porneste LED_2");
        	client.println("rel2off (buton LEG OFF) - opreste LED_2");
        	client.println("rel3on (buton FA ON) - porneste LED_3");
        	client.println("rel3off (buton FA OFF) - opreste LED_3");
        	client.println("checktime - apeleaza NTP");
        	client.println("boardTime - [sec] timp dupa reset");
        	client.println("localtime - experimental");
        	client.println("SystemRestart (buton RESET) - Reset sistem");
        	client.println("meniu_1 - O tura REL_1 15 min, acum");
        	client.println("meniu_1_stop - Anuleaza meniu_1");
        	client.println("meniu_2 - O tura REL_1 20 min, acum");
        	client.println("meniu_2_stop - Anuleaza meniu_2");
        	client.println("meniu_3 - O tura REL_1 25 min, acum");
        	client.println("meniu_3_stop - Anuleaza meniu_3");
        	client.println("meniu_4 - O tura REL_1 30 min, acum");
        	client.println("meniu_4_stop - Anuleaza meniu_4");
        	client.println("meniu_5 - O tura REL_3 15 min, acum");
        	client.println("meniu_5_stop - Anuleaza meniu_5");
        	client.println("meniu_6 - O tura REL_3 20 min, acum");
        	client.println("meniu_6_stop - Anuleaza meniu_6");
        	client.println("meniu_7 - O tura REL_3 25 min, acum");
        	client.println("meniu_7_stop - Anuleaza meniu_7");
        	client.println("meniu_8 - O tura REL_3 30 min, acum");
        	client.println("meniu_8_stop - Anuleaza meniu_8");
        	client.println("meniu_9 - O tura REL_2 15 min, acum");
        	client.println("meniu_9_stop - Anuleaza meniu_9");
        	client.println("meniu_10 - O tura REL_2 20 min, acum");
        	client.println("meniu_10_stop - Anuleaza meniu_10");
        	client.println("meniu_11 - O tura REL_2 25 min, acum");
        	client.println("meniu_11_stop - Anuleaza meniu_11");
        	client.println("meniu_12 - O tura REL_2 30 min, acum");
        	client.println("meniu_12_stop - Anuleaza meniu_12");
        	client.println("meniu_21 - O tura programata la 05:00 REL_1, apoi REL_2, apoi REL_3, cate 15 min");
        	client.println("meniu_21_stop - Anuleaza meniu_21");
        	client.println("meniu_22 - O tura programata la 05:00 REL_1, apoi REL_2, apoi REL_3, cate 20 min");
        	client.println("meniu_22_stop - Anuleaza meniu_22");
        	client.println("meniu_23 - O tura programata la 05:00 REL_1, apoi REL_2, apoi REL_3, cate 25 min");
        	client.println("meniu_23_stop - Anuleaza meniu_23");
        	client.println("meniu_24 - O tura programata la 05:00 REL_1, apoi REL_2, apoi REL_3, cate 30 min");
        	client.println("meniu_24_stop - Anuleaza meniu_24");
        	client.println("meniu_30 - Program zilnic de la ora 05:00 REL_1, apoi REL_2, apoi REL_3, cate 15 min");
			client.println("meniu_30_stop - Anuleaza meniu_30");
        	client.println("meniu_31 - Program zilnic de la ora 05:00 REL_1, apoi REL_2, apoi REL_3, cate 20 min");
			client.println("meniu_31_stop - Anuleaza meniu_31");
        	client.println("meniu_32 - Program zilnic de la ora 05:00 REL_1, apoi REL_2, apoi REL_3, cate 25 min");
			client.println("meniu_32_stop - Anuleaza meniu_32");
        	client.println("meniu_33 - Program zilnic de la ora 05:00 REL_1, apoi REL_2, apoi REL_3, cate 30 min");
			client.println("meniu_33_stop - Anuleaza meniu_33");
        	client.println("timers_status - Afiseaza status programe active");
        	client.println("print_eeprom - Afiseaza toate valorile din EEPROM");
#endif


        	client.println("****************************");

        }

        if (request.indexOf("localtime") != -1)
        {
          get24HMaxMillis();
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
          Serial.println("");


          //de facut calcul si comparatie daca millis trece de 1 zi - done, se face in get24HMaxMillis()

        }

        if (request.indexOf("SystemRestart") != -1)
        {
          ESP.restart();
        }

        if (request.indexOf("BoardStartupTime") != -1)	// "Status" button
        {
           client.println("*** System status ***");
#ifdef ESPBOX1
           client.println("* Device: ESPBOX1 - lumini curte SPATE *");
#endif
#ifdef ESPBOX2
           client.println("* Device: ESPBOX2 - lumini curte FATA *");
#endif
#ifdef ASP
           client.println("* Device: ASPERSOARE si PICURATOARE *");
#endif
#ifdef DEVBABY1
           client.println("* Device: Placa dezvoltare <DEVBABY1> *");
#endif
           //client.println("* Device: ESPBOX2 *");
           client.print("Board START-UP Time: ");
           client.println(formattedStartupTime);
           client.print("Board START-UP Date: ");
           client.println(startupDate);
           client.print("Stored number of resets: ");
           client.println(nbofresets);
           //client.print("Board Connected Time (s): ");
           //client.println(wifiConnectedTimeBySystemTime);
           //client.print("Board Connected Time: ");
           //client.println(connectedTime);
           client.print("Board Nb of times connected: ");
           client.println(nbOfWifiConnectedTimes);
//           client.print("nbOfWifiConnectionLosses= ");
//           client.println(nbOfWifiConnectionLosses);
           client.print("connectedIP: ");
           client.println(IPAddress(connectedIP));
           client.println(connectedIP);
           client.print("WifiConnectionLosses: ");
           client.println(wifiDisconnectedCounter);
#ifdef ESPBOX1
           client.print("rel1_status: (backup)");
           client.println(rel1_status);
           client.print("rel2_status (lampa spate): ");
           client.println(rel2_status);
           client.print("rel3_status (lampa laterala): ");
           client.println(rel3_status);
#endif
#ifdef ESPBOX2
           client.print("rel1_status (lampa fara senzor): ");
           client.println(rel1_status);
           client.print("rel2_status (priza backup): ");
           client.println(rel2_status);
           client.print("rel3_status (lampa cu senzor): ");
           client.println(rel3_status);
#endif
#ifdef ASP
           client.print("rel1_status (Asp spate): ");
           client.println(rel1_status);
           client.print("rel2_status (Irigatii legume): ");
           client.println(rel2_status);
           client.print("rel3_status (Asp fata): ");
           client.println(rel3_status);
#endif
#ifdef DEVBABY1
           client.print("rel1_status (LED_1): ");
           client.println(rel1_status);
           client.print("rel2_status (LED_2): ");
           client.println(rel2_status);
           client.print("rel3_status (LED_3): ");
           client.println(rel3_status);
#endif
           client.println("");
        }



        if (request.indexOf("timeAspects") != -1)
		{
        	client.print("boardMillis: ");
        	client.println(millis());
		}

        if (request == "meniu_1")
        {
        	// O tura aspersor spate 15 min
        	// rel1
        	if (timerInstantInProgress != TRUE)
			{
				rel1_status = HIGH;
				digitalWrite(REL_1, rel1_status);
				menuNumberInstantInProgress = 1;

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE pornite pt 15 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 is ON");
								client.println("for 15 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[0] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }

        if (request == "meniu_1_stop")
		{
			// Stop program: O tura aspersor spate 15 min
        	if (timerInstantInProgress != FALSE)
        	{
        		if (menuNumberInstantInProgress == 1)
        		{
					rel1_status = LOW;
					digitalWrite(REL_1, rel1_status);

					#ifdef ASP
									client.println("RELAY_1 is OFF");
									client.println("Aspersoare SPATE oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_1 is OFF");
					#endif
									client.println("");

					//unsigned long delta = (millis()/1000) - instantProgramStartTime;
					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[0] = FALSE;
					menuNumberInstantInProgress = 0;
        		}
        		else
				{
        			printThisMenuIsNotInProgress_1(client);
				}
        	}
        	else
        		client.println("No instant activation programs in progress");
		}

        if (request == "meniu_2")
		{
			// O tura aspersor spate 20 min
			// rel1
			if (timerInstantInProgress != TRUE)
			{
				rel1_status = HIGH;
				digitalWrite(REL_1, rel1_status);
				menuNumberInstantInProgress = 2;

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE pornite pt 20 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 is ON");
								client.println("for 20 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[0] = TRUE;
			}
			else
				client.println("A timer is already in progress, stop it first");

		}

		if (request == "meniu_2_stop")
		{
			// Stop program: O tura aspersor spate 20 min
			if (timerInstantInProgress != FALSE)
			{
				if (menuNumberInstantInProgress == 2)
				{
						rel1_status = LOW;
						digitalWrite(REL_1, rel1_status);

						#ifdef ASP
										client.println("RELAY_1 is OFF");
										client.println("Aspersoare SPATE oprite");
						#endif
						#ifdef DEVBABY1
										client.println("LED_1 is OFF");
						#endif
										client.println("");

						client.print("Was ON for [s]: ");
						client.println((millis()/1000) - instantProgramStartTime);

						timerInstantInProgress = FALSE;
						loadsInProgress[0] = FALSE;
						menuNumberInstantInProgress = 0;
				}
				else
				{
					printThisMenuIsNotInProgress_1(client);
				}
			}
			else
				client.println("No instant activation programs in progress");
		}

        if (request == "meniu_3")
		{
			// O tura aspersor spate 25 min
			// rel1
			if (timerInstantInProgress != TRUE)
			{
				rel1_status = HIGH;
				digitalWrite(REL_1, rel1_status);
				menuNumberInstantInProgress = 3;

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE pornite pt 25 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 is ON");
								client.println("for 25 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[0] = TRUE;
			}
			else
				client.println("A timer is already in progress, stop it first");

		}

		if (request == "meniu_3_stop")
		{
			// Stop program: O tura aspersor spate 25 min
			if (timerInstantInProgress != FALSE)
			{
				if (menuNumberInstantInProgress == 3)
				{
						rel1_status = LOW;
						digitalWrite(REL_1, rel1_status);

						#ifdef ASP
										client.println("RELAY_1 is OFF");
										client.println("Aspersoare SPATE oprite");
						#endif
						#ifdef DEVBABY1
										client.println("LED_1 is OFF");
						#endif
										client.println("");

						client.print("Was ON for [s]: ");
						client.println((millis()/1000) - instantProgramStartTime);

						timerInstantInProgress = FALSE;
						loadsInProgress[0] = FALSE;
						menuNumberInstantInProgress = 0;
				}
				else
				{
					printThisMenuIsNotInProgress_1(client);
				}
			}
			else
				client.println("No instant activation programs in progress");
		}

        if (request == "meniu_4")
		{
			// O tura aspersor spate 30 min
			// rel1
			if (timerInstantInProgress != TRUE)
			{
				rel1_status = HIGH;
				digitalWrite(REL_1, rel1_status);
				menuNumberInstantInProgress = 4;

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE pornite pt 30 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 is ON");
								client.println("for 30 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[0] = TRUE;
			}
			else
				client.println("A timer is already in progress, stop it first");

		}

		if (request == "meniu_4_stop")
		{
			// Stop program: O tura aspersor spate 30 min
			if (timerInstantInProgress != FALSE)
			{
				if (menuNumberInstantInProgress == 4)
				{
						rel1_status = LOW;
						digitalWrite(REL_1, rel1_status);

						#ifdef ASP
										client.println("RELAY_1 is OFF");
										client.println("Aspersoare SPATE oprite");
						#endif
						#ifdef DEVBABY1
										client.println("LED_1 is OFF");
						#endif
										client.println("");

						client.print("Was ON for [s]: ");
						client.println((millis()/1000) - instantProgramStartTime);

						timerInstantInProgress = FALSE;
						loadsInProgress[0] = FALSE;
						menuNumberInstantInProgress = 0;
				}
				else
				{
					printThisMenuIsNotInProgress_1(client);
				}
			}
			else
				client.println("No instant activation programs in progress");
		}

        if (request == "meniu_5")
        {
        	// O tura aspersor fata 15 min
        	// rel3
        	if (timerInstantInProgress != TRUE)
			{
				rel3_status = HIGH;
				digitalWrite(REL_3, rel3_status);
				menuNumberInstantInProgress = 5;

				#ifdef ASP
								client.println("RELAY_3 is ON");
								client.println("Aspersoare FATA pornite pt 15 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_3 is ON");
								client.println("for 15 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[2] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }

        if (request == "meniu_5_stop")
		{
			// Stop program: O tura aspersor fata 15 min
        	if (timerInstantInProgress != FALSE)
        	{
        		if (menuNumberInstantInProgress == 5)
        		{
					rel3_status = LOW;
					digitalWrite(REL_3, rel3_status);

					#ifdef ASP
									client.println("RELAY_3 is OFF");
									client.println("Aspersoare FATA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_3 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[2] = FALSE;
					menuNumberInstantInProgress = 0;
        		}
        		else
				{
        			printThisMenuIsNotInProgress_1(client);
				}
        	}
        	else
        		client.println("No instant activation programs in progress");
		}

        if (request == "meniu_6")
        {
        	// O tura aspersor fata 20 min
        	// rel3
        	if (timerInstantInProgress != TRUE)
			{
				rel3_status = HIGH;
				digitalWrite(REL_3, rel3_status);
				menuNumberInstantInProgress = 6;

				#ifdef ASP
								client.println("RELAY_3 is ON");
								client.println("Aspersoare FATA pornite pt 20 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_3 is ON");
								client.println("for 20 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[2] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }

        if (request == "meniu_6_stop")
		{
			// Stop program: O tura aspersor fata 20 min
        	if (timerInstantInProgress != FALSE)
        	{
        		if (menuNumberInstantInProgress == 6)
        		{
					rel3_status = LOW;
					digitalWrite(REL_3, rel3_status);

					#ifdef ASP
									client.println("RELAY_3 is OFF");
									client.println("Aspersoare FATA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_3 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[2] = FALSE;
					menuNumberInstantInProgress = 0;
        		}
        		else
				{
        			printThisMenuIsNotInProgress_1(client);
				}
        	}
        	else
        		client.println("No instant activation programs in progress");
		}

        if (request == "meniu_7")
        {
        	// O tura aspersor fata 25 min
        	// rel3
        	if (timerInstantInProgress != TRUE)
			{
				rel3_status = HIGH;
				digitalWrite(REL_3, rel3_status);
				menuNumberInstantInProgress = 7;

				#ifdef ASP
								client.println("RELAY_3 is ON");
								client.println("Aspersoare FATA pornite pt 25 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_3 is ON");
								client.println("for 25 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[2] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }

        if (request == "meniu_7_stop")
		{
			// Stop program: O tura aspersor fata 25 min
        	if (timerInstantInProgress != FALSE)
        	{
        		if (menuNumberInstantInProgress == 7)
        		{
					rel3_status = LOW;
					digitalWrite(REL_3, rel3_status);

					#ifdef ASP
									client.println("RELAY_3 is OFF");
									client.println("Aspersoare FATA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_3 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[2] = FALSE;
					menuNumberInstantInProgress = 0;
        		}
        		else
				{
        			printThisMenuIsNotInProgress_1(client);
				}
        	}
        	else
        		client.println("No instant activation programs in progress");
		}

        if (request == "meniu_8")
        {
        	// O tura aspersor fata 30 min
        	// rel3
        	if (timerInstantInProgress != TRUE)
			{
				rel3_status = HIGH;
				digitalWrite(REL_3, rel3_status);
				menuNumberInstantInProgress = 8;

				#ifdef ASP
								client.println("RELAY_3 is ON");
								client.println("Aspersoare FATA pornite pt 30 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_3 is ON");
								client.println("for 30 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[2] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }

        if (request == "meniu_8_stop")
		{
			// Stop program: O tura aspersor fata 30 min
        	if (timerInstantInProgress != FALSE)
        	{
        		if (menuNumberInstantInProgress == 8)
        		{
					rel3_status = LOW;
					digitalWrite(REL_3, rel3_status);

					#ifdef ASP
									client.println("RELAY_3 is OFF");
									client.println("Aspersoare FATA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_3 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[2] = FALSE;
					menuNumberInstantInProgress = 0;
        		}
        		else
				{
        			printThisMenuIsNotInProgress_1(client);
				}
        	}
        	else
        		client.println("No instant activation programs in progress");
		}

        if (request == "meniu_9")
         {
         	// O tura picurator gradina 15 min
         	// rel3
         	if (timerInstantInProgress != TRUE)
 			{
 				rel2_status = HIGH;
 				digitalWrite(REL_2, rel2_status);
 				menuNumberInstantInProgress = 9;

 				#ifdef ASP
 								client.println("RELAY_2 is ON");
 								client.println("Picuratoare GRADINA pornite pt 15 min");
 				#endif
 				#ifdef DEVBABY1
 								client.println("LED_2 is ON");
 								client.println("for 15 minutes");
 				#endif
 								client.println("");

 				instantProgramStartTime = millis()/1000;
 				client.print("Board time (s) at program start: ");
 				client.println(instantProgramStartTime);
 				timerInstantInProgress = TRUE;
 				loadsInProgress[1] = TRUE;
 			}
         	else
         		client.println("A timer is already in progress, stop it first");

         }

         if (request == "meniu_9_stop")
 		{
 			// Stop program: O tura picurator gradina 15 min
         	if (timerInstantInProgress != FALSE)
         	{
         		if (menuNumberInstantInProgress == 9)
         		{
 					rel2_status = LOW;
 					digitalWrite(REL_2, rel2_status);

 					#ifdef ASP
 									client.println("RELAY_2 is OFF");
 									client.println("Picuratoare GRADINA oprite");
 					#endif
 					#ifdef DEVBABY1
 									client.println("LED_2 is OFF");
 					#endif
 									client.println("");

 					client.print("Was ON for [s]: ");
 					client.println((millis()/1000) - instantProgramStartTime);

 					timerInstantInProgress = FALSE;
 					loadsInProgress[1] = FALSE;
 					menuNumberInstantInProgress = 0;
         		}
         		else
 				{
         			printThisMenuIsNotInProgress_1(client);
 				}
         	}
         	else
         		client.println("No instant activation programs in progress");
 		}

         if (request == "meniu_10")
          {
          	// O tura picurator gradina 20 min
          	// rel3
          	if (timerInstantInProgress != TRUE)
  			{
  				rel2_status = HIGH;
  				digitalWrite(REL_2, rel2_status);
  				menuNumberInstantInProgress = 10;

  				#ifdef ASP
  								client.println("RELAY_2 is ON");
  								client.println("Picuratoare GRADINA pornite pt 20 min");
  				#endif
  				#ifdef DEVBABY1
  								client.println("LED_2 is ON");
  								client.println("for 20 minutes");
  				#endif
  								client.println("");

  				instantProgramStartTime = millis()/1000;
  				client.print("Board time (s) at program start: ");
  				client.println(instantProgramStartTime);
  				timerInstantInProgress = TRUE;
  				loadsInProgress[1] = TRUE;
  			}
          	else
          		client.println("A timer is already in progress, stop it first");

          }

          if (request == "meniu_10_stop")
  		{
  			// Stop program: O tura picurator gradina 20 min
          	if (timerInstantInProgress != FALSE)
          	{
          		if (menuNumberInstantInProgress == 10)
          		{
  					rel2_status = LOW;
  					digitalWrite(REL_2, rel2_status);

  					#ifdef ASP
  									client.println("RELAY_2 is OFF");
  									client.println("Picuratoare GRADINA oprite");
  					#endif
  					#ifdef DEVBABY1
  									client.println("LED_2 is OFF");
  					#endif
  									client.println("");

  					client.print("Was ON for [s]: ");
  					client.println((millis()/1000) - instantProgramStartTime);

  					timerInstantInProgress = FALSE;
  					loadsInProgress[1] = FALSE;
  					menuNumberInstantInProgress = 0;
          		}
          		else
  				{
          			printThisMenuIsNotInProgress_1(client);
  				}
          	}
          	else
          		client.println("No instant activation programs in progress");
  		}

		  if (request == "meniu_11")
		   {
			// O tura picurator gradina 25 min
			// rel3
			if (timerInstantInProgress != TRUE)
			{
				rel2_status = HIGH;
				digitalWrite(REL_2, rel2_status);
				menuNumberInstantInProgress = 11;

				#ifdef ASP
								client.println("RELAY_2 is ON");
								client.println("Picuratoare GRADINA pornite pt 25 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_2 is ON");
								client.println("for 25 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[1] = TRUE;
			}
			else
				client.println("A timer is already in progress, stop it first");

		   }

		   if (request == "meniu_11_stop")
		{
			// Stop program: O tura picurator gradina 25 min
			if (timerInstantInProgress != FALSE)
			{
				if (menuNumberInstantInProgress == 11)
				{
					rel2_status = LOW;
					digitalWrite(REL_2, rel2_status);

					#ifdef ASP
									client.println("RELAY_2 is OFF");
									client.println("Picuratoare GRADINA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_2 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[1] = FALSE;
					menuNumberInstantInProgress = 0;
				}
				else
				{
					printThisMenuIsNotInProgress_1(client);
				}
			}
			else
				client.println("No instant activation programs in progress");
		}

		if (request == "meniu_12")
		 {
			// O tura picurator gradina 30 min
			// rel3
			if (timerInstantInProgress != TRUE)
			{
				rel2_status = HIGH;
				digitalWrite(REL_2, rel2_status);
				menuNumberInstantInProgress = 12;

				#ifdef ASP
								client.println("RELAY_2 is ON");
								client.println("Picuratoare GRADINA pornite pt 30 min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_2 is ON");
								client.println("for 30 minutes");
				#endif
								client.println("");

				instantProgramStartTime = millis()/1000;
				client.print("Board time (s) at program start: ");
				client.println(instantProgramStartTime);
				timerInstantInProgress = TRUE;
				loadsInProgress[1] = TRUE;
			}
			else
				client.println("A timer is already in progress, stop it first");

		 }

		 if (request == "meniu_12_stop")
		{
			// Stop program: O tura picurator gradina 3 min
			if (timerInstantInProgress != FALSE)
			{
				if (menuNumberInstantInProgress == 12)
				{
					rel2_status = LOW;
					digitalWrite(REL_2, rel2_status);

					#ifdef ASP
									client.println("RELAY_2 is OFF");
									client.println("Picuratoare GRADINA oprite");
					#endif
					#ifdef DEVBABY1
									client.println("LED_2 is OFF");
					#endif
									client.println("");

					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - instantProgramStartTime);

					timerInstantInProgress = FALSE;
					loadsInProgress[1] = FALSE;
					menuNumberInstantInProgress = 0;
				}
				else
				{
					printThisMenuIsNotInProgress_1(client);
				}
			}
			else
				client.println("No instant activation programs in progress");
		}




        if (request == "meniu_35")
        {	// O tura REL_1, REL_2, REL_3 cate 5 min fiecare, la ora X
        	if (timerScheduledOneTime != TRUE)
        	{
				#ifdef ASP
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 5 min incepand cu ora TBD (pt teste)");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1, then LED_2, then LED_3 turned on for 5 min each, beginning XX:XX (for tests)");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduledOneTime = 35;
				setEeprom_allParametersForScheduledOneTime();
        	}
        	else
        	{
        		printAnother1TimeProgramIsScheduled(client);
        	}
        }

        if (request == "meniu_35_stop")
        {	// Anuleaza: O tura REL_1, REL_2, REL_3 cate 5 min fiecare, la ora X
			if (menuNumberScheduledOneTime == 35)
			{
				#ifdef ASP
					client.println("meniu_35 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_35 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 35)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
        }

        if (request == "meniu_36")
        {	// O tura REL_1, pt 5 min, la ora X
        	if (timerScheduledOneTime != TRUE)
        	{
				#ifdef ASP
								client.println("Aspersoare SPATE pt 5 min incepand cu ora TBD (pt teste)");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 turned on for 5 min, beginning XX:XX (for tests)");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 36;
				setEeprom_allParametersForScheduledOneTime();
        	}
        	else
        	{
        		printAnother1TimeProgramIsScheduled(client);
        	}
        }

        if (request == "meniu_36_stop")
        {	// Anuleaza: O tura REL_1, pt 5 min, la ora X
			if (menuNumberScheduledOneTime == 36)
			{
				#ifdef ASP
					client.println("meniu_36 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_36 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 36)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
        }


        if (request == "meniu_21")
		{	// O tura REL_1, REL_2, REL_3 cate 15 min fiecare, la ora 5
			if (timerScheduledOneTime != TRUE)
			{
				#ifdef ASP
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 15 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1, then LED_2, then LED_3 turned on for 15 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduledOneTime = 21;
				setEeprom_allParametersForScheduledOneTime();
			}
			else
			{
				printAnother1TimeProgramIsScheduled(client);
			}
		}

		if (request == "meniu_21_stop")
		{	// Anuleaza: O tura REL_1, REL_2, REL_3 cate 15 min fiecare, la ora 5
			if (menuNumberScheduledOneTime == 21)
			{
				#ifdef ASP
					client.println("meniu_21 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_21 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 21)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
		}

		if (request == "meniu_22")
		{	// O tura REL_1, REL_2, REL_3 cate 20 min fiecare, la ora 5
			if (timerScheduledOneTime != TRUE)
			{
				#ifdef ASP
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 20 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1, then LED_2, then LED_3 turned on for 20 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduledOneTime = 22;
				setEeprom_allParametersForScheduledOneTime();
			}
			else
			{
				printAnother1TimeProgramIsScheduled(client);
			}
		}

		if (request == "meniu_22_stop")
		{	// Anuleaza: O tura REL_1, REL_2, REL_3 cate 20 min fiecare, la ora 5
			if (menuNumberScheduledOneTime == 22)
			{
				#ifdef ASP
					client.println("meniu_22 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_22 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 22)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
		}

		if (request == "meniu_23")
		{	// O tura REL_1, REL_2, REL_3 cate 25 min fiecare, la ora 5
			if (timerScheduledOneTime != TRUE)
			{
				#ifdef ASP
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 25 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1, then LED_2, then LED_3 turned on for 25 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduledOneTime = 23;
				setEeprom_allParametersForScheduledOneTime();
			}
			else
			{
				printAnother1TimeProgramIsScheduled(client);
			}
		}

		if (request == "meniu_23_stop")
		{	// Anuleaza: O tura REL_1, REL_2, REL_3 cate 25 min fiecare, la ora 5
			if (menuNumberScheduledOneTime == 23)
			{
				#ifdef ASP
					client.println("meniu_23 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_23 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 23)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
		}

		if (request == "meniu_24")
		{	// O tura REL_1, REL_2, REL_3 cate 30 min fiecare, la ora 5
			if (timerScheduledOneTime != TRUE)
			{
				#ifdef ASP
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 30 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1, then LED_2, then LED_3 turned on for 30 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduledOneTime = 24;
				setEeprom_allParametersForScheduledOneTime();
			}
			else
			{
				printAnother1TimeProgramIsScheduled(client);
			}
		}

		if (request == "meniu_24_stop")
		{	// Anuleaza: O tura REL_1, REL_2, REL_3 cate 30 min fiecare, la ora 5
			if (menuNumberScheduledOneTime == 24)
			{
				#ifdef ASP
					client.println("meniu_24 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_24 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 24)
				{
					resetAllLoads();
				}
				timerScheduledOneTime = FALSE;
				loadsScheduledOneTime[0] = FALSE;
				loadsScheduledOneTime[1] = FALSE;
				loadsScheduledOneTime[2] = FALSE;
				menuNumberScheduledOneTime = 0;
				setEeprom_allParametersForScheduledOneTime();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_1(client);
			}
		}


		if (request == "meniu_30")
		{	// Zilnic REL_1, REL_2, REL_3 cate 15 min fiecare, la ora 5
			if (timerScheduledDaily != TRUE)
			{
				#ifdef ASP
								client.println("ZILNIC Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 15 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("DAILY LED_1, then LED_2, then LED_3 turned on for 15 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledDaily = TRUE;
				loadsScheduledDaily[0] = TRUE;
				loadsScheduledDaily[1] = TRUE;
				loadsScheduledDaily[2] = TRUE;
				menuNumberScheduledDaily = 30;
				setEeprom_allParametersForScheduledDaily();
			}
			else
			{
				printAnotherDailyProgramIsScheduled(client);
			}
		}

		if (request == "meniu_30_stop")
		{	// Anuleaza: // Zilnic REL_1, REL_2, REL_3 cate 15 min fiecare, la ora 5
			if (menuNumberScheduledDaily == 30)
			{
				#ifdef ASP
					client.println("meniu_30 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_30 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 30)
				{
					resetAllLoads();
				}
				timerScheduledDaily = FALSE;
				loadsScheduledDaily[0] = FALSE;
				loadsScheduledDaily[1] = FALSE;
				loadsScheduledDaily[2] = FALSE;
				menuNumberScheduledDaily = 0;
				setEeprom_allParametersForScheduledDaily();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_2(client);
			}
		}

		if (request == "meniu_31")
		{	// Zilnic REL_1, REL_2, REL_3 cate 30 min fiecare, la ora 5
			if (timerScheduledDaily != TRUE)
			{
				#ifdef ASP
								client.println("ZILNIC Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 20 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("DAILY LED_1, then LED_2, then LED_3 turned on for 20 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledDaily = TRUE;
				loadsScheduledDaily[0] = TRUE;
				loadsScheduledDaily[1] = TRUE;
				loadsScheduledDaily[2] = TRUE;
				menuNumberScheduledDaily = 31;
				setEeprom_allParametersForScheduledDaily();
			}
			else
			{
				printAnotherDailyProgramIsScheduled(client);
			}
		}

		if (request == "meniu_31_stop")
		{	// Anuleaza: // Zilnic REL_1, REL_2, REL_3 cate 20 min fiecare, la ora 5
			if (menuNumberScheduledDaily == 31)
			{
				#ifdef ASP
					client.println("meniu_31 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_31 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 31)
				{
					resetAllLoads();
				}
				timerScheduledDaily = FALSE;
				loadsScheduledDaily[0] = FALSE;
				loadsScheduledDaily[1] = FALSE;
				loadsScheduledDaily[2] = FALSE;
				menuNumberScheduledDaily = 0;
				setEeprom_allParametersForScheduledDaily();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_2(client);
			}
		}

		if (request == "meniu_32")
		{	// Zilnic REL_1, REL_2, REL_3 cate 25 min fiecare, la ora 5
			if (timerScheduledDaily != TRUE)
			{
				#ifdef ASP
								client.println("ZILNIC Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 25 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("DAILY LED_1, then LED_2, then LED_3 turned on for 25 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledDaily = TRUE;
				loadsScheduledDaily[0] = TRUE;
				loadsScheduledDaily[1] = TRUE;
				loadsScheduledDaily[2] = TRUE;
				menuNumberScheduledDaily = 32;
				setEeprom_allParametersForScheduledDaily();
			}
			else
			{
				printAnotherDailyProgramIsScheduled(client);
			}
		}

		if (request == "meniu_32_stop")
		{	// Anuleaza: // Zilnic REL_1, REL_2, REL_3 cate 25 min fiecare, la ora 5
			if (menuNumberScheduledDaily == 32)
			{
				#ifdef ASP
					client.println("meniu_32 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_32 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 32)
				{
					resetAllLoads();
				}
				timerScheduledDaily = FALSE;
				loadsScheduledDaily[0] = FALSE;
				loadsScheduledDaily[1] = FALSE;
				loadsScheduledDaily[2] = FALSE;
				menuNumberScheduledDaily = 0;
				setEeprom_allParametersForScheduledDaily();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_2(client);
			}
		}

		if (request == "meniu_33")
		{	// Zilnic REL_1, REL_2, REL_3 cate 30 min fiecare, la ora 5
			if (timerScheduledDaily != TRUE)
			{
				#ifdef ASP
								client.println("ZILNIC Aspersoare SPATE, apoi GRADINA, apoi FATA pornite fiecare cate 30 min incepand cu ora 05:00");
				#endif
				#ifdef DEVBABY1
								client.println("DAILY LED_1, then LED_2, then LED_3 turned on for 30 min each, beginning 05:00");
				#endif
								client.println("");

				timerScheduledDaily = TRUE;
				loadsScheduledDaily[0] = TRUE;
				loadsScheduledDaily[1] = TRUE;
				loadsScheduledDaily[2] = TRUE;
				menuNumberScheduledDaily = 33;
				setEeprom_allParametersForScheduledDaily();
			}
			else
			{
				printAnotherDailyProgramIsScheduled(client);
			}
		}

		if (request == "meniu_33_stop")
		{	// Anuleaza: // Zilnic REL_1, REL_2, REL_3 cate 30 min fiecare, la ora 5
			if (menuNumberScheduledDaily == 33)
			{
				#ifdef ASP
					client.println("meniu_33 ANULAT");
				#endif
				#ifdef DEVBABY1
					client.println("meniu_33 ANULAT");
				#endif
				client.println("");

				if (menuInProgress == 33)
				{
					resetAllLoads();
				}
				timerScheduledDaily = FALSE;
				loadsScheduledDaily[0] = FALSE;
				loadsScheduledDaily[1] = FALSE;
				loadsScheduledDaily[2] = FALSE;
				menuNumberScheduledDaily = 0;
				setEeprom_allParametersForScheduledDaily();
				menuInProgress = 0;
				setEeprom_menuInProgress(menuInProgress);
			}
			else
			{
				printThisProgramIsNotScheduled_2(client);
			}
		}

        /*if (request.indexOf("meniu_30") != -1)
        {
        	// O tura REL_2, de la ora X la ora Y

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite pt 5min");
				#endif
				#ifdef DEVBABY1
								//client.println("LED_1 is ON");
								client.println("LED_2 turned on from time X to Y");
				#endif
								client.println("");

				timerScheduledOneTimeOneLoadOnly = TRUE;
				loadsScheduledOneTimeOneLoadOnly[0] = FALSE;
				loadsScheduledOneTimeOneLoadOnly[1] = TRUE;
				loadsScheduledOneTimeOneLoadOnly[2] = FALSE;
				menuNumberScheduledOneLoadOnly = 30;
        }*/

        /*if (request.indexOf("meniu_40") != -1)
		{
			// Zilnic REL_2, de la ora X la ora Y

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite pt 5min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_2 turned on daily from time X to Y");
				#endif
				#ifdef ESPBOX1
								client.println("Lampa spate pornita zilnic de la 21:00 la 23:59");
				#endif
								client.println("");

				timerScheduledDailyOneLoadOnly = TRUE;		// Must be stored in EEPROM
				setEeprom_timerScheduledDailyOneLoadOnly(timerScheduledDailyOneLoadOnly);
				loadsScheduledDailyOneLoadOnly[0] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyOneLoadOnly_0(loadsScheduledDailyOneLoadOnly[0]);
				loadsScheduledDailyOneLoadOnly[1] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyOneLoadOnly_1(loadsScheduledDailyOneLoadOnly[1]);
				loadsScheduledDailyOneLoadOnly[2] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyOneLoadOnly_2(loadsScheduledDailyOneLoadOnly[2]);
				menuNumberScheduledDailyOneLoadOnly = 40;	// Must be stored in EEPROM
				setEeprom_menuNumberScheduledDailyOneLoadOnly(menuNumberScheduledDailyOneLoadOnly);
				getEeprom_timerScheduledDailyOneLoadOnly();
		}*/

        /*if (request.indexOf("m40_stop") != -1)
		{
			// Dezactiveaza meniu 40

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite pt 5min");
				#endif
				#ifdef DEVBABY1
								client.println("Cancel program: LED_2 turned on daily from time X to Y");
				#endif
				#ifdef ESPBOX1
								client.println("CANCELLED: Lampa spate pornita zilnic de la 21:00 la 23:59");
				#endif
								client.println("");

				timerScheduledDailyOneLoadOnly = FALSE;		// Must be stored in EEPROM - bit 0 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_timerScheduledDailyOneLoadOnly(timerScheduledDailyOneLoadOnly);
				loadsScheduledDailyOneLoadOnly[0] = FALSE; // Must be stored in EEPROM - bit 1 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyOneLoadOnly_0(loadsScheduledDailyOneLoadOnly[0]);
				loadsScheduledDailyOneLoadOnly[1] = FALSE;  // Must be stored in EEPROM - bit 2 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyOneLoadOnly_1(loadsScheduledDailyOneLoadOnly[1]);
				loadsScheduledDailyOneLoadOnly[2] = FALSE; // Must be stored in EEPROM - bit 3 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyOneLoadOnly_2(loadsScheduledDailyOneLoadOnly[2]);
				menuNumberScheduledDailyOneLoadOnly = 0;  // Must be stored in EEPROM - EEPROM_ADDR_MENU_NB_SCH_DAILY
				setEeprom_menuNumberScheduledDailyOneLoadOnly(menuNumberScheduledDailyOneLoadOnly);
				getEeprom_timerScheduledDailyOneLoadOnly();
				timerScheduledDailyOneLoadOnlyLoadHasBeenSwitchedOn = FALSE;
				rel2_status = LOW;
				digitalWrite(REL_2, rel2_status);
		}*/

        if (request == "timers_status")
        {
        	if (timerInstantInProgress != FALSE) // Instant timer programs in progress
			{
        		client.println("Running program:");
				client.print("Meniu_");
				client.println(menuNumberInstantInProgress);
        		client.println("Running loads:");
				client.print("REL_1, REL_2, REL_3: ");
				client.print(loadsInProgress[0]);
				client.print(loadsInProgress[1]);
				client.print(loadsInProgress[2]);
				client.println("");


        		unsigned long delta = (millis()/1000) - instantProgramStartTime;
        		client.print("Seconds in progress: ");
				client.println(delta);

        	}
        	else
        		client.println("No instant activation programs at this time.");

        	if (menuInProgress != 0)
        	{
        		client.print("A 1-Time or Daily program is in progress: meniu_ ");
				client.println(menuInProgress);
				client.println("Running loads:");
				client.print("REL_1, REL_2, REL_3: ");
				client.print(rel1_status);
				client.print(rel2_status);
				client.print(rel3_status);
        	}
        	else
				client.println("No 1-Time or Daily programs in progress right now.");

        	if (menuNumberScheduledOneTime != 0)
        	{
        		client.print("A 1-Time program is scheduled to run: meniu_");
        		client.println(menuNumberScheduledOneTime);
        	}

        	if (menuNumberScheduledDaily != 0)
        	{
        		client.print("A Daily program is scheduled to run: meniu_");
        		client.println(menuNumberScheduledDaily);
        	}

        }

        if (request == "print_eeprom")
        {
        	eepromPrintAllVariables(client);
        }

        if (request == "get_date")
        {
        	String currentDate;
        	getDateFromNTP(currentDate);
        	client.print("Current date: ");
        	client.println(currentDate);
        }

        if (request == "get_date2")
        {
        	getDateFromNTPToStruct(gs_last_successful_menu_run);
        	client.println("Current date: ");
        	client.print("Y: ");
        	client.println(gs_last_successful_menu_run.y);
        	client.print("M: ");
        	client.println(gs_last_successful_menu_run.mo);
        	client.print("D: ");
        	client.println(gs_last_successful_menu_run.d);
        }



         new_client = 0;
      }

  //Serial.print("Main loop millis: ");
  //Serial.println(millis());

#ifdef DEVBABY1
  if (cnt<10)
  {
	  cnt++;
  }
  else
  {
  Serial.print("localTime: ");
  Serial.println(localTime);
  Serial.print("localTimeMs: ");
  Serial.println(localTimeMs);
  Serial.println("");
  cnt=0;
  }
#endif

  delay(1);
  //Serial.println("Client disonnected");
  //Serial.println("");
}
