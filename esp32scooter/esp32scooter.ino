/**
 * \file esp32scooter.ino
 *
 * \brief This is the Arduino .ino file.
 *
 * This is the ESP32 implementation file, which implement the overall flow of the project by calling different files/functions.
 * It implements the state-machine like flow as described in the <a href="StatemachineFlow.pdf" target="_blank"><b>flowchart</b></a>.  
 * 
 * \date 07.09.2020
 */


#include "main.h"


/**************************************************************************/
/*!
    @brief  initialization of peripherals attached to ESP32 board
    @returns void
*/
/**************************************************************************/

void setup() 
{
  delay(100);
  gprsSerial.begin(gprsBaud, SERIAL_8N1, gprsRX, gprsTX);
  //gpsSerial.begin(gpsBaud, SERIAL_8N1, GPSRX, GPSTX);
  //gprsSerial.setTimeout(20000);
  
  //Serial.begin(115200);    //        rxPin = 3; txPin = 1;
  Serial.begin(gpsBaud, SERIAL_8N1, gpsRX,1);

  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) Serial.print("Wire1 failed to init");
  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);
  
  Serial.println("Start");

  pinMode(gprsPWR, OUTPUT);
  digitalWrite(gprsPWR, HIGH);
  delay(2000);
  digitalWrite(gprsPWR, LOW);
  //delay(2000);
  //digitalWrite(gprsPWR, HIGH);
  delay(5000);

  Serial.println("Setting GPRS MODULE");


  //while( command("AT", "OK", 5000, false)==NOTOK);
  while( command("AT+RST=1", "RST", 20000, false)==NOTOK);
   
  Serial.println("Ready GPRS MODULE");
  
  setupTimer(1);currentTimeout=10;
}

