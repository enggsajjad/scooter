#include <String.h>

#define OK 1
#define NOTOK 2
#define TIMEOUT 3


#define A9RX 16
#define A9TX 17
#define gprsSerial_Power_pin 4
#define gprsSerial_Reset_pin 34

#define gprsBaud 115200
#define gprsSerialTimeout 3000
#define gprsSerial Serial1

float temp=32;
/// humidity values
float hum=12;
/// pressure values
float atm=0;

float pm10=3;
/// sensor results for dust sensor pm25
float pm25=6;
String message= "";
String host = "0.tcp.ngrok.io";
String port = "15339";
byte r;
int rxState = 0;
int nState = 0;

String resp;
byte error;
bool rcvd;
volatile int interruptCounter;
bool timeoutIntrupt;
unsigned long nSec=10;
 
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  timeoutIntrupt=true;
  
  portEXIT_CRITICAL_ISR(&timerMux);

  Serial.println("Serial Timeout: " );
  error=TIMEOUT;
  rxState = nState-1;//current
  //timerAlarmDisable(timer);
}

void setup() 
{
  delay(100);
  gprsSerial.begin(gprsBaud, SERIAL_8N1, A9RX, A9TX);
  gprsSerial.setTimeout(10000);
  Serial.begin(115200);    // the gprsSerial baud rate

  Serial.println("Start");

  pinMode(gprsSerial_Power_pin, OUTPUT);
  digitalWrite(gprsSerial_Power_pin, HIGH);
  delay(2000);
  digitalWrite(gprsSerial_Power_pin, LOW);
  //delay(2000);
  //digitalWrite(gprsSerial_Power_pin, HIGH);
  delay(5000);

  //gprsSerial.println("AT");resp="OK";
  //rxState = 100;
  //nState = 1;
  /*gprsCommand("AT+RST=1", "+CREG: 1", "+CREG: 0", 20000, 3);
  
  if (gprsBegin() != OK) {
    Serial.println("Error");
    while (1 == 1);
  }
  Serial.println("Reseted!");
  
  Serial.println("------Setup TCP------");
  r = gprsSetupTCP();
  */
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, nSec*1000000, true);
  timerAlarmEnable(timer);
}
String rd;

void loop()
{
  switch(rxState)
  {
    case 100:
      while ( gprsSerial.available())
      {
        rd = gprsSerial.readString();
        Serial.println("-------------------");
        Serial.println("Serial " +String(rd.length()) +": " + rd );
        Serial.println("===================");
      }
      if (rd!="")
      {
        /*if(nState==3)
        {
          Serial.println("Serial Resp: "+ resp + " Ind " + String(rd.indexOf(resp)));
          Serial.println("Serial Resp: CREG: 0 Ind: " + String(rd.indexOf("\0")));
          Serial.println("Serial Resp: service Ind: " + String(rd.indexOf("RST")));
          char buf[255];
          rd.toCharArray(buf, 255);
          Serial.print("Serial2: ");
          Serial.println(buf );
          Serial.println("Serial2 " +String(rd.length()) +": " + String(buf) );
        }*/
          if(rd.indexOf("ERROR")>0){
            Serial.println("Serial Error: " );
            error=NOTOK;
            rxState = nState-1;//current
          }
          else if(rd.indexOf(resp)>0){
            Serial.println("Serial Resp: "+ resp );
            error=OK;
            rxState = nState;
          }
          //else{
          //  Serial.println("Serial Error1: " );
          //  error=NOTOK;
          //  rxState = nState-1;//current
          //}

        rd = "";
        //delay(10000);
        //rxState = nState;
      }

      break;
    case 0:
      Serial.println("Command AT1");resp="OK"; rcvd = false;
      gprsSerial.println("AT");
      rxState = 100;
      nState = 1;
      break;
    case 1:
      Serial.println("Command AT2");
      resp="OK"; rcvd = false;
      gprsSerial.println("AT");
      nState=2;
      rxState = 100;
      break;
    case 2:
      Serial.println("Command AT+RST=1");
      timerAlarmWrite(timer, 90*1000000, true);resp="RST"; rcvd = false;
      gprsSerial.println("AT+RST=1");
      nState=3;
      rxState = 100;
      break;
    case 3:
      delay(1000); //skip extra response of AT+RST=1, missed due to \0
      timerAlarmWrite(timer, 20*1000000, true);
      Serial.println("Command AT+GPS=1");resp="OK"; rcvd = false;
      gprsSerial.println("AT+GPS=1");
      nState=4;
      rxState = 100;
      break;
    case 4:
      Serial.println("Command AT+CREG?");resp="+CREG: 1,1"; rcvd = false;
      gprsSerial.println("AT+CREG?");
      nState=5;
      rxState = 100;
      break;
    case 5:
      Serial.println("Command AT+CGACT?");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CGACT?");
      nState=6;
      rxState = 100;
      break;
    case 6:
      Serial.println("Command AT+CGATT");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CGATT=1");
      nState=7;
      rxState = 100;
      break;
    case 7:
      Serial.println("Command AT+CSTT");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CSTT=\"internet\",\"\",\"\"");
      nState=8;
      rxState = 100;
      break;
    case 8:
      Serial.println("Command AT+CIICR");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CIICR");
      nState=9;
      rxState = 100;
      break;
    case 9:
      Serial.println("Command AT+CIFSR");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CIFSR");
      nState=10;
      rxState = 100;
      break;
    case 10:
      Serial.println("Command AT+CIPSTART");resp="CONNECT OK"; rcvd = false;
      gprsSerial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port+"");
      nState=11;
      rxState = 100;
      break;
    case 11:
      Serial.println("Command AT+CIPSEND");resp="OK"; rcvd = false;
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      gprsSerial.println("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"");
      nState=12;
      rxState = 100;
      break;
    case 12:
      Serial.println("Command AT+CIPCLOSE");resp="OK"; rcvd = false;
      gprsSerial.println("AT+CIPCLOSE");
      nState=10;
      rxState = 100;
      break;
    case 13:
      
      Serial.println("Command AT+RST=1");
      gprsSerial.println("AT+CGATT?"); rcvd = false;
      nState=6;
      rxState = 100;
      break;
  }
  /*
  {
  Serial.println("------Post value to TCP------");
  r = gprsSendTCPData();
    
  }*/
