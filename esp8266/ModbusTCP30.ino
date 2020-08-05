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


void setup(){
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip,gateway,subnet);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  MBserver.begin();
}//setup

void loop(){
  MBtransaction();
  if(flag) Serial.println(HoldReg[5]);
  delay(300);

}//loop
