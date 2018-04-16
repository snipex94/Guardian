#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11 
#define BME_CS 10

#define BME_SDA D3
#define BME_SDL D4
 
Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
 
#define SEALEVELPRESSURE_HPA (1013.25)

#include "UbidotsMicroESP8266.h"

#define TOKEN  "A1E-XpOk57qiHRBJjjHjQaoVVI6Yn4okDg"  // Put here your Ubidots TOKEN
#define WIFISSID "MakerLab" // Put here your Wi-Fi SSID
#define PASSWORD "makerlab2018" // Put here your Wi-Fi password
//Ubidots client(TOKEN);

HTTPClient http;
WiFiClient client;
const int httpPort = 3000;
const char* host = "192.168.1.197";

unsigned long delayTime;
 
void setup() {
    Serial.begin(9600);
    Serial.println("BME280 test");
 
    bool status;
   
    // default settings
    status = bme.begin();
    if (status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
   
    Serial.println("-- Default Test --");
    delayTime = 1000;
 
    Serial.println();
 
    delay(100); // let sensor boot up

   // client.wifiConnection(WIFISSID, PASSWORD);
    //client.setDebug(true); // Uncomment this line to set DEBUG on
     WiFi.begin(WIFISSID, PASSWORD);
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 
     
   // http.begin("http://192.168.11.97:3000");      //Specify request destination
}
 
 
void loop() {
    printValues();
    delay(delayTime);
}
 
 
void printValues() {
    Serial.print("Temperature = ");
    //Serial.print(bme.readTemperature());
    //Serial.println(" *C");
    float fTemp = bme.readTemperature()*9.0/5.0+32;
    Serial.print(fTemp);
    Serial.println(" *F");

    //float value2 = analogRead(2)
    //client.add("temperature", fTemp);
    //client.add("switch", value2);
    //client.sendAll(true);

    
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

   String data = "hello world";

   Serial.print("Requesting POST: ");
   // Send request to the server:
   client.println("POST /push HTTP/1.1");
   client.println("Host: 192.168.1.197");
   client.println("Accept: */*");
   client.println("Content-Type: application/x-www-form-urlencoded");
   client.print("Content-Length: ");
   client.println(data.length());
   client.println();
   client.print(data);

   delay(500); // Can be changed
  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  Serial.println();
  Serial.println("closing connection");
  delay(5000);
}
