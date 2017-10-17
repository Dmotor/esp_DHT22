/*
 Name:		DHT22_roomsensor.ino
 Created:	17.10.2017 18:10:17
 Author:	Dmotor
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "192.168.1.21";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg_temp[50];
char msg_hum[50];
char msg_hic[50];
int value = 0;

void setup() {
	Serial.begin(9600);
	
	Serial.println("DHTxx test!");
	dht.begin();

	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
}

void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
//Нет в плане функции приема

}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("ESP8266Client")) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish("outTopic", "hello world");
			// ... and resubscribe
			client.subscribe("inTopic");
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

void loop() {



	//Serial.print("Humidity: ");
	//Serial.print(h);
	//Serial.print(" %\t");
	//Serial.print("Temperature: ");
	//Serial.print(t);
	//Serial.print(" *C ");
	//Serial.print(f);
	//Serial.print(" *F\t");
	//Serial.print("Heat index: ");
	//Serial.print(hic);
	//Serial.print(" *C ");
	//Serial.print(hif);
	//Serial.println(" *F");

	//----------отправка MQTT данных
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	long now = millis();
	if (now - lastMsg > 2000) {
		lastMsg = now;

		// Reading temperature or humidity takes about 250 milliseconds!
		// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
		float h = dht.readHumidity();
		// Read temperature as Celsius (the default)
		float t = dht.readTemperature();
		// Read temperature as Fahrenheit (isFahrenheit = true)
		float f = dht.readTemperature(true);
		// Check if any reads failed and exit early (to try again).
		if (isnan(h) || isnan(t) || isnan(f)) {
			Serial.println("Failed to read from DHT sensor!");
			return;
			
			// Compute heat index in Celsius (isFahreheit = false)
			float hic = dht.computeHeatIndex(t, h, false);

			sprintf(msg_temp, "%d.%02d", (int)t, abs((int)(t * 100) % 100));
			client.publish("Room1/Temp", msg_temp);
			Serial.print("Temperature = ");
			Serial.print(t);
			Serial.println(" *C");
			
			sprintf(msg_hum, "%d.%02d", (int)h, abs((int)(h * 100) % 100));
			client.publish("Room1/Hum", msg_hum);
			Serial.print("Hum = ");
			Serial.print(h);
			Serial.println(" %");

			sprintf(msg_his, "%d.%02d", (int)hic, abs((int)(hic * 100) % 100));
			client.publish("Room1/His", msg_hum);
			Serial.print("His = ");
			Serial.print(hic);
			Serial.println(" .");

		}
	}

