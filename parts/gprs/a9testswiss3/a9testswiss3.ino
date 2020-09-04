#include <String.h>

#define OK 1
#define NOTOK 2
#define TIMEOUT 3
#define RST 2

#define A9RX 16
#define A9TX 17
#define GPRS_Power_pin 4
#define GPRS_Reset_pin 34

#define A6board Serial1

//#ifndef A6board
//SoftwareSerial A6board (10, 3);
//#define A6baud 9600
//#else
#define A6baud 115200
//#endif
#define SERIALTIMEOUT 3000

char end_c[2];
float temp=32;
/// humidity values
float hum=12;
/// pressure values
float atm=0;

float pm10=3;
/// sensor results for dust sensor pm25
float pm25=6;
String message= "";
void setup() {
  delay(100);
  //A6board.begin(A6baud);   // the GPRS baud rate
  A6board.begin(A6baud, SERIAL_8N1, A9RX, A9TX);
  Serial.begin(115200);    // the GPRS baud rate
  // ctrlZ String definition
  end_c[0] = 0x1a;
  end_c[1] = '\0';
  Serial.println("Start");

  pinMode(GPRS_Power_pin, OUTPUT);
  digitalWrite(GPRS_Power_pin, HIGH);
  delay(5000);
  digitalWrite(GPRS_Power_pin, LOW);
  delay(500);

  A6command("AT+RST=1", "+CREG: 1", "+CREG: 0", 20000, 3);
  Serial.println("Reseted");
  if (A6begin() != OK) {
    Serial.println("Error");
    while (1 == 1);
  }
        Serial.println("----------------------Post value to Sparkfun-------------------------------");
        float batt = 23.23;//(float)analogRead(A0) * 5.165 / 594.0; // conversion factor measured with multimeter
        sendSparkfunGSM(1, batt);
}

void loop()
{

  Serial.println("----------------------Post value to Sparkfun-------------------------------");
  float batt = 23.23;//(float)analogRead(A0) * 5.165 / 594.0; // conversion factor measured with multimeter
  sendSparkfunGSM0(1, batt);
        
  if ( A6board.available())
    Serial.write( A6board.read());

  delay(2000);
}


///sparkfun()///
bool sendSparkfunGSM(byte sparkfunType, float value1) {
  String host = "0.tcp.ngrok.io";
  String port = "18172";
  A6command("AT+CREG?", "+CREG: 1,1", "yy", 20000, 1);
  A6command("AT+CGATT?", "+CGACT: 0,0", "OK", 20000, 1);
  A6command("AT+CGATT=1", "OK", "yy", 20000, 1);
  
  A6command("AT+CSTT=\"internet\",\"\",\"\"", "OK", "yy", 20000, 1); 

   A6command("AT+CIICR", "OK", "yy", 20000, 1); //bring up wireless connection
   A6command("AT+CIFSR", "OK", "yy", 20000, 1); //bring up wireless connection

  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",18172", "CONNECT OK", "yy", 25000, 1); //start up the connection

  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      
  A6command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 20000, 1); //begin send data to remote server
  delay(3000);

  A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1); //sending
  //A6command("AT+CIPSHUT", "OK", "yy", 15000, 1);
  delay(1000);
  Serial.println("-------------------------End------------------------------");
}

///sparkfun()///
bool sendSparkfunGSM0(byte sparkfunType, float value1) {
  String host = "0.tcp.ngrok.io";
  String port = "18172";

  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",18172", "CONNECT OK", "yy", 25000, 1); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";    
  A6command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 20000, 1); //begin send data to remote server
  delay(3000);
  
  unsigned long   entry = millis();

  //A6command("AT+CIPSHUT", "OK", "yy", 15000, 1); //sending
  A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1);
  
  delay(1000);
  Serial.println("-------------------------End------------------------------");
}


byte A6waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;

  while(A6board.available() > 0) {
  A6board.read(); //clear data
  }
  
  String reply = A6read();
  byte retVal = 99;
  do {
    reply = A6read();
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeOut );
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }

  //  Serial.print("retVal = ");
  //  Serial.println(retVal);
  return retVal;
}

byte A6command(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) {
    A6board.println(command);
    Serial.print("Command: ");
    Serial.println(command);
    if (A6waitFor(response1, response2, timeOut) == OK) {
      //     Serial.println("OK");
      returnValue = OK;
    } else returnValue = NOTOK;
    count++;
  }
  return returnValue;
}






bool A6begin() {
  A6board.println("AT+CREG?");
  byte hi = A6waitFor("1,", "5,", 1500);  // 1: registered, home network ; 5: registered, roaming
  while ( hi != OK) {
    A6board.println("AT+CREG?");
    hi = A6waitFor("1,", "5,", 1500);
  }

  //if (A6command("AT&F0", "OK", "yy", 5000, 2) == OK) {   // Reset to factory settings
    if (A6command("ATE0", "OK", "yy", 5000, 2) == OK) {  // disable Echo
      if (A6command("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) return OK;  // enable better error messages
      else return NOTOK;
    }
  //}
}



String A6read() {
  String reply = "";
  if (A6board.available())  {
    reply = A6board.readString();
  }
    //Serial.print("Reply: ");
    //Serial.println(reply);
  return reply;
}
