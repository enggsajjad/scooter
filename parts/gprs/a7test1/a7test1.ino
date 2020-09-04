//a7test1 direct commands entering
#define RXD2 16
#define TXD2 17
#include <SoftwareSerial.h>
int PWR_KEY_PIN = 4;
#define RXD1 23
#define TXD1 -1
SoftwareSerial gps(23, -1); // RX, TX
void setup() {
  delay(1000);
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  delay(500); 
  gps.begin(115200);//, SERIAL_8N1, RXD2, TXD2);
  delay(500); 
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(500); 

  Serial.println("Serial Txd is on pin: "+String(TX));
  
  pinMode(PWR_KEY_PIN, OUTPUT);
  digitalWrite(PWR_KEY_PIN, HIGH);
  delay(3000);
  digitalWrite(PWR_KEY_PIN, LOW);

  

  Serial.println("Serial Rxd is on pin: "+String(RX));


}
 
void loop() { //Choose Serial1 or Serial2 as required
  if (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
  if (Serial.available())
  {
    Serial2.print(char(Serial.read()));
  }

    /*if ( gps.available() > 0) {
    Serial.println("GPS:" + gps.readString());
  }*/
  if ( gps.available()) {
    //Serial.print(char(Serial2.read()));
    Serial.println("GPS:" + gps.readString());
  }
}
