#include "dweetESP8266.h"
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h> // กรณีขึ้น Error ให้เอาบรรทัดนี้ออก
#define THIG_NAME  "my-thing-smaz"  // Put here your thing name
#define WIFISSID "Supod"
#define PASSWORD "palace66"
#define time_loop 60000   // Time per round
#define count_loop 2      // number of same response to start Notify
#define LINE_TOKEN "ezkvxSUO8aNZJRhJD5IA33joh8kG1wgHWxbZPsao23n"
String Last="";

dweet client;

void Line_Notify(String message) {

  axTLS::WiFiClientSecure clients;
  if (!clients.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  clients.print(req);
    
  delay(20);

  // Serial.println("-------------");
  while(clients.connected()) {
    String line = clients.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}

void setup() {
    Serial.begin(115200);
    delay(10);
    client.wifiConnection(WIFISSID, PASSWORD);
}

String value_c = "";
unsigned long m = millis();
int count = 1;
void loop() {
    // specifies the args of type "String"
    String value = client.getTimeDweet(THIG_NAME);
    Serial.println(value);
    if(value!="nan"){
      if(value_c=="") {
        value_c = value;
      } else {
        if(value_c==value && m-millis()>time_loop) {
          if(count>=count_loop) {
            if(Last!=value){
              Last = value;
              Serial.println("Line Notify...");
              Line_Notify("dweet my-thing-smaz หายไปจากระบบ");
            }
          } else {
            count++;
          }
        } else {
          Last="";
          value_c = value;
          count = 0;
          m = millis();
        }
      }
    }
    delay(3000); 
}
