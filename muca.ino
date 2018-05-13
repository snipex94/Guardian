//#include <Wire.h>
//#include <SPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

#include "UbidotsMicroESP8266.h"

#define TOKEN  "A1E-JcwU4OP6GUxV79JyO3crQyoFSP098W"  // Put here your Ubidots TOKEN
#define WIFISSID "MakerLab" // Put here your Wi-Fi SSID
#define PASSWORD "makerlab2018" // Put here your Wi-Fi password
Ubidots client(TOKEN);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

unsigned long delayTime;
 
void setup() {
    Serial.begin(9600);
    Serial.println("BME280 test");
 
    bool status;
   
    // default settings
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
   
    Serial.println("-- Default Test --");
    delayTime = 1000;
 
    Serial.println();
 
    delay(100); // let sensor boot up

    client.wifiConnection(WIFISSID, PASSWORD);
    //client.setDebug(true); // Uncomment this line to set DEBUG on

    sensors.begin();
}
 
 
void loop() {
    printValues();
    delay(delayTime);
}
 
 
void printValues() {
    Serial.print("Temperature = ");
    //Serial.print(bme.readTemperature());
    //Serial.println(" *C");
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures(); // Send the command to get temperatures
    Serial.println("DONE");
    float fTemp = sensors.getTempCByIndex(0);
    Serial.println(" *F");

    //float value2 = analogRead(2)
    client.add("temperature", fTemp);
    //client.add("switch", value2);
    client.sendAll(true);
}
