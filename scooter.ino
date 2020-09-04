//first state machine based example for scooter

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


#include <SoftwareSerial.h>
#include <HardwareSerial.h>

HardwareSerial gps(1);
HardwareSerial a7g(2);


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
#define close_gprs_connection 21


boolean command0(String cmd, String response, unsigned long timeout, boolean output);
boolean command(String cmd, String response, unsigned long timeout, boolean output);

unsigned char rxState;
unsigned long packet;
void setup() {
  delay(1000);
  pinMode(PWR_KEY_PIN, OUTPUT);
 // pinMode(RESET_PIN, OUTPUT);
  //digitalWrite(RESET_PIN, HIGH);
  digitalWrite(PWR_KEY_PIN, HIGH);
  
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(baudrate);
  delay(500); 
  //gps.begin(9600);
  gps.begin(baudrate, SERIAL_8N1, GPSRX, -1);
  delay(500); 
  a7g.begin(baudrate, SERIAL_8N1, A9RX, A9TX);
  delay(500); 
  
  Serial.println("ScooterAQNET Started....");
  
  
  digitalWrite(PWR_KEY_PIN, LOW);
  delay(3000);
  digitalWrite(PWR_KEY_PIN, HIGH);

  Serial.println("A9G PWR KEY asserted....");
 //rxState = get_a9g_init_msg;//a9g
 rxState = get_a9g_init_msg;//a7

}
 boolean rsp;
void loop() 
{
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
      if (command("AT+CREG?", "CREG: 1,13", 8000, false)) {
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
        rxState = open_tcp_socket;
      }
      break;
    case open_tcp_socket:
      if(command("AT+CIPSTART=\"TCP\",\"0.tcp.ngrok.io\",17255", "CONNECT OK", 2000, false)){
        packet = 0;
        Serial.println("TCP SOCKET OPENED");
        rxState = send_tcp_data;
      }
      break;
    case send_tcp_data:
      if(command("AT+CIPSEND=138,\"{ scooterId: 'C45ZA1', pm25: 234, pm10: 110 }\"", "OK", 2000, false)){
        rxState = tcp_data_seccessful;
      }else
        rxState = tcp_data_failure;
      packet++;
      break;
    case tcp_data_seccessful:
      Serial.println("TCP DATA SENT");
      //if (packet == 1) 
        rxState = close_tcp_socket;
      //else
      //  rxState = send_tcp_data;
      break;
    case tcp_data_failure:
      Serial.println("TCP DATA Failure");
      //if (packet == 1) 
        rxState = close_tcp_socket;
      //else
      //  rxState = send_tcp_data;
      break;
    case close_tcp_socket:
      delay(1000);
      if(command("AT+CIPCLOSE", "OK", 2000, true)){
        Serial.println("TCP Socket Closed");
        
        packet=0;
        //rxState = close_gprs_connection;
        rxState = open_tcp_socket;
      }
      break;
    case close_gprs_connection:
      //if(command("AT+CIPSHUT", "OK", 4000, true)){
       // Serial.println("GPRS Connection Closed");
        rxState = idle_state;
      //}
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
