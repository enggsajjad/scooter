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

// !!!IMPORTANT!!!
// this is for test only
#define TOTAL_RECORDS 10000//10




/**************************************************************************/
/*!
    @brief  initialization of peripherals attached to ESP32 board
    @returns void
*/
/**************************************************************************/

void setup() 
{
  delay(100);

  Serial.begin(usbBaud, SERIAL_8N1, recv_from_usb,trans_to_usb);
  gsmSerial.begin(gprsBaud, SERIAL_8N1, recv_from_gprs, trans_to_gprs);
  gpsSerial.begin(gpsBaud);

  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(gprsPWR, OUTPUT);
  //digitalWrite(gprsPWR, HIGH);
  digitalWrite(gprsPWR, LOW);
  delay(1000);
  rgbSetColor(noColor);
  
  Serial.println("SmartAQnet Scooter Sensors...");

  //delay(1000);
  //digitalWrite(gprsPWR, LOW);
  delay(3000);
  digitalWrite(gprsPWR, HIGH);
  Serial.println("Starting...");

  //r1 = readATResponse( "+CREG: 2", "ERROR", 60000); // !!!IMPORTANT!!!
  //r1 = readATResponse( "\n", "ERROR", 60000); // !!!IMPORTANT!!!
  r2 = readATResponse( "READY", "ERROR", 5*60000); // !!!IMPORTANT!!!

  //if ( (r1==1) and (r2 ==1))
  if ( (r2 ==1))
  {
    Serial.println("Init Ready Done!");
    a9gPowerOnCheck();
    rxState = config_sensors;
  }
  else
  {
    Serial.println("Init Error!");
    Serial.println("Check PWR and RESET of the A9G or SIM Serivices");
    
    rgbSetColor(rColor);
    last = init_errors;
    rxState = try_init_again;
    Serial.println("Reseting A9G again...");

    sendATCommand("AT+RST=1", "OK", "ERROR", 6000);
  }
  


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
    case config_sensors:
      rxState = chk_sim_cpin;
      sds.begin(&SDS_SERIAL, recv_from_sds, trans_to_sds);  // initialize SDS011 sensor
    
      bmeAddress = BME_ADDR;
    
      if (!bme.begin(bmeAddress, true)) 
      {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        last = bme_setting_error;
        rxState = error_state;
        //while (1);
      }
      // Set up oversampling and filter initialization
      bme.setTemperatureOversampling(BME680_OS_8X);
      bme.setHumidityOversampling(BME680_OS_2X);
      bme.setPressureOversampling(BME680_OS_4X);
      bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
      bme.setGasHeater(320, 150); // 320*C for 150 ms
      //LEDS
      delay(1000);
      rgbSetColor(bColor);
      delay(1000);
      rgbSetColor(gColor);
      break;
    case try_init_again:
      r1 = sendATCommand("AT+RST=1", "OK", "ERROR", 6000); // !!!IMPORTANT!!!
      r2 = readATResponse( "READY", "ERROR", 5*60000); // !!!IMPORTANT!!!
    
      if ( (r1==1) and (r2 ==1))
      {
        Serial.println("Init Ready Done!");
        rxState = chk_sim_cpin;
      }
      else
      {
        Serial.println("Init Error!");
        Serial.println("Check PWR and RESET of the A9G or SIM Serivices");
        rgbSetColor(rColor);
        last = init_errors;
        rxState = error_state;
      }
      //a9gPowerOnCheck();
      break;
    case chk_sim_cpin:
      r = sendATCommand("AT+CPIN?", "READY", "SIM", 2000);
      if (r == 1)
      {
        Serial.println("Msg: SIM PIN is ready!");
        rxState = set_network;
      }else if (r == 2)
       rxState = error_state;
      else
       rxState = timeout_state;
      break;
    case set_network:
      r = sendATCommand("AT+CREG=1", "OK", "ERROR", 60000);
      if (r == 1)
      {
        Serial.println("Msg: Netwoek Set!");
        rxState = chk_network_register;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case chk_network_register:
      //r = sendATCommand("AT+CREG?", "+CREG: 1,1", "+CREG: 0,5", 2000); // !!!IMPORTANT!!!
      r = sendATCommand("AT+CREG?", "+CREG: 1", "+CREG: 0", 2000); // !!!IMPORTANT!!!
      if (r == 1)
      {
        Serial.println("Msg: Network Registered!");
        rxState = set_gps;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case set_gps:
      r = sendATCommand("AT+GPS=1", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Set!");
        rxState = attach_ps;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case attach_ps:
      r = sendATCommand("AT+CGATT=1", "OK", "ERROR", 60000);
      if (r == 1)
      {
        Serial.println("Msg: PS Attached!");
        rxState = check_context;
      }else
        rxState = attach_ps_again;
      break;
    case attach_ps_again:
      r = sendATCommand("AT+CGATT=1", "OK", "NO RESPONSE!", 90000);      
      if (r == 1)
      {
        Serial.println("Msg: PS2 Attached!");
        rxState = check_context;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case check_context:
      r = sendATCommand("AT+CGDCONT?", (char*)apn.c_str(), "OK", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context Exists!");
        rxState = select_context;
      }else if (r == 2)
      {
        Serial.println("Msg: Context Set!");
        rxState = set_context;
      }else
        rxState = timeout_state;
      break;
    case set_context:
      r = sendATCommand((char*)("AT+CGDCONT=1,\"IP\",\"" + apn +"\"").c_str(), "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context Changed!");
        rxState = select_context;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case select_context:
      r = sendATCommand("AT+CGACT=1,1", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context Selected!");
        rxState = show_context;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case show_context:
      r = sendATCommand("AT+CGACT?", "+CGACT: 1, 1", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context is Correct!");
        rxState = chk_cip_status;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case chk_cip_status:
      r = sendATCommand("AT+CIPSTATUS", "INITIAL", "CLOSED", 2000);
      if (r == 1)
      {
        Serial.println("Msg: Status INITIAL!");
        rxState = set_cipstart;
      }else if (r == 2)
      {
        Serial.println("Msg: Status CLOSED!");
        rxState = set_cipstart;
      }else
        rxState = timeout_state;
      break;
    case set_cipstart:
      #ifdef THINGSPEAK
        r = sendATCommand((char*)("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port).c_str(), "CONNECT OK", "CONNECT FAIL", 20000);//for thinkspeak
      #else
        r = sendATCommand((char*)("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port).c_str(), "CONNECT OK", "ERROR", 20000);//for ngrok TCP tunneling
      #endif
      if (r == 1)
      {
        Serial.println("Msg: Connected!");
        delay(100);
        rxState = set_gpsrd_read;
        
      }else if (r == 2)
      {
        Serial.println("Msg: Connection in Process!");
        delay(10000);//Try again after some time
        rxState = set_cipstart;
      }else
        rxState = timeout_state;
      break;
    case set_gpsrd_read:
      //r = sendATCommand("AT+GPSRD=2", "OK", "ERROR", 2000);// Will also work
      r = sendATCommand("AT+GPSRD=5", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Read Set!");
        rxState = read_gps_msg;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case read_gps_msg:
      MY_DBGln("Msg: Getting data from GPS using AT Cmd");
      gpsReadMessages();
      break;
    case read_gps_done:
      running = true;
      MY_DBGln("Msg: Getting data from GPS using AT done");
      rxState = set_gpsrd_off;
      break;
    case set_gpsrd_off:
      r = sendATCommand("AT+GPSRD=0", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Read Off!");
        rxState = sensor_data;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case sensor_data:
      MY_DBGln("Msg: Getting data from Sensors");
      //status_sds = sds.read(&pm25, &pm10);
      //debugging
      pm25 = random(20, 25);//debugging
      pm10 = random(30, 35);//debugging
      //status_sds = sds.dataQueryCmd(&pm10, &pm25 );
      // put your main code here, to run repeatedly:
      if (! bme.performReading()) 
      {
        Serial.println("Msg: Failed to perform reading!");
        //last = bme_reading_error;
        //rxState = error_state;
      }
      //temp = bme.temperature;
      //hum = bme.humidity;
      //atm = bme.pressure / 100.0;
      //debugging
      temp = random(2, 10);
      hum = random(10, 20);
      atm = cntr1;//bme.readPressure() / 100;
      #ifdef THINGSPEAK
        //channel: eScooter
        message = "GET https://api.thingspeak.com/update?api_key=AYKVFH212TKGNW2B&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(loc);//for thinkspeak
        //channel: eScooter2 
        //message = "GET https://api.thingspeak.com/update?api_key=TJ85HJBF1XTV1GH7&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(loc);//for thinkspeak
      #else
        message = "{ Id: \'A1\', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " , loc: " + loc + " }";//for ngrok TCP tunneling
      #endif
      //Serial.println("Message: "+message);
      rxState = set_cipsend;
      break;
    case set_cipsend:
      #ifdef THINGSPEAK
        cntrlChar[0] = 26;
        cntrlChar[1] = '\n';
        r = sendATCommand("AT+CIPSEND", ">", "ERROR", 10000);
        delay(200);
      #else
        r = sendATCommand((char*)("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"").c_str(), "OK", "ERROR", 10000);
      #endif 
      if (r == 1)
      {
        Serial.println("Msg: Send Started!");
        rxState = set_cipsend_complete;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case set_cipsend_complete:
      #ifdef THINGSPEAK
        r = sendATCommand((char*)message.c_str(), "ERROR", "ERROR", 2000);
        delay(300);
        r = sendATCommand(cntrlChar, "CLOSED", "ERROR", 10000);
      #else

      #endif 
      if (r == 1)
      {
        Serial.println("Msg: Send Completed!");
        rgbSetColor(gColor);
        cntr1++;
        if(cntr1==TOTAL_RECORDS)
        {
          rxState=set_cipclose;
          cntr1 = 0;
        }
        else
          rxState = chk_cipstatus_again;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      //delay(3000);//with this from 0,3,6 improved to 0,2,4,6,8
      delay(5000);//with this from  0,2,4,6,8 improved to 0,1,2,3,4,5,6,8
      break;
    case chk_cipstatus_again:
      r = sendATCommand("AT+CIPSTATUS", "CONNECT", "CLOSED", 2000);
      rgbSetColor(noColor);
      if (r == 1)
      {
        Serial.println("Msg: Status CONNECT!");
        rxState = set_network;
      }else if (r == 2)
      {
        //Serial.println("Msg: Status CLOSED!");
        rxState = set_cipstart;
        delay(5000);//with this from  0,2,4,6,8 improved to 0,1,2,3,4,5,6,8
      }else
        rxState = timeout_state;
      break;
    case set_cipclose:
      r = sendATCommand("AT+CIPCLOSE", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Connection Closed!");
        rxState = set_cipshut;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case set_cipshut:
      r = sendATCommand("AT+CIPSHUT", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Connection Shut!");
        rxState = dummy_state;
      }else if (r == 2)
        rxState = error_state;
      else
        rxState = timeout_state;
      break;
    case dummy_state:
      delay(1500);
      rxState = set_network;      
      break;
    case timeout_state:
      showTimeout();
      rxState = hang_state;
      break;
    case error_state:
      showError();
      rgbSetColor(rColor);
      MY_DBG("\r\n*****Error No.");
      MY_DBGln(resetCntr);
      resetCntr++;
      if (resetCntr < 5)
      {
        delay(5000);
        digitalWrite(LED1, 1);
        rxState=set_network;
      }
      else
      {
        resetCntr = 0;
        rxState = hang_state;
      }
      break;
    case hang_state:
      //rgbSetColor(resetCntr++);
      //delay(1000);
      //if (resetCntr == 8) resetCntr = 0;
      loopSerial();
      break;

  }
  last = rxState;
}

/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop1()
{
  loopSerial();
}
