#ifndef cconfig_h
#define cconfig_h
#include <Arduino.h>
//WIFI stuff
#include <ESP8266WiFi.h>
const char* ssid     = "TIM-23836387";
const char* password = "51vEBuMvmALxNQHVIHQKkn52";
const char* host = "www.developteamgold.altervista.org";
const int httpPort = 80;
const char* webpass ="admin";
IPAddress ip(192, 168, 1, 212); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
const char* nodeID="NodeCaldaia";
const char* mqtt_server = "192.168.1.100";
const uint16_t mqtt_port = 8883;
const char* mqttUser ="sergio";
const char* mqttPass ="donatella";
bool isHot = 0;
#endif
