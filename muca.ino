#include <OneWire.h>
#include <DallasTemperature.h>

#include <SparkFun_ADXL345.h>

#define ONE_WIRE_BUS 2
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS D0
 
//Adafruit_BMP280 bme; // I2C
Adafruit_BMP280 bme(BMP_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
 
#define SEALEVELPRESSURE_HPA (1013.25)
#include "UbidotsMicroESP8266.h"

#define TOKEN  "A1E-XpOk57qiHRBJjjHjQaoVVI6Yn4okDg"  // Put here your Ubidots TOKEN
#define WIFISSID "Tenda" // Put here your Wi-Fi SSID
#define PASSWORD "6541je!geslo" // Put here your Wi-Fi password
//Ubidots client(TOKEN);

HTTPClient http;
WiFiClient client;
const int httpPort = 3000;
const char* host = "192.168.1.197";

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

ADXL345 adxl = ADXL345(); 
int interruptPin = D8;

unsigned long delayTime;
 
void setup() {
    Serial.println("Buzzer Setup");
    pinMode(D3, OUTPUT);
    Serial.println("DONE");
    
    Serial.begin(9600);
    Serial.println("BME280 Setup");
    bool status;
    // default settings
    if (!status) {
    if (!bme.begin(0x77)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    delay(100); // let sensor boot up
    Serial.println("DONE");    

    Serial.println("ADX345 Setup");
    SetupADX345();
    Serial.println("DONE");
    
    Serial.println("Connecting to Wi-Fi");
    WiFi.begin(WIFISSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); 
    Serial.println("WiFi connected");
  }
}
 
 
void loop() {
    //printValues();
    //readADX345();
    digitalWrite(D3, HIGH);
    delay(1);
    digitalWrite(D3, LOW);
    delay(1);
    //delay(delayTime);
}
 
void readADX345(){
  // Accelerometer Readings
  int x,y,z;
  Serial.println("Reading ADX345");   
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
  Serial.print("X-axis = ");
  Serial.println(x);
  Serial.print("Y-axis = ");
  Serial.println(y);
  Serial.print("Z-axis = ");
  Serial.println(z);
  Serial.println("DONE");
}
 
void printValues() {

    Serial.print("Requesting temperatures...");
    float fTemp = bme.readTemperature();
    Serial.print(fTemp);
    Serial.println(" *C");
    Serial.println("DONE");

    Serial.println("Sending data to server");
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

   String data = String(fTemp, 2);
   String msg = "Naprava_1:" + data;

   Serial.print("Requesting POST: ");
   // Send request to the server:
   client.println("POST /push HTTP/1.1");
   client.println("Host: 192.168.1.197");
   client.println("Accept: */*");
   client.println("Content-Type: application/x-www-form-urlencoded");
   client.print("Content-Length: ");
   client.println(msg.length());
   client.println();
   client.print(msg);

   delay(500); // Can be changed
  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
  Serial.println();
  Serial.println("closing connection");
  delay(5000);
}

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */
void ADXL_ISR() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  } 
  
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
     //add code here to do when activity is sensed
  }
  
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("*** DOUBLE TAP ***");
     //add code here to do when a 2X tap is sensed
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     //add code here to do when a tap is sensed
  } 
}


void SetupADX345(){
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
   
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt
}



