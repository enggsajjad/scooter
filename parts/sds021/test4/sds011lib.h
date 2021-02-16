//! ESP32 C/C++ Arduino library for the Nova Fitness SDS011 PM sensor (interface)

/// @file sds011lib.h
/// @author Sajjad Hussain
/// @version 0.1

#ifndef PM_SDS011_h 
#define PM_SDS011_h

#include "Arduino.h"

/// Reporting mode as auto
#define AUTO_REPORT_MODE 0
/// Reporting mode as queries
#define QUERY_MODE  1
/// get or receive
#define READ_MODE 0
/// set or write
#define WRITE_MODE 1
/// mode set to sleep
#define SLEEP_MODE 0
/// mode set to normal
#define WORK_MODE  1
/// set the maximum trials for response
#define MAX_WAIT 30
/// dont care while reading
#define DONT_CARE  0

/// sds response header
#define MSG_HEAD 0xAA //170
/// sds response footer
#define MSG_TAIL       0xAB  //172
/// sds response/command reserve word
#define MSG_RESERVED   0x00 
/// sds sending command
#define CMD_WRITE_MODE    0xB4  //180
/// response query data indication
#define REPLY_DATA     0xC0  //192
/// response query configuration indication
#define REPLY_CFG      0xC5  //197
/// sending command for reporting mode
#define CMD_REPORTING_MODE       0x02 
/// sending command for query data mode
#define CMD_QUERY_DATA      0x04 
/// sending command to set device id
#define CMD_SET_DEVICE_ID         0x05 
/// sending command for sleep and work mode
#define CMD_SLEEP_AND_WORK      0x06 
/// sending command for geting firmware version
#define CMD_FIRMWARE_VERSION  0x07 
/// sending command for working period setting
#define CMD_WORKING_PERIOD      0x08 
/// all set values as one
#define MSG_FF      0xff 

/// sds011 sensor class interface to interace with the hardware
class sds011 {
	public:
		sds011(void);
		bool begin(HardwareSerial* uart,	uint8_t rx, uint8_t tx, uint8_t id_1 = MSG_FF, uint8_t id_2 = MSG_FF );
		bool dataReportingModeCmd( 			uint8_t *response, 	uint8_t mod = AUTO_REPORT_MODE,	uint8_t wr = READ_MODE );
		bool dataQueryCmd( float *ppm10, 		float *ppm25 );
    bool dataAutoQueryCmd( float *ppm10,    float *ppm25 );
		bool deviceIdCmd( 		uint8_t response[2], 	uint8_t new_Id1, uint8_t new_Id2 );
		bool sleepWorkModeCmd( 		uint8_t *response, 	uint8_t mod = WORK_MODE,uint8_t wr = READ_MODE);
		bool workPeriodCmd(		uint8_t *response, 	uint8_t minutes = 0,uint8_t wr = READ_MODE);
		bool deviceInfoCmd( String *ver, uint16_t *id );
    void setDebug( bool on );
  private:
    /// uart rx pin
    uint8_t _rx;
    /// uart tx pin
    uint8_t _tx;
    /// serial number lower byte
    uint8_t _id_1;
    /// serial number lower byte
    uint8_t _id_2;
    /// the debugging flag
    bool _debug;
    /// the hardware uart port
    Stream *_uart;  
    uint8_t sendCommand( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2 );
    bool getResponse(uint8_t cmd, uint8_t reply[10] );
    bool sdsCommunicate( uint8_t command, uint8_t option_1, uint8_t  option_2, uint8_t id_1, uint8_t id_2, uint8_t reply[10]  );
};

#endif
