void setup() {
  delay(1000);
  //Serial.begin(115200,SERIAL_8N1,3,1);//rx,tx
  Serial.begin(115200,SERIAL_8N1,15,2);//rx,tx
  Serial.println("ESP32:Hussain"); 
}
void loop() {
  delay(10000);
  Serial.println("ESP32:HW0:Sajjad Hussain");
}
