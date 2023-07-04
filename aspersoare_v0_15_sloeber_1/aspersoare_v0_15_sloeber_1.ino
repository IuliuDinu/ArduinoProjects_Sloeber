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
#define EEPROM_ADDR_MAX							4095
#define EEPROM_ADDR_RST_COUNTER                	0
#define EEPROM_ADDR_WIFI_CONN_COUNTER          	1
#define EEPROM_ADDR_REL_1_LAST_STATE           	2
#define EEPROM_ADDR_REL_1_LAST_STATE_INV       	3
#define EEPROM_ADDR_REL_2_LAST_STATE           	4
#define EEPROM_ADDR_REL_2_LAST_STATE_INV       	5
#define EEPROM_ADDR_REL_3_LAST_STATE           	6
#define EEPROM_ADDR_REL_3_LAST_STATE_INV       	7
#define EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS	   	8
#define EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y	9
#define EEPROM_ADDR_MENU_IN_PROGRESS		   	10
#define EEPROM_ADDR_LAST_MENU_USED			   	11

 #define EEPROM_TOTAL_NB_OF_DEFINED_BYTES       10

#define ONE_DAY_IN_MILISECONDS  86400000
#define MAX_WIFI_DISC_LOOP_COUNTER 60
//#define TIME_INTERVAL_TO_RECHECK_NTP	300000 // 300000ms = 5 min
#define TIME_INTERVAL_TO_RECHECK_NTP	900000 // 900000ms = 15 min
//#define TIME_INTERVAL_TO_RECHECK_NTP	60000 // 60000ms = 1 min
#define AVG_MEASURED_MAIN_LOOP_DURATION_MS	502

#define TRUE 1
#define FALSE 0

