#define gsmSerial     Serial1
void setup()
{
  delay(100);
  
  Serial.begin(115200,SERIAL_8N1,3,1);

  gsmSerial.begin(115200,SERIAL_8N1, 34,25);
  
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
