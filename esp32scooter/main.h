/**
 * \file main.h
 * \brief This the main page for the SMARTAQNET which is the core of the project. 
 * \mainpage Smart Air Quality Network (SMARTAQNET) for Scooter
 * 
 * \section intro Introduction
 * SMARTAQNET project is aimed at development of a close-meshed network of local fine-dust data, which can be fed and used well by general public.
 * Further details can be checked at: http://www.smartaq.net.\n
 * This script uses a ESP32 Board, a SDS011 particulate matter sensor and a BME280 sensor or a DHT22 sensor. It used A9G GPRS/GPS Module to setup 
 * data communication with a server. The Fig-1 shows the main board having different sensors connected together. The ESP32 is also refered to 
 * as crowdsensor. Following is the details for differnt features and steps to start:\n
 * - Install the arduino, board and required libraies as explained in ...
 * - Connect the hardware as explained in ....
 * - Configure the constants in the code as explained in .....
 * - Compile and upload the cdes as explained in \ref upload and ....
 * - Configure the SIM and GPRS and other parameters as explained in ....
 * - If everything is installed and configured perfectly, there should be no error i.e., E0 as explained in ....
 * - If there is any errors please look into error or known bug section  as explained in  ...
 * - For detailed debugging uncomment RELEASE constant. Some quick trick will also save the time as shown in ....
 * \subsection sdso11 SDS011 Dust Sensor Usage
 * - Hardware Pins: it uses transmit #SDS011_TXD (13) and recieve #SDS011_RXD  (17) pins
 * - SERIAL Port: it uses the HardwareSerial port 2 for #SDS_SERIAL
 * \subsection bme280 BME280 Sensor Usage
 * - Hardware Pins: it uses SDA #BME_SDA (21) and SCL #BME_SCL (22) pins
 * - I2C Port: to avoid any mismatch it uses Wire1 (default is Wire0) in bme.begin(BME_ADDR, &Wire1)
 * - I2C address: #BME_ADDR as 0x77 for Red, 0x76 for Purple (chinese)
 * 
 * \subsection flowchart FlowCharts
 * The project work-flow can be see in the TODO
 * 
 * \subsection const Important Constants
 * Have a look at the following constants to configure them accordingly. These constants needs to be configured before the firmware is uploaded.
 * - in the file sensor.h
 *  - #DHT_PIN 21, set DHT22 sensor communication pin
 *  - #BME_ADDR  0x77, set BME280 I2C Address 0x77 for Red, 0x76 for Purple 
 *  - #BME_SDA, set BME280 I2C SDA pin
 *  - #BME_SCL, set BME280 I2C SCL pin
 *  - #SDS011_TXD, set SDS011 TXD pin is connected at RXD of Serial2 Object
 *  - #SDS011_RXD, set SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
 * - in the file main.h
 *  - #FIRMWARE_VERSION controls the hard-coded firmware version of the code
 *  - #sw_version describes the software version of the Thing's software properties
 *  - #hw_date describes the hardware revision timestamp of the Thing's hardware properties
 *  - #BASE_URL HTTP Server Address
 *  - RELEASE controls the debugging prints and #MY_DEBUG
 * 
 * \section  prereq Pre-requisites
 * -# Arrange the required \ref hw_sec and carry out the \ref wiring.
 * -# Install the the \ref arduinoide
 * -# Install the \ref boardlib
 * -# Install the recommended \ref lib
 * 
 * \section hw_sec Hardware
 * It consists of ESP32 microcontrollers, which is "HelTec WiFi Kit 32" ----> https://heltec.org/project/wifi-kit-32/
 * Following board, hardware and libraries are used to develop this project.
 * 
 * \subsection peri Peripherals
 * -# OLED Built-in OLED Display ----> https://heltec.org/project/wifi-kit-32/
 * -# SDS011 Sensor ----> https://learn.watterott.com/sensors/sds011/ 
 * -# BME280 Sensor for Humidity and Temperature ----> https://www.ebay.de/itm/3-3V-Bosch-BME280-Barometer-Luftdruck-Luftfeuchte-Sensor-Modul-BMP280-SPI-I2C-/263548911232 or https://eckstein-shop.de/SparkFun-Barometric-Pressure-Sensor-Breakout-MPL115A1?curr=EUR&gclid=Cj0KCQjww7HsBRDkARIsAARsIT6QsuuszbyaQ4r1DLuYTOT5pgwMfpIhRTtwQKAA2vtHkuHGrBZa9JQaAo-gEALw_wcB
 * -# DHT22 Sensor for Temperature and Humidity ----> https://www.sparkfun.com/products/10167 
 * 
 * \subsection arduinoide Arduino IDE
 * The Arduino IDE Version 1.8 or later is needed. Please install from the official Arduino IDE download page. This step is not required if you already have the most recent version.
 * 
 * \subsubsection dialout Pyserial
 * If you use linux, you have to install the python pyserial package and add the current user to the dialout group:
 * <pre><code class="text">
 * sudo apt install python-pip
 * pip install pyserial
 * sudo /usr/sbin/usermod -a -G dialout \<MY_USER_NAME\>
 * </code></pre>
 * 
 * \subsubsection boardlib ESP32 Arduino core
 * Also, to apply AutoConnect to ESP32, the arduino-esp32 core provided by Espressif is needed. Stable 1.0.3 or required and the latest release is recommended.
 * Install third-party platform using the Boards Manager of Arduino IDE (for instructions see here: https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md). 
 * You can add multiple URLs into Additional Board Manager URLs field, separating them with commas. Package URL is https://dl.espressif.com/dl/package_esp32_index.json for ESP32.
 * 
 * \subsubsection lib Required Libraries
 * Following are the commonly used libraries, and should be added in the Arduino IDE, before the compiling.
 * -# FIRMWARE_VERSION 0.1.0.0 onwards
 *  - ESP32 Board by Espressif Systems v1.0.3 (https://github.com/espressif/arduino-esp32)
 *  - Adafruit Unified Sensor by Adafruit v1.0.3 (https://github.com/adafruit/Adafruit_Sensor)    
 *  - Adafruit BME280 Library by Adafruit v1.0.8 (https://github.com/adafruit/Adafruit_BME280_Library)    
 *  - DHT sensor library by Adafruit v1.3.4 (https://github.com/adafruit/DHT-sensor-library)    
 *  - eztime by Rop Gonggrijp v0.7.10 (https://github.com/ropg/ezTime)    
 *  - name=SDS011 sensor Library by R. Zschiegner v0.0.6 (https://github.com/ricki-z/SDS011). This library is kept in src/lib/SDS011 folder. Even the library is updated on the github. But when tried to installed from library manager it is not installed properly for esp32.
 *  
 * \subsection upload Firmware Uploading
 * Following steps are recommended to upload the firmware before handing over the new crowd-sensor node.
 * -# To erase flash completely before delivering the product: "C:\Users\Sajjad\Documents\ArduinoData\packages\esp32\tools\esptool_py>esptool.py --chip esp32 erase_flash" from the command terminal.
 * -# Upload the firmware using the Ardiuno IDE
 * 
 * \section wiring  Wirings
 * -# GPRS/GPS Module\n
 *  There might be two different modules A7 or A9G. The wiring that just require pin definitions, as:
 *  - #gprsRX, The GPRS Serial Receive pin
 *  - #gprsTX, The GPRS Serial Transmit pin
 *  - #gpsRX, The GPS Serial Receive pin
 *  - #gpsTX, The GPS Serial Transmit pin 
 *  - #gprsPWR, The GPRS PWR_KEY pin 
 *  - #gprsRST, The GPRS Reset pin 
 * -# SDS011 Dust Sensor\n
 * One has to:
 *  - connect TXD of Dust Sensor SDS011 to Pin 13 of ESP32 (ESP-RX)
 *  - connect 5V of Dust Sensor SDS011 to Pin 5V of ESP32
 *  - connect GND of Dust Sensor SDS011 to Pin GND of ESP32
 * -# BME280 Sensor for Humidity and Temperature\n
 *  - connect SCK of BME280 sensor to Pin 22 of ESP32 (ESP-SCL)
 *  - connect SDA of BME280 sensor to Pin 21 of ESP32 (ESP-SDA) 
 *  - connect VCC of BME280 sensor to 3V3 of ESP32 
 *  - connect GND of BME280 sensor to GND of ESP32
 *  - the I2C address for different BME280 (purple/red) can be configure in sensor.h using #BME_ADDR constant
 * -# DHT22 Sensor for Temperature and Humidity (if BME280 is not used)\n
 * Look at the sensor from the front (grid shows in your direction), put the pins facing down, start counting the pins from left (pin1) to right (pin4). pin 3 is not connected.\n One has to:
 *  - Connect pin 1 of DHT22 sensor (on the left) to 3V3 of ESP32
 *  - Connect pin 2 of DHT22 sensor to Pin 21 of ESP32 (data)
 *  - Connect pin 4 of DHT22 sensor (on the right) to GND of ESP32
 *  - Connect a resistor in range 4.7k to 10k from pin 2 (data) to pin 1 (power) of the sensor
 *  
 * \section knownbugs  Known Bugs
 * There are following bugs and discrepensies that needs to be take care off.
 * -# xxx
 * -# yyyy
 * 
 * \section changelog  Change Log
 * \subsection changelog2 FIRMWARE_VERSION 0.1.0.0 onwards
 * - [0.1.0.0]  Sep. 7, 2020
 *  - First State Machine and integration of whole sensors
 *  
 *\section author Author(s)
 * - Sajjad Hussain, 
 * - Tobias Röddiger, 
 *
 * The team at TECO (www.teco.edu) and at Chair for Pervasive Computing Systems (https://pcs.tm.kit.edu).
 *
 * \section license License
 * This code belongs to TECO www.teco.edu and must be reffered when used. BSD license, all text here must be included in any redistribution.
 * \subsection ack Acknowledgments
 * The following libraries were used:\n
 * - Espressif Systems: Licenses and Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 * - Adafruit BME280 is licensed under Adafruit. https://adafruit.com/
 * - DHT sensor is licensed under Adafruit. https://adafruit.com/
 * - ezTime is licensed under the MIT License. https://github.com/ropg/ezTime
 * - SDS011 sensor Library is licensed under the MIT License. (https://github.com/ricki-z/SDS011)
 * 
 * \subsection mit MIT License
 * 
 *        Copyright (c) 2017-2019 Hieromon Ikasamo
 * 
 *        Permission is hereby granted, free of charge, to any person obtaining a copy
 *        of this software and associated documentation files (the "Software"), to deal
 *        in the Software without restriction, including without limitation the rights
 *        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *        copies of the Software, and to permit persons to whom the Software is
 *        furnished to do so, subject to the following conditions:
 * 
 *        The above copyright notice and this permission notice shall be included in all
 *        copies or substantial portions of the Software.
 * 
 *        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *        SOFTWARE.
 * 
 * 
 * 
 * \subsection EspressifLic Espressif Systems License
 * 
 *        Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
 *        
 *        Licensed under the Apache License, Version 2.0 (the "License");    
 *        you may not use this file except in compliance with the License.    
 *        You may obtain a copy of the License at    
 *        http://www.apache.org/licenses/LICENSE-2.0    
 *        Unless required by applicable law or agreed to in writing, software    
 *        distributed under the License is distributed on an "AS IS" BASIS,    
 *        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    
 *        See the License for the specific language governing permissions and    
 *        limitations under the License.
 *        
 *        
 *\date 07.09.2020
 */



