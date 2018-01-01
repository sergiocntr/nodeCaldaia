#ifndef config_h
#define config_h
#include <stdint.h>
const char* mqtt_server = "localhost";
const uint16_t mqtt_port = 8883;
const char* mqttUser ="sergio";
const char* mqttPass ="donatella";
const char* sensorsTopic = "/casa/interno/camera_noi/sensori";
//const char* inTopic ="/casa/esterno/terrazza_leo/input";
const char* logTopic ="/casa/esterno/terrazza_leo/log";
#endif
