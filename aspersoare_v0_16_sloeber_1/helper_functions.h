#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "gpio_defs.h"
#include "specific_typedefs.h"
#include "time_defs.h"

extern unsigned long gul_max24hMillis;
extern byte rel1_status;
extern byte rel2_status;
extern byte rel3_status;
extern WiFiServer server;
extern u32_t connectedIP;


void blinkAllLeds(byte nbOfTimes, byte period);
void blinkOneLed(byte load, byte nbOfTimes, byte period);
void convertFromSecToStructHMS(unsigned long ul_sec, clock_type *hms_var);
void get24HMaxMillis();
void resetAllLoads();
void serverBegin();
void OTASetup();
void checkCorrectIPObtained();
