/*
 *  dht.c:
 *	read temperature and humidity from DHT11 or DHT22 sensor
 */
#include </home/pi/dht22/src/config.h>
#include <mosquitto.h>
#include </home/pi/dht22/src/ArduinoJson-v5.11.1.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_TIMINGS	85
#define DHT_PIN		3	/* GPIO-22 */

int data[5] = { 0, 0, 0, 0, 0 };
float myHum,myTemp;
void read_dht_data()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j			= 0, i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHT_PIN, OUTPUT );
	digitalWrite( DHT_PIN, LOW );
	delay( 18 );

	/* prepare to read the pin */
	pinMode( DHT_PIN, INPUT );

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHT_PIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHT_PIN );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) )
		{
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	 myHum = 0;
	 myTemp = 0;
	if ( (j >= 40) &&
	     (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
	{
		myHum = (float)((data[0] << 8) + data[1]) / 10;
		//myHum = ceil(myHum*100)/100;
		if ( myHum > 100 )
		{
			myHum = data[0];	// for DHT11
		}
		myTemp = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		//myTemp = ceil(myTemp*100)/100;
		//printf( "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );
	}//else  {
		//printf( "Data not good, skip\n" );
	//}
}
void sendMqtt(){
	struct mosquitto *mosq = NULL;
	// Initialize the Mosquitto library
	mosquitto_lib_init();

	// Create a new Mosquito runtime instance with a random client ID,
	//  and no application-specific callback data.
	mosq = mosquitto_new (NULL, true, NULL);
	if (!mosq)
		{
		fprintf (stderr, "Can't initialize Mosquitto library\n");
		exit (-1);
		}

	mosquitto_username_pw_set (mosq, mqttUser, mqttPass);

	// Establish a connection to the MQTT server. Do not use a keep-alive ping
	int ret = mosquitto_connect (mosq, mqtt_server, mqtt_port, 0);
	if (ret)
		{
		fprintf (stderr, "Can't connect to Mosquitto server\n");
		exit (-1);
		}

	StaticJsonBuffer<300> JSONbuffer;
	JsonObject& JSONencoder = JSONbuffer.createObject();
	//JSONencoder["sensorType"] = "Hum";
	JSONencoder["HumCasa"] = myHum;
	//JSONencoder["sensorType"] = "Temp";
	JSONencoder["TempCasa"] = myTemp;
	//JSONencoder["sensorType"] = "Pres";
	char JSONmessageBuffer[100];
	JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
	//printf(JSONmessageBuffer);

	ret = mosquitto_publish (mosq, NULL, sensorsTopic,
  strlen(JSONmessageBuffer), JSONmessageBuffer, 0, false);
	if (ret)
    {
    fprintf (stderr, "Can't publish to Mosquitto server\n");
    //exit (-1);
    }

	// We need a short delay here, to prevent the Mosquito library being
	//  torn down by the operating system before all the network operations
	//  are finished.
	sleep (1);

	// Tidy up
	mosquitto_disconnect (mosq);
	mosquitto_destroy (mosq);
	mosquitto_lib_cleanup();
}
int main( void )
{
	if ( wiringPiSetup() == -1 ) exit( 1 );
	read_dht_data();
	while ((myHum == 0) |( myTemp == 0)) {
		delay( 2000 ); /* wait 2 seconds before next read */
		read_dht_data();
	}
	//printf( "Humidity = %.1f %% Temperature = %.1f *C \n", myHum, myTemp );
	//}
	sendMqtt();
	return(0);
}
