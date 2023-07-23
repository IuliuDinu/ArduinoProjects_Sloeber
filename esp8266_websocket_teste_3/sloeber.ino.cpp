#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2023-07-23 14:08:10

#include "Arduino.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>

String getSliderValues();
void initFS() ;
void initWiFi() ;
void notifyClients(String sliderValues) ;
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) ;
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) ;
void initWebSocket() ;
void setup() ;
void loop() ;

#include "esp8266_websocket_teste_3.ino"


#endif
