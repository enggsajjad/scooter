//05.02.2022 adding mosfet and testing reset ciruit and code...

//#include <SoftwareSerial.h>

#define gsmSerial     Serial1
#define gpsSerial     Serial2
#define GPS_RX 13
#define GPS_TX 12
#define GPRS_TX 27
#define GPRS_RX 14

#define LED1 4 //RED
#define LED2 16 //Green
#define LED3 17 //Blue
unsigned long previousMillis = 0; 
unsigned long previous = 0;
int i=0;
#define gprsPWR  26
#define gprsUEN  33
//SoftwareSerial gpsSerial(12,13);

void setup()
{
  delay(100);
  Serial.begin(115200,SERIAL_8N1,3,1);//9600 will not work as GPRS TX/RS works at 115200
  //Serial.begin(115200,SERIAL_8N1,2,15);//PCB2 USB-SERIAL Port at SDS Header begin(baud,recv, trans)
  //Serial.begin(115200,SERIAL_8N1,5,21);//PCB2 USB-SERIAL Port at GPIO Header

  gsmSerial.begin(115200,SERIAL_8N1, 27,14);//PCB2 A9G GPRS 
  gpsSerial.begin(9600,SERIAL_8N1, 13,12);//PCB2 A9G GPS DIRECT
  //gpsSerial.begin(9600);//pcb2 gps
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(gprsUEN, OUTPUT);
  pinMode(gprsPWR, OUTPUT);
  //digitalWrite(gprsPWR, HIGH);
  digitalWrite(gprsPWR, HIGH);
  digitalWrite(gprsUEN, HIGH);
  delay(500);
  digitalWrite(gprsUEN, LOW);
  
  
  digitalWrite(gprsPWR, LOW);
  delay(1000);
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 0);
  //just after reseting, pull down the pwr key
  Serial.println("Hello Loop Testing....");
  
  //delay(100);
  //digitalWrite(gprsPWR, LOW);
  delay(3000);
  digitalWrite(gprsPWR, HIGH);
  Serial.println("gprsPWR....");

}

void loop()
{

  if (Serial.available())
  {
    gsmSerial.write(Serial.read());
  }
  if (gsmSerial.available())
  {
    Serial.write(gsmSerial.read());
  }
  //Uncomment the following if you want to get the A9G GPS direct data
  /*if (gpsSerial.available())
  {
    Serial.write(gpsSerial.read());
  }*/

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    i++;
    if (i==8) i =0;
    digitalWrite(LED1, i&1);
    digitalWrite(LED2, (i>>1)&1);
    digitalWrite(LED3, (i>>2)&1);
  }

  //check the reset again after 2min
  unsigned long current = millis();
  if (current - previous >= (2*60*1000) ) {
    previous = current;
    //////////////////
    digitalWrite(gprsUEN, HIGH);
    delay(500);
    digitalWrite(gprsUEN, LOW);
    digitalWrite(gprsPWR, LOW);
    delay(4000);
    digitalWrite(gprsPWR, HIGH);
    Serial.println("A9G Reset....");
  }


}