/*
  //if ( gprsSerial.available())
    //Serial.write( gprsSerial.read());
  if (Serial.available()){
    byte bye=Serial.read();
    gprsSerial.print(char(bye));
    Serial.print(char(bye));
  }
*/
}

byte gprsSetupTCP(void) 
{
  byte ret;
  ret = gprsCommand("AT+GPS=1", "OK", "yy", 20000, 1);
  ret = gprsCommand("AT+CREG?", "+CREG: 1,1", "yy", 20000, 1);
  ret = gprsCommand("AT+CGATT?", "+CGACT: 0,0", "OK", 20000, 1);
  ret = gprsCommand("AT+CGATT=1", "OK", "yy", 20000, 1);
  ret = gprsCommand("AT+CSTT=\"internet\",\"\",\"\"", "OK", "yy", 20000, 1); 
  ret = gprsCommand("AT+CIICR", "OK", "yy", 20000, 1); //bring up wireless connection
  ret = gprsCommand("AT+CIFSR", "OK", "yy", 20000, 1); //bring up wireless connection
  ret = gprsCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 25000, 1); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
  ret = gprsCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "}", 20000, 1); //begin send data to remote server
  delay(3000);
  ret = gprsCommand("AT+CIPCLOSE", "OK", "yy", 15000, 1); //"AT+CIPSHUT"
  delay(1000);
  return(ret);
}

byte gprsSendTCPData(void) 
{
  byte ret;
  //ret = gprsCommand("AT+GPS=1", "OK", "yy", 20000, 1);
  ret = gprsCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 25000, 1); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";    
  /*if (ret==1)*/ {ret = gprsCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "}", 20000, 1);} //begin send data to remote server
  delay(3000);
  /*if (ret==1)*/ {ret = gprsCommand("AT+CIPCLOSE", "OK", "yy", 15000, 1);} // //"AT+CIPSHUT"
  delay(1000);
  return(ret);
}


byte gprsCommandnew(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
   while(gprsSerial.available() > 0) {
    gprsSerial.read(); //clear data
   }
  //while (count < repetitions && returnValue != OK) {
    gprsSerial.println(command);
    Serial.print("Command: ");
    Serial.println(command);
    //if (gprsCheckResponse(response1, response2, timeOut) == OK) {
    //  returnValue = OK;
    //} else returnValue = NOTOK;
    ////////////
    unsigned long entry = millis();

    String reply;// = gprsReadString();
    returnValue = 99;
    while ((millis() - entry) < timeOut ) {
      reply = gprsSerial.readString();//gprsReadString();
      if (reply == "" || reply == "\n" || reply == " " || reply == "\t" || reply == "\v" || reply == "\f" || reply == "\r") continue;
      
      if (reply != "") {
        Serial.print((millis() - entry));
        Serial.print(" ms ");
        Serial.println(reply);
      }
    } //while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && ((millis() - entry) < timeOut ));
    
    if ((millis() - entry) >= timeOut) {
      returnValue = TIMEOUT;
    } else {
      if (reply.indexOf(response1) + reply.indexOf(response2) > -2) returnValue = OK;
      else returnValue = NOTOK;
    }

    ////////////
    count++;

  //}
  return returnValue;
}

