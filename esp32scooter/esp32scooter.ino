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

  pinMode(gprsPWR, OUTPUT);
  digitalWrite(gprsPWR, HIGH);
  //delay(2000);
  //digitalWrite(gprsPWR, LOW);
  delay(1000);

  gprsSerial.begin(gprsBaud, SERIAL_8N1, gprsRX, gprsTX);
  //gpsSerial.begin(gpsBaud, SERIAL_8N1, GPSRX, GPSTX);
  Serial.begin(gpsBaud, SERIAL_8N1, gpsRX,usbTX);//        rxPin = 3; txPin = 1;
  Serial.println("Start");
  
  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) Serial.println("Wire1 failed to init");
  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);
  

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
            //Err=NOTOK;
            //rxState = current;
            rxState = 101;
          }
          else if(rd.indexOf(resp)>0){
            Serial.println("RcvdResp: "+ resp );
            Err=OK;
            rxState = nState;
          }

        rd = "";
        Serial.println("RcvdAt: "+ String(interruptCounter) +"s" );
        Serial.println("===================");
        interruptCounter=0;
        timerAlarmDisable(timer);
      }

      break;
    case 0:
      setATCommand("AT", "OK", 1, 10);
      nState = 1;
      break;
    case 1:
      setATCommand("AT", "OK", 1, 10);
      nState=2;
      break;
    case 2:
      //setATCommand("AT+RST=1", "RST", 1, 60);
      setATCommand("AT", "OK", 1, 60);
      nState=32;
      break;
    case 32:
      setATCommand("AT+GPSRD=0", "OK", 1, 20);
      nState=3;
      break;
    case 3:
      //delay(3000); //skip extra response of AT+RST=1, missed due to \0
      setATCommand("AT+GPS=1", "OK", 1, 20);
      nState=24;//4;
      break;
    case 4:
      setATCommand("AT+CREG?", "+CREG: 1,1", 1, 20);
      nState=5;
      break;
    case 5:
      setATCommand("AT+CGACT?", "OK", 1, 20);
      nState=6;
      break;
    case 6:
      setATCommand("AT+CGATT=1", "OK", 1, 20);
      nState=7;
      break;
    case 7:
      setATCommand("AT+CSTT=\"internet\",\"\",\"\"", "OK", 1, 20);
      nState=8;
      break;
    case 8:
      setATCommand("AT+CIICR", "OK", 1, 20);
      nState=9;
      break;
    case 9:
      setATCommand("AT+CIFSR", "OK", 1, 20);
      nState=10;
      break;
    case 10:
      setATCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", 1, 30);
      nState=11;
      break;
    case 11:
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      setATCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", 1, 30);
      nState=12;
      break;
    case 12:
      setATCommand("AT+CIPCLOSE", "OK", 1, 20);
      nState=10;
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
      setATCommand("AT+GPSRD=5", "OK", 1, 20);
      nState=27;
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
      setATCommand("AT+GPSRD=0", "OK", 1, 20);
      nState=30;
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
    case 101://NOTOK ERROR
      Err=NOTOK;
      rxState = current;
      break;
    case 102://TIMEOUT ERROR
      Err=TIMEOUT;
      rxState = current;
      break;
    }
}