//******************************************************** constants ***************************

/// firmware version, (for intermediate versions being actively worked on, append "pre" to number, indicating that it is "not yet" the specified number)
#define FIRMWARE_VERSION "0.1.0.0 pre"
/// Software.version 
#define sw_version FIRMWARE_VERSION
/// Hardware.Revision 
#define hw_date "2020-01-17T12:00:00.000Z"

/// Debugging, uncomment to switch servers to release version, enable sending to madavi and luftdaten.info, and supress some debug output
//#define RELEASE // !!!IMPORTANT!!!

// Setting the servers and debugging control using the RELEASE
#ifndef RELEASE
  ///Non-Release Version: Frost Server Base address used in Frost_Server.cpp
  //#define BASE_URL "http://smartaqnet-dev.dmz.teco.edu/v1.0"
  #define BASE_URL "http://193.196.38.108:8080/FROST-Server/v1.0"
  
  ///Define MYDEBUG for controlling debugging
  #define MY_DEBUG
  
#else
  ///Release Version: Frost Server Base address used in Frost_Server.cpp
  #define BASE_URL "http://api.smartaq.net/v1.0"
  
  ///unDefine MYDEBUG for controlling debugging
  #undef MY_DEBUG
  
#endif 

#ifdef MY_DEBUG
  ///Define MY_DEB() for controlling usbSerial.print debugging
  #define MY_DBG(...) do {usbSerial.print( __VA_ARGS__ );} while (0)
  ///Define MY_DEBln() for controlling usbSerial.println debugging
  #define MY_DBGln(...) do {usbSerial.println( __VA_ARGS__ );} while (0)
