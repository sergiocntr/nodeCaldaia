/*

 It connects to an MQTT server then:
  - on 0 switches off relay
  - on 1 switches on relay
  - on 2 switches the state of the relay

  - sends 0 on off relay
  - sends 1 on on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 The current state is stored in EEPROM and restored on bootup

 //su questo file ci deve stare solo la connessione

*/
#include <math.h>
#include <ota.h>
#include <caldaia.h>
#include <config.h>
#include <topic.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <nodeRelay.h>
#include <pin.h>

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

int readingIn = 0;
char temperatureString[6];
float getTemperature() {
  float temp;
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    smartDelay(100);
  } while (temp == 85.0 || temp == (-127.0));
  temp = roundf(temp * 100) / 100;
  return temp;
}
void setup_wifi() {

  smartDelay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet); // Set static IP (2,7s) or 8.6s with DHCP  + 2s on battery

  while (WiFi.status() != WL_CONNECTED) {
    //extButton();
    //for(int i = 0; i<500; i++){
    //  extButton();
      delay(10000);
    //}
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  //se arriva il topic relay passo il primo carattere alla sub
  //che sara' 0 o 1 attivare o non attivare caldaia
  if (strcmp(topic, riscaldaTopic) == 0) {
    riscaldamento.relay((char)payload[0]);
    //Serial.println("riscaldatopicAttivato");
    //delay(10);
  }
  //se arriva il topic acquacalda faccio bypassare il micro del flussostato
  //che sara' 0 o 1 attivare o non attivare caldaia
  if (strcmp(topic, acquaTopic) == 0) {
    acquacalda.relay((char)payload[0]);
    smartDelay(10);
  }
  //TOPIC Reset Manuale Caldaia se allarme
  if (strcmp(topic, resetTopic) == 0) {
    if((char)payload[0] == '1'){
      //allarmeCaldaia.relay('1');
      smartDelay(10);
      //allarmeCaldaia.relay('0');
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("NodeCaldaia",mqttUser,mqttPass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(logTopic, "NodeMCU Caldaia connesso");
      client.subscribe(riscaldaTopic);
      client.loop();
      client.subscribe(acquaTopic);
      client.loop();
      //client.subscribe(alarmTopic);
      //client.loop();
      //client.subscribe(resetTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //for(int i = 0; i<5000; i++){
      //  extButton();
        smartDelay(2000);
      //}
    }
  }
}
void setup() {
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);          // Blink to indicate setup
  //delay(500);
  //digitalWrite(13, HIGH);
  //delay(500);

  acquacalda.relay('0');
  riscaldamento.relay('0');
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi
  setupOTA();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  DS18B20.begin();
  //se il piedino di allarme scende a zero (spia accesa)
  //resetta la caldaia
  pinMode(acquaIntPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(acquaIntPin), acquaInterrupt, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(alarmPin), alarmInterrupt, FALLING);

}
void acquaInterrupt(){
  float temp = getTemperature();
  client.publish(logTopic, "Interrupt");
}
void sendThing(valori dati,const char* topic,char* argomento) {
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  if (strcmp(topic, tempH20Topic) == 0) {
    JSONencoder[argomento] = dati.acquaTemp;
    dtostrf(dati.acquaTemp, 2, 2, temperatureString);
    client.publish(topic, temperatureString); //prova prova
  }

  if (strcmp(topic, powerTopic) == 0) {
    JSONencoder[argomento] = dati.power;
  }

  char JSONmessageBuffer[50];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  client.publish(topic, JSONmessageBuffer);
  //Serial.println(JSONmessageBuffer);
  /*if (client.publish(topic, JSONmessageBuffer) == true) {
      Serial.println("Success sending message");
  } else {
      Serial.println("Error sending message");
  }*/
  //client.publish(logTopic, "Allarme Blocco ,resettato!");
}
void loop() {

  reconnect();
  valori val;
  val.acquaTemp = getTemperature();
  sendThing(val,tempH20Topic,"tempH20");
  //sendTemp(val);
  //dtostrf(val.acquaTemp, 2, 2, temperatureString);
  //Serial.println(temperatureString);
  for (int i = 0; i < 10; i++) {
    val.power = analogRead(valvePin);
    //Serial.println(readingIn);
    sendThing(val,powerTopic,"power");
    smartDelay(1000);
  }
}
void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do
  {
    ArduinoOTA.handle();
    client.loop();
  } while (millis() - start < ms);
}
