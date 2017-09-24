#ifndef topic_h
#define topic_h
//#include <Arduino.h>
//TOPIC COMANDO IN INGRESSO -> subscrive
const char* riscaldaTopic = "/casa/esterno/caldaia/relay"; // 0 o 1 per attivare il riscaldamento
const char* acquaTopic ="/casa/esterno/caldaia/acqua";  //0 o 1 per attivare il preriscaldamento acqua
const char* resetTopic ="/casa/esterno/caldaia/reset"; //0 o 1 per premere pulsante reset
//TOPIC DI RISPOSTA/ALLARME -> publish
//const char* scaldaTopic ="/casa/esterno/caldaia/scalda/set"; // ON o OFF sensore attivita' caldaia
//const char* scaldaAcquaTopic ="/casa/esterno/caldaia/acqua/set"; // ON o OFF sensore attivita' caldaia
//const char* scaldaAvailableTopic ="/casa/esterno/caldaia/scalda"; // ON o OFF sensore attivita' caldaia
const char* alarmTopic ="/casa/esterno/caldaia/alarm";   //ON OFF alarme blocco attivo
const char* tempH20Topic ="/casa/esterno/caldaia/tempH20"; // temp H20 caldaia
const char* powerTopic ="/casa/esterno/caldaia/power"; // temp H20 caldaia
const char* logTopic ="/casa/esterno/caldaia/log"; // temp H20 caldaia
#endif
