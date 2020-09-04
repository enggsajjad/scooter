void setup() {
  delay(1000);
  //Serial.begin(115200,SERIAL_8N1,3,1);//rx,tx
  Serial.begin(115200,SERIAL_8N1,15,2);//rx,tx
  Serial.println("ESP32:Hussain");
}
void loop() {
  if (Serial.available()> 0) {
    int inByte = Serial.read();
    //Serial.print('2');
    Serial.print(char(inByte));
  }
}
