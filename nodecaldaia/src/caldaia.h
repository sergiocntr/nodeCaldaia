#ifndef caldaia_h
#define caldaia_h
#include <Arduino.h>
#include <cconfig.h>
#include <topic.h>
#include <pin.h>
#include <ota.h>
#include <nodeRelay.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h>
#include <MedianFilter.h>
MedianFilter mfPower(31, 0);
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
//ONE WIRE stuff
const int ONE_WIRE_BUS = D1;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
//WIFI Stuff

WiFiClient espClient;
//MQTT stuff
PubSubClient client(espClient);
//relecaldaia
nodeRelay riscaldamento(caldaiaPin); //usato x normale riscaldamento
nodeRelay acquacalda(acquaPin); //usato per preriscaldo acqua calda
//nodeRelay allarmeCaldaia(resetPin); //usato per resettare l allarme caldaia
//valori val;
int readingIn = 0;
char temperatureString[6];
void smartDelay(unsigned long ms);
void alarmInterrupt();
void reconnect();
void setup_wifi();
float getTemperature();
void callback(char* topic, byte* payload, unsigned int length);
void sendThing(datiCaldaia dati,const char* topic,char* argomento);
void sendMySql(datiCaldaia dati);
#endif
