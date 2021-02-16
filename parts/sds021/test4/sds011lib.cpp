//! ESP32 C/C++ Arduino library for the Nova Fitness sds011 PM sensor (implementation)

/// @file sds011lib.cpp 
/// @author Sajjad Hussain
/// @version 0.1

#include "sds011lib.h"

/**
 * @mainpage 
 * @section Description
 *
 * ESP32 C/C++ Arduino library for the Nova Fitness sds011 PM sensor 
 *
 * @author Sajjad Hussain
 * @version 0.1
 *
 * @section License
 *
 * Copyright 2020 Sajjad Hussain
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @see https://opensource.org/licenses/MIT
 *
 * @section Specs
 *
 * Relevant specs are included in the documentation of the individual methods. The full and detailed specification can be found in the Nova Fitness Co. Ltd. sds011 data sheet, V1.3 and the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
 * 
 * @see https://web.archive.org/web/20200525083221/https://www-sd-nf.oss-cn-beijing.aliyuncs.com/%E5%AE%98%E7%BD%91%E4%B8%8B%E8%BD%BD/sds011%20laser%20PM2.5%20sensor%20specification-V1.4.pdf
 * @see https://www.arduinoforum.de/attachment.php?aid=3023
 */


/**************************************************************************/
/*!
    @brief  constructor for the class
*/
/**************************************************************************/
sds011::sds011(void) {

}
/**************************************************************************/
/*!
    @brief function to communicate to sensor by sending a command and fetch the response into a buffer.
    @param command one byte of the command to be sent
    @param option_1 first parameter of the command, depends on different positions in the command array
    @param option_2 second parameter of the command, depends on different positions in the command array
    @param id_1 the id_lsb where commands to be send
    @param id_2 the id_msb where commands to be send
    @param reply ten bytes of the command response
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::sdsCommunicate( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2, uint8_t reply[10]  )
{
  bool status=false;
  uint8_t i, lc, wait = MAX_WAIT;
  for(uint8_t lc = 0; lc < 10 && status == false; ++lc)
  {
    i = sendCommand(  command,  option_1,   option_2,  id_1,  id_2 );
    
    if( i >= wait )
    {
      if( _debug)Serial.println("data unavailable, exiting...");
      i = 0;
      status = false;
      if ( (command == CMD_SLEEP_AND_WORK && option_1 == WRITE_MODE && option_2 == WORK_MODE) ) { 
        // wake from sleep.
        break;
      }
    }
    status = getResponse( command, reply );
  }
  return( status );
}

/// Sends command to the sensor. 
/// 
/// Specification from the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
/// 
/// | Byte  |  Name           ||  Reporting Mode     |  Query Data         |  Set ID             |  Manual Sleep / Work| Get Firmware Version| Set Working Period  |
/// | :---: | :-------------- || :------------------ | :------------------ | :------------------ | :------------------ | :------------------ | :------------------ |
/// |  0    |  Message header || AA                  | AA                  | AA                  | AA                  | AA                  | AA                  |
/// |  1    |  Command ID     || B4                  | B4                  | B4                  | B4                  | B4                  | B4                  |
/// |  2    |  DATA 1         || 2                   | 4                   | 5                   | 6                   | 7                   | 8                   |
/// |  3    |  DATA 2         || 0: query / 1: set   | 0                   | 0                   | 0: query / 1: set   | 0                   | 0: query / 1: set   |
/// |  4    |  DATA 3         || 0: auto / 1: query  | 0                   | 0                   | 0: sleep / 1: work  | 0                   | 0: continuous / 1-30: minutes sleep |
/// |  5    |  DATA 4         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  6    |  DATA 5         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  7    |  DATA 6         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  8    |  DATA 7         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// |  9    |  DATA 8         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 10    |  DATA 9         || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 11    |  DATA 10        || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 12    |  DATA 11        || 0                   | 0                   | 0                   | 0                   | 0                   | 0                   |
/// | 13    |  DATA 12        || 0                   | 0                   | New ID Byte 1       | 0                   | 0                   | 0                   |
/// | 14    |  DATA 13        || 0                   | 0                   | New ID Byte 2       | 0                   | 0                   | 0                   |
/// | 15    |  DATA 14        || ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) | ID Byte 1 (FF: all) |
/// | 16    |  DATA 15        || ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) | ID Byte 2 (FF: all) |
/// | 17    |  Check-sum      || checksum            | checksum            | checksum            | checksum            | checksum            | checksum            |
/// | 18    |  Message tail   || AB                  | AB                  | AB                  | AB                  | AB                  | AB                  |
///
/// | Calculation  ||
/// | :----------  | :-------------------------------------------------------------|
/// | Check-sum:   | Check-sum = DATA1 + DATA2 + ... + DATA15                      |
///
/// @see https://www.arduinoforum.de/attachment.php?aid=3023
///
/**************************************************************************/
/*!
    @brief function to send the command to sensor and fetch the response into a buffer.
    When no reply is received, usually this is because device was just reporting.
    This happens when device is in reporting mode, as then the device spits out a
    a continuous stream of data. In such cases. command has to be sent again to get an answer.    
    @param command one byte of the command to be sent
    @param option_1 first parameter of the command, depends on different positions in the command array
    @param option_2 second parameter of the command, depends on different positions in the command array
    @param id_1 the id_lsb where commands to be send
    @param id_2 the id_msb where commands to be send
    @returns status tells the seccessful execution
*/
/**************************************************************************/
uint8_t sds011::sendCommand( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2 )
{
  uint8_t wait=MAX_WAIT,i;
  //bool status=false;  
  if( _debug)Serial.println("Sending command...       ");
  
  uint8_t checksum = command + option_1 + option_2 + id_1 + id_2;
  _uart->write((uint8_t)MSG_HEAD);
  _uart->write((uint8_t)CMD_WRITE_MODE);
  _uart->write(command);
  if ((command == (uint8_t)CMD_REPORTING_MODE) || (command == (uint8_t)CMD_SLEEP_AND_WORK) || (command == (uint8_t)CMD_WORKING_PERIOD)) 
  {
    _uart->write(option_1); 
    _uart->write(option_2);
  }
  for ( i = 0; i < 10; i++) 
  {
    _uart->write((uint8_t)MSG_RESERVED);
  }
  if (command == (uint8_t)CMD_SET_DEVICE_ID) 
  {
    _uart->write(option_1); 
    _uart->write(option_2);
  }else if ((command == (uint8_t)CMD_QUERY_DATA) || (command == (uint8_t)CMD_FIRMWARE_VERSION)) 
  {
    _uart->write((uint8_t)MSG_RESERVED);
    _uart->write((uint8_t)MSG_RESERVED);
  }
  _uart->write(id_1);
  _uart->write(id_2);
  _uart->write(checksum);
  _uart->write((uint8_t)MSG_TAIL);
  _uart->flush();
  
  delay(300);
    
  for( i=0; !_uart->available() && i < wait ; ++i )
  {
    delay(20);
  }
  
  //status=true;
  return(i);
}

