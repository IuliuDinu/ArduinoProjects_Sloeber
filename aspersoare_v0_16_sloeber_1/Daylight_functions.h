#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "gpio_defs.h"
#include "specific_typedefs.h"
#include "time_defs.h"
#include "general_defs.h"
#include "platform_defs.h"
#include "E2P_daylight_defs.h"
#include "helper_functions.h"

extern uint8_t sunsetTimeValues[12][31];
extern clock_and_date_type gs_current_time_and_date;

void Init_sunsetTimeValues(uint8_t values[12][31]);
void Refresh_sunsetTime(unsigned long *sunsetTime);

