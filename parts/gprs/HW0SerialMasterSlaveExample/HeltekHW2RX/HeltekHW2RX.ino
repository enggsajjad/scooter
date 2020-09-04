void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,12,13);
  Serial.println("Heltek:HW0:Hussain");
}
void loop() {
  if (Serial2.available()> 0) {
    int inByte = Serial2.read();
    Serial.print(char(inByte));
  }
}
