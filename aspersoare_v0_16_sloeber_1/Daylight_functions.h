#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "gpio_defs.h"
#include "specific_typedefs.h"
#include "time_defs.h"
#include "general_defs.h"
#include "platform_defs.h"
#include "E2P_daylight_defs.h"

void Init_sunsetTimeValues(uint8_t values[12][31]);

