#define gsmSerial     Serial1

#define GPS_RX 13
#define GPS_TX 12
#define GPRS_TX 27
#define GPRS_RX 14

void setup()
{
  delay(100);
  
  Serial.begin(115200,SERIAL_8N1,3,1);

  gsmSerial.begin(115200,SERIAL_8N1, 27,14);//PCB2
  //gsmSerial.begin(115200,SERIAL_8N1,17,16);//A7 Module
  //gsmSerial.begin(115200,SERIAL_8N1,14,12);//A9G Module
  //gsmSerial.begin(115200,SERIAL_8N1, 34,25);//PCB1
  
  Serial.println("Hello Loop Testing....");
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


}