#else
  ///UnDefine MY_DEB() for controlling usbSerial.print debugging
  #define MY_DBG(...)
  ///UnDefine MY_DEBln() for controlling usbSerial.println debugging
  #define MY_DBGln(...)
#endif // !MY_DEBUG


#ifdef RELEASE
  /// switch for sending to madavi api
  #define ENABLE_SEND2MADAVI true  
  /// switch for sending to luftdaten api
  #define ENABLE_SEND2LUFTDATEN true  
#else
  /// switch for sending to madavi api
  #define ENABLE_SEND2MADAVI false 
  /// switch for sending to luftdaten api
  #define ENABLE_SEND2LUFTDATEN false
#endif


//******************************************************** includes ***************************

#include "src/lib/SDS011/SDS011.h"
#include <HardwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_BME680.h"
#include <DHT.h>
#include <Wire.h>
//#include <time.h>
#include <ezTime.h>
#include "sensor.h"
//#include "timer.h"
//#include "thinker.h"
#include <String.h>
#include <TinyGPS++.h>

///RGB COLOR CODES
#define rgbColor 0
#define rgColor 1
#define rbColor 2
#define rColor 3
#define gbColor 4
#define gColor 5
#define bColor 6
#define noColor 7

///STATES
#define chk_sim_cpin 1
#define set_network 2
#define chk_network_register 3
#define set_gps 4
#define attach_ps 5
#define check_context 6
#define set_context 7
#define select_context 8
#define show_context 9
#define chk_cip_status 10
#define set_cipstart 11
#define set_gpsrd_read 12
#define read_gps_msg 13
#define read_gps_done 14
#define set_gpsrd_off 15
#define sensor_data 16
#define set_cipsend 17
#define set_cipsend_complete 18
#define chk_cipstatus_again 19
#define set_cipclose 20
#define set_cipshut 21
#define dummy_state 22
//#define timeout_state 23
#define error_state 24
#define hang_state 25
#define bme_setting_error 26
#define bme_reading_error 27
#define init_errors 28
#define try_init_again 29
#define config_sensors 30
#define attach_ps_again 31
#define chk_cip_status2 32
#define set_cipstart2 33



