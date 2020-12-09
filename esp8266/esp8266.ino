#include <ESP8266WiFi.h>
#include <Servo.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "Modbus.h"
/* Servo includes */
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

WiFiServer MBserver(_ModbusTCP_port);
WiFiClient MBclient = MBserver.available();

IPAddress ip(192, 168, 0, 100);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

extern int16_t HoldReg[];
extern boolean modbusFlag;
extern long ReqInterval_ms;

const char* ssid = "**";
const char* password = "***";

/* Servo control start */

enum Servos
{
    SERVO0 = 0,
    SERVO1,
    SERVO2,
    SERVO3,
    SERVO4,
    SERVO5,
    SERVO_COUNTER // should be after SERVO defines
};

int servo_angle[SERVO_COUNTER] = {0,0,0,0,0,0};

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)

Adafruit_PWMServoDriver servo_pwm = Adafruit_PWMServoDriver();

/* Servo control end */

ESP8266WebServer server(80);

void setup() {
  Serial.begin(9600);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip,gateway,subnet);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/script.js", SPIFFS, "/script.js");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  for(int i = 0; i < 6; i++){
    server.on("/servo"+String(i), server.send(200, "text/plain", String(servo_angle[i])));
  }
  server.begin();
  
  MBserver.begin();

  servo_pwm.begin();
  servo_pwm.setOscillatorFrequency(27000000);
  servo_pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
}//setup

void loop(){
  MBtransaction();
  if(modbusFlag) {
    for(byte i = 0; i < SERVO_COUNTER; i++){
      servo_angle[i] = HoldReg[i+5];
    }
  }

  servo_pwm.setPWM(SERVO0, 0, getPulseLength(servo_angle[SERVO0], false) );
  servo_pwm.setPWM(SERVO1, 0, getPulseLength(servo_angle[SERVO1], false) );
} //loop

int getPulseLength(int value, boolean inverted){

  if( inverted != true){
    return map(value, 0, 180, SERVOMIN, SERVOMAX);
  } else {
    return map(value, 180, 0, SERVOMIN, SERVOMAX);
  }

}//getPulseLength

void argReceive(){
  String n = server.argName(0);
  for(int i = 0; i < 6; i++){
    if(n == ("servo"+String(i)) && (servo_angle[i] != server.arg("servo"+String(i)).toInt()))
      servo_angle = server.arg("servo"+String(i)).toInt();
  }
}//argReceive
