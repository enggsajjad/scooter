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
#define TIMEOUT 3

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

byte command(String cmd, String response, unsigned long timeout, boolean output) 
{
  while(gprsSerial.available() > 0) {
    gprsSerial.read(); //clear data
  }
  gprsSerial.println(cmd);

  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) {
    String result = gprsSerial.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) {
      Serial.println("Received Data: ");
      Serial.println(result);
    }
    

    if (result.indexOf(response) > 0) {
      Serial.println("Command Executed: " + cmd + " - " + response);
      return OK;
    }
  }

  Serial.println("Command Failed: " + cmd);

  return NOTOK;
}
