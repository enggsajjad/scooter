/**
 * \file thinker.h
 *
 * \brief variable and function definitions for timings
 *
 * This is the files that contains the variables, macros, pins definitions and functions for timing related routines.
 * 
 * \date 07.09.2020
 */

/// OK response for AT Command
#define OK 1
/// Other than OK response for AT Command
#define NOTOK 2
/// Timeout for no response for AT Command
#define TIMED 3
/// Error for no response for AT Command
#define ERR 4
/// Define the AiThinker Initialization End Detection String
String initString = "READY";


/**************************************************************************/
/*!
    @brief  just to loop back the GPRS to USB Serial
    @returns void
*/
/**************************************************************************/
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
/**************************************************************************/
/*!
    @brief  Check the AT Response after the power up
    @returns void
*/
/**************************************************************************/
void a9gPowerOnCheck(void)
{
  uint8_t resp=0;//response of AT
    
  // checks if the module is started
  resp = sendATCommand("AT", "OK", "ERROR", 1000);
  if (resp == 0)
  {
    // power on pulse
    digitalWrite(gprsPWR,LOW);
    delay(3000);
    digitalWrite(gprsPWR,HIGH);
    
    // waits for an answer from the module
    while(resp == 0)     // Send AT every two seconds and wait for the answer
    {    
      resp = sendATCommand("AT", "OK", "ERROR", 1000);    
    }
  }  
}

/**************************************************************************/
/*!
    @brief  Routine to send AT Command and output the status
    @param cmd the AT Command to be sent
    @param response1 the response to be expected
    @param response2 the 2nd response to be expected
    @param timeout the timeout to wait for cmd command
    @returns byte the errors
*/
/**************************************************************************/

int8_t sendATCommand(char* cmd, char* expected_resp1, char* expected_resp2, unsigned int timeout)
{
  
  uint8_t answer=0;
  int  x=0;
  char resp[300];
  unsigned long previous;
  
  memset(resp, '\0', 300);    // Initialize the string
  
  delay(100);
  
  while( gsmSerial.available() > 0) gsmSerial.read();    // Clean the input buffer
  
  gsmSerial.println(cmd);    // Send the AT command 
  
  previous = millis();
  
  // this loop waits for the answer
  do
  {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(gsmSerial.available() != 0)
    {    
      resp[x] = gsmSerial.read();
      x++;
      // check if the desired answer 1  is in the resp of the module
      if (strstr(resp, expected_resp1) != NULL)    
      {
          answer = 1;
      }
      // check if the desired answer 2 is in the resp of the module
      else if (strstr(resp, expected_resp2) != NULL)    
      {
          answer = 2;
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    
  MY_DBGln("Resp:");
  MY_DBGln(resp);
  MY_DBGln("----");

  return answer;
}
/**************************************************************************/
/*!
    @brief  Routine to wait for AT Command response
    @param response1 the response to be expected
    @param response2 the 2nd response to be expected "yy" if not needed
    @param timeout the timeout to wait for cmd command
    @returns byte error code
*/
/**************************************************************************/
int8_t readATResponse( char* expected_resp1, char* expected_resp2, unsigned int timeout)
{
  
  uint8_t answer=0;
  int  x=0;
  char resp[300];
  unsigned long previous;
  
  memset(resp, '\0', 300);    // Initialize the string
  
  delay(100);
  
  while( gsmSerial.available() > 0) gsmSerial.read();    // Clean the input buffer
  
  //gsmSerial.println(cmd);    // Send the AT command 
  
  x = 0;
  previous = millis();
  
  // this loop waits for the answer
  do
  {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(gsmSerial.available() != 0)
    {    
        resp[x] = gsmSerial.read();
        x++;
        // check if the desired answer 1  is in the resp of the module
        if (strstr(resp, expected_resp1) != NULL)    
        {
            answer = 1;
        }
        // check if the desired answer 2 is in the resp of the module
        else if (strstr(resp, expected_resp2) != NULL)    
        {
            answer = 2;
        }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    
  MY_DBGln("Resp:");
  MY_DBGln(resp);
  MY_DBGln("----");
  return answer;
} 

void gpsReadMessages(void)
{
        
  while(running)
  {
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
          msg = msg + "\n" + result;
          if (result.indexOf("$GNVTG") > 0) 
          {
            cntr = 0;
            MY_DBGln("Resp:");
            MY_DBGln(msg);
            MY_DBGln("---");
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
}
