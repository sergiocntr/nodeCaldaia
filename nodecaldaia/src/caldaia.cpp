#include <caldaia.h>
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
  //qui solo inizializzazione del wifi
  smartDelay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet); // Set static IP (2,7s) or 8.6s with DHCP  + 2s on battery
}
void callback(char* topic, byte* payload, unsigned int length) {
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
  //bool conn = False;
  for (char i = 0; i < 10; i++) {
    Serial.print("Attempting MQTT connection...");
    //(clientID, username, password, willTopic, willQoS, willRetain, willMessage)
    if (client.connect(nodeID,mqttUser,mqttPass))
    {
      Serial.println("connected");
      //conn = True;
      client.publish(logTopic, "NodeMCU Caldaia connesso");
      client.subscribe(riscaldaTopic);
      client.loop();
      client.subscribe(acquaTopic);
      client.loop();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      smartDelay(5000);
    }
  }
  //if(!conn) loop();
}
void acquaInterrupt(){
  float temp = getTemperature();
  client.publish(logTopic, "Interrupt");
}
void sendThing(datiCaldaia dati,const char* topic,char* argomento) {
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  dtostrf(dati.acquaTemp, 2, 2, temperatureString);
  JSONencoder["topic"] = argomento;
  JSONencoder["acqua"] = dati.acquaTemp;
  JSONencoder["power"] = dati.power;
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  client.publish(topic, JSONmessageBuffer,TRUE);

}
void setup() {
  acquacalda.relay('0');
  riscaldamento.relay('0');
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi
  setupOTA();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  DS18B20.begin();
  pinMode(acquaIntPin, INPUT_PULLUP);
}
void scaldaacqua(){
  //smartDelay(100);
  if(valori.acquaTemp < 3.0){
    smartDelay(10);
    riscaldamento.relay('1');
    while (getTemperature() < 10) {
      smartDelay(500);
    }
    riscaldamento.relay('0');
    smartDelay(10);
  }
}
void loop() {

  for (char i = 0; i < 10; i++) if (WiFi.status() != WL_CONNECTED) delay(1000);
  //se scollegato ... dormi
  if (WiFi.status() != WL_CONNECTED){
      WiFi.disconnect();
      WiFi.mode( WIFI_OFF );
      WiFi.forceSleepBegin();
      delay( 1 );
      for (char z = 0; z < 60; z++) {
        scaldaacqua();
        smartDelay(5000);
      }
      WiFi.forceSleepWake();
      delay(1);
      //riparti d'accapo
      setup();
    }
  scaldaacqua();
  smartDelay(100);
  reconnect();
  valori.power=0;
  for (char z = 0; z < 120; z++) {
    valori.power += analogRead(valvePin);
    smartDelay(500);
  }
  valori.acquaTemp = getTemperature();
  sendThing(valori,extSensTopic,"Caldaia");
  sendMySql(valori);
}
void sendMySql(datiCaldaia dati){
  if (espClient.connect(host, httpPort))
  {
    Serial.println("connected");
    // Make a HTTP request:
    String s =String("GET /meteofeletto/caldaia_logger.php?gaspower=" + String(dati.power) +
    +"&&pwd=" + webpass +
    +"&&temp=" + String(dati.acquaTemp) +
    + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    //Serial.println(s);
    espClient.println(s);

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
