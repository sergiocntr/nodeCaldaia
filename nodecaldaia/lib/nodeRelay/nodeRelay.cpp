//#include <ESP8266WiFi.h>
//#include <Bounce2.h>
#include <nodeRelay.h>
#include "Arduino.h"
//char* myTopic = "/casa/esterno/caldaia/";
// Instantiate a Bounce object :
//Bounce debouncer = Bounce();

int _relayPin;
int _buttonPin;
bool _relayState;
nodeRelay::nodeRelay(int relayPin) {

  pinMode(relayPin, OUTPUT);     // Initialize the relay pin as an output
  //pinMode(buttonPin, INPUT);     // Initialize the relay pin as an output
  digitalWrite(relayPin,LOW);
  //debouncer.attach(buttonPin);   // Use the bounce2 library to debounce the built in button
  //debouncer.interval(50);         // Input must be low for 50 ms
  _relayPin = relayPin;
}
nodeRelay::nodeRelay(int relayPin,int buttonPin) {

  pinMode(relayPin, OUTPUT);     // Initialize the relay pin as an output
  pinMode(buttonPin, INPUT_PULLUP);     // Initialize the relay pin as an INPUT_PULLUP
  digitalWrite(relayPin,HIGH);
  //debouncer.attach(buttonPin);   // Use the bounce2 library to debounce the built in button
  //debouncer.interval(50);         // Input must be low for 50 ms
  _relayPin = relayPin;
  _buttonPin = buttonPin;
}
void nodeRelay::relay(char mychar){ //funziona al contrario mettendo a zero il positivo
if (mychar == '1') {
  digitalWrite(_relayPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
  Serial.println("relayPin -> LOW");
  _relayState = LOW;
  //EEPROM.write(0, relayState);    // Write state to EEPROM
  //EEPROM.commit();
} else if (mychar == '0') {
  digitalWrite(_relayPin, HIGH);  // Turn the LED off by making the voltage HIGH
  Serial.println("relayPin -> HIGH");
  _relayState = HIGH;
  //EEPROM.write(0, relayState);    // Write state to EEPROM
  //EEPROM.commit();
}/* else if (mychar == '2') {
  relayState = !relayState;
  digitalWrite(_relayPin, relayState);  // Turn the LED off by making the voltage HIGH
  Serial.print("relayPin -> switched to ");
  Serial.println(relayState);
  //EEPROM.write(0, relayState);    // Write state to EEPROM
  //EEPROM.commit();
}*/
}
bool relayState(){
  return _relayState;
}
