#include <ESP8266WiFi.h>
#include <Servo.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ESP8266WebServer.h>
#include "Modbus.h"

WiFiServer MBserver(_ModbusTCP_port);
WiFiClient MBclient = MBserver.available();

IPAddress ip(192, 168, 0, 100);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

extern int16_t HoldReg[];
extern boolean flag;
extern long ReqInterval_ms; 

const char* ssid = "Wi-Fi";
const char* password = "14423108";

const char* ssid     = "Wi-Fi";
const char* password = "14423108";

const byte servoPin1 = 12; //6
const byte servoPin2 = 13; //7
const byte servoPin3 = 15; //8

int pos[3] = 0;

Servo servo1;
Servo servo2;
Servo servo3;

unsigned long time1 = 0;

AsyncWebServer server(80);

void setup() {
  Serial.begin(9600);
  
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
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
}//setup

void loop(){
  if(millis() - time1 > 5*60*1000){
    localserver();
    time1 = millis();
  }
  servo1.write(pos[0]);
  servo2.write(pos[1]);
  servo3.write(pos[2]);
    
} //loop
void localserver(){
  server.on("/states", HTTP_GET, [](AsyncWebServerRequest *request){
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        String nam = p->name();
        String value = p->value();
        if(nam == "servo1") pos[0] = value.toInt();
        if(nam == "servo2") pos[1] = value.toInt();
        if(nam == "servo3") pos[2] = value.toInt();
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
    request->send_P(200, "text/plain", String(pos[0]).c_str());
  });
  server.on("/servo2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(pos[1]).c_str());
  });
  server.on("/servo3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(pos[2]).c_str());
  });
  
  server.begin();
  }//localserver