/************* MENU MAP *****************************************
 * "meniu_1" - O tura aspersor spate 15 min
 * "meniu_2" - O tura aspersor spate 20 min
 * "meniu_3" - O tura aspersor spate 25 min
 * "meniu_4" - O tura aspersor spate 30 min
 * "meniu_5" - O tura aspersor fata 15 min
 * "meniu_6" - O tura aspersor fata 20 min
 * "meniu_7" - O tura aspersor fata 25 min
 * "meniu_8" - O tura aspersor fata 30 min
 * "meniu_9" - O tura picurator gradina 15 min
 * "meniu_10" - O tura picurator gradina 20 min
 * "meniu_11" - O tura picurator gradina 25 min
 * "meniu_12" - O tura picurator gradina 30 min
 * "meniu_13" - O tura aspersor fata, apoi spate, cate 15 min
 * "meniu_14" - O tura aspersor fata, apoi spate, cate 20 min
 * "meniu_15" - O tura aspersor fata, apoi spate, cate 25 min
 * "meniu_16" - O tura aspersor fata, apoi spate, cate 30 min
 * "meniu_17" - O tura aspersor fata, apoi spate, apoi picurator cate 15 min
 * "meniu_18" - O tura aspersor fata, apoi spate, apoi picurator cate 20 min
 * "meniu_19" - O tura aspersor fata, apoi spate, apoi picurator cate 25 min
 * "meniu_20" - O tura aspersor fata, apoi spate, apoi picurator cate 30 min
 * "meniu_21" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 15 min
 * "meniu_22" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 20 min
 * "meniu_23" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 25 min
 * "meniu_24" - O tura programata la 05:00 aspersor spate, apoi picurator, apoi aspersor fata, cate 30 min
 * "meniu_25" - O tura programata la XX whatever ora, aspersor spate, apoi picurator, apoi fata cate 5 min
 * "meniu_25_stop" - Anuleaza programarea pt meniu_25
 * "meniu_26" - O tura programata la orele X pana Y, lampa SPATE (rel_2)
 * "meniu_30" - O tura programata de la ora X pana la ora Y, o singura data, pt LED_1 (rel_1)
****************************************************************/
#define MENIU_21_LOCALTIME_START		18000
#define MENIU_21_LOCALTIME_DURATION		900
#define MENIU_22_LOCALTIME_START		18000
#define MENIU_22_LOCALTIME_DURATION		1200
#define MENIU_23_LOCALTIME_START		18000
#define MENIU_23_LOCALTIME_DURATION		1500
#define MENIU_24_LOCALTIME_START		18000
#define MENIU_24_LOCALTIME_DURATION		1800
#define MENIU_25_LOCALTIME_START		80400
#define MENIU_25_LOCALTIME_DURATION		300
#define MENIU_30_LOCALTIME_START		84600
#define MENIU_30_LOCALTIME_END			84900
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
bool timerInProgress = FALSE; // refers to instant timer activation
bool timerScheduledOneTime = FALSE;	// refers to 1-time scheduled activation
bool timerScheduledDaily = FALSE;	// refers to daily scheduled activation
bool timerScheduledOneTimeXtoY = FALSE;	// refers to 1-time scheduled activation, time X to Y, only 1 load
bool timerScheduledDailyXtoY = FALSE;	// refers to daily scheduled activation, time X to Y, only 1 load
bool loadsScheduledOneTime[3] = {{FALSE}}; // refers to 1-time scheduled activation
bool loadsScheduledDaily[3] = {{FALSE}}; // refers to daily scheduled activation
bool loadsScheduledOneTimeXtoY[3] = {{FALSE}}; // refers to 1-time scheduled activation, time X to Y, only 1 load
bool loadsScheduledDailyXtoY[3] = {{FALSE}}; // refers to daily scheduled activation, time X to Y, only 1 load
bool timerScheduledOneTimeStarted = FALSE; // refers to 1-time scheduled activation
bool timerScheduledDailyStarted = FALSE; // refers to daily scheduled activation
bool timerScheduledDailyXtoYLoadHasBeenSwitchedOn = FALSE;
byte menuNumberScheduled = 0;	// refers to 1-time scheduled activation
byte menuNumberScheduledXtoY = 0;	// refers to 1-time scheduled activation, time X to Y, only 1 load
byte menuNumberScheduledDailyXtoY = 0;	// refers to daily scheduled activation, time X to Y, only 1 load; TO BE SWITCHED TO ARRAY
byte menuNumberProgrammed = 0; // refers to instant timer activation

byte loadsInProgress[3] = {{FALSE}};
byte actualLoadInProgress = 0;


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
unsigned long programStartTime = 0;
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

byte setEeprom_timerScheduledDailyXtoY(bool state)
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

byte setEeprom_loadsScheduledDailyXtoY_0(bool state)
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

byte setEeprom_loadsScheduledDailyXtoY_1(bool state)
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

byte setEeprom_loadsScheduledDailyXtoY_2(bool state)
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

