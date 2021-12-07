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

#define TOTAL_RECORDS 10000//10

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
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  

   
  gsmSerial.begin(gprsBaud, SERIAL_8N1, recv_from_gprs, trans_to_gprs);
  //gpsSerial.begin(gpsBaud, SERIAL_8N1, recv_from_gps, trans_to_gps);
  //Serial.begin(gpsBaud, SERIAL_8N1, recv_from_gps,trans_to_usb);//        rxPin = 3; txPin = 1;
  
  gpsSerial.begin(gpsBaud);
  
  /*pinMode(gprsPWR, OUTPUT);
  pinMode(gprsRST, OUTPUT);
  //pinMode(gprsLPWR, OUTPUT);
  //digitalWrite(gprsRST, LOW);
  //digitalWrite(gprsLPWR, HIGH);
  digitalWrite(gprsPWR, HIGH);*/

  usbSerial.begin(usbBaud, SERIAL_8N1, recv_from_usb,trans_to_usb);
  while (!usbSerial)
  {
      ; // wait for serial port to connect
  }
  usbSerial.println("Start");
  /*digitalWrite(gprsPWR, LOW);
  delay(3000);
  digitalWrite(gprsPWR, HIGH);
  delay(5000);*/
  delay(8000);
  
  ModuleState=gsmCheckInitialization();
  if(ModuleState==false)//if it's off, turn on it.
  {
    //usbSerial.println("Now turnning the A9/A9G READY.");
    ModuleState=gsmCheckInitialization();
    usbSerial.println("A9G not initialized, reset again!");
    while(1);
  }
  ModuleState=gsmCheckATCommunication();
  if(ModuleState==false)//if it's off, turn on it.
  {
    /*digitalWrite(gprsPWR, LOW);
    delay(3000);
    digitalWrite(gprsPWR, HIGH);
    delay(5000);
    */
    //usbSerial.println("Now turnning the A9/A9G on.");
  }
    
  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) usbSerial.println("Wire1 failed to init");
  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);

  rxState = set_gprsd_off;
  
  rgbSetColor(noColor);
  delay(1000);
  rgbSetColor(bColor);
  delay(1000);
  rgbSetColor(gColor);

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
    
    case set_gprsd_off:
      gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      rxState = set_gps_on;
      break;
    case set_gps_on:
      gsmCommand("AT+GPS=1", "OK", "yy", 2000, 1);
      rxState = chk_creg;
      break;
    case chk_creg:
      gsmCommand("AT+CREG?", "OK", "yy", 2000, 1);
      rxState = chk_cgact;
      break;
    case chk_cgact:
      gsmCommand("AT+CGACT?", "OK", "yy", 2000, 1);
      rxState = set_cgatt_on;
      break;
    case set_cgatt_on:
      gsmCommand("AT+CGATT=1", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = chk_cgdcont;
      break;
    case chk_cgdcont:
      r = gsmCommand("AT+CGDCONT?", "pinternet.interkom.de", "yy", 2000, 1);
      delay(1000);
      if (r == OK) rxState = set_cgact_on;
      else rxState = set_cgdcont_on;
      break;
    case set_cgdcont_on:
      gsmCommand("AT+CGDCONT=1,\"IP\",\"pinternet.interkom.de\"", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = set_cgact_on;
      break;
    case set_cgact_on:
      gsmCommand("AT+CGACT=1,1", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = chk_cgact_again;
      break;
    case chk_cgact_again:
      gsmCommand("AT+CGACT?", "OK", "yy", 2000, 1);
      delay(1000);
      rxState = chk_cipstatus;
      break;
    case chk_cipstatus:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);

      switch(resp)
      {
        case 1:
          usbSerial.println("Status: INITIAL");
          rxState = set_cipstart;
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
          rxState = set_gpsrd_read;
          break;
        case 5:
          usbSerial.println("Status: CLOSE");
          rxState = set_cipstart;
          break;
      }
      delay(1500);
      break;
    case set_cipstart:
      gsmCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 4000, 1);
      rxState = chk_cipstatus;
      break;
    case set_gpsrd_read:
      gsmCommand("AT+GPSRD=5", "OK", "yy", 2000, 1);
      delay(1000);
      rxState=read_gps_msg;
      break;
    case read_gps_msg:
      MY_DBGln("Getting data from GPS using AT Cmd");
      rgbSetColor(rColor);
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
                loc = msg.substring(msg.indexOf("GNGGA")-1, msg.indexOf(",*")+4);
                msg = "";
                //usbSerial.println("-----------");
                //usbSerial.println(loc);
                //usbSerial.println("-----------");
                rxState = read_gps_done;   
              }
              break;
          }
        
        }
      }
      break;
    case read_gps_done:
      running = true;
      MY_DBGln("Getting data from GPS using AT done");
      rxState = set_gpsrd_off;
      break;
    case set_gpsrd_off:
      gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      rxState=sensor_data;
      break;      

    case sensor_data:
      MY_DBGln("Getting data from Sensors");
      status_sds = sds.read(&pm25, &pm10);
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = cntr1;//bme.readPressure() / 100;
      message = "{ Id: \'A1\', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " , loc: " + loc + " }";
      //usbSerial.println("Message: "+message);
      rxState = set_cipsend;
      break;
    case set_cipsend:
      gsmCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1);
      delay(6000);
      cntr1++;
      if(cntr1==TOTAL_RECORDS)
      {
        rxState=set_cipclose;
        cntr1 = 0;
      }
      else
        rxState = chk_cipstatus;
      break;
    case set_cipclose:
      gsmCommand("AT+CIPCLOSE", "OK", "yy", 6000, 1);
      rxState=chk_cipstatus_again;
      break;
    case chk_cipstatus_again:
      resp = gsmCheckStatus("AT+CIPSTATUS?",10500);
      switch(resp)
      {
        case 5:
          usbSerial.println("Status: CLOSE");
          rxState = set_cipshut;
          break;
      }
      delay(1500);
      break;
    case set_cipshut:
      gsmCommand("AT+CIPSHUT", "OK", "yy", 6000, 1);
      rxState=dummy_state;
      break;
    case dummy_state:
      delay(1500);
      rxState=set_gprsd_off;      
      break;

    case 20:
      break;
    case 30:
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
