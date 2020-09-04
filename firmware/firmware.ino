/*
  (c) TECO - Tobias Röddiger
*/

#include <SoftwareSerial.h>

SoftwareSerial particleSensor(0, 2); // RX, TX
SoftwareSerial gps(4, -1); // RX, TX
SoftwareSerial a7Interface(13, 15); // RX, TX

float pm25; //2.5um particles detected in ug/m3
float pm10; //10um particles detected in ug/m3

int PWR_KEY_PIN = 12;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("ScooterAQNet ");
  delay(500);


  particleSensor.begin(9600); //SDS021 reports at 1Hz at 9600bps
  delay(500);
  a7Interface.begin(115200);
  delay(500);
  gps.begin(9600); // A7 GPS reports at 9600bps
  delay(500);

  pinMode(PWR_KEY_PIN, OUTPUT);
  digitalWrite(PWR_KEY_PIN, HIGH);
  delay(3000);
  digitalWrite(PWR_KEY_PIN, LOW);
}

int incomingByte = 0;
int i = 1;

boolean simUnlocked = false;
boolean networkConnected = false;
boolean networkAttached = false;
boolean apnConfigured = false;
boolean gprsConnected = false;
boolean networkReady = false;
boolean TCPSocketOpen = false;
boolean gpsConfigured = false;

void loop()
{
  if (!gpsConfigured) {
    if(command("AT+GPS=1","OK", 4000, true)) {
      gpsConfigured = true;
      Serial.println("GPS CONFIGURED");
    }
  }
  
  if(!simUnlocked && command("AT+CPIN?", "+CPIN:SIM PIN", 4000, true)) {
    if(command("AT+CPIN=\"2580\"", "OK", 4000, true)) {
      if(command("AT+CPIN?", "+CPIN:READY", 4000, true)) {
        simUnlocked = true;
        Serial.println("SIM UNLOCKED");
      } 
    }  
  }

  if (simUnlocked && !networkConnected) {
    if(command("AT+CREG?", "CREG: 1,13", 4000, false)) {
      delay(3000);
      if (command("AT+COPS=0", "OK", 4000, false)) {
          // done setting auto connection
      }
    }

    if (command("AT+CREG?", "CREG: 1,1", 4000, false)) {
        // connected to network
        networkConnected = true;
        Serial.println("NETWORK REGISTERED");
    }
  }

  if (simUnlocked && networkConnected && !networkAttached) {
    /*if (command("AT+USOCR=6", "+USOCR:", 4000, true)) {
      
    }*/
    if (command("AT+CGACT?", "+CGACT: 0,0", 4000, false)) {
      delay(2000);
      if (command("AT+CGATT=1", "OK", 4000, false)) {
        if (command("AT+CGATT?", "+CGATT:1", 4000, false)) {
          networkAttached = true;
          delay(7000);
          Serial.println("NETWORK ATTACHED");
        }
      }
    }
  }

  if (simUnlocked && networkConnected && networkAttached & !apnConfigured) {
    if(command("AT+CSTT=\"internet\",\"\",\"\"", "OK", 4000, false)) { // APN OF THE PROVIDER
      Serial.println("APN CONFIGURED");
      apnConfigured = true;
    }
  }

  if (simUnlocked && networkConnected && networkAttached & apnConfigured & !gprsConnected) {
    if(command("AT+CIICR", "OK", 4000, false)) { // APN OF THE PROVIDER
      delay(6000);
      gprsConnected = true;
      Serial.println("GPRS CONNECTED");
    }
  } //  23.235.209.134

  if (simUnlocked && networkConnected && networkAttached & apnConfigured & gprsConnected & !TCPSocketOpen) {
    if(command("AT+CIPSTART=\"TCP\",\"0.tcp.ngrok.io\",17850", "CONNECT OK", 4000, false)) {
      TCPSocketOpen = true;
      Serial.println("TCP SOCKET OPENED");
      networkReady = true;
    }
  }

  if (gpsConfigured && gps.available() > 0) {
    Serial.println("GPS:" + gps.readString());
  }

  if (dataAvailable())
  {
    if (networkReady) {
      Serial.print("Particle Matter [2.5]:");
      Serial.print(pm25, 1);
      Serial.print("ug/m3 [10]:");
      Serial.print(pm10, 1);
      Serial.print("ug/m3");
      Serial.println();


      String message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + " }";
      String commandString = "AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"";
      
      if(command(commandString, "OK", 4000, true)) {
          delay(1000);
      } else {
        TCPSocketOpen = false;
        networkReady = false;
      }
    }
  }
  else
  {
    //Serial.println("Timeout or CRC error");
    //Serial.println("Double check blue wire goes to pin 0");
  }

  

  delay(50);
}

boolean command(String command, String response, unsigned long timeout, boolean output) {
  //Serial.print("Sending: ");
  //Serial.println(command);

  while(a7Interface.available() > 0) {
    a7Interface.read(); //clear data
  }

  a7Interface.print(command + "\r\n");

  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) {
    String result = a7Interface.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) {
      Serial.println("Received Data: ");
      Serial.println(result);
      Serial.println();
    }
    

    if (result.indexOf(response) > 0) {
      Serial.println("Command Executed: " + command + " - " + response);
      return true;
    }
  }

  Serial.println("Command Failed: " + command);

  return false;
}

void sendATCommand(char* command) {
  Serial.println();
  Serial.println();
  Serial.println("-------------- REQUEST ---------------");
  Serial.println(command);
  a7Interface.print(command);
}

boolean readATCommandResponse(char* resp) {
  int i = 0;
  int elements = sizeof(resp) / sizeof(char);

  Serial.print(elements);
  Serial.println("-------------- RESPONSE ---------------");
  delay(500);
  
  while (a7Interface.available() > 0) {
    uint chr = a7Interface.read();

    if (i < elements && ((char)chr) == resp[i]) {
      i++;
    }
    else if (i != elements) {
      i = 0;
    }
    
    Serial.print((char)chr);
  }
  Serial.print("Matches: ");
  Serial.println(i);
  Serial.println();
  return (i == elements);
}

//Scans for incoming packet
//Times out after 1500 miliseconds
boolean dataAvailable(void)
{
  //Spin until we hear meassage header byte
  long startTime = millis();

  while (1)
  {
    while (!particleSensor.available())
    {
      delay(1);
      if (millis() - startTime > 1500) return (false); //Timeout error
    }

    if (particleSensor.read() == 0xAA) break; //We have the message header
  }

  //Read the next 9 bytes
  byte sensorValue[10];
  for (byte spot = 1 ; spot < 10 ; spot++)
  {
    startTime = millis();
    while (!particleSensor.available())
    {
      delay(1);
      if (millis() - startTime > 1500) return (false); //Timeout error
    }

    sensorValue[spot] = particleSensor.read();
  }

  //Check CRC
  byte crc = 0;
  for (byte x = 2 ; x < 8 ; x++) //DATA1+DATA2+...+DATA6
    crc += sensorValue[x];
  if (crc != sensorValue[8])
    return (false); //CRC error

  //Update the global variables
  pm25 = ((float)sensorValue[3] * 256 + sensorValue[2]) / 10;
  pm10 = ((float)sensorValue[5] * 256 + sensorValue[4]) / 10;

  return (true); //We've got a good reading!
}
