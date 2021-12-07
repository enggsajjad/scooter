
#define LED1 27 //RED
#define LED2 14 //Green
#define LED3 12 //Blue
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  for (int i=0;i<8;i++)
  {
    digitalWrite(LED1, i&1);
    digitalWrite(LED2, (i>>1)&1);
    digitalWrite(LED3, (i>>2)&1);
    delay(2000);                       // wait for a second  
  }

}
