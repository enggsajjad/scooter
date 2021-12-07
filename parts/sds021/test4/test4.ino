//using sds021 library with software serial
#include <SoftwareSerial.h>
//#include "src/lib/SDS011/SDS011.h"
#include "sds011lib.h"

//SoftwareSerial particleSensor(26, 27); // RX, TX
//SoftwareSerial SDS_SERIAL(26, 27); // SDS011: use Serial 2 
/// use Serial 2 Object
HardwareSerial SDS_SERIAL(2); // SDS011: use Serial 2 

/// SDS011 TXD pin is connected at RXD of Serial2 Object
#define SDS011_TXD  13
/// SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
#define SDS011_RXD  16


/// Dust sensor object
//SDS011 sds; 
sds011 sds;

long previousMillis_sensor;
/// Debugging, uncomment to switch servers to release version, enable sending to madavi and luftdaten.info, and supress some debug output
//#define RELEASE 

// Setting the servers and debugging control using the RELEASE
#ifndef RELEASE
  ///Non-Release Version: Frost Server Base address used in Frost_Server.cpp
  //#define BASE_URL "http://smartaqnet-dev.dmz.teco.edu/v1.0"
  #define BASE_URL "http://193.196.38.108:8080/FROST-Server/v1.0"
  
  ///Define MYDEBUG for controlling debugging
  #define MY_DEBUG
  
#else
  ///Release Version: Frost Server Base address used in Frost_Server.cpp
  #define BASE_URL "http://api.smartaq.net/v1.0"
  
  ///unDefine MYDEBUG for controlling debugging
  #undef MY_DEBUG
  
#endif 

#ifdef MY_DEBUG
  ///Define MY_DEB() for controlling serial.print debugging
  #define MY_DBG(...) do {Serial.print( __VA_ARGS__ );} while (0)
  ///Define MY_DEBln() for controlling serial.println debugging
  #define MY_DBGln(...) do {Serial.println( __VA_ARGS__ );} while (0)
#else
  ///UnDefine MY_DEB() for controlling serial.print debugging
  #define MY_DBG(...)
  ///UnDefine MY_DEBln() for controlling serial.println debugging
  #define MY_DBGln(...)
#endif // !MY_DEBUG

/// intervall of reading sensor data in ms
#define SENSOR_INTERVAL 2000 




unsigned char bmeAddress;
/// vars to store sensor results, sensor results for dust sensor pm10
float pm10;
/// sensor results for dust sensor pm25
float pm25;
/// temperature values
float temp;
/// humidity values
float hum;
/// pressure values
float atm;
bool status_sds;
void setup() {
  delay(2000);
    // make serial monitor printing available
  Serial.begin(115200);
  
  // put your setup code here, to run once:
    MY_DBG("[Setup] SDS_SERIAL... ");
    
  sds.begin(&SDS_SERIAL,SDS011_TXD,SDS011_RXD);
  //sds.begin((unsigned char)26,(unsigned char) 27);
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