/// Prompts sensor to report measurement data. If sensor is in report query mode, an according query command is sent first, otherwise, the next measurent reported in the 
/// 
/// Specification from the Nova Fitness Co. Ltd. Laser Dust Sensor Control Protocol, V1.4  
/// 
/// | Byte  |  Name           || Set Mode Reply    | Query Data Reply| Set ID reply    | Sleep / Work Reply | FW Version Reply| Working Period Reply|                 
/// | :---: | :-------------- || :---------------- | :-------------- | :-------------- | :----------------- | :-------------- | :------------------ |                  
/// | 0     |  Message header || AA                | AA              | AA              | AA                 | AA              | AA                  |                 
/// | 1     |  Command ID     || C5                | C0              | C5              | C5                 | C5              | C5                  |                 
/// | 2     |  DATA 1         || 2                 | PM2.5 Low byte  | 5               | 6                  | 7               | 8                   |                 
/// | 3     |  DATA 2         || 0: query / 1: set | PM2.5 High byte | 0               | 0: query / 1: set  | Byte 1: year    | 0: query / 1: set   |                 
/// | 4     |  DATA 3         || 0: auto / 1: query| PM10 Low byte   | 0               | 0: sleep / 1: work | Byte 2: month   | 0: continuous / 1-30: minutes sleep | 
/// | 5     |  DATA 4         || 0                 | PM10 High byte  | 0               | 0                  | Byte 3: day     | 0                   |
/// | 6     |  DATA 5         || ID byte 1         | ID byte 1       | New ID byte 1   | ID byte 1          | ID byte 1       | ID byte 1           |
/// | 7     |  DATA 6         || ID byte 2         | ID byte 2       | New ID byte 2   | ID byte 2          | ID byte 2       | ID byte 2           |
/// | 8     |  Check-sum      || Check-sum         | Check-sum       | Check-sum       | Check-sum          | Check-sum       | Check-sum           |
/// | 9     |  Message tail   || AB                | AB              | AB              | AB                 | AB              | AB                  |
///                                                                
/// | Calculation  ||
/// | :----------  | :-------------------------------------------------------------|
/// | Check-sum:   | Check-sum = DATA1 + DATA2 + ... + DATA6                       |
/// | PM2.5 value: | PM2.5 (μg /m3) = ((PM2.5 High byte *256) + PM2.5 low byte)/10 |
/// | PM10 value:  | PM10 (μg /m3) = ((PM10 high byte*256) + PM10 low byte)/10     |
/// 
/// @see https://web.archive.org/web/20200525083221/https://www-sd-nf.oss-cn-beijing.aliyuncs.com/%E5%AE%98%E7%BD%91%E4%B8%8B%E8%BD%BD/sds011%20laser%20PM2.5%20sensor%20specification-V1.4.pdf
/// @see https://www.arduinoforum.de/attachment.php?aid=3023
/// 
/**************************************************************************/
/*!
    @brief function to send the command to sensor and fetch the response into a buffer.
    When no reply is received, usually this is because device was just reporting.
    This happens when device is in reporting mode, as then the device spits out a
    a continuous stream of data.   
    In such cases. command has to be sent again to get an answer.    
    @param cmd one byte of the command to be sent
    @param reply ten bytes of the command response
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::getResponse(uint8_t cmd, uint8_t reply[10] )
{
  uint8_t checksum=0,i, lc, wait=MAX_WAIT;
  bool status=false;

  if( _debug )Serial.println("Receiving response... ");
  
  for( i=0, status=true, checksum=0; _uart->available() && i < 10; ++i) 
  {
    reply[i] = _uart->read();
    if (i>1 && i < 8)checksum += reply[i];
    if( _debug){ Serial.print(" "); Serial.print(reply[i], HEX); }
    switch(i)
    {
      case 0:
        if ( reply[0] != MSG_HEAD )
        {
          if( _debug){ Serial.print("invalid header received "); Serial.println( reply[i], HEX );}
          status = false;
        }
        break;
      case 1:
        if ( cmd != CMD_QUERY_DATA )
        { 
          if ( reply[1] != REPLY_CFG )
          {
            if( _debug){ Serial.print(" invalid reply received "); Serial.println( reply[i], HEX ); }
            status = false;
          }
        }else
        {
          if ( reply[1] != REPLY_DATA )
          {
            if( _debug){ Serial.print(" invalid reply received "); Serial.println( reply[i], HEX);}
            status = false;
          }
        } 
        break;
      case 2:
        if ( cmd != CMD_QUERY_DATA )
        {
          if ( reply[2] != cmd )
          {
            if( _debug){ Serial.print(" wrong command received "); Serial.println( reply[i], HEX );}
            status = false;
          }
        }
        break;
      case 8:
        if ( checksum != reply[8] )
        {
          if( _debug){ Serial.print(" wrong checksum found"); Serial.print( checksum, HEX );Serial.print(" while received "); Serial.println(reply[8], HEX );}
          status = false;
        }
        break;
      case 9:
        if( reply[9] != MSG_TAIL )
        {
          status = false;
        }
        break;
      default:
        break;
    }
    yield();
  }
    
  if ( i < 10 ) status = false;
  if( _debug){ Serial.println(""); Serial.print("seccessful response, read "); Serial.print(i); Serial.print( " bytes: "); Serial.print( status?"ok":"error" ); Serial.println("--"); } 
  
  return( status );

}

/**************************************************************************/
/*!
    @brief function to set the reporting mode to auto or query
    response will contain either QUERY_MODE or AUTO_REPORT_MODE,
    depending on how it was set by this function ( by setting
    argument wr to WRITE_MODE), or how it was set before if argument
    wr was set to READ_MODE.
    @param response the return the response of the command
    @param mod the mode to be set 0:auto, 1:query
    @param wr the read (0) or write (1)
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::dataReportingModeCmd( uint8_t *response, uint8_t mod, uint8_t wr ){
	//    0    1    2       3       4       5    6    7    8    9   10   11   12   13   14         15          16    17    18
	//{ 0xAA,0xB4,0x02,  0:query, 0:auto,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,_id_1, _id_2, 0x00, 0xAB}; 
  //                   1:set    1:query
	uint8_t reply[10];
	bool status=false;
	
	*response = MSG_FF;
	
    if( (status = sdsCommunicate( CMD_REPORTING_MODE, wr, mod,_id_1, _id_2, reply )) ){ 
		if( _debug){
			char str[50];
			sprintf( str, "Device Id = %02X %02X and  Mode = %s\n", reply[6],reply[7],reply[4] == QUERY_MODE?"Querymode":"Reportingmode" );
			Serial.println ( str );
		}
	
		if ( wr == WRITE_MODE && reply[4] != mod ){
			status = false;
		}
		if ( status ){
			*response = reply[4];
		}
	}
	
	return( status );	
}

/**************************************************************************/
/*!
    @brief function to retrieve PM values
    @param pm10 returned value of PM10 sensor value
    @param pm25 returned value of PM25 sensor value
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::dataQueryCmd( float *pm10, float *pm25 ){
	//     0    1    2    3    4    5    6    7    8    9   10   11   12   13   14         15          16    17    18
	// { 0xAA,0xB4,0x04,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,_id_1, _id_2, 0x00, 0xAB}; 
	uint8_t reply[10];
	bool status=false;
	

    if( (status = sdsCommunicate( CMD_QUERY_DATA, 0, 0,_id_1, _id_2, reply )) ){ 
		*pm25 = (float) ( reply[2] + (reply[3]<<8 ) ) / 10.0;
		*pm10 = (float) ( reply[4] + (reply[5]<<8 ) ) / 10.0;
		if( _debug){
			Serial.print("Data : pm10 ");	Serial.print( *pm10, 2);Serial.print(" pm2.5 ");Serial.print( *pm25, 2);Serial.print(" status : ");	Serial.println( status);		}	
	}
	
	return( status );	
}
/**************************************************************************/
/*!
    @brief function to retrieve PM values with out Query command, it will send automatically at the interval
    set by the work period command
    @param pm10 returned value of PM10 sensor value
    @param pm25 returned value of PM25 sensor value
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::dataAutoQueryCmd( float *pm10, float *pm25 )
{
  uint8_t reply[10];
  bool status=false;
  uint8_t i, lc, wait = MAX_WAIT;
  
  for(uint8_t lc = 0; lc < 10 && status == false; ++lc)
  {
    if( i >= wait )
    {
      if( _debug)Serial.println("data unavailable, exiting...");
      i = 0;
      status = false;
    }
    status = getResponse( (uint8_t)CMD_QUERY_DATA, reply );
  }

  *pm25 = (float) ( reply[2] + (reply[3]<<8 ) ) / 10.0;
  *pm10 = (float) ( reply[4] + (reply[5]<<8 ) ) / 10.0;
  if( _debug){
    Serial.print("Data : pm10 "); Serial.print( *pm10, 2);Serial.print(" pm2.5 ");Serial.print( *pm25, 2);Serial.print(" status : "); Serial.println( status);    } 
  
  return( status ); 
}
/**************************************************************************/
/*!
    @brief function to set new id/serial number for the sensor.
    Device id is two bytes, the factory sets each device to a unique id.
    New id will be returned in response. 
    By default any device is addressed by address FF FF. This is the default
    for this library as not many people will connect more than one device
    to the same serial lines. If this default (FF FF) is used, class instance
    device id (_id_1 and _id_2 ) will not be updated with this
    new device address, and FF FF will continued to be used to address the device.
    @param response the return ID value
    @param new_Id1 new id lower byte
    @param new_Id2 new id higher byte
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::deviceIdCmd( uint8_t response[2], uint8_t new_Id1, uint8_t new_Id2 ){
	//     0    1    2   3    4    5    6    7    8    9   10   11   12          13          14           15          16     17    18
	//{ 0xAA,0xB4,0x05,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, new_Id1, new_Id2, _id_1, _id_2, 0x00, 0xAB}; 
	uint8_t reply[10];
	bool status=false;
	
	response[0] = _id_1;
	response[1] = _id_2;
	

		if( (status = sdsCommunicate( CMD_SET_DEVICE_ID, new_Id1, new_Id2,_id_1, _id_2, reply )) ){ 
		
		if ( reply[6] != new_Id1 || reply[7] != new_Id2 ) {
			if( _debug){
				char str[50];
				sprintf( str,"Setting Device Id Failed. Tried = %02X %02X, Returned = %02X %02X\n", new_Id1, new_Id2, reply[6], reply[7]  );
				Serial.println( str );
			}
		
			status = false;
		}else{
			if ( _id_1 != MSG_FF || _id_2 != MSG_FF ){
				_id_1 = new_Id1;
				_id_2 = new_Id2;	
			}
			
			response[0] = reply[6];
			response[1] = reply[7];
		}
	}
	
	return( status );	
}


/**************************************************************************/
/*!
    @brief function to set sleep or work mode and check the response. 
    response will contain either sds011_dataReportingModeCmd or sds011_SLEEPING,
    depending on how it was set by this function ( by setting
    argument set to WRITE_MODE), or how it was set before if argument
    wr was set to sds011_ASK.    
    @param response the return value of the command
    @param mod sleep or work mode
    @param wr parameter to set (write) or unset (get)
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::sleepWorkModeCmd( uint8_t *response, uint8_t mod,uint8_t wr){
	//    0    1    2        3          4      5    6    7    8    9   10   11   12   13   14         15          16    17    18
	//{ 0xAA,0xB4,0x06,  0:query,   0:sleep,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,_id_1, _id_2, 0x00, 0xAB}; 
  //                   1:set      1:work  
	uint8_t reply[10];
	bool status=false;
	
	*response = MSG_FF;
	

		if( (status = sdsCommunicate( CMD_SLEEP_AND_WORK, wr, mod,_id_1, _id_2, reply )) ){ 
		if( _debug){
			char str[50];
			sprintf(str, "Device Id = %02X %02X and Mode = %s\n", reply[6],reply[7],reply[4] == WORK_MODE?"Working":"Sleeping" );
			Serial.println( str );
		}
		*response = reply[4];
		if ( wr == WRITE_MODE && reply[4] != mod ){
			status = false;
		}
	}else{
		if ( wr == WRITE_MODE && mod == WORK_MODE ){
			if( _debug)Serial.println("No Reply Received After Entering Working Mode." );
		}
	}
	return( status );	
}

/**************************************************************************/
/*!
    @brief function to set work period for reporting.     
    response will contain 0 ( for continuous mode) or the Interval
    time in minutes (for interval mode). According to the datasheet
    device will sleep during the interval and become active 30 seconds 
    before the interval is reached. 
    @param response the return value of the work perion command
    @param minutes duration of sleep time
    @param wr parameter to set (write) or unset (get)
    @returns status tells the seccessful execution
*/
/**************************************************************************/
bool sds011::workPeriodCmd( uint8_t *response, uint8_t minutes,uint8_t wr ){
	//    0    1    2     3              4       5    6    7    8    9   10   11   12   13   14    1     16    17    18
	//{ 0xAA,0xB4,0x08, 0:query, 0:continuous,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,_id_1, _id_2, 0x00, 0xAB};
  //                  1:set    1:sleep minutes 
	uint8_t reply[10];
	bool status=false;
	
	*response = MSG_FF;
	
		if( (status = sdsCommunicate( CMD_WORKING_PERIOD, wr, minutes,_id_1, _id_2, reply )) ){ 
		if( _debug){
			char str[50];
			sprintf(str, "Device id = %02X %02X : Work Period = %s %d %s\n", reply[6],reply[7],reply[4] == 0?"Continuous":"Interval = ",reply[4] == 0?0:reply[4], reply[4] == 0?".":"minutes." );
			Serial.println( str );
			}
	}	
	if ( wr == WRITE_MODE && reply[4] != minutes ){
		status = false;
	}
	if ( status ){
		*response = reply[4];
	}
	return( status );	
}


