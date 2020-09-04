////a9test1 direct commands entering both for a9 and a7

//A7
#define baudrate  115200
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

#include <SoftwareSerial.h>
#include <HardwareSerial.h>

HardwareSerial gps(1);
HardwareSerial a7g(2);


//SoftwareSerial gps(GPSRX, -1); // RX, TX
#define at_ready 1

unsigned char rxState;
void setup() {
  delay(1000);
  pinMode(PWR_KEY_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  //digitalWrite(RESET_PIN, HIGH);
  digitalWrite(PWR_KEY_PIN, HIGH);

  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(baudrate);
  delay(100); 
  //gps.begin(9600);
  gps.begin(baudrate, SERIAL_8N1, GPSRX, -1);
  delay(500); 
  a7g.begin(baudrate, SERIAL_8N1, A9RX, A9TX);
  delay(500); 
  
  Serial.println("Serial Txd is on pin: "+String(TX));
  

  digitalWrite(PWR_KEY_PIN, LOW);
  delay(3000);
  digitalWrite(PWR_KEY_PIN, HIGH);
  

    Serial.println("Serial Rxd is on pin: "+String(RX));
 rxState = at_ready;

}
 
void loop() 
{

  if (a7g.available()) {
    Serial.print(char(a7g.read()));
  }
  if (Serial.available())
  {
    a7g.print(char(Serial.read()));
  }
/*
    if ( gps.available() > 0) {
    Serial.print(char(gps.read()));
  }*/

}
