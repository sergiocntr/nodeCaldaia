#ifndef caldaia_h
#define caldaia_h
#include <Arduino.h>
#include <config.h>
#include <topic.h>
#include <pins_arduino.h>
#include <nodeRelay.h>
//#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
void smartDelay(unsigned long ms);
void alarmInterrupt();
void reconnect();
void setup_wifi();
void myOTAUpdate();
float getTemperature();
void callback(char* topic, byte* payload, unsigned int length);
void myOTAUpdate();
#endif
