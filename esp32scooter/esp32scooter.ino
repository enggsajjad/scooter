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

  gsmSerial.begin(gprsBaud, SERIAL_8N1, gprsRX, gprsTX);
  //gpsSerial.begin(gpsBaud, SERIAL_8N1, GPSRX, GPSTX);
  Serial.begin(gpsBaud, SERIAL_8N1, gpsRX,usbTX);//        rxPin = 3; txPin = 1;
  Serial.println("Start");
  
  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) Serial.println("Wire1 failed to init");
  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);
  

  Serial.println("SMARTAQNET Scooter is Reseting...");

  gsmSerial.println("AT+RST=1");
  delay(5000);
  Serial.println("Waiting for GSM become ready...");
  gsmWaitForReady();
  Serial.println("GSM is Ready.");
  rxState = 0;

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
    
    case 0:
      gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      rxState = 1;
      break;
    case 1:
      gsmCommand("AT+GPS=1", "OK", "yy", 2000, 1);
      rxState = 2;
      break;
    case 2:
      gsmCommand("AT+CREG?", "OK", "yy", 2000, 1);
      rxState = 3;
      break;
    case 3:
      gsmCommand("AT+CGACT?", "OK", "yy", 2000, 1);
      rxState = 4;
      break;
    case 4:
      gsmCommand("AT+CGATT=1", "OK", "yy", 2000, 1);
      delay(7000);
      rxState = 5;
      break;
    case 5:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);

      switch(resp)
      {
        case 1:
          Serial.println("Status: INITIAL");
          rxState = 10;
          break;
        case 2:
          Serial.println("Status: START");
          rxState = 20;
          break;
        case 3:
          Serial.println("Status: GPRSACT");
          rxState = 30;
          break;
        case 4:
          Serial.println("Status: CONNECT");
          rxState = 40;
          break;
        case 5:
          Serial.println("Status: CLOSE");
          rxState = 50;
          break;
      }
      delay(1500);
      break;
    case 10:
      gsmCommand("AT+CSTT=\"internet\",\"\",\"\"", "OK", "yy", 2000, 1);
      rxState=5;
      break;
    case 20:
      gsmCommand("AT+CIICR", "OK", "yy", 5000, 1);
      delay(6000);
      rxState=5;
      break;
    case 30:
      gsmCommand("AT+CIFSR", "OK", "yy", 2000, 1);
      rxState=31;
      break;
    case 31:
      gsmCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 4000, 1);
      rxState=5;
      break;
    case 40:
      MY_DBGln("Getting data from Sensors");
      status_sds = sds.read(&pm25, &pm10);
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = cntr;//bme.readPressure() / 100;
      message = "{ scooterId: \'C45ZA1\', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      //Serial.println("Message: "+message);
      rxState = 41;
      break;
    case 41:
      gsmCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1);
      delay(6000);
      cntr++;
      if(cntr==10)
      {
        rxState=42;
        cntr = 0;
      }
      else
        rxState=5;
      break;
    case 42:
      gsmCommand("AT+CIPCLOSE", "OK", "yy", 6000, 1);
      rxState=5;
      break;
    case 50:
      gsmCommand("AT+CIPSHUT", "OK", "yy", 6000, 1);
      rxState=51;
      break;
    case 51:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);

      switch(resp)
      {
        case 1://"IP INITIAL":
          Serial.println("Status: INITIAL");
          rxState = 124;
          break;
      }
      delay(1500);
      break;
      //////////

    case 124:
      MY_DBGln("Getting data from GPS-TX Pin");
      //while ( gpsSerial.available())
      if ( Serial.available())
      {
        //rd = gpsSerial.readString();
        //Serial.println("..-------------------");
        //Serial.println("Rcvd(Len): " +String(rd.length()) +"\n" + rd );
        //Serial.println("..-------------------");
        tiny.encode(Serial.read());//tiny.encode(gpsSerial.read());
        //if (tiny.location.isUpdated()){
          MY_DBG("Latitude1= "); 
          MY_DBG(tiny.location.lat(), 6);
          MY_DBG(" Longitude1= "); 
          MY_DBGln(tiny.location.lng(), 6);
        //}
        rxState = 125;
      }
      break;
    case 125:
      MY_DBGln("Getting data from GPS-TX Pin done");
      rxState = 126;
      break;
    case 126:
      gsmCommand("AT+GPSRD=5", "OK", "yy", 2000, 1);
      rxState=127;
      break;
    case 127:
      MY_DBGln("Getting data from GPS using AT Cmd");
      rxState = 128;
      if ( gsmSerial.available())
      {
        //rd = gpsSerial.readString();
        //Serial.println("..-------------------");
        //Serial.println("Rcvd(Len): " +String(rd.length()) +"\n" + rd );
        //Serial.println("..-------------------");
        tiny.encode(gsmSerial.read());
        //if (tiny.location.isUpdated()){
          MY_DBG("Latitude2= "); 
          MY_DBG(tiny.location.lat(), 6);
          MY_DBG(" Longitude2= "); 
          MY_DBGln(tiny.location.lng(), 6);
        //}
        //rxState = 128;
      }
      break;
    case 128:
      MY_DBGln("Getting data from GPS using AT done");
      rxState = 129;
      break;
    case 129:
      gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      rxState=130;
      break;
    case 130:
      MY_DBGln("Getting data from Sensors: Tests");
      status_sds = sds.read(&pm25, &pm10);
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = bme.readPressure() / 100;
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " }";
      MY_DBGln("Message: "+message);
      delay(10000);
      rxState = 0;
      break;
    case 131:
      
      break;

    }
}
