//A7
#define gpsbaud  115200
#define gprsbaud  9600
#define A9RX 16
#define A9TX 17
#define GPSRX 23
#define GPSTX -1
int PWR_KEY_PIN = 4;
int RESET_PIN = 34;

/*
//A9G
#define baud  9600
#define A9RX 14
#define A9TX 12
#define GPSRX 13
#define GPSTX -1
int PWR_KEY_PIN = 33;
int RESET_PIN = 34;
*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

HardwareSerial gps(1);
HardwareSerial a7g(2);
// The TinyGPS++ object
TinyGPSPlus tiny;

//SoftwareSerial gps(GPSRX, -1); // RX, TX

unsigned char rxState;
void setup() {
  delay(1000);
  pinMode(PWR_KEY_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  //digitalWrite(RESET_PIN, HIGH);
  digitalWrite(PWR_KEY_PIN, HIGH);
  //delay(3000);
  //digitalWrite(PWR_KEY_PIN, LOW);

  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(gprsbaud);
  delay(100); 
  //gps.begin(9600);
  gps.begin(gprsbaud, SERIAL_8N1, GPSRX, -1);
  delay(500); 
  a7g.begin(gpsbaud, SERIAL_8N1, A9RX, A9TX);
  delay(500); 
  
  Serial.println("Debugging GPS and GPRS Serial");
  
}
 
void loop() 
{
  /*
   // working with both gps and gprs, getting also from GPS
  if (a7g.available()) {
    Serial.print(char(a7g.read()));
  }
  if (Serial.available())
  {
    a7g.print(char(Serial.read()));
  }
    while ( gps.available()) {
    //Serial.print(char(gps.read()));
    tiny.encode(gps.read());
    //if (tiny.location.isUpdated()){
      Serial.print("Latitude= "); 
      Serial.print(tiny.location.lat(), 6);
      Serial.print(" Longitude= "); 
      Serial.println(tiny.location.lng(), 6);
    //}
  }
  */

/* 
  // working with both gps and gprs, getting also from GPS but without decoding
  if (a7g.available()) {
    Serial.print(char(a7g.read()));
  }
  if (Serial.available())
  {
    a7g.print(char(Serial.read()));
  }

   if ( gps.available()) {
    Serial.print(char(gps.read()));
  }
*/

  // working with both gprs only
  if (a7g.available()) {
    Serial.print(char(a7g.read()));
  }
  if (Serial.available())
  {
    a7g.print(char(Serial.read()));
  }

}
