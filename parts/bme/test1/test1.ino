

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#include <Wire.h>


/// BME280 I2C Address 0x77 for Red, 0x76 for Purple (chinese)
#define BME_ADDR  0x77
/// BME280 I2C SDA pin
#define BME_SDA 21
/// BME280 I2C SCL pin
#define BME_SCL 22

/// BME280 Pressure & Humidity Sensor Object 
Adafruit_BME280 bme; 

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
#define SENSOR_INTERVAL 10000 




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
bool use_bme280;
void setup() {
  delay(2000);
    // make serial monitor printing available
  Serial.begin(115200);
  
  // put your setup code here, to run once:
    MY_DBG("[Setup] BME280... ");
    
    bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
    if (!wireStatus) Serial.print("Wire1 failed to init");

    bmeAddress = BME_ADDR;
    use_bme280 = bme.begin(bmeAddress, &Wire1);
    
    MY_DBG(" done: ");
    MY_DBGln(use_bme280);
}

void loop() {

  if ((unsigned long)(millis() - previousMillis_sensor) >= SENSOR_INTERVAL) 
      {
        previousMillis_sensor = millis();
        

        // put your main code here, to run repeatedly:
        temp = bme.readTemperature();
        hum = bme.readHumidity();
        atm = bme.readPressure();
        atm /= 100;
        
        MY_DBG("[sensor] temperature: ");
        MY_DBGln(temp);
        MY_DBG("[sensor] humidity: ");
        MY_DBGln(hum);
        MY_DBG("[sensor] pressure: ");
        MY_DBGln(atm);
      }
        
}
