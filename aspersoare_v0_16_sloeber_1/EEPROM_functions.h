#include "Arduino.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include "EEPROM_defs.h"
#include "logical_defs.h"
#include "specific_typedefs.h"

extern bool timerScheduledOneTime;
extern bool timerScheduledDaily;
extern bool loadsScheduledOneTime[3];
extern bool loadsScheduledDaily[3];
extern byte menuNumberScheduledOneTime;
extern byte menuNumberScheduledDaily;
extern byte menuInProgress;
extern byte lastMenuSuccessfullyEnded;

extern clock_and_date_type gs_last_successful_menu_run;

extern byte nbOfWifiConnectedTimes;


byte eepromInitParticularByte(byte address);
byte eepromInitParticularBytesFromTo(byte address_from, byte address_to);
void eepromEraseAllDefinedBytes();
void eepromEraseAddr01();
void eepromEraseRelLastState();
byte setEepromRel_1(byte state);
byte setEepromRel_2(byte state);
byte setEepromRel_3(byte state);
byte setEeprom_timerScheduledDaily(bool state);
byte setEeprom_loadsScheduledDaily_0(bool state);
byte setEeprom_loadsScheduledDaily_1(bool state);
byte setEeprom_loadsScheduledDaily_2(bool state);
byte setEeprom_menuNumberScheduledDaily(byte val);
byte setEeprom_timerScheduledOneTime(bool state);
byte setEeprom_loadsScheduledOneTime_0(bool state);
byte setEeprom_loadsScheduledOneTime_1(bool state);
byte setEeprom_loadsScheduledOneTime_2(bool state);
byte setEeprom_menuNumberScheduledOneTime(byte val);
byte setEeprom_menuInProgress(byte val);
byte setEeprom_lastMenuSuccessfullyEnded(byte val);
byte setEeprom_lastMenuSuccessfullyEnded_Day(byte val);
byte setEeprom_lastMenuSuccessfullyEnded_Hour(byte val);
byte setEeprom_lastMenuSuccessfullyEnded_Min(byte val);
byte setEeprom_lastMenuSuccessfullyEnded_Sec(byte val);
void setEeprom_allParametersForScheduledOneTime();
void setEeprom_allParametersForScheduledDaily();
byte getEepromRel_1();
byte getEepromRel_2();
byte getEepromRel_3();
bool getEeprom_timerScheduledDaily();
bool getEeprom_loadsScheduledDaily_0();
bool getEeprom_loadsScheduledDaily_1();
bool getEeprom_loadsScheduledDaily_2();
byte getEeprom_menuNumberScheduledDaily();
bool getEeprom_timerScheduledOneTime();
bool getEeprom_loadsScheduledOneTime_0();
bool getEeprom_loadsScheduledOneTime_1();
bool getEeprom_loadsScheduledOneTime_2();
byte getEeprom_menuNumberScheduledOneTime();
byte getEeprom_menuInProgress();
byte getEeprom_lastMenuSuccessfullyEnded();
byte getEeprom_lastMenuSuccessfullyEnded_Day();
byte getEeprom_lastMenuSuccessfullyEnded_Hour();
byte getEeprom_lastMenuSuccessfullyEnded_Min();
byte getEeprom_lastMenuSuccessfullyEnded_Sec();
void eepromPrintAllVariables(WiFiClient client);
void loadTimersDataFromEEPROM();
void saveLastMenuSuccessfullyEnded_Parameters();

void updateWifiConnectionCounter();
