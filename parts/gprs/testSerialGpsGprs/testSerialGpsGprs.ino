//A7
#define gpsbaud  9600
#define gprsbaud  115200
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

void waitForGSMBecomeReady()
{
  for( int i=0; i<15; i++)
  {
    char buffer[3];
    char index = 0;

    a7g.flush();
    delay(1000);
    Serial.print('.');
    a7g.print("AT\r");
    a7g.flush();
    delay(100);

    while( a7g.available())
    {
      char c = a7g.read();

      if ( c == '\r' || c == '\n' )
      {
        if ( buffer[0] == 'O' && buffer[1] == 'K' && index == 2 )
        {
          Serial.print('\n');
          return;
        }

        index = 0;
      }
      else
      if( index < 3 )
      {
        buffer[index++] = c;
      }
    }
  }
  
}

unsigned char rxState;
void rest(void)
{
  Serial.println("Reseting....");
  digitalWrite(RESET_PIN, LOW);
  delay(2000);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);
}
void setup() {
  delay(1000);
  pinMode(PWR_KEY_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(PWR_KEY_PIN, HIGH);
  rest();
  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(gpsbaud);
  delay(100); 
  //gps.begin(9600);
  gps.begin(gpsbaud, SERIAL_8N1, GPSRX, -1);
  delay(500); 
  a7g.begin(gprsbaud, SERIAL_8N1, A9RX, A9TX);
  delay(500); 
  waitForGSMBecomeReady();
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
  //rest();
  }

}