/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop()
{
  switch(rxState)
  {
    case 100:
      while ( gprsSerial.available())
      {
        rd = gprsSerial.readString();
        Serial.println("-------------------");
        Serial.println("Rcvd(Len): " +String(rd.length()) +"\n" + rd );
        Serial.println("-------------------");
      }
      if (rd!="")
      {
          if(rd.indexOf("ERROR")>0){
            Serial.println("RcvdErr" );
            Err=NOTOK;
            //rxState = nState-1;//current
            rxState = current;
          }
          else if(rd.indexOf(resp)>0){
            Serial.println("RcvdResp: "+ resp );
            Err=OK;
            rxState = nState;
          }
          //else{
          //  Serial.println("Serial Error1: " );
          //  Err=NOTOK;
          //  rxState = nState-1;//current
          //}

        rd = "";
        Serial.println("RcvdAt: "+ String(interruptCounter) +"s" );
        Serial.println("===================");
        interruptCounter=0;
        timerAlarmDisable(timer);
        //delay(10000);
        //rxState = nState;
      }

      break;
    case 0:
      Serial.println("Sending AT1");resp="OK"; 
      //setupTimer(10);
      setupTimer(1);currentTimeout=10;
      gprsSerial.println("AT");
      rxState = 100;
      nState = 1;
      current = 0;
      break;
    case 1:
      Serial.println("Sending AT2");
      resp="OK"; 
      //setupTimer(10);
      setupTimer(1);currentTimeout=10;
      gprsSerial.println("AT");
      nState=2;
      current = 1;
      rxState = 100;
      break;
    case 2:
      //Serial.println("Sending AT+RST=1");
      Serial.println("Sending AT3");
      //setupTimer(60);
      //gprsSerial.setTimeout(10000);
      setupTimer(1);currentTimeout=60;
      resp="OK"; //
      //resp="RST"; //
      //resp="+CREG: 0";
      //gprsSerial.println("AT+RST=1");
      gprsSerial.println("AT");
      nState=32;
      current = 2;
      rxState = 100;
      break;
    case 32:
      Serial.println("Sending AT+GPSRD=0");
      //gprsSerial.setTimeout(10000);
      setupTimer(1);currentTimeout=20;
      resp="OK"; 
      gprsSerial.println("AT+GPSRD=0");
      nState=3;
      current = 32;
      rxState = 100;
      break;
    case 3:
      //gprsSerial.setTimeout(1000);
      delay(10000); //skip extra response of AT+RST=1, missed due to \0
      //setupTimer(20);
      
      setupTimer(1);currentTimeout=20;
      Serial.println("Sending AT+GPS=1");resp="OK"; 
      gprsSerial.println("AT+GPS=1");
      nState=24;//4;
      current = 3;
      rxState = 100;
      break;
    case 4:
      Serial.println("Sending AT+CREG?");resp="+CREG: 1,1"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CREG?");
      nState=5;
      current = 4;
      rxState = 100;
      break;
    case 5:
      Serial.println("Sending AT+CGACT?");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CGACT?");
      nState=6;
      current = 5;
      rxState = 100;
      break;
    case 6:
      Serial.println("Sending AT+CGATT");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CGATT=1");
      nState=7;
      current = 6;
      rxState = 100;
      break;
    case 7:
      Serial.println("Sending AT+CSTT");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CSTT=\"internet\",\"\",\"\"");
      nState=8;
      current = 7;
      rxState = 100;
      break;
    case 8:
      Serial.println("Sending AT+CIICR");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CIICR");
      nState=9;
      current = 8;
      rxState = 100;
      break;
    case 9:
      Serial.println("Sending AT+CIFSR");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CIFSR");
      nState=10;
      current = 9;
      rxState = 100;
      break;
    case 10:
      Serial.println("Sending AT+CIPSTART");resp="CONNECT OK"; 
      //setupTimer(30);
      setupTimer(1);currentTimeout=30;
      gprsSerial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port);
      nState=11;
      current = 10;
      rxState = 100;
      break;
    case 11:
      Serial.println("Sending AT+CIPSEND");resp="OK"; 
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      //setupTimer(30);
      setupTimer(1);currentTimeout=30;
      gprsSerial.println("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"");
      nState=12;
      rxState = 100;
      break;
    case 12:
      Serial.println("Sending AT+CIPCLOSE");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+CIPCLOSE");
      nState=10;
      rxState = 100;
      break;
      //////////
    case 13:
      break;
    case 24:
      Serial.println("Getting data from GPS-TX");
      //while ( gpsSerial.available())
      if ( Serial.available())
      {
        //rd = gpsSerial.readString();
        //Serial.println("..-------------------");
        //Serial.println("Rcvd(Len): " +String(rd.length()) +"\n" + rd );
        //Serial.println("..-------------------");
        tiny.encode(Serial.read());//tiny.encode(gpsSerial.read());
        //if (tiny.location.isUpdated()){
          Serial.print("Latitude1= "); 
          Serial.print(tiny.location.lat(), 6);
          Serial.print(" Longitude1= "); 
          Serial.println(tiny.location.lng(), 6);
        //}
        rxState = 25;
      }
      break;
    case 25:
      Serial.println("Getting data from GPS-TX done");
      rxState = 26;
      break;
    case 26:
      Serial.println("Sending AT+GPSRD=5");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+GPSRD=5");
      nState=27;
      current = 26;
      rxState = 100;
      break;
    case 27:
      Serial.println("Getting data from GPS using AT");
      rxState = 28;
      if ( gprsSerial.available())
      {
        //rd = gpsSerial.readString();
        //Serial.println("..-------------------");
        //Serial.println("Rcvd(Len): " +String(rd.length()) +"\n" + rd );
        //Serial.println("..-------------------");
        tiny.encode(gprsSerial.read());
        //if (tiny.location.isUpdated()){
          Serial.print("Latitude2= "); 
          Serial.print(tiny.location.lat(), 6);
          Serial.print(" Longitude2= "); 
          Serial.println(tiny.location.lng(), 6);
        //}
        rxState = 28;
      }
      break;
    case 28:
      Serial.println("Getting data from GPS using AT done");
      rxState = 29;
      break;
    case 29:
      Serial.println("Sending AT+GPSRD=0");resp="OK"; 
      //setupTimer(20);
      setupTimer(1);currentTimeout=20;
      gprsSerial.println("AT+GPSRD=0");
      nState=30;
      current = 29;
      rxState = 100;
      break;
    case 30:
      Serial.println("Getting data from Sensors");
      status_sds = sds.read(&pm25, &pm10);
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = bme.readPressure() / 100;
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      Serial.println("Message: "+message);
      delay(10000);
      //rxState = 31;
      break;
    case 31:
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
