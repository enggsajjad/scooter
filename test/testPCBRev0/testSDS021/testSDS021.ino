#include "sds011lib.h"

HardwareSerial SDS_SERIAL(2); // SDS011: use Serial 2 

/// SDS011 TXD pin is connected at RXD of Serial2 Object
#define SDS011_TXD  -1
/// SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
#define SDS011_RXD  13

/// Dust sensor object
//SDS011 sds; 
sds011 sds;

long previousMillis_sensor;

/// intervall of reading sensor data in ms
#define SENSOR_INTERVAL 2000 

float pm10;
float pm25;
bool status_sds;

void setup() {
  delay(2000);
    // make serial monitor printing available
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  Serial.println("[Setup] SDS_SERIAL... ");
  sds.begin(&SDS_SERIAL,13,-1);
}

void loop() {

  if ((unsigned long)(millis() - previousMillis_sensor) >= SENSOR_INTERVAL) 
      {
        previousMillis_sensor = millis();
        

        // put your main code here, to run repeatedly:
        //status_sds = sds.read(&pm25, &pm10);
        status_sds = sds.dataQueryCmd(&pm10, &pm25 );
        
        Serial.println("[sensor] Posting sds011 (PM10, PM2.5) values: ("+String(pm10)+","+String(pm25)+")");
      }
        
}