byte gprsCheckResponse(String response1, String response2, int timeOut) 
{
  unsigned long entry = millis();


  String reply;// = gprsReadString();
  byte retVal = 99;
  do {
    reply = gprsSerial.readString();//gprsReadString();
    if (reply == "" || reply == "\n" || reply == " " || reply == "\t" || reply == "\v" || reply == "\f" || reply == "\r") continue;
    if (reply != "") {
      Serial.print((millis() - entry));
      Serial.print(" ms ");
      Serial.println(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && ((millis() - entry) < timeOut ));
  if ((millis() - entry) >= timeOut) {
    retVal = TIMEOUT;
  } else {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else retVal = NOTOK;
  }
      Serial.print("retVal=");
      Serial.println(retVal);
      return retVal;
}

byte gprsCommand(String command, String response1, String response2, int timeOut, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  
  while(gprsSerial.available() > 0) {
  gprsSerial.read(); //clear data
  }
  
  while (count < repetitions && returnValue != OK) {
    gprsSerial.println(command);
    Serial.print("Command: ");
    Serial.println(command);
    if (gprsCheckResponse(response1, response2, timeOut) == OK) {
      returnValue = OK;
    } else returnValue = NOTOK;
    count++;

  }
  return returnValue;
}

bool gprsBegin() 
{
  gprsSerial.println("AT");
  byte hi = gprsCheckResponse("OK", "yy", 1500);  // 1: registered, home network ; 5: registered, roaming
  
  while ( hi != OK) {
    gprsSerial.println("AT");
    hi = gprsCheckResponse("OK", "yy", 1500);
  
  }

  //if (gprsCommand("AT&F0", "OK", "yy", 5000, 2) == OK) {   // Reset to factory settings
    if (gprsCommand("ATE0", "OK", "yy", 5000, 2) == OK) {  // disable Echo
      if (gprsCommand("AT+CMEE=2", "OK", "yy", 5000, 2) == OK) return OK;  // enable better error messages
      else return NOTOK;
    }
  //}
}



String gprsReadString() {
  String reply = "";
  if (gprsSerial.available())  {
    reply = gprsSerial.readString();//
  }
  return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
bool gprsSetupTCP1(void) 
{
  command("AT+CREG?", "+CREG: 1,1",  20000, 1);
  command("AT+CGATT?", "+CGACT: 0,0", 20000, 1);
  command("AT+CGATT=1", "OK",  20000, 1);
  command("AT+CSTT=\"internet\",\"\",\"\"", "OK",  20000, 1); 
  command("AT+CIICR", "OK",  20000, 1); //bring up wireless connection
  command("AT+CIFSR", "OK",  20000, 1); //bring up wireless connection
  command("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK",  25000, 1); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
  command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK",  20000, 1); //begin send data to remote server
  delay(3000);
  command("AT+CIPCLOSE", "OK",  15000, 1); //"AT+CIPSHUT"
  delay(1000);
}

bool gprsSendTCPData1(void) 
{
  command("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK",  25000, 1); //start up the connection
  message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";    
  command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK",  20000, 1); //begin send data to remote server
  delay(3000);
  command("AT+CIPCLOSE", "OK",  15000, 1); // //"AT+CIPSHUT"
  delay(1000);
}


boolean command(String cmd, String response, unsigned long timeout, boolean output) {
  //Serial.print("Sending: ");
  //Serial.println(cmd);

  while(gprsSerial.available() > 0) {
    gprsSerial.read(); //clear data
  }
  if(cmd != "NONE")
  gprsSerial.print(cmd + "\r\n");

  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) {
    String result = gprsSerial.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) {
      Serial.println("Received Data: ");
      Serial.println(result);
      Serial.println();
    }
    

    if (result.indexOf(response) > 0) {
      Serial.println("Command Executed: " + cmd + " - " + response);
      return true;
    }
  }

  Serial.println("Command Failed: " + cmd);

  return false;
}
