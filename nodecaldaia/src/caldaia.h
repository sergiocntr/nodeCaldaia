#ifndef caldaia_h
#define caldaia_h
#include <Arduino.h>
#include <config.h>
#include <topic.h>
#include <pins_arduino.h>
#include <nodeRelay.h>
//#include <time.h>
//#include <string>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
struct CaldaiaData{
  float acquaTemp;
  int power;
};
typedef struct CaldaiaData datiCaldaia;
datiCaldaia valori;
//union valori{
//  float acquaTemp;
//  int power;
//};
void smartDelay(unsigned long ms);
void alarmInterrupt();
void reconnect();
void setup_wifi();
void myOTAUpdate();
float getTemperature();
void acquaInterrupt();
void callback(char* topic, byte* payload, unsigned int length);
void sendThing(datiCaldaia dati,const char* topic,char* argomento);
void myOTAUpdate();
#endif