byte setEeprom_menuNumberScheduledDailyXtoY(byte val)
{
    byte status = 0;
    EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
    delay(100);
    EEPROM.write(EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y, val);
    delay(100);
    status = EEPROM.commit();
    delay(100);
    return status;
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

bool getEeprom_timerScheduledDailyXtoY()
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

bool getEeprom_loadsScheduledDailyXtoY_0()
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

bool getEeprom_loadsScheduledDailyXtoY_1()
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

bool getEeprom_loadsScheduledDailyXtoY_2()
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

byte getEeprom_menuNumberScheduledDailyXtoY()
{
  byte val = 0;
  EEPROM.begin(EEPROM_TOTAL_NB_OF_DEFINED_BYTES); //1 byte used now
  val = EEPROM.read(EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y);
  delay(100);
  Serial.print("EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y: ");
  Serial.println(val);
  return val;
}

void loadTimersDataFromEEPROM()
{
	timerScheduledDailyXtoY = getEeprom_timerScheduledDailyXtoY();
	loadsScheduledDailyXtoY[0] = getEeprom_loadsScheduledDailyXtoY_0();
	loadsScheduledDailyXtoY[1] = getEeprom_loadsScheduledDailyXtoY_1();
	loadsScheduledDailyXtoY[2] = getEeprom_loadsScheduledDailyXtoY_2();
	menuNumberScheduledDailyXtoY = getEeprom_menuNumberScheduledDailyXtoY();
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


void setup()
{
   //eepromEraseAddr01();
   //eepromEraseRelLastState();
	//eepromEraseAllDefinedBytes();
	//eepromInitParticularByte(EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS);
	//eepromInitParticularByte(EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y);
	//eepromInitParticularByte(EEPROM_ADDR_RST_COUNTER);
	//eepromInitParticularByte(EEPROM_ADDR_WIFI_CONN_COUNTER);
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
	   setEepromRel_2(HIGH);
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

  //load EEPROM values related to timer scheduled daily from X to Y
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

  // Handlers for 1-time scheduled program, one or multiple loads (but in order):
  if (timerScheduledOneTime != FALSE)
  {	Serial.println("timerScheduledOneTime != FALSE");
	if (menuNumberScheduled == 25)	// Handler for menu Nb 25
	{	Serial.println("menuNumberScheduled == 25");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_25_LOCALTIME_START) && (localTime < MENIU_25_LOCALTIME_DURATION))
			{	Serial.println("!!!!! localTime >= MENIU_25_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				timestampForNextLoadSwitch = boardTime + MENIU_25_LOCALTIME_DURATION;
				actualLoadInProgress = 0;
			}
		}
		else	// programmed timer has already started
		{	Serial.println("IN PROGRESS");
			if ((boardTime > timestampForNextLoadSwitch) && (actualLoadInProgress <3))
			{
				timestampForNextLoadSwitch = boardTime + 300;
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
				menuNumberScheduled = 0;
				timerScheduledOneTimeStarted = FALSE;
            	rel1_status = LOW;
            	rel2_status = LOW;
            	rel3_status = LOW;
            	loadsScheduledOneTime[0] = FALSE;
            	loadsScheduledOneTime[1] = FALSE;
            	loadsScheduledOneTime[2] = FALSE;
			}

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);

		}
	}		// End of handler for menu Nb 25
	//if (menuNumberScheduled == XX)	// Handler for menu Nb XX
  } 	// End of handlers for 1-time scheduled program

  // Handlers for 1-time scheduled program from X to Y time, only one load programs:
  if (timerScheduledOneTimeXtoY != FALSE)
  {	Serial.println("timerScheduledOneTimeXtoY != FALSE");
	if (menuNumberScheduledXtoY == 30)	// Handler for menu Nb 30, only rel_2
	{	Serial.println("menuNumberScheduled == 30");
		if (timerScheduledOneTimeStarted == FALSE)	// scheduled timer not yet started
		{
			if (localTime >= MENIU_30_LOCALTIME_START)
			{	Serial.println("!!!!! localTime >= MENIU_30_LOCALTIME_START !!!!!!!!");
				timerScheduledOneTimeStarted = TRUE;
				//timestampForNextLoadSwitch = localTime + 300; // TO DO: to handle with millis to avoid problem around 12 AM
			}
		}
		else	// programmed timer has already started
		{
			Serial.println("IN PROGRESS");
			if (localTime >= MENIU_30_LOCALTIME_END)
			{
				timerScheduledOneTimeXtoY = FALSE;
				menuNumberScheduledXtoY = 0;
				timerScheduledOneTimeStarted = FALSE;
				loadsScheduledOneTimeXtoY[0] = FALSE;
				loadsScheduledOneTimeXtoY[1] = FALSE;
				loadsScheduledOneTimeXtoY[2] = FALSE;
			}

			rel1_status = loadsScheduledOneTimeXtoY[0];
			rel2_status = loadsScheduledOneTimeXtoY[1];
			rel3_status = loadsScheduledOneTimeXtoY[2];

			digitalWrite(REL_1, rel1_status);
			digitalWrite(REL_2, rel2_status);
			digitalWrite(REL_3, rel3_status);

		}
	}		// End of handler for menu Nb 30
	//if (menuNumberScheduled == XX)	// Handler for menu Nb XX
  } 	// End handlers of for 1-time scheduled program from X to Y time, only one load programs:

  // Handlers for daily scheduled program from X to Y time, only one load programs:
  if (timerScheduledDailyXtoY != FALSE)
  {	//Serial.println("timerScheduledDailyXtoY != FALSE");
	if (menuNumberScheduledDailyXtoY == 40)	// Handler for menu Nb 40, only rel_2
	{	//Serial.println("menuNumberScheduled == 40");
		if (timerScheduledDailyStarted == FALSE)	// scheduled timer not yet started
		{
			if ((localTime >= MENIU_40_LOCALTIME_START) && (localTime < MENIU_40_LOCALTIME_END))
			{	//Serial.println("!!!!! localTime >= MENIU_40_LOCALTIME_START !!!!!!!!");
				timerScheduledDailyStarted = TRUE;
				//timestampForNextLoadSwitch = localTime + 300; // TO DO: to handle with millis to avoid problem around 12 AM
			}
		}
		else	// programmed timer has already started
		{
			//Serial.println("IN PROGRESS");

			if (timerScheduledDailyXtoYLoadHasBeenSwitchedOn == FALSE)
			{
				//rel1_status = loadsScheduledDailyXtoY[0];
				rel2_status = loadsScheduledDailyXtoY[1];
				//rel3_status = loadsScheduledDailyXtoY[2];

				//digitalWrite(REL_1, rel1_status);
				digitalWrite(REL_2, rel2_status);
				//digitalWrite(REL_3, rel3_status);
				timerScheduledDailyXtoYLoadHasBeenSwitchedOn = TRUE;
			}

			if (localTime >= MENIU_40_LOCALTIME_END)
			{
				timerScheduledDailyStarted = FALSE;
				//rel1_status = LOW;
				rel2_status = LOW;
				//rel3_status = LOW;
				//digitalWrite(REL_1, rel1_status);
				digitalWrite(REL_2, rel2_status);
				//digitalWrite(REL_3, rel3_status);
				timerScheduledDailyXtoYLoadHasBeenSwitchedOn = FALSE;
			}

		}
	}		// End of handler for menu Nb 40
	//if (menuNumberScheduled == XX)	// Handler for menu Nb XX
  } 	// End handlers of for daily scheduled program from X to Y time, only one load programs:


  ArduinoOTA.handle(); // OTA usage

  WiFiClient client = server.available();

  // Handlers for instant timer activation:
  if ((menuNumberProgrammed == 1) && (timerInProgress != FALSE))
  {
	  if (((currentMillis/1000)-programStartTime) > 900 )
		  {
		  	  rel1_status = LOW;
		  	  digitalWrite(REL_1, rel1_status);
		  	  timerInProgress = FALSE;
		  	  loadsInProgress[0] = FALSE;
		  	  menuNumberProgrammed = 0;
		  }
  }
  // End of handlers for instant timer activation

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
                if (setEepromRel_1(rel1_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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
                if (setEepromRel_1(rel1_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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
                if (setEepromRel_2(rel2_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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
                if (setEepromRel_2(rel2_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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
                if (setEepromRel_3(rel3_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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
                if (setEepromRel_3(rel3_status))
                  client.println("EEPROM status saved OK.");
                else
                  client.println("Failed to save EEPROM status.");
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

        /*if (request.indexOf("meniu_1") != -1)
        {
        	// O tura aspersor spate 15 min
        	// rel1
        	if (timerInProgress != TRUE)
			{
				rel1_status = HIGH;
				digitalWrite(REL_1, rel1_status);
				menuNumberProgrammed = 1;

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE pornite pt 15min");
				#endif
				#ifdef DEVBABY1
								client.println("LED_1 is ON");
								client.println("for 15 minutes");
				#endif
								client.println("");

				programStartTime = millis()/1000;
				client.print("Local time (s) at program start: ");
				client.println(programStartTime);
				timerInProgress = TRUE;
				loadsInProgress[0] = TRUE;
			}
        	else
        		client.println("A timer is already in progress, stop it first");

        }*/

        /*if (request.indexOf("m1_stop") != -1)
		{
                	// O tura aspersor spate 15 min
                	// rel1
        	if (timerInProgress != FALSE)
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

					//unsigned long delta = (millis()/1000) - programStartTime;
					client.print("Was ON for [s]: ");
					client.println((millis()/1000) - programStartTime);

        			timerInProgress = FALSE;
        			loadsInProgress[0] = FALSE;
        			menuNumberProgrammed = 0;

        	}
        	else
        		client.println("This menu is not in progress");

		}*/

        if (request == "meniu_25")
        {
        	// O tura REL_1, REL_2, REL_3 cate 5 min fiecare, la ora X

				#ifdef ASP
								client.println("RELAY_1 is ON");
								client.println("Aspersoare SPATE, apoi GRADINA, apoi FATA pornite pt 5min");
				#endif
				#ifdef DEVBABY1
								//client.println("LED_1 is ON");
								client.println("LED_1, then LED_2, then LED_3 turned on for 5min each, beginning XX:XX");
				#endif
								client.println("");

				timerScheduledOneTime = TRUE;
				loadsScheduledOneTime[0] = TRUE;
				loadsScheduledOneTime[1] = TRUE;
				loadsScheduledOneTime[2] = TRUE;
				menuNumberScheduled = 25;
        }

        if (request == "meniu_25_stop")
        {
        		//TO DO: stopper for menu 25
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

				timerScheduledOneTimeXtoY = TRUE;
				loadsScheduledOneTimeXtoY[0] = FALSE;
				loadsScheduledOneTimeXtoY[1] = TRUE;
				loadsScheduledOneTimeXtoY[2] = FALSE;
				menuNumberScheduledXtoY = 30;
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

				timerScheduledDailyXtoY = TRUE;		// Must be stored in EEPROM
				setEeprom_timerScheduledDailyXtoY(timerScheduledDailyXtoY);
				loadsScheduledDailyXtoY[0] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyXtoY_0(loadsScheduledDailyXtoY[0]);
				loadsScheduledDailyXtoY[1] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyXtoY_1(loadsScheduledDailyXtoY[1]);
				loadsScheduledDailyXtoY[2] = TRUE;	// Must be stored in EEPROM
				setEeprom_loadsScheduledDailyXtoY_2(loadsScheduledDailyXtoY[2]);
				menuNumberScheduledDailyXtoY = 40;	// Must be stored in EEPROM
				setEeprom_menuNumberScheduledDailyXtoY(menuNumberScheduledDailyXtoY);
				getEeprom_timerScheduledDailyXtoY();
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

				timerScheduledDailyXtoY = FALSE;		// Must be stored in EEPROM - bit 0 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_timerScheduledDailyXtoY(timerScheduledDailyXtoY);
				loadsScheduledDailyXtoY[0] = FALSE; // Must be stored in EEPROM - bit 1 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyXtoY_0(loadsScheduledDailyXtoY[0]);
				loadsScheduledDailyXtoY[1] = FALSE;  // Must be stored in EEPROM - bit 2 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyXtoY_1(loadsScheduledDailyXtoY[1]);
				loadsScheduledDailyXtoY[2] = FALSE; // Must be stored in EEPROM - bit 3 of EEPROM_ADDR_TIMER_SCH_DAILY_PARAMS
				setEeprom_loadsScheduledDailyXtoY_2(loadsScheduledDailyXtoY[2]);
				menuNumberScheduledDailyXtoY = 0;  // Must be stored in EEPROM - EEPROM_ADDR_MENU_NB_SCH_DAILY_X_TO_Y
				setEeprom_menuNumberScheduledDailyXtoY(menuNumberScheduledDailyXtoY);
				getEeprom_timerScheduledDailyXtoY();
				timerScheduledDailyXtoYLoadHasBeenSwitchedOn = FALSE;
				rel2_status = LOW;
				digitalWrite(REL_2, rel2_status);
		}*/

        if (request.indexOf("timers_status") != -1)
        {
        	if (timerInProgress != FALSE)
			{
				client.println("Programmed loads:");
				client.print("LED_1, LED_2, LED_3: ");
				client.print(loadsInProgress[0]);
				client.print(loadsInProgress[1]);
				client.print(loadsInProgress[2]);
				client.println("");


        		unsigned long delta = (millis()/1000) - programStartTime;
        		client.print("Seconds in progress: ");
				client.println(delta);

        	}
        	else
        		client.println("No programmed loads at this time.");

        }

        if (request == "meniu_1")
		{
        	client.println("meniu_1 called");
		}

        if (request == "meniu_2")
		{
        	client.println("meniu_2 called");
		}

        if (request == "meniu_3")
		{
        	client.println("meniu_3 called");

        	if ("abcd" == "abcd")
        		client.println("abcd equal abcd");
        	else
        		client.println("abcd not equal abcd");

        	if ("Abcd" == "abcd")
        		client.println("Abcd equal abcd");
        	else
        		client.println("Abcd not equal abcd");

        	if ("abcd_1" == "abcd_1")
        		client.println("abcd_1 equal abcd_1");
        	else
        		client.println("abcd_1 not equal abcd_1");

        	if ("abcd_1" == "abcd_2")
        		client.println("abcd_1 equal abcd_2");
        	else
        		client.println("abcd_1 not equal abcd_2");

        	if ("abcd_1" == "abcd_11")
        		client.println("abcd_1 equal abcd_11");
        	else
        		client.println("abcd_1 not equal abcd_11");


        	if ("abcd_11" == "abcd_13")
        		client.println("abcd_11 equal abcd_13");
        	else
        		client.println("abcd_11 not equal abcd_13");
		}

        if (request == "meniu_4")
		{
        	client.println("meniu_4 called");
		}

        if (request == "meniu_5")
		{
        	client.println("meniu_5 called");
		}

        if (request == "meniu_6")
		{
        	client.println("meniu_6 called");
		}

        if (request == "meniu_7")
		{
        	client.println("meniu_7 called");
		}

        if (request == "meniu_8")
		{
        	client.println("meniu_8 called");
		}

        if (request == "meniu_9")
		{
        	client.println("meniu_9 called");
		}

        if (request == "meniu_10")
		{
        	client.println("meniu_10 called");
		}

        if (request == "meniu_11")
		{
        	client.println("meniu_11 called");
		}

        if (request == "meniu_12")
		{
        	client.println("meniu_12 called");
		}

        if (request == "meniu_20")
		{
        	client.println("meniu_20 called");
		}

        if (request == "meniu_21")
		{
        	client.println("meniu_21 called");
		}

        if (request == "meniu_22")
		{
        	client.println("meniu_22 called");
		}

        if (request == "meniu_30")
		{
        	client.println("meniu_30 called");
		}

        if (request == "meniu_31")
		{
        	client.println("meniu_31 called");
		}

        if (request == "meniu_32")
		{
        	client.println("meniu_32 called");
		}

        if (request == "meniu_30_stop")
		{
        	client.println("meniu_30_stop called");
		}

        if (request == "meniu_20_stop")
		{
        	client.println("meniu_20_stop called");
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
