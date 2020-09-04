void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial2.begin(115200,SERIAL_8N1,12,13);
  Serial.println("Heltek:HW0:Hussain");
}
void loop() {
  delay(10000);
  Serial2.println("Heltek:HW2:Sajjad Hussain");
}
