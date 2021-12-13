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

#define THINGSPEAK 


#include "main.h"

#define TOTAL_RECORDS 10000//10

unsigned long previousMillis = 0; 
int ii=0;

bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
Adafruit_BME680 bme(&Wire1);

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

  /*while (!Serial)
  {
      ; // wait for serial port to connect
  }*/
  // initialize digital pin LED as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(gprsPWR, OUTPUT);
  //digitalWrite(gprsPWR, HIGH);
  
  digitalWrite(gprsPWR, LOW);
  delay(1000);
  rgbSetColor(noColor);
  
  Serial.print("Start");

  //delay(1000);
  //digitalWrite(gprsPWR, LOW);
  delay(2000);
  digitalWrite(gprsPWR, HIGH);
  Serial.println("...");

  char r1 = sendATcommand3( "+CREG: 2", "ERROR", 60000);
  char r2 = sendATcommand3( "READY", "ERROR", 60000);
  if ( (r1==0) and (r2 ==0))
  {
    Serial.println("Init Timeout!");
    Serial.println("Check PWR and RESET of the A9G");
  }
  else
  if ( (r1==1) and (r2 ==1))
  {
    Serial.println("Init Ready Done!");
  }
  else
  {
    Serial.println("Init Error!");
    Serial.println("Check PWR and RESET of the A9G or SIM Serivices");
  }
    power_on();
  /*
  ModuleState=gsmCheckInitialization();
  if(ModuleState==false)//if it's off, turn on it.
  {
    Serial.println("Err: Now turnning the A9/A9G READY.");
    digitalWrite(gprsPWR, LOW);
    delay(3000);
    digitalWrite(gprsPWR, HIGH);
    ModuleState=gsmCheckInitialization();
    Serial.println("Err: A9G not initialized, reset again!");
    rgbSetColor(rColor);
    while(1);
  }

  ModuleState=gsmCheckATCommunication();
  if(ModuleState==false)//if it's off, turn on it.
  {
    //Serial.println("Now turnning the A9/A9G on.");
  }
  */
  sds.begin(&SDS_SERIAL, recv_from_sds, trans_to_sds);  // initialize SDS011 sensor

  bmeAddress = BME_ADDR;

  if (!bme.begin(bmeAddress, true)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    //while (1);
  }
  
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  
  rxState = set_gprsd_off;
  
  
  delay(1000);
  rgbSetColor(bColor);
  delay(1000);
  rgbSetColor(gColor);
  /////////////


    
    //delay(3000);
    
    //sets the PIN code
    //snprintf(aux_str, sizeof(aux_str), "AT+CPIN=%s", pin);
    //sendATcommand(aux_str, "OK", 2000);
    sendATcommand2("AT+CPIN?", "READY", "SIM", 2000);
    
    delay(3000);
    
    Serial.println("Connecting to the network...");

    if( sendATcommand2("AT+CREG?", "+CREG: 1,1", "+CREG: 0,5", 60000)== 1 )
    {
      Serial.println("Network Registered!");
      if( sendATcommand2("AT+CGATT=1", "+CGATT:1", "OK", 60000)== 1 )
      {
        Serial.println("CGATT donoe!");
        if( sendATcommand2("AT+CGDCONT?", "OK", "ERROR", 5000)== 1 )
        {
          Serial.println("CGDCONT? donoe!");
          if( sendATcommand2("AT+CGDCONT=1,\"IP\",\"pinternet.interkom.de\"", "OK", "ERROR", 8000)== 1 )
          {
            Serial.println("CGDCONT=1 donoe!");
            if( sendATcommand2("AT+CGDCONT?", "\"pinternet.interkom.de\"", "OK", 5000)== 1 )
            {
              Serial.println("CGDCONT? donoe!");
              if( sendATcommand2("AT+CGACT=1,1", "OK", "ERROR", 60000)== 1 )
              {
                Serial.println("AT+CGACT=1,1 donoe!");
              }
            }
          }
        }
      }
    }

    
    
    
      

}

