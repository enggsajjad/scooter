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
//#define RELEASE 

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
#include <DHT.h>
#include <Wire.h>
//#include <time.h>
#include <ezTime.h>
#include "sensor.h"
#include "timer.h"
#include "thinker.h"
#include <String.h>
#include <TinyGPS++.h>

#define rgbColor 0
#define rgColor 1
#define rbColor 2
#define rColor 3
#define gbColor 4
#define gColor 5
#define bColor 6
#define noColor 7

#define set_gprsd_off 0
#define set_gps_on 1
#define chk_creg 2
#define chk_cgact 3
#define set_cgatt_on 4
#define chk_cgdcont 9
#define set_cgdcont_on 6
#define set_cgact_on 7
#define chk_cgact_again 8
#define chk_cipstatus 5
#define set_gpsrd_read 40
#define set_cipstart 10
#define read_gps_msg 127
#define read_gps_done 128
#define set_gpsrd_off 129
#define sensor_data 126
#define set_cipsend 41
#define set_cipclose 42
#define chk_cipstatus_again 43
#define set_cipshut 50
#define dummy_state 51

/// GPS Decoder for NMEA Formate
TinyGPSPlus tiny;

/// Read messages for GPRS reciever, Response for AT Commands
String rd;
/// messages to be sent to TCP
String message= "";
/// TCP HOST
String host = "0.tcp.ngrok.io";
/// TCP Port
String port = "12088";
/// Receive from Serial States
int rxState = -1;
/// Receive from Serial Next States
int nState;
/// Receive from Serial Current States
int current;
/// Error Indicating for AT Command Respnse
byte Err;

/// the temporary response for AT+CIPSTATUS? Command 
char resp;
/// counter to count total number of transmitted TCP packets
int cntr;
/// counter to count total number of transmitted TCP packets
int cntr1;

byte r;
String result,msg;
/// Location Message
String loc;
bool running = true;
const char* msg1;
bool ModuleState=false;


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
