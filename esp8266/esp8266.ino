#include <ESP8266WiFi.h>
#include <Servo.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
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

const char* ssid = "Wi-Fi";
const char* password = "14423108";

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

unsigned long time1 = 0;

AsyncWebServer server(80);

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

  localserver();
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
  if(millis() - time1 > 5*60*1000){
    localserver();
    time1 = millis();
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

}

void localserver(){
  server.on("/states", HTTP_GET, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        String nam = p->name();
        String value = p->value();
        if(nam == "servo1") servo_angle[SERVO0] = value.toInt();
        if(nam == "servo2") servo_angle[SERVO1] = value.toInt();
        if(nam == "servo3") servo_angle[SERVO2] = value.toInt();
    }
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "application/javascript");
  });

  server.on("/servo1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(servo_angle[0]).c_str());
  });
  server.on("/servo2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(servo_angle[1]).c_str());
  });
  server.on("/servo3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(servo_angle[2]).c_str());
  });

  server.begin();
  }//localserver