/**************************************************************************/
/*!
    @brief  reading the Microphone with timer function
    @returns void
*/
/**************************************************************************/
void loop(){
    
 
    // Selects Single-connection mode
    if (sendATcommand2("AT+CIPMUX=0", "OK", "ERROR", 2000) == 1)
    {
        // Waits for status IP INITIAL
        while(sendATcommand2("AT+CIPSTATUS", "INITIAL", "CLOSED", 2000)  == 0 );
        delay(5000);
        
                    Serial.println("Opening TCP");

                    //snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",%d", IP_address, 80);
                    message = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
                    // Opens a TCP socket
                    //if (sendATcommand2(aux_str, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                    if (sendATcommand2((char*)message.c_str(), "CONNECT OK", "CONNECT FAIL", 30000) == 1)
                    {
                        Serial.println("Connected");
                        pm25 = random(20, 25);//debugging
                        pm10 = random(30, 35);//debugging
                        temp = random(2, 10);
                        hum = random(10, 20);
                        atm = random(13, 20);//bme.readPressure() / 100;
                        loc = "Test Locations";
                        message = "GET https://api.thingspeak.com/update?api_key=AYKVFH212TKGNW2B&field1=" + String(temp) +"&field2="+String(hum) +"&field3="+String(atm) +"&field4="+String(pm25) +"&field5="+String(pm10) +"&field6="+String(loc);//for thinkspeak
                        //message.toCharArray(ip_data, 200);
    
                        // Sends some data to the TCP socket
                        //sprintf(aux_str,"AT+CIPSEND=%d", strlen(ip_data));

                        char aux_str[10];
                        aux_str[0] = 26;

                        //if (sendATcommand2(aux_str, ">", "ERROR", 10000) == 1)    
                        if (sendATcommand2("AT+CIPSEND", ">", "ERROR", 10000) == 1)    
                        {
                            Serial.println("Sending...");
                            //gsmSerial.println(message);
                            sendATcommand2((char*)message.c_str(), "ERROR", "ERROR", 500); //== 0)
                            //sendATcommand2(ip_data, "+CIPRCV", "ERROR", 10000);
                           if( sendATcommand2(aux_str, "CLOSED", "ERROR", 10000) == 1)
                           {
                            Serial.println("Sent!");
                            
                            }

                        }
                        
                        // Closes the socket
                        Serial.println("Closing...");
                        sendATcommand2("AT+CIPCLOSE", "CLOSE", "ERROR", 10000);
                    }
                    else
                    {
                        Serial.println("Error opening the connection");
                    }  

    }
    else
    {
        Serial.println("Error setting the single connection");
    }
    Serial.println("Shutting...");
    sendATcommand2("AT+CIPSHUT", "OK", "ERROR", 10000);
    delay(1000);
}

