#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <stdlib.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

unsigned long delayTime;

const char* ssid = "VIVOFIBRA-50B0";  
const char* password = "981743351";

const char* mqtt_server = "18.222.213.239";
int ledAr = 0;
int ledUmi = 0;
int TempM = 0;
int UmiM = 0;
int umax;
int umin ;
int tmax ;
int tmin ;  
int estar = 0;
int estumi = 0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;




const int Ar = 2;
const int Umi = 5;


void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  bool status;
  
 
  status = bme.begin();  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
 
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "ESP32/UMI/MAX") {
    Serial.print("Umidade Max: ");
    umax = messageTemp.toInt();
    Serial.println(umax);
}
  if (String(topic) == "ESP32/UMI/MIN") {
    Serial.print("Umidade Min: ");
    umin = messageTemp.toInt();
    Serial.println(umin);
  }
if (String(topic) == "ESP32/TEMP/MAX") {
    Serial.print("Temperatura Max: ");
    tmax = messageTemp.toInt();
    Serial.println(tmax);
}
  if (String(topic) == "ESP32/TEMP/MIN") {
    Serial.print("Temperatura Min: ");
    tmin = messageTemp.toInt();
    Serial.println(tmin);
  }
  

  if (String(topic) == "ESP32/LED/AR") {
    Serial.print("LED: ");
    ledAr = messageTemp.toInt();
    Serial.println(ledAr);
}
if (String(topic) == "ESP32/LED/UMI") {
    Serial.print("LED: ");
    ledUmi = messageTemp.toInt();
    Serial.println(ledUmi);
}
if (String(topic) == "UMI/Manual") {
    Serial.print("LED: ");
    UmiM = messageTemp.toInt();
    Serial.println(UmiM);
}
if (String(topic) == "TEMP/Manual") {
    Serial.print("LED: ");
    TempM = messageTemp.toInt();
    Serial.println(TempM);
}
  
}

void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
   
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      
      client.subscribe("UMI/Manual");
      client.subscribe("TEMP/Manual");
      client.subscribe("ESP32/LED/AR");
      client.subscribe("ESP32/LED/UMI");
      client.subscribe("ESP32/UMI/MAX");
      client.subscribe("ESP32/UMI/MIN");
      client.subscribe("ESP32/TEMP/MAX");
      client.subscribe("ESP32/TEMP/MIN");
      client.subscribe("LED/TEMP");
      client.subscribe("LED/AR");
      
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(1000);
    }
  }
}
void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  char tempString[8];
    dtostrf(bme.readTemperature(), 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("ESP32/TEMP", tempString);
  
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  char humString[8];
    dtostrf(bme.readHumidity(), 1, 2, humString);
    Serial.print("Humidity: ");
    Serial.println(humString);
    client.publish("ESP32/UMI", humString);
  
  Serial.println();

  char estumiString [8];
  char estarString [8];

   if ( tmin > bme.readTemperature()|| TempM == 1 )
        {
          digitalWrite(ledAr, HIGH);
          estar = 1;
           dtostrf(estar, 1, 2, estarString);
           client.publish("LED/AR", estarString);
         }
  
        if ( tmax > bme.readTemperature()|| TempM == 0 )
        {
           digitalWrite(ledAr, LOW);
          Serial.println ("Desligado");
          estar = 0;
           dtostrf(estar, 1, 2, estarString);
           client.publish("LED/AR", estarString);
        
        }
         if ( umin > bme.readHumidity()|| UmiM == 1 )
        {
          digitalWrite(ledUmi, HIGH);
          Serial.println ("Ligado");
          estumi = 1;
           dtostrf(estumi, 1, 2, estumiString);
           client.publish("LED/UMI", estumiString);
         }
  
        if ( umax > bme.readHumidity()|| UmiM == 0 )
        {
           digitalWrite(ledUmi, LOW);
          Serial.println ("Desligado");
          estumi = 0;
           dtostrf(estumi, 1, 2, estumiString);
           client.publish("LED/UMI", estumiString);
        }

 
}