/**************************************************************************/
/*!
    @brief Used to get the device firmware version and serial number.
    response[3] contains firmware date as YY, MM, DD if succesful else FF FF FF.
    @param ver string version to hold the firware version
    @param id integer to hold the serial number
    @returns status the status of the command
*/
/**************************************************************************/
bool sds011::deviceInfoCmd( String *ver, uint16_t *id ){
// 0    1    2    3    4    5    6    7    8    9   10   11   12   13   14         15          16    17    18
//{ 0xAA,0xB4,0x07,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,_id_1, _id_2, 0x00, 0xAB}; 
	uint8_t reply[10];
	bool status = false;
  char str[50];
		
		if( (status = sdsCommunicate( CMD_FIRMWARE_VERSION, 0, 0,_id_1, _id_2, reply )) ){ 
		if ( _debug){
				
				sprintf(str, "Device Id = %02X %02X and Firmware Version = 20%02d-%02d-%02d\n", reply[6],reply[7], reply[3],reply[4],reply[5]);
				Serial.println( str);	
		}		
	}
 sprintf(str, "20%02d-%02d-%02d\n", reply[3],reply[4],reply[5]);
 *ver = str;
 *id = reply[7]*256+reply[6];
 return( status );
}

/**************************************************************************/
/*!
    @brief  setting a debugging flag
    @param on bool flag for debugging
    @returns void
*/
/**************************************************************************/
void sds011::setDebug( bool on ){
	_debug = on;
}


/// Specification from the Nova Fitness Co. Ltd. sds011 data sheet, V1.3 
///
/// | UART ||
/// | :-------------------- | :-------------|
/// | Bit rate              | 9600          |
/// | Data bit              | 8             |
/// | Parity bit            | NO            |
/// | Stop bit              | 1             |
/// | Data Packet frequency | 1Hz           |

/**************************************************************************/
/*!
    @brief  initializes the sds011 sensor with hardware port and pins
    @param uart hardware uart port
    @param rx uart recieve pin
    @param tx uart transmit pin
    @param id_1 device lower byter
    @param id_2 device higher byter
    @returns status true when executed seccessfully
*/
/**************************************************************************/
bool sds011::begin(HardwareSerial* uart, uint8_t rx, uint8_t tx, uint8_t id_1, uint8_t id_2 ) 
{
  _rx = rx;
	_tx = tx;
	_id_1 = id_1;
	_id_2 = id_2;
	
	_debug  = false;
  uart->begin(9600,SERIAL_8N1, rx, tx);
  _uart=uart;
  if ( _debug) Serial.println("sensor is init.");
  return true;
}
