
//#define DEBUGMIO
#include <caldaia.h>
float getTemperature() {
  float temp=22.22;
  #ifndef DEBUGMIO
    do {
      DS18B20.requestTemperatures();
      temp = DS18B20.getTempCByIndex(0);
      smartDelay(100);
    } while (temp == 85.0 || temp == (-127.0));
    temp = roundf(temp * 100) / 100;
  #endif
  return temp;
}
void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,updateTopic) == 0){
    delay(10);
    if(payload[0]==49){
      send(logTopic, "aggiornamento Caldaia");
      delay(10);
      uint8_t miocheck = checkForUpdates(versione);
      switch(miocheck) {
        case 1:
          send(logTopic, "HTTP_UPDATE_FAIL"); 
          break;
        case 2:
          send(logTopic, "HTTP_UPDATE_NO_UPDATES");
          break;
        case 0:
          send(logTopic, "Already on latest version" );
          break;
        default:
          send(logTopic, "Firmware version check failed, got HTTP response code " + String(miocheck));
          break;
      }
    }
  }
  else if (strcmp(topic, riscaldaTopic) == 0) 
  {
    riscaldamento.relay(payload[0]);
  }
  else if (strcmp(topic, acquaTopic) == 0) 
  {
    acquacalda.relay(payload[0]);
    if(payload[0]==49) caldaiaOnTime=millis(); //se accendo l'acqua mi segno il tempo
  }
  smartDelay(100);
}
void reconnect() {
  client.publish(logTopic, "NodeMCU Caldaia connesso");
  client.subscribe(riscaldaTopic);
  client.subscribe(acquaTopic);
  client.subscribe(updateTopic);
  client.loop();
}
void sendThing(datiCaldaia dati,const char* topic) {
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  dtostrf(dati.acquaTemp, 2, 2, temperatureString);
  JSONencoder["topic"] = "Caldaia";
  JSONencoder["acqua"] = dati.acquaTemp;
  JSONencoder["power"] = dati.power;
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  yield();
  client.publish(topic, JSONmessageBuffer,true);
  client.loop();
}
void setup() {
  acquacalda.relay('0');
  riscaldamento.relay('0');
  #ifdef DEBUGMIO
    Serial.begin(9600);
    delay(3000);
    DEBUG_PRINT("Booting!");
    DEBUG_PRINT("Versione: " + String(versione));
    setIP(ipCaldaiaProva,caldaiaId);
  #else
    setIP(ipCaldaia,caldaiaId);
    DS18B20.begin();
  #endif // DEBUG
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  connectMQTT();
  smartDelay(500);
  reconnect();
  wifi_reconnect_time=millis();
}
void scaldaacqua(){
  valori.acquaTemp = getTemperature();
  if(valori.acquaTemp < 3.0){
    smartDelay(10);
    riscaldamento.relay('1');
    while (getTemperature() < 10) {
      smartDelay(500);
    }
    riscaldamento.relay('0');
    smartDelay(100);
  }
}
void loop() {
  uint8_t valori_da_inviare=0;     // questo è il flag se i valori sono da trasmettere
  smartDelay(500);              //ogni 0,5 sec
  uint16_t currpower= analogRead(valvePin); //leggo la valvola del gas
  mfPower.in(currpower);
  currpower=mfPower.out();      //questo serve a non avere picchi
  sumPower+=currpower;          //somma col valore precedente
  if((currpower > 50) && (client.connected())) //se il gas è aperto update della temperatura immediatamente
  { 
    valori_da_inviare=1;
  } 
  //questo è il controllo periodico della connessione da fare
  //se sono passati piu x secondi dall ultimo controllo e non sia già stato appurato che sia la fiamma è accesa
  //e MQTT è collegato,cosa che di fatto verifica connessione e acqua non ghiacciata nella riga sopra
  // quindi else if
  else if((millis() - wifi_reconnect_time) > wifi_check_time){ 
    if(((millis() - caldaiaOnTime) > 300*1000UL) && (acquacalda.relayState()==0) && (getTemperature()<30)){
    //se dopo 5 minuti con la acqua abilitata la temperatura non è sopra i 30 ° C
    //non abbiamo bisogno di acqua calda
    send(acquaTopic,0);
    }
    DEBUG_PRINT("Controllo WIFI");
    wifi_reconnect_time=millis();
    if (client.state()!=0) {  // se non connesso a MQTT
      DEBUG_PRINT("MQTT NON VA");
      mqtt_reconnect_tries++;
      connectWiFi();    //verifico connessione WIFI
      delay(100);
      connectMQTT();
      smartDelay(500);
      reconnect();
    }else {
      DEBUG_PRINT("WIFI OK");
      mqtt_reconnect_tries=0;
      wifi_check_time = 300000; //ogni 5 minuti
      valori_da_inviare=2;
    }
    if ((mqtt_reconnect_tries > 5) && (!client.connected())) wifi_check_time = 1200000;  //venti minuti
    scaldaacqua();              //ogni volta vedo che non ghiacci
  }
  if(valori_da_inviare==1)
  {
    valori.acquaTemp = getTemperature();
    sendThing(valori,extSensTopic);
    delay(100);
  }
  else if(valori_da_inviare==2)
  {
    valori.power=sumPower;
    valori.acquaTemp = getTemperature();
    sendThing(valori,extSensTopic);
    delay(100);
    sendMySql(valori);
    sumPower=0;
  }
}
void sendMySql(datiCaldaia dati){
  WiFiClient mySqlclient;
  if (mySqlclient.connect(host, httpPort))
  {
    String s =String("GET /meteofeletto/caldaia_logger.php?gaspower=" + String(dati.power) +
    +"&&pwd=" + webpass +
    +"&&temp=" + String(dati.acquaTemp) +
    + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    yield();
    mySqlclient.println(s);
    smartDelay(100);
    mySqlclient.stop();
  }
}
void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do
  {
    client.loop();
    delay(10);
  } while (millis() - start < ms);
}