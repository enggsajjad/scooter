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
    @brief  Routine to send AT Command and output the status
    @param cmd the AT Command to be sent
    @param response the response to be expected
    @param timeout the timeout to wait for cmd command
    @param output the boolean whether to show output
    @returns byte
*/
/**************************************************************************/
/*
byte command(String cmd, String response, unsigned long timeout, boolean output) 
{
  while(gsmSerial.available() > 0) {
    gsmSerial.read(); //clear data
  }
  gsmSerial.println(cmd+"\r\n");
  delay(100);
  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) 
  {
    String result = gsmSerial.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) 
    {
      MY_DBGln("Received Data: ");
      MY_DBGln(result);
    }
    

    if (result.indexOf(response) > 0) 
    {
      MY_DBGln("Command Executed: " + cmd + " - " + response);
      return OK;
    }
  }

  MY_DBGln("Command Failed: " + cmd);

  return NOTOK;
}
*/

/**************************************************************************/
/*!
    @brief  Routine to send AT Command and wait for OK
    @returns void
*/
/**************************************************************************/
/*
void gsmWaitForReady()
{
  for( int i=0; i<25; i++)
  {
    char buffer[3];
    char index = 0;

    gsmSerial.flush();
    delay(1000);
    MY_DBG('.');
    gsmSerial.print("AT\r");
    gsmSerial.flush();
    delay(100);

    while( gsmSerial.available())
    {
      char c = gsmSerial.read();

      if ( c == '\r' || c == '\n' )
      {
        if ( buffer[0] == 'O' && buffer[1] == 'K' && index == 2 )
        {
          MY_DBG('\n');
          return;
        }

        index = 0;
      }
      else
      if( index < 3 )
      {
        buffer[index++] = c;
      }
    }
  }
  
}
*/
/**************************************************************************/
/*!
    @brief  Routine to read GSM data
    @returns String 
*/
/**************************************************************************/

String gsmRead() 
{
  String reply = "";
  if (gsmSerial.available())  
  {
    reply = gsmSerial.readString();
  }
  return reply;
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

byte gsmWaitFor(String response1, String response2, int timeout) 
{
  unsigned long entry = millis();

  while(gsmSerial.available() > 0) 
  {
    gsmSerial.read(); //clear data
  }
  
  String reply = gsmRead();
  byte retVal = 99;
  do 
  {
    reply = gsmRead();
    if (reply != "") 
    {
      MY_DBG("Get @");
      MY_DBG((millis() - entry));
      MY_DBG("ms ");
      MY_DBG(reply);
    }
  } while ((reply.indexOf(response1) + reply.indexOf(response2) == -2) && millis() - entry < timeout );
  if ((millis() - entry) >= timeout) 
  {
    retVal = TIMED;
  } else 
  {
    if (reply.indexOf(response1) + reply.indexOf(response2) > -2) retVal = OK;
    else if (reply.indexOf("ERROR") > -1) retVal = ERR;
    else retVal = NOTOK;
  }

  return retVal;
}


/**************************************************************************/
/*!
    @brief  Routine to send AT Command and output the status
    @param cmd the AT Command to be sent
    @param response1 the response to be expected
    @param response2 the 2nd response to be expected
    @param timeout the timeout to wait for cmd command
    @param repetitions the counts to repeat the said command if timeout
    @returns byte the errors
*/
/**************************************************************************/

byte gsmCommand(String cmd, String response1, String response2, int timeout, int repetitions) {
  byte returnValue = NOTOK;
  byte count = 0;
  while (count < repetitions && returnValue != OK) 
  {
    gsmSerial.print(cmd+"\r\n");
    MY_DBG("Cmd: ");
    MY_DBGln(cmd);
    if (gsmWaitFor(response1, response2, timeout) == OK) 
    {
      returnValue = OK;
    } 
    else 
      returnValue = NOTOK;
    count++;
  }
  return returnValue;
}

/**************************************************************************/
/*!
    @brief  Routine to wait for command response in string, basically for AT+CIPSTATUS?
    @param timeout the timeout to wait for cmd command
    @returns String indicating the commands response
*/
/**************************************************************************/

String gsmWaitForResponse( int timeout) 
{
  unsigned long entry = millis();
  String retVal = "";
  
  while(gsmSerial.available() > 0) 
  {
    gsmSerial.read(); //clear data
  }
  
  String reply = gsmRead();

  do 
  {
    reply = gsmRead();
    if (reply != "") 
    {
      MY_DBG("Get @");
      MY_DBG((millis() - entry));
      MY_DBG("ms ");
      MY_DBG(reply);
    }
  } while ((reply.indexOf("OK") + reply.indexOf("yy") == -2) && millis() - entry < timeout );
  if ((millis() - entry) >= timeout) 
  {
    retVal = "TIMED";
  } else 
  {
    int st=reply.indexOf("STATE:")+6;
    int en=reply.indexOf("\r\n",st)-1;
    retVal = reply.substring(st,en);
    MY_DBGln("Start: "+String(st)+" End: "+String(en)+" Res: "+String(retVal));
  }

  return retVal;
}

/**************************************************************************/
/*!
    @brief  Routine to send AT Command and output the status
    @param cmd the AT Command to be sent
    @param timeout the timeout to wait for cmd command
    @returns char the errors number to indicate different responses
*/
/**************************************************************************/

char gsmCheckStatus(String cmd, int timeout) 
{
  String rv = "NOTOK";
  gsmSerial.print(cmd+"\r\n");
  MY_DBG("Cmd: ");
  MY_DBGln(cmd);
   
  rv = gsmWaitForResponse(timeout);
  //MY_DBGln("Res: "+String(rv)+" len: "+String(rv.length()));
  if (rv == "IP INITIAL")//"IP INITIAL")
    return 1;
  else if (rv == "START")//"IP START")
    return 2;
  else if (rv == "GPRSACT")//"IP GPRSACT")
    return 3;
  else if (rv == "CONNECT OK")//"CONNECT OK")
    return 4;
  else if (rv == "TCP/UDP CONNECTING")//"TCP/UDP CONNECTING")
    return 4;
  else if (rv == "CLOSED")//"IP CLOSE")
    return 5;
}


/**************************************************************************/
/*!
    @brief  Routine to send AT Command and output the status
    @param command the AT Command to be sent
    @param timeout the timeout to wait for cmd command
    @param debug the debuging option
    @returns byte the errors
*/
/**************************************************************************/

String gsmSimpleSendAndRead(String command, const int timeout, boolean debug)
{
    String response = "";
    gsmSerial.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (gsmSerial.available())
        {
            char c = gsmSerial.read();
            response += c;
        }
    }
    if (debug)
    {
        MY_DBGln(response);
    }
    return response;
}
/**************************************************************************/
/*!
    @brief  Routine to get AT Command output
    @param timeout the timeout to wait for cmd command
    @param debug the debuging option
    @returns byte the errors
*/
/**************************************************************************/

String gsmSimpleRead( const int timeout, boolean debug)
{
    String response = "";
    
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (gsmSerial.available())
        {
            char c = gsmSerial.read();
            response += c;
        }
    }
    if (debug)
    {
        MY_DBGln(response);
    }
    return response;
}
/**************************************************************************/
/*!
    @brief  Routine to get AT Command output working
    @returns state of the command
*/
/**************************************************************************/
bool gsmCheckATCommunication()
{
    int i = 0;
    bool state=false;
    for (i = 0; i < 10; i++)
    {
        String msg3 = String("");
        msg3 = gsmSimpleSendAndRead("AT", 1000, true);
        if (msg3.indexOf("OK") >= 0)
        {
            usbSerial.println("A9/A9G Module AT working.");
            state=true;
            return state;
        }
        delay(500);
    }
    return state;
}
/**************************************************************************/
/*!
    @brief  Routine to get initial messages 
    @returns state of the command
*/
/**************************************************************************/
bool gsmCheckInitialization()
{
    int i = 0;
    bool state=false;
    for (i = 0; i < 10; i++)
    {
        String msg3 = String("");
        msg3 = gsmSimpleRead( 1000, true);
        if (msg3.indexOf(initString) >= 0)
        {
            usbSerial.println("A9/A9G Module READY.");
            state=true;
            return state;
        }
        delay(500);
    }
    return state;
}
