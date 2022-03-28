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
#include <Preferences.h>

Preferences preferences;


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
  pinMode(gprsUEN, OUTPUT);
  pinMode(gprsPWR, OUTPUT);

  //digitalWrite(gprsPWR, HIGH);
  digitalWrite(gprsPWR, HIGH);
  digitalWrite(gprsUEN, HIGH);
  delay(500);
  digitalWrite(gprsUEN, LOW);
  
  digitalWrite(gprsPWR, LOW);
  delay(1000);
  rgbSetColor(noColor);
  
  Serial.println("\nSmartAQnet Scooter Sensors!");

  //delay(1000);
  //digitalWrite(gprsPWR, LOW);
  delay(3000);
  digitalWrite(gprsPWR, HIGH);
  Serial.println("Initializing A9G...");

  r = readATResponse( "READY", "ERROR", 5*60000); // !!!IMPORTANT!!!

  if ( (r ==1))
  {
    Serial.println("Init Ready Done!");
    a9gPowerOnCheck();
    rxState = config_sensors;
  }
  else
  {
    Serial.println("Msg: Init Error!, Try again!");
    Serial.println("Msg: Check PWR and RESET of the A9G or SIM Serivices");

    rgbSetColor(rColor);
    rxState = try_init_again;
    Serial.println("Reseting A9G again...");

    //sendATCommand("AT+RST=1", "OK", "ERROR", 6000);
    a9gReset();
  }
  //read/write flash memory values
  preferences.begin("my-app", false);
  int counter = preferences.getUInt("resetCntr", 0);
  
  for(int dumy=0;dumy < counter; dumy++) {
    key = items+String(dumy);
    Serial.print("key : "+key);
    value = preferences.getUInt(key.c_str(), 0);
    Serial.printf(" value: %u\n", value);
    Serial.println();
  }
  preferences.end();
  Serial.println("Reading NVS done...");
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
      last = rxState;
      rxState = chk_sim_cpin;
      #ifdef REAL_SENSORS
        //sds sensor
        sds.begin(&SDS_SERIAL, recv_from_sds, trans_to_sds);  // initialize SDS011 sensor
        //bme sensor
        bmeAddress = BME_ADDR;
      
        if (!bme.begin(bmeAddress, true)) 
        {
          Serial.println("Msg: Could not find a valid BME680 sensor, check wiring!");
          rxState = error_state;
          //while (1);
        }
        // Set up oversampling and filter initialization
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320*C for 150 ms

        // Start up the ds18b20 library
        //ds1820.begin();
      #endif
      //LEDS
      delay(1000);
      rgbSetColor(bColor);
      delay(1000);
      rgbSetColor(gColor);
      break;
    case try_init_again:
      last = rxState;
      r = readATResponse( "READY", "ERROR", 5*60000); // !!!IMPORTANT!!!
      if ((r ==1))
      {
        Serial.println("Init Ready Done!");
        //rxState = chk_sim_cpin;
        rxState = config_sensors;
      }
      else
      {
        Serial.println("Msg: Init2 Error!");
        Serial.println("Msg: Check PWR and RESET of the A9G or SIM Serivices");
        rgbSetColor(rColor);
        rxState = error_state;
      }
      //a9gPowerOnCheck();
      break;
    case chk_sim_cpin:
      last = rxState;
      r = sendATCommand("AT+CPIN?", "READY", "SIM", 2000);
      if (r == 1)
      {
        Serial.println("Msg: SIM PIN is ready!");
        rxState = set_network;
      }else if (r == 2)
       {Serial.println("Msg: SIM PIN is locked!"); rxState = error_state;}
      else
       {Serial.println("Msg: SIM PIN Timeout!");rxState = error_state;}
      break;
    case set_network:
      last = rxState;
      r = sendATCommand("AT+CREG=1", "OK", "ERROR", 60000);
      if (r == 1)
      {
        Serial.println("Msg: Netwoek Set!");
        rxState = chk_network_register;
      }else if (r == 2)
        {Serial.println("Msg: Network Setting Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Netwrok Setting Timeout!");rxState = error_state;}
      break;
    case chk_network_register:
      last = rxState;
      //r = sendATCommand("AT+CREG?", "+CREG: 1,1", "+CREG: 0,5", 2000); // !!!IMPORTANT!!!
      r = sendATCommand("AT+CREG?", "+CREG: 1", "+CREG: 0", 2000); // !!!IMPORTANT!!!
      if (r == 1)
      {
        Serial.println("Msg: Network Registered!");
        rxState = set_gps;
      }else if (r == 2)
        {Serial.println("Msg: Network Register Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Netwrok Register Timeout!");rxState = error_state;}
      break;
    case set_gps:
      last = rxState;
      r = sendATCommand("AT+GPS=1", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Set!");
        rxState = attach_ps;
      }else if (r == 2)
        {Serial.println("Msg: GPS Set Error!");rxState = error_state;}
      else
        {Serial.println("Msg: GPS Set Timeout!");rxState = error_state;}
      break;
    case attach_ps:
      last = rxState;
      r = sendATCommand("AT+CGATT=1", "OK", "ERROR", 60000);
      if (r == 1)
      {
        Serial.println("Msg: PS Attached!");
        rxState = check_context;
      }else
        {Serial.println("Msg: PS Attach Error or Timeout!"); rxState = attach_ps_again;}
      break;
    case attach_ps_again:
      last = rxState;
      r = sendATCommand("AT+CGATT=1", "OK", "NO RESPONSE!", 90000);      
      if (r == 1)
      {
        Serial.println("Msg: PS2 Attached!");
        rxState = check_context;
      }else if (r == 2)
        {Serial.println("Msg: PS Attach Error!"); rxState = error_state;}
      else
        {Serial.println("Msg: PS2 Attach Timeout!");rxState = error_state;}
      break;
    case check_context:
      last = rxState;
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
        {Serial.println("Msg: Context Set Timeout!");rxState = error_state;}
      break;
    case set_context:
      last = rxState;
      r = sendATCommand((char*)("AT+CGDCONT=1,\"IP\",\"" + apn +"\"").c_str(), "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context Changed!");
        rxState = select_context;
      }else if (r == 2)
        {Serial.println("Msg: Context Change Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Context Change Timeout!");rxState = error_state;}
      break;
    case select_context:
      last = rxState;
      r = sendATCommand("AT+CGACT=1,1", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context Selected!");
        rxState = show_context;
      }else if (r == 2)
        {Serial.println("Msg: Context Select Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Context Select Timeout!");rxState = error_state;}
      break;
    case show_context:
      last = rxState;
      r = sendATCommand("AT+CGACT?", "+CGACT: 1, 1", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Context is Correct!");
        rxState = chk_cip_status;
      }else if (r == 2)
        {Serial.println("Msg: Context Shown Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Context Shown Timeout!");rxState = error_state;}
      break;
    case chk_cip_status:
      last = rxState;
      r = sendATCommand("AT+CIPSTATUS", "INITIAL", "CLOSED", 6000);
      if (r == 1)
      {
        Serial.println("Msg: Status INITIAL!");
        rxState = set_cipstart;
      }else if (r == 2)
      {
        Serial.println("Msg: Status CLOSED!");
        rxState = set_cipstart;
      }else
        {Serial.println("Msg: Status Timeout!");rxState = chk_cip_status2;}
      break;
    case chk_cip_status2:
      last = rxState;
      r = sendATCommand("AT+CIPSTATUS", "CONNECT", "CLOSED", 6000);
      if (r == 1)
      {
        Serial.println("Msg: Status2 INITIAL!");
        rxState = set_cipstart;
      }else if (r == 2)
      {
        Serial.println("Msg: Status2 CLOSED!");
        rxState = set_cipstart;
      }else
        {Serial.println("Msg: Status2 Timeout!");rxState = error_state;}
      break;
    case set_cipstart:
      last = rxState;
      #ifdef THINGSPEAK
        r = sendATCommand((char*)("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port).c_str(), "CONNECT OK", "ALREAY CONNECT", 120000);//for thinkspeak
        if ((r == 1) or (r == 2))
        {
          Serial.println("Msg: Connected!");
          delay(100);
          rxState = set_gpsrd_read;
          
        }else
          {Serial.println("Msg: Connection Timeout!");rxState = set_cipstart2;}
      #else
        r = sendATCommand((char*)("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port).c_str(),"CONNECT OK", "ALREAY CONNECT", 20000);//for ngrok TCP tunneling
        if (r == 1)
        {
          Serial.println("Msg: Connected!");
          delay(100);
          rxState = set_gpsrd_read;
          
        }else if (r == 2)
        {
          Serial.println("Msg: Already Connection!");
          rxState = set_gpsrd_read;
        }else
          {Serial.println("Msg: Connection Timeout!");rxState = error_state;}
      #endif
      break;
    case set_cipstart2:
      last = rxState;
      r = sendATCommand((char*)("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port).c_str(), "CONNECT OK", "CONNECT FAIL", 120000);//for thinkspeak
      if (r == 1)
      {
        Serial.println("Msg: Connected!");
        delay(100);
        rxState = set_gpsrd_read;
        
      }else if (r == 2)
      {
        Serial.println("Msg: Connection in Process!");
        delay(10000);//Try again after some time
        sendATCommand("AT+CIPCLOSE", "OK", "ERROR", 20000);
        delay(10000);//Try again after some time
        rxState = set_cipstart2;
      }else
        {Serial.println("Msg: Connection2 Timeout!");rxState = error_state;}
      break;
    case set_gpsrd_read:
      last = rxState;
      //r = sendATCommand("AT+GPSRD=2", "OK", "ERROR", 2000);// Will also work but thingspeak needs more time
      r = sendATCommand("AT+GPSRD=5", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Read Set!");
        rxState = read_gps_msg;
      }else if (r == 2)
        {Serial.println("Msg: GPS Read Error!");rxState = error_state;}
      else
        {Serial.println("Msg: GPS Read Timeout!");rxState = error_state;}
      break;
    case read_gps_msg:
      last = rxState;
      MY_DBGln("Msg: Getting data from GPS using AT Cmd");
      gpsReadMessages();
      break;
    case read_gps_done:
      last = rxState;
      running = true;
      MY_DBGln("Msg: Getting data from GPS using AT done");
      rxState = set_gpsrd_off;
      break;
    case set_gpsrd_off:
      last = rxState;
      r = sendATCommand("AT+GPSRD=0", "OK", "ERROR", 2000);
      if (r == 1)
      {
        Serial.println("Msg: GPS Read Off!");
        rxState = sensor_data;
      }else if (r == 2)
        {Serial.println("Msg: GPS Off Error!");rxState = error_state;}
      else
        {Serial.println("Msg: GPS Off Timeout!");rxState = error_state;}
      break;
    case sensor_data:
      last = rxState;
      MY_DBGln("Msg: Getting data from Sensors");
      #ifdef REAL_SENSORS
        //sds sensor
        status_sds = sds.read(&pm25, &pm10);
        //status_sds = sds.dataQueryCmd(&pm10, &pm25 );
        //bme sensor
        if (! bme.performReading()) 
        {
          Serial.println("Msg: Failed to perform reading!");
          //rxState = error_state;
        }
        temp = bme.temperature;
        hum = bme.humidity;
        atm = bme.pressure / 100.0;

        //ds18b20
        /*delay(100);
        ds1820.requestTemperatures(); // Send the command to get temperatures
        tempC = ds1820.getTempCByIndex(0);
        // Check if reading was successful
        if(tempC != DEVICE_DISCONNECTED_C) 
        {
          Serial.print("Temperature is: ");
          Serial.println(tempC);
        } 
        else
        {
          Serial.println("Error: Could not read temperature data");
        }*/
      #else
        //debugging
        pm25 = random(20, 25);//debugging
        pm10 = random(30, 35);//debugging
        temp = random(2, 10);
        hum = random(10, 20);
        atm = pcktCntr;//bme.readPressure() / 100;
      #endif
      
      MY_DBGln("Msg: 1");
      #ifdef THINGSPEAK
        //channel: eScooter
        message = "GET https://api.thingspeak.com/update?api_key=AYKVFH212TKGNW2B&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(resetCntr) +"&field7="+String(errorState) +"&field8="+String(loc);//for thinkspeak
        //same message but errorState ==> tempC
        //message = "GET https://api.thingspeak.com/update?api_key=AYKVFH212TKGNW2B&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(resetCntr) +"&field7="+String(tempC) +"&field8="+String(loc);//for thinkspeak
        //channel: eScooter2 
        //message = "GET https://api.thingspeak.com/update?api_key=TJ85HJBF1XTV1GH7&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(resetCntr) +"&field7="+String(errorState) +"&field8="+String(loc);//for thinkspeak
      #else
        //message = "{ Id: \'A1\', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm++) + " , loc: \"" + loc + "\" }\n";//for ngrok TCP tunneling
        //message = "\'A1\'," + String(pm25) + "," + String(pm10) + "," + String(temp) + "," + String(hum) + "," + String(atm++) + ",\'" + loc + "\'\n";//for ngrok TCP tunneling for csv file
        message = String(pcktCntr) + "," + String(pm25) + "," + String(pm10) + "," + String(temp) + "," + String(hum) + "," + String(atm++) + "," + String(resetCntr) + "," + String(errorState) + ",\'" + loc + "\'\n";//for ngrok TCP tunneling for csv file
      #endif
      MY_DBGln("Msg: 2");
      rxState = set_cipsend;
      break;
    case set_cipsend:
      last = rxState;
      
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
        {Serial.println("Msg: Send Start Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Send Start Timeout!");rxState = error_state;}
      break;
    case set_cipsend_complete:
      last = rxState;
      errorState = 0;
      #ifdef THINGSPEAK
        r = sendATCommand((char*)message.c_str(), "ERROR", "ERROR", 2000);
        delay(300);
        r = sendATCommand(cntrlChar, "CLOSED", "ERROR", 60000);
      #else
        //
      #endif 
      if (r == 1)
      {
        Serial.println("Msg: Send Completed!");
        rgbSetColor(gColor);
        pcktCntr++;
        //if(pcktCntr==TOTAL_RECORDS)
        //{
        //  rxState=set_cipclose;
        //  pcktCntr = 0;
        //}
        //else
          rxState = chk_cipstatus_again;
      }else if (r == 2)
        {Serial.println("Msg: Send Complete Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Send Complete Timeout!");rxState = error_state;}
      //delay(3000);//with this from 0,3,6 improved to 0,2,4,6,8 cntr vales sending to thingspeak
      delay(5000);//with this from  0,2,4,6,8 improved to 0,1,2,3,4,5,6,8 cntr vales sending to thingspeak
      break;
    case chk_cipstatus_again:
      last = rxState;
      r = sendATCommand("AT+CIPSTATUS", "CONNECT", "CLOSED", 6000);
      rgbSetColor(noColor);
      if (r == 1)
      {
        Serial.println("Msg: Status CONNECT!");
        #ifdef THINGSPEAK
          rxState = set_network;
        #else
          rxState = set_cipstart;
        #endif 

      }else if (r == 2)
      {
        rxState = set_cipstart;
        //rxState = error_state;//reset_module;// debugging/testing reset_module manually 
        delay(5000);//with this from  0,2,4,6,8 improved to 0,1,2,3,4,5,6,8
      }else
        {Serial.println("Msg: Status Again Timeout!");rxState = error_state;}
      break;
    case set_cipclose:
      last = rxState;
      r = sendATCommand("AT+CIPCLOSE", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Connection Closed!");
        rxState = set_cipshut;
      }else if (r == 2)
        {Serial.println("Msg: Connection Close Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Connection Close Timeout!");rxState = error_state;}
      break;
     
    case set_cipshut:
      last = rxState;
      r = sendATCommand("AT+CIPSHUT", "OK", "ERROR", 20000);
      if (r == 1)
      {
        Serial.println("Msg: Connection Shut!");
        rxState = dummy_state;
      }else if (r == 2)
        {Serial.println("Msg: Connection Shut Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Connection Shut Timeout!");rxState = error_state;}
      break;
    case dummy_state:
      delay(1500);
      rxState = set_network;      
      break;
    case error_state:
      rgbSetColor(rColor);
      MY_DBG("\r\n*****Error No.");
      MY_DBGln(resetCntr);
      errorState = last;
      MY_DBG("\r\n*****Error State.");
      MY_DBGln(errorState);

      
      preferences.begin("my-app", false);
      key = items+String(resetCntr);
      preferences.putUInt(key.c_str(), errorState);
      // Store the counter to the Preferences
      preferences.putUInt("resetCntr", resetCntr);
      // Close the Preferences
      preferences.end();
  
      resetCntr++;
            
      delay(20000);//20s
      digitalWrite(LED1, 1);
      rxState=set_network;
      //try just from beginning for 3 times, if does not work then reset using Hardware pins
      if ((resetCntr % 3) == 0) rxState = reset_module;
      /*if (resetCntr < 10)
      {
        delay(5000);
        digitalWrite(LED1, 1);
        rxState=set_network;
      }
      else
      {
        resetCntr = 0;
        rxState = hang_state;
      }*/
      break;
    case hang_state:
      loopSerial();
      break;
    case reset_module:
      //resetModule();
      last = rxState;
      a9gReset();rxState = waiting_ready;
      /*r = sendATCommand("AT+RST=1", "OK", "ERROR", 6000);
      if (r == 1)
      {
        Serial.println("Msg: Module Reset!");
        delay(2000);
        rxState = waiting_ready;
      }else if (r == 2)
        {Serial.println("Msg: Module Reset Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Module Reset Timeout!");rxState = error_state;}
      */
      break;
    case waiting_ready:
      last = rxState;
      r = readATResponse( "READY", "ERROR", 5*60000); // !!!IMPORTANT!!!
      if (r == 1)
      {
        a9gPowerOnCheck();
        Serial.println("Msg: Module READY!");
        delay(2000);
        rxState = set_network;
      }else if (r == 2)
        {Serial.println("Msg: Waiting READY Error!");rxState = error_state;}
      else
        {Serial.println("Msg: Waiting READY Timeout!");rxState = error_state;}
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
  loopSerial();
}