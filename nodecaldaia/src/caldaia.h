#ifndef caldaia_h
#define caldaia_h
#include <Arduino.h>
#include <debugutils.h>
#include <myFunctions.h>
#include <nodeRelay.h>
#include <DallasTemperature.h>
#include <MedianFilter.h>
#include <pin.h>
MedianFilter mfPower(31, 0);
const uint16_t versione =22;
//dopo ritorno vers 20 25/08/19
uint8_t clientLoop=0;
struct CaldaiaData{
  float acquaTemp;
  uint16_t power;
};
unsigned long caldaiaOnTime;
typedef struct CaldaiaData datiCaldaia;
datiCaldaia valori;
const int ONE_WIRE_BUS = D1;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
nodeRelay riscaldamento(caldaiaPin); //usato x normale riscaldamento
nodeRelay acquacalda(acquaPin); //usato per preriscaldo acqua calda
unsigned long sumPower = 0;
char temperatureString[6];
void smartDelay(unsigned long ms);
void alarmInterrupt();
void reconnect();
void setup_wifi();
float getTemperature();
void callback(char* topic, byte* payload, unsigned int length);
void sendThing(datiCaldaia dati,const char* topic);
void sendMySql(datiCaldaia dati);
#endif