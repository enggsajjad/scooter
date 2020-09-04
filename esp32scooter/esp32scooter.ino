//adding different sensors into the state machine code
//first state machine based example for scooter

#include "src/lib/SDS011/SDS011.h"
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>


//HardwareSerial gps(2);
HardwareSerial a7g(1);
/// use Serial 2 Object
HardwareSerial SDS_SERIAL(2); // SDS011: use Serial 2 

/// SDS011 TXD pin is connected at RXD of Serial2 Object
#define SDS011_TXD  27
/// SDS011 RXD pin is connected at TXD of Serial2 Object (no need to physically connect)
#define SDS011_RXD  26

unsigned long previousMillis_sensor = 0; 
/// intervall of reading sensor data in ms
#define SENSOR_INTERVAL 10000 

/// Dust sensor object
SDS011 sds; 
/// vars to store sensor results, sensor results for dust sensor pm10
float pm10;
/// sensor results for dust sensor pm25
float pm25;
/// temperature values

unsigned char rxState;
unsigned long packet;
unsigned long fPacket;

#define nPacket  10
bool status_sds;

#define USBTX 3
#define USBTX 1
//A7
#define baudrate  115200
#define A9RX 16
#define A9TX 17
#define GPSRX 23
#define GPSTX -1
int PWR_KEY_PIN = 4;

/*
//A9G
#define baud  9600
#define A9RX 14
#define A9TX 12
#define GPSRX 13
#define GPSTX -1
int PWR_KEY_PIN = 33;
int RESET_PIN = 34;
*/
/// BME280 I2C Address 0x77 for Red, 0x76 for Purple (chinese)
#define BME_ADDR  0x77
/// BME280 I2C SDA pin
#define BME_SDA 21
/// BME280 I2C SCL pin
#define BME_SCL 22

/// BME280 Pressure & Humidity Sensor Object 
Adafruit_BME280 bme; 
/// temperature values
float temp;
/// humidity values
float hum;
/// pressure values
float atm;
bool use_bme280;
unsigned char bmeAddress;

#define get_a9g_init_msg  1
#define chk_at_response   2
#define chk_sim_lock      3
#define configure_gps   4
#define chk_ntwrk_connection  5
#define chk_sim_pin 6
#define try_sim_pin 7
#define set_auto_connection 8
#define chk_reg_network 9
#define chk_ntwrk_attached  10
#define attach_a_network  11
#define chk_ntwrk_again 12
#define configure_apn 13
#define connect_gprs  14
#define open_tcp_socket 15
#define idle_state  16
#define send_tcp_data 17
#define close_tcp_socket  18
#define tcp_data_seccessful 19
#define tcp_data_failure  20
#define gps_data_logging 21
#define get_sds_data  22
#define get_bme_data  23

boolean command0(String cmd, String response, unsigned long timeout, boolean output);
boolean command(String cmd, String response, unsigned long timeout, boolean output);

void setup() {
  delay(1000);
  pinMode(PWR_KEY_PIN, OUTPUT);
 // pinMode(RESET_PIN, OUTPUT);
  //digitalWrite(RESET_PIN, HIGH);
  digitalWrite(PWR_KEY_PIN, HIGH);
  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(baudrate,SERIAL_8N1, GPSRX, USBTX);//downloading,debugging
  //gps.begin(baudrate, SERIAL_8N1, GPSRX, -1);
  a7g.begin(baudrate, SERIAL_8N1, A9RX, A9TX);
  sds.begin(&SDS_SERIAL, SDS011_RXD, SDS011_TXD);  // initialize SDS011 sensor
  
  bool wireStatus = Wire1.begin(BME_SDA, BME_SCL);
  if (!wireStatus) Serial.print("Wire1 failed to init");

  bmeAddress = BME_ADDR;
  use_bme280 = bme.begin(bmeAddress, &Wire1);
  
  Serial.println("ScooterAQNET Started....");
  
  
  digitalWrite(PWR_KEY_PIN, LOW);
  delay(3000);
  digitalWrite(PWR_KEY_PIN, HIGH);

  Serial.println("A9G PWR KEY asserted....");
 //rxState = get_a9g_init_msg;//a9g
 rxState = get_a9g_init_msg;//a7

}

void loop() 
{
  String message= "";
  switch(rxState)
  {
    //case get_a9g_init_msg://a9g
      //if(command0("NONE", "READY", 20000, false))
      //  rxState = chk_at_response;
      //break;
    case get_a9g_init_msg://a7
      if(command("AT+RST=1", "+CREG: 0", 20000, true))
        rxState = chk_at_response;
      break;
    case chk_at_response:
      if(command("AT", "OK", 10000, false)) {
        Serial.println("AT Established");
        rxState = chk_sim_lock;
      }else{
        Serial.println("AT Error");
        rxState = chk_at_response;
      }
      break;
    case chk_sim_lock:
      if(command("AT+CPIN?", "+CPIN:READY", 8000, false)) {
        Serial.println("SIM UNLOCKED");
        rxState = configure_gps;
      }else{
        Serial.println("SIM PIN LOCKED");
        rxState = chk_sim_pin;
      }
      break;
    case chk_sim_pin:
      if(command("AT+CPIN?", "+CPIN:SIM PIN", 8000, true))  {
        Serial.println("SIM has a pin");
        rxState = try_sim_pin;
      }
      break;
    case try_sim_pin:
      if(command("AT+CPIN=\"2580\"", "OK", 8000, true))  {
        Serial.println("SIM pin entered");
        rxState = chk_sim_lock;
      }
      break; 
    case configure_gps:
      if(command("AT+GPS=1","OK", 8000, true)) {
        Serial.println("GPS CONFIGURED");
        rxState = chk_ntwrk_connection;
      }
      break;
    case chk_ntwrk_connection:
      if(command("AT+CREG?", "CREG: 1,1", 8000, false)) {
        
        Serial.println("connection already");
        rxState = chk_ntwrk_attached;
      }else{
        delay(3000);
        Serial.println("connection tried");
        rxState = set_auto_connection;
      }
      break;
    case set_auto_connection:
      if (command("AT+COPS=0", "OK", 60000, false)) {
        Serial.println("auto connection");
        rxState = chk_reg_network;
      }
      break;
    case chk_reg_network:
      if (command("AT+CREG?", "CREG: 1", 8000, false)) {
        Serial.println("NETWORK REGISTERED");
        rxState = chk_ntwrk_attached;
      }
      break;
    case chk_ntwrk_attached:
      if (command("AT+CGACT?", "+CGACT: 0,0", 8000, false)) {
        delay(2000);
        Serial.println("no network attached");
        rxState = attach_a_network;
      }
      break;
    case attach_a_network:
      if (command("AT+CGATT=1", "OK", 8000, false))  {
        Serial.println("network attached");
        rxState = chk_ntwrk_again;
      }
      break;  
    case chk_ntwrk_again:
      if (command("AT+CGATT?", "+CGATT:1", 8000, false)) {
        delay(7000);
        Serial.println("NETWORK ATTACHED");
        rxState = configure_apn;
      }
      break; 
    case configure_apn:
      if(command("AT+CSTT=\"internet\",\"\",\"\"", "OK", 8000, false)) {
        Serial.println("APN CONFIGURED");
        rxState = connect_gprs;
      }
      break; 
    case connect_gprs:
      if(command("AT+CIICR", "OK", 8000, false)){
        delay(6000);
        Serial.println("GPRS CONNECTED");
        //rxState = open_tcp_socket;
        rxState = get_sds_data;
      }
      break;
    case get_sds_data:
        // put your main code here, to run repeatedly:
        status_sds = sds.read(&pm25, &pm10);
        rxState = get_bme_data;
      break;
    case get_bme_data:
      // put your main code here, to run repeatedly:
      temp = bme.readTemperature();
      hum = bme.readHumidity();
      atm = bme.readPressure() / 100;
      rxState = open_tcp_socket;
      break;
    case open_tcp_socket:
      if(command("AT+CIPSTART=\"TCP\",\"0.tcp.ngrok.io\",18172", "CONNECT OK", 2000, true)){
        packet = 0;
        fPacket = 0;
        Serial.println("TCP SOCKET OPENED");
        rxState = send_tcp_data;
      }
      break;
    case send_tcp_data:
      message = "{ scooterId: 'C45ZA1', pm25: " + String(pm25) + ", pm10: " + String(pm10) + ", temp: " + String(temp) + ", hum: " + String(hum) + ", atm: " + String(atm) + " }";
      //if(command("AT+CIPSEND=138,\"{ scooterId: 'C45ZA1', pm25: 234, pm10: 110 }\"", "OK", 2000, false)){
      if(command("AT+CIPSEND=" + String(message.length()) + ",\"" + message + "\"", "OK", 2000, true)){
        delay(3000);
        rxState = tcp_data_seccessful;
      }else
        rxState = tcp_data_failure;
      packet++;
      break;
    case tcp_data_seccessful:
      Serial.println("TCP DATA SENT");
      rxState = close_tcp_socket;
      break;
    case tcp_data_failure:
      fPacket++;
      Serial.println("TCP DATA Failure");
      rxState = close_tcp_socket;
      break;
    case close_tcp_socket:
      //delay(500);
      //if(command("AT+CIPCLOSE", "OK", 2000, true)){
      if(command("AT+CIPSHUT", "OK", 2000, true)){
        delay(1000);
        Serial.println("TCP Socket Closed");
        //rxState = close_gprs_connection;
        if (packet==nPacket)
        {
          Serial.println("Packets:"+String(packet)+" Failed Packets:"+String(fPacket));
          packet = 0;fPacket = 0;
          //rxState = gps_data_logging;
          rxState = idle_state;
        }else
          rxState = open_tcp_socket;
      }
      break;
    case gps_data_logging:
      // check if time since last measurement is greater than sensor_interval
      if ((unsigned long)(millis() - previousMillis_sensor) >= SENSOR_INTERVAL) 
      {
        previousMillis_sensor = millis();
        
        // read from port 0, send to port 1:
        if (Serial.available()> 0) {
          int inByte = Serial.read();
          Serial.print(char(inByte+1));
        }
      }
      rxState = idle_state;
      break;
    case idle_state:
      break;
  } 

  /*if (a7g.available()) {
    Serial.print(char(a7g.read()));
  }*/
 /* if (Serial.available())
  {
    a7g.print(char(Serial.read()));
  }

    if ( gps.available() > 0) {
    Serial.print(char(gps.read()));
  }
*/
}

boolean command0(String cmd, String response, unsigned long timeout, boolean output) {
  //Serial.print("Sending: ");
  //Serial.println(cmd);

  while(a7g.available() > 0) {
    a7g.read(); //clear data
  }

  //a7g.print(cmd + "\r\n");

  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) {
    String result = a7g.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) {
      Serial.println("Received Data: ");
      Serial.println(result);
      Serial.println();
    }
    

    if (result.indexOf(response) > 0) {
      Serial.println("Response Fetched: " + response);
      return true;
    }
  }

  Serial.println("Response Failed." );

  return false;
}
boolean command(String cmd, String response, unsigned long timeout, boolean output) {
  //Serial.print("Sending: ");
  //Serial.println(cmd);

  while(a7g.available() > 0) {
    a7g.read(); //clear data
  }
  if(cmd != "NONE")
  a7g.print(cmd + "\r\n");

  unsigned long currentMillis = millis();

  while(currentMillis + timeout > millis()) {
    String result = a7g.readString();
    if (result == "" || result == "\n" || result == " " || result == "\t" || result == "\v" || result == "\f" || result == "\r") continue;

    if (output) {
      Serial.println("Received Data: ");
      Serial.println(result);
      Serial.println();
    }
    

    if (result.indexOf(response) > 0) {
      Serial.println("Command Executed: " + cmd + " - " + response);
      return true;
    }
  }

  Serial.println("Command Failed: " + cmd);

  return false;
}
