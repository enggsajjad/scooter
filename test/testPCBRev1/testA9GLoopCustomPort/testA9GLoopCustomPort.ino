#define gsmSerial     Serial1

#define GPS_RX 13
#define GPS_TX 12
#define GPRS_TX 27
#define GPRS_RX 14

#define LED1 4 //RED
#define LED2 16 //Green
#define LED3 17 //Blue
unsigned long previousMillis = 0; 
int i=0;
void setup()
{
  delay(100);
  
  //Serial.begin(115200,SERIAL_8N1,3,1);
  Serial.begin(115200,SERIAL_8N1,2,15);//PCB2 USB-UART at SDS Header

  gsmSerial.begin(115200,SERIAL_8N1, 27,14); //PCB2
  
  Serial.println("Hello Loop Testing....");
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  delay(1000);
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 0);
  delay(1000);
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

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    i++;
    if (i==8) i =0;
    digitalWrite(LED1, i&1);
    digitalWrite(LED2, (i>>1)&1);
    digitalWrite(LED3, (i>>2)&1);
  }

}
