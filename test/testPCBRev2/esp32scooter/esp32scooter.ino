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
#include <SoftwareSerial.h>

#define TOTAL_RECORDS 10
SoftwareSerial gpsSerial(recv_from_gps, trans_to_gps);
byte r;
String result,msg,msg2;
bool running = true;
const char* msg1;
bool ModuleState=false;

#define LED1 4 //RED
#define LED2 16 //Green
#define LED3 17 //Blue
unsigned long previousMillis = 0; 
int i=0;
/**************************************************************************/
/*!
    @brief  initialization of peripherals attached to ESP32 board
    @returns void
*/
/**************************************************************************/

void setup() 
{
  delay(100);

  gsmSerial.begin(gprsBaud, SERIAL_8N1, recv_from_gprs, trans_to_gprs);
  //gpsSerial.begin(gpsBaud, SERIAL_8N1, recv_from_gps, trans_to_gps);
  //Serial.begin(gpsBaud, SERIAL_8N1, recv_from_gps,trans_to_usb);//        rxPin = 3; txPin = 1;
  
  gpsSerial.begin(gpsBaud);
  
  
  pinMode(gprsUPWR, OUTPUT);
  pinMode(gprsRST, OUTPUT);
  //pinMode(gprsLPWR, OUTPUT);
  //digitalWrite(gprsRST, LOW);
  //digitalWrite(gprsLPWR, HIGH);
  digitalWrite(gprsUPWR, HIGH);

  Serial.begin(usbBaud, SERIAL_8N1, recv_from_usb,trans_to_usb);
  while (!Serial)
  {
      ; // wait for serial port to connect
  }
  Serial.println("Start");
    digitalWrite(gprsUPWR, LOW);
    delay(3000);
    digitalWrite(gprsUPWR, HIGH);
    delay(5000);
    
    ModuleState=gsmCheckInitialization();
    if(ModuleState==false)//if it's off, turn on it.
    {
      //Serial.println("Now turnning the A9/A9G READY.");
      ModuleState=gsmCheckInitialization();
    }
    ModuleState=gsmCheckATCommunication();
    if(ModuleState==false)//if it's off, turn on it.
    {
      /*digitalWrite(gprsPWR, LOW);
      delay(3000);
      digitalWrite(gprsPWR, HIGH);
      delay(5000);
      */
      //Serial.println("Now turnning the A9/A9G on.");
    }
    
  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) Serial.println("Wire1 failed to init");
  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);
  
/*
  Serial.println("SMARTAQNET Scooter is Reseting...");

  //gsmSerial.println("AT+RST=1");//Check the initial like AT or AT+RST=1
  //delay(5000);
  gsmSerial.println("AT");
  Serial.println("Waiting for GSM become ready...");
  gsmWaitForReady();
  Serial.println("GSM is Ready.");
  */
  rxState = 0;

  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  delay(1000);
  digitalWrite(LED1, 1);
  digitalWrite(LED2, 1);
  digitalWrite(LED3, 0);
  delay(1000);
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
      delay(1000);
      rxState = 9;
      break;
    case 9:
      r = gsmCommand("AT+CGDCONT?", "pinternet.interkom.de", "yy", 2000, 1);
      delay(1000);
      if (r == OK) rxState = 7;
      else rxState = 6;
      break;
    case 6:
      gsmCommand("AT+CGDCONT=1,\"IP\",\"pinternet.interkom.de\"", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = 7;
      break;
    case 7:
      gsmCommand("AT+CGACT=1,1", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = 8;
      break;
    case 8:
      gsmCommand("AT+CGACT?", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = 5;
      break;
    case 5:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);

      switch(resp)
      {
        case 1:
          usbSerial.println("Status: INITIAL");
          rxState = 10;
          break;
        case 2:
          usbSerial.println("Status: START");
          rxState = 20;
          break;
        case 3:
          usbSerial.println("Status: GPRSACT");
          rxState = 30;
          break;
        case 4:
          usbSerial.println("Status: CONNECT");
          rxState = 40;
          break;
        case 5:
          usbSerial.println("Status: CLOSE");
          rxState = 10;
          break;
      }
      delay(1500);
      break;
    case 10:
      gsmCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 4000, 1);
      rxState = 5;
      break;
    case 40:
      gsmCommand("AT+GPSRD=5", "OK", "yy", 2000, 1);
      delay(1000);
      rxState=127;
      break;
    case 127:
      MY_DBGln("Getting data from GPS using AT Cmd");
      while(running){
        if ( gsmSerial.available()) {
          result = gsmSerial.readStringUntil('\r');
          //usbSerial.println(result);
          switch(cntr)
          {
            case 0:
              if (result.indexOf("$GNGGA") > 0) 
              {
                cntr = 1;
                msg = result.substring(result.indexOf("$GNGGA"));
              }
              break;
            case 1:
              msg = msg + result;
              if (result.indexOf("$GNVTG") > 0) 
              {
                cntr = 0;
                usbSerial.println("-----------");
                usbSerial.println(msg);
                usbSerial.println("-----------");
                running = false;
                //Converting to readables
                /*msg1 = msg.c_str();
                usbSerial.println("..-------------------");
                usbSerial.println(msg1);
                usbSerial.println("..-------------------");
                while (*msg1)
                if(tiny.encode(*msg1++))
                  displayInfo();*/
                //getting location string
                msg2 = msg.substring(msg.indexOf("GNGGA")+6, msg.indexOf(",*"));
                msg = "";
                //usbSerial.println("-----------");
                //usbSerial.println(msg2);
                //usbSerial.println("-----------");
                rxState = 128;   
              }
              break;
          }
        
        }
      }
      break;
    case 128:
      running = true;
      MY_DBGln("Getting data from GPS using AT done");
      rxState = 129;
      break;
    case 129:
      gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      rxState=126;
      break;      

    case 126:
      MY_DBGln("Getting data from Sensors");
      status_sds = sds.read(&pm25, &pm10);
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = cntr1;//bme.readPressure() / 100;
      message = "{ scooterId: \'C45ZA1\', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " , loc: " + msg2 + " }";
      //usbSerial.println("Message: "+message);
      rxState = 41;
      break;
    case 41:
      gsmCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1);
      delay(6000);
      cntr1++;
      if(cntr1==TOTAL_RECORDS)
      {
        rxState=42;
        cntr1 = 0;
      }
      else
        rxState = 5;
      break;
    case 42:
      gsmCommand("AT+CIPCLOSE", "OK", "yy", 6000, 1);
      rxState=43;
      break;
    case 43:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);

      switch(resp)
      {
        case 5:
          usbSerial.println("Status: CLOSE");
          rxState = 50;
          break;
      }
      delay(1500);
      break;
    case 50:
      gsmCommand("AT+CIPSHUT", "OK", "yy", 6000, 1);
      rxState=51;
      break;
    case 51:
      delay(1500);
      rxState=0;      
      break;

    case 131:
      
      break;

    }
}
/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop1()
{

  if (usbSerial.available())
  {
    gsmSerial.write(usbSerial.read());
  }
  if (gsmSerial.available())
  {
    usbSerial.write(gsmSerial.read());
  }
  //Uncomment the following if you want to get the A9G GPS direct data
  /*if (gpsSerial.available())
  {
    usbSerial.write(gpsSerial.read());
  }*/

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    i++;
    if (i==8) i =0;
    digitalWrite(LED1, i&1);
    digitalWrite(LED2, (i>>1)&1);
    digitalWrite(LED3, (i>>2)&1);
  }


}
