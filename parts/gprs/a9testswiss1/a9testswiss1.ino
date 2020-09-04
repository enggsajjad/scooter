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

  A6command("AT+RST=1", "+CREG:", "yy", 20000, 1);
  Serial.println("Reset");
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
  String host = "2.tcp.ngrok.io";
  String port = "13141";
  //String publicKey   = "QGyWvOR04Mcqw7oZ2Qrp";
  //String privateKey = "Jqym6Ma281SynPV6qer5";
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CGATT?", "OK", "yy", 20000, 2);
  A6command("AT+CGATT=1", "OK", "yy", 20000, 2);
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  //A6command("AT+CGDCONT=1,\"IP\",\"gprs.swisscom.ch\"", "OK", "yy", 20000, 2); //bring up wireless connection
  
  A6command("AT+CSTT=\"internet\",\"\",\"\"", "OK", "yy", 20000, 2); 
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  //A6command("AT+CGACT=1,1", "OK", "yy", 10000, 2);
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
   A6command("AT+CIICR", "OK", "yy", 20000, 2); //bring up wireless connection
   //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  //A6command("AT+CIFSR", "OK", "yy", 20000, 2); //get local IP adress
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",13141", "CONNECT OK", "yy", 25000, 2); //start up the connection
  // A6input();
  //A6command("AT+CIPSTATUS", "OK", "yy", 10000, 2);
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      
  A6command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1); //begin send data to remote server
  delay(3000);
  //A6board.print("Sajjad Hussain");
  //A6board.println(end_c); //sending ctrlZ
  unsigned long   entry = millis();
  //A6command("AT+CIPCLOSE", "OK", "yy", 15000, 1); //sending
  A6command("AT+CIPSHUT", "OK", "yy", 15000, 1);
  delay(1000);
  Serial.println("-------------------------End------------------------------");
}

///sparkfun()///
bool sendSparkfunGSM0(byte sparkfunType, float value1) {
  String host = "2.tcp.ngrok.io";
  String port = "13141";

  A6command("AT+CIPSTART=\"TCP\",\"" + host + "\",13141", "CONNECT OK", "yy", 25000, 2); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";    
  A6command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1); //begin send data to remote server
  delay(3000);
  
  unsigned long   entry = millis();

  A6command("AT+CIPSHUT", "OK", "yy", 15000, 1); //sending

  delay(1000);
  Serial.println("-------------------------End------------------------------");
}


byte A6waitFor(String response1, String response2, int timeOut) {
  unsigned long entry = millis();
  int count = 0;
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




void A6input() {
  String hh;
  char buffer[100];
  while (1 == 1) {
    if (Serial.available()) {
      hh = Serial.readStringUntil('\n');
      hh.toCharArray(buffer, hh.length() + 1);
      if (hh.indexOf("ende") == 0) {
        A6board.write(end_c);
        Serial.println("ende");
      } else {
        A6board.write(buffer);
        A6board.write('\n');
      }
    }
    if (A6board.available()) {
      Serial.write(A6board.read());
    }
  }
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

void ShowSerialData()
{
  unsigned long entry = millis();
  while ( A6board.available() != 0 && millis() - entry < SERIALTIMEOUT)
    Serial.println(A6board.readStringUntil('\n'));
}

String A6read() {
  String reply = "";
  if (A6board.available())  {
    reply = A6board.readString();
  }
  //  Serial.print("Reply: ");
  //  Serial.println(reply);
  return reply;
}
