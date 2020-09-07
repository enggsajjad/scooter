/**
 * \file sensor.h
 *
 * \brief variable and definitions for sensors
 *
 * This is the files that contains the variables, pins definitions and functions for sensors to be connected.
 * 
 * \date 07.09.2020
 */


/// DHT22 sensor communication pin
#define DHT_PIN 21
/// BME280 I2C Address 0x77 for Red, 0x76 for Purple (chinese)
#define BME_ADDR  0x77
/// BME280 I2C SDA pin
#define BME_SDA 21
/// BME280 I2C SCL pin
#define BME_SCL 22

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
#define SDS011_TXD  27
/// SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
#define SDS011_RXD  26

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

//A7G
/// GPRS RX Pin
#define gprsRX 16
/// GPRS TX Pin
#define gprsTX 17
/// GPRS PWR Pin
#define gprsPWR 4
/// GPRS RST Pin
#define gprsRST 34
/// GPS RX Pin
#define gpsRX 23
/// GPS TX Pin
#define gpsTX -1
/// GPRS Baudrate for AT Commands
#define gprsBaud 115200
/// GPS TX pin Baudrate
#define gpsBaud 9600
/// Serial port for GPRS (AT) Commands
#define gprsSerial Serial1
/*
//A9G

#define baud  9600
/// GPRS RX Pin
#define gprsRX 14
/// GPRS TX Pin
#define gprsTX 12
/// GPS RX Pin
#define GPSRX 13
/// GPS TX Pin
#define GPSTX -1
/// GPS PWR Pin
#define gprsPWR  33;
/// GPS RST Pin
#define gprsRST  34;
*/