//uncomment the following if not using thingspeak, comment if for NGROK 
//#define THINGSPEAK  // !!!IMPORTANT!!!
//uncomment the following if  using actual sensors, comment if for debugging 
//#define REAL_SENSORS  // !!!IMPORTANT!!!

#ifdef THINGSPEAK
  /// TCP HOST
  String host = "api.thingspeak.com"; // !!!IMPORTANT!!!
  /// TCP Port
  String port = "80"; // !!!IMPORTANT!!!
#else
  /// TCP HOST
  String host = "8.tcp.ngrok.io"; // !!!IMPORTANT!!!
  /// TCP Port
  String port = "19482"; // !!!IMPORTANT!!!
#endif

/// APN of the SIM
String apn = "pinternet.interkom.de"; // !!!IMPORTANT!!!
/// Define the AiThinker Initialization End Detection String
String initString = "READY"; // !!!IMPORTANT!!!


/// GPS Decoder for NMEA Formate
TinyGPSPlus tiny;

/// temporary millis counter for time measurement
unsigned long previousMillis = 0; 
/// counter for led blinking
int cntrLED=0;
/// cntrl character ascii 26
char cntrlChar[2];
/// messages to be sent to TCP
String message= "";
/// Receive from Serial States
int rxState = -1;
/// the temporary response for AT+CIPSTATUS? Command 
char resp;
/// counter to count total number of transmitted TCP packets
int cntr;
/// counter to count total number of transmitted TCP packets
int cntr1;
/// last state
//int last;
/// response byte of the commands
byte r;
/// temporary string used for reading GPS messages
String result;
/// temporary string used for reading GPS messages
String msg;
/// Location Message
String loc;
/// boolean flag used for reading GPS messages
bool running = true;
/// count the reset counter after each reset
unsigned char resetCntr;
/// initialization the wire I2C interface
bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
/// instantiate the BME680 object
Adafruit_BME680 bme(&Wire1);

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
    cntrLED++;
    if (cntrLED==8) cntrLED =0;
    digitalWrite(LED1, cntrLED&1);
    digitalWrite(LED2, (cntrLED>>1)&1);
    digitalWrite(LED3, (cntrLED>>2)&1);
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
  char resp[1000];//[300];
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
  char resp[1000];//[300];
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
    @brief  Reading the GPS Messages after AT+GPSRD = n
    @returns void