void loop2()
{
  switch(rxState)
  {
    case set_gprsd_off:
      r = gsmCommand("AT+GPSRD=0", "OK", "yy", 10000, 1);
      if (r == OK) rxState = set_gps_on;
      else rxState = error_state;
      break;
    case set_gps_on:
      r = gsmCommand("AT+GPS=1", "OK", "yy", 10000, 1);
      if (r == OK) rxState = chk_creg;
      else rxState = error_state;
      break;
    case chk_creg:
      r = gsmCommand("AT+CREG?", "OK", "yy", 20000, 1);
      if (r == OK) rxState = chk_cgact;
      else rxState = error_state;
      break;
    case chk_cgact:
      r = gsmCommand("AT+CGACT?", "OK", "yy", 20000, 1);
      if (r == OK) rxState = set_cgatt_on;
      else rxState = error_state;
      break;
    case set_cgatt_on:
      r = gsmCommand("AT+CGATT=1", "OK", "yy", 15000, 1);
      delay(1000);
      if (r == OK) rxState = chk_cgdcont;
      else rxState = error_state;
      break;
    case chk_cgdcont:
      r = gsmCommand("AT+CGDCONT?", "pinternet.interkom.de", "yy", 4000, 1);
      delay(1000);
      if (r == OK) rxState = set_cgact_on;
      else rxState = set_cgdcont_on;
      break;
    case set_cgdcont_on:
      r = gsmCommand("AT+CGDCONT=1,\"IP\",\"pinternet.interkom.de\"", "OK", "yy", 20000, 1);
      delay(1000);
      if (r == OK) rxState = set_cgact_on;
      else rxState = error_state;
      break;
    case set_cgact_on:
      r = gsmCommand("AT+CGACT=1,1", "OK", "yy", 55000, 1);
      delay(1000);
      if (r == OK) rxState = chk_cgact_again;
      else rxState = error_state;
      break;
    case chk_cgact_again:
      r = gsmCommand("AT+CGACT?", "OK", "yy", 20000, 1);
      delay(1000);
      if (r == OK) rxState = chk_cipstatus;
      else rxState = error_state;
      break;
    case chk_cipstatus:
      
      resp = gsmCheckStatus("AT+CIPSTATUS?",15500);
      switch(resp)
      {
        case 1:
          Serial.println("Status: INITIAL");
          rxState = set_cipstart;
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
          rxState = set_gpsrd_read;
          break;
        case 5:
          Serial.println("Status: CLOSE");
          rxState = set_cipstart;
          break;
        case 6:
          Serial.println("Status: PROCESSING");
          rxState = set_gpsrd_read;
          break;
      }
      delay(3000);
      break;
    case set_cipstart:
      #ifdef THINGSPEAK
        r = gsmCommand("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80", "CONNECT OK", "yy", 15000, 1);//for thinkspeak
      #else
        //r = gsmCommand("AT+CIPSTART=\"TCP\",\"" + host + "\"," + port, "CONNECT OK", "yy", 4000, 1);//for ngrok TCP tunneling
      #endif
      delay(1000);
      if (r == OK) rxState = set_gpsrd_read;//chk_cipstatus;
      else rxState = error_state;
      break;
    case set_gpsrd_read:
      r = gsmCommand("AT+GPSRD=5", "OK", "yy", 20000, 1);
      delay(1000);
      if (r == OK) rxState=read_gps_msg;
      else rxState = error_state;
      break;
    case read_gps_msg:
      MY_DBGln("Getting data from GPS using AT Cmd");
      
      while(running){
        if ( gsmSerial.available()) {
          result = gsmSerial.readStringUntil('\r');
          //Serial.println(result);
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
                Serial.println("-----------");
                Serial.println(msg);
                Serial.println("-----------");
                running = false;
                //Converting to readables
                /*msg1 = msg.c_str();
                Serial.println("..-------------------");
                Serial.println(msg1);
                Serial.println("..-------------------");
                while (*msg1)
                if(tiny.encode(*msg1++))
                  displayInfo();*/
                //getting location string
                loc = msg.substring(msg.indexOf("GNGGA")-1, msg.indexOf(",*")+4);
                msg = "";
                //Serial.println("-----------");
                //Serial.println(loc);
                //Serial.println("-----------");
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
      r = gsmCommand("AT+GPSRD=0", "OK", "yy", 2000, 1);
      delay(500);
      rxState=sensor_data;
      //if (r == OK) rxState=sensor_data;
      //else rxState = error_state;
      break;      
    case sensor_data:
      MY_DBGln("Getting data from Sensors");
      status_sds = sds.read(&pm25, &pm10);
      //debugging
      //pm25 = random(20, 25);//debugging
      //pm10 = random(30, 35);//debugging
      //status_sds = sds.dataQueryCmd(&pm10, &pm25 );
      // put your main code here, to run repeatedly:
      if (! bme.performReading()) {
        Serial.println("Failed to perform reading :(");
      }
      temp = bme.temperature;
      hum = bme.humidity;
      atm = bme.pressure / 100.0;
      //debugging
      //temp = random(2, 10);
      //hum = random(10, 20);
      //atm = cntr1;//bme.readPressure() / 100;
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
        r = gsmCommand("AT+CIPSEND", ">", "yy", 20000, 1);
        delay(100);
        gsmSerial.println(message);
        delay(100);
        gsmSerial.write(26);
        Serial.print("\r\n");
        //r = gsmCommand("\r\n", "OK", "yy", 20000, 1);
        r = gsmCommand("\r\n", "CLOSED", "yy", 20000, 1);
        
      #else
        //r = gsmCommand("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", "yy", 10000, 1);//for ngrok TCP tunneling
      #endif      
      rgbSetColor(gColor);

      delay(3000);//delay(6000);
      if (r == OK)
      {
        digitalWrite(LED2, led);
        led = ~led;
        cntr1++;
        if(cntr1==TOTAL_RECORDS)
        {
          rxState=set_cipclose;
          cntr1 = 0;
        }
        else
          rxState = chk_cipstatus;
      }
      else
      {
        rxState = error_state;
      }
      rgbSetColor(noColor);
      break;
      
    case set_cipclose:
      r = gsmCommand("AT+CIPCLOSE", "OK", "yy", 16000, 1);
      if (r == OK) rxState=chk_cipstatus_again;
      else rxState = error_state;
      break;
    case chk_cipstatus_again:
      resp = gsmCheckStatus("AT+CIPSTATUS?",15500);
      switch(resp)
      {
        case 5:
          Serial.println("Status: CLOSE");
          rxState = set_cipshut;
          break;
      }
      delay(1500);
      break;
    case set_cipshut:
      r = gsmCommand("AT+CIPSHUT", "OK", "yy", 16000, 1);
      if (r == OK) rxState=dummy_state;
      else rxState = error_state;
      break;
    case dummy_state:
      delay(1500);
      rxState=set_gprsd_off;      
      break;
    case error_state:
      rgbSetColor(rColor);
      MY_DBG("\r\n*****Error No.");
      MY_DBGln(resetCntr);
      resetCntr++;
      if (resetCntr < 5)
      {
        delay(5000);
        digitalWrite(LED1, 1);
        rxState=set_gprsd_off;
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
  loopSerial();
}

void loopSerial(void)
{

  if (Serial.available())
  {
    gsmSerial.write(Serial.read());
  }
  if (gsmSerial.available())
  {
    Serial.write(gsmSerial.read());
  }
  //Uncomment the following if you want to get the A9G GPS direct data
  /*if (gpsSerial.available())
  {
    Serial.write(gpsSerial.read());
  }*/

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;
    ii++;
    if (ii==8) ii =0;
    digitalWrite(LED1, ii&1);
    digitalWrite(LED2, (ii>>1)&1);
    digitalWrite(LED3, (ii>>2)&1);
  }
}

void power_on(){

    uint8_t answer=0;
    
    // checks if the module is started
    answer = sendATcommand2("AT", "OK", "ERROR", 1000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(gprsPWR,LOW);
        delay(3000);
        digitalWrite(gprsPWR,HIGH);
    
        // waits for an answer from the module
        while(answer == 0){     // Send AT every two seconds and wait for the answer
            answer = sendATcommand2("AT", "OK", "ERROR", 1000);    
        }
    }
    
}

int8_t sendATcommand2(char* ATcommand, char* expected_answer1, 
        char* expected_answer2, unsigned int timeout){

    uint8_t answer=0;
    int  x=0;
    char resp[300];
    unsigned long previous;

    memset(resp, '\0', 300);    // Initialize the string

    delay(100);

    while( gsmSerial.available() > 0) gsmSerial.read();    // Clean the input buffer

    gsmSerial.println(ATcommand);    // Send the AT command 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(gsmSerial.available() != 0){    
            resp[x] = gsmSerial.read();
            x++;
            // check if the desired answer 1  is in the resp of the module
            if (strstr(resp, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the resp of the module
            else if (strstr(resp, expected_answer2) != NULL)    
            {
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    
    Serial.println("Resp:");
    Serial.println(resp);
    Serial.println("----");

    return answer;
}

int8_t sendATcommand3( char* expected_answer1, 
        char* expected_answer2, unsigned int timeout){

    uint8_t answer=0;
    int  x=0;
    char resp[300];
    unsigned long previous;

    memset(resp, '\0', 300);    // Initialize the string

    delay(100);

    while( gsmSerial.available() > 0) gsmSerial.read();    // Clean the input buffer

    //gsmSerial.println(ATcommand);    // Send the AT command 

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if(gsmSerial.available() != 0){    
            resp[x] = gsmSerial.read();
            x++;
            // check if the desired answer 1  is in the resp of the module
            if (strstr(resp, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the resp of the module
            else if (strstr(resp, expected_answer2) != NULL)    
            {
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    
    Serial.println("Resp:");
    Serial.println(resp);
    Serial.println("----");
    return answer;
}   
