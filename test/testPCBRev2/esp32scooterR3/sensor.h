/**
 * \file sensor.h
 *
 * \brief variable and definitions for sensors
 *
 * This is the files that contains the variables, pins definitions and functions for sensors to be connected.
 * 
 * \date 07.09.2020
 */

#include <SoftwareSerial.h>

/// DHT22 sensor communication pin
#define DHT_PIN 21
/// BME280 I2C Address 0x77 for Red, 0x76 for Purple (chinese)
#define BME_ADDR  0x76
/// BME280 I2C SDA pin
#define BME_SDA 19//21
/// BME280 I2C SCL pin
#define BME_SCL 18//22

/// BME280 Pressure & Humidity Sensor Object 
Adafruit_BME280 bme; 

/// temperature values
float temp;
/// humidity values
float hum;
/// pressure values
float atm;
/// status of bme sensor begin
bool use_bme280;
/// i2c address of bme sensor begin
unsigned char bmeAddress;

/// use Serial 2 Object
HardwareSerial SDS_SERIAL(2); // SDS011: use Serial 2 
/// SDS011 TXD pin is connected at RXD of Serial2 Object
#define trans_to_sds  2
/// SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
#define recv_from_sds  15

/// Dust sensor object
SDS011 sds; 
/// vars to store sensor results, sensor results for dust sensor pm10
float pm10;
/// sensor results for dust sensor pm25
float pm25;
/// status of sds sensor begin
bool status_sds;


/// DHT22 Temperature & Humidity Sensor Object
DHT dht(DHT_PIN, DHT22);
/// conflicts of same defining by dht lib and esp32_thingsapi lib
#ifdef DHT22 
#undef DHT22
#endif 

/// Default Serial0 used for USB TX (Debugging)
#define trans_to_usb 1
/// Default Serial0 used for USB RX (Programming) but here connected for GPS_TX
#define recv_from_usb 3


/// Serial port for GPRS (AT) Commands
#define gsmSerial Serial1
/// Serial port for USB
#define usbSerial Serial

/// Debugging, uncomment to switch servers to use A9G Module otherwise A7
#define A9G 

/// Setting the servers and debugging control using the A9G
#ifndef A9G
  //A7G
  /// GPRS RX Pin
  #define recv_from_gprs 16
  /// GPRS TX Pin
  #define trans_to_gprs 17
  /// GPRS PWR Pin
  #define gprsPWR 4
  /// GPRS RST Pin
  #define gprsRST 34
  /// GPS RX Pin
  #define recv_from_gps 23
  /// GPS TX Pin
  #define trans_to_gps -1
  /// GPRS Baudrate for AT Commands
  #define gprsBaud 115200
  /// GPS TX pin Baudrate
  #define gpsBaud 9600
  
#else
  //A9G
  #define usbBaud  9600
  /// GPRS RX Pin
  //#define recv_from_gprs 14 //Module AiThinker
  #define recv_from_gprs 27 //PCB2,3,4 
  /// GPRS TX Pin
  //#define trans_to_gprs 12//Module AiThinker
  #define trans_to_gprs 14 //PCB2,3,4
  /// GPS RX Pin
  //#define recv_from_gps 13//Module AiThinker
  #define recv_from_gps 13 //PCB2,3,4
  /// GPS TX Pin
  //#define trans_to_gps -1
  #define trans_to_gps 12 //PCB2,3,4
  /// GPS PWR Pin
  //#define gprsPWR  33//Module AiThinker
  #define gprsUPWR  25 //PCB2,3,4
  /// GPS RST Pin
  //#define gprsRST  34//Module AiThinker
  #define gprsRST  33 //PCB2,3,4
  /// GPS PWR Pin without Button
  #define gprsPWR  26 //PCB2,3,4
  /// GPS LOW PWR Pin 
  #define gprsLPWR  32 //PCB2,3,4
  /// GPRS Baudrate for AT Commands
  #define gprsBaud 115200
  /// GPS TX pin Baudrate
  #define gpsBaud 9600
#endif 

/// GPS Serial
SoftwareSerial gpsSerial(recv_from_gps, trans_to_gps);

/// Red leg
#define LED1 4 //RED
/// Green leg
#define LED2 16 //Green
/// Blue leg
#define LED3 17 //Blue