*/
/**************************************************************************/
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
          if (result.indexOf("$GNGGA") > 0)  // !!!IMPORTANT!!!
          {
            cntr = 1;
            msg = result.substring(result.indexOf("$GNGGA"));
          }
          break;
        case 1:
          msg = msg  + result;
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
/**************************************************************************/
/*!
    @brief  ISR functions called for Reset ESP32
    @returns void
*/
/**************************************************************************/


void IRAM_ATTR resetModule() 
{
  ets_printf("reboot in 5sec...\n");
  delay(5000);
  esp_restart();
}

void displayInfo()
{
  usbSerial.print(F("Location: ")); 
  if (tiny.location.isValid())
  {
    usbSerial.print(tiny.location.lat(), 6);
    usbSerial.print(F(","));
    usbSerial.print(tiny.location.lng(), 6);
  }
  else
  {
    usbSerial.print(F("INVALID"));
  }

  usbSerial.print(F("  Date/Time: "));
  if (tiny.date.isValid())
  {
    usbSerial.print(tiny.date.month());
    usbSerial.print(F("/"));
    usbSerial.print(tiny.date.day());
    usbSerial.print(F("/"));
    usbSerial.print(tiny.date.year());
  }
  else
  {
    usbSerial.print(F("INVALID"));
  }

  usbSerial.print(F(" "));
  if (tiny.time.isValid())
  {
    if (tiny.time.hour() < 10) usbSerial.print(F("0"));
    usbSerial.print(tiny.time.hour());
    usbSerial.print(F(":"));
    if (tiny.time.minute() < 10) usbSerial.print(F("0"));
    usbSerial.print(tiny.time.minute());
    usbSerial.print(F(":"));
    if (tiny.time.second() < 10) usbSerial.print(F("0"));
    usbSerial.print(tiny.time.second());
    usbSerial.print(F("."));
    if (tiny.time.centisecond() < 10) usbSerial.print(F("0"));
    usbSerial.print(tiny.time.centisecond());
  }
  else
  {
    usbSerial.print(F("INVALID"));
  }

  usbSerial.println();
}


void rgbSetColor(int color)
{
  switch(color)
  {
    case rgbColor:
      digitalWrite(LED1, 0); digitalWrite(LED2, 0); digitalWrite(LED3, 0);
      break;
    case rgColor:
      digitalWrite(LED1, 0); digitalWrite(LED2, 0); digitalWrite(LED3, 1);
      break;
    case rbColor:
      digitalWrite(LED1, 0); digitalWrite(LED2, 1); digitalWrite(LED3, 0);
      break;
    case rColor:
      digitalWrite(LED1, 0); digitalWrite(LED2, 1); digitalWrite(LED3, 1);
      break;
    case gbColor:
      digitalWrite(LED1, 1); digitalWrite(LED2, 0); digitalWrite(LED3, 0);
      break;
    case gColor:
      digitalWrite(LED1, 1); digitalWrite(LED2, 0); digitalWrite(LED3, 1);
      break;
    case bColor:
      digitalWrite(LED1, 1); digitalWrite(LED2, 1); digitalWrite(LED3, 0);
      break;
    case noColor:
      digitalWrite(LED1, 1); digitalWrite(LED2, 1); digitalWrite(LED3, 1);
      break;
  }
}
