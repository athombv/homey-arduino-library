#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <homey.h>

//Objects
Homey homey("my_sensor");

//Arduino functions
void setup() {
  Serial.begin(115200);  
  WiFi.begin("put-your-ssid-here", "put-your-password-here");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); } //Wait for WiFi to be connected

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  homey.begin();
  homey.on("my_command", onMyCommand);
  homey.on("other_command", onOtherCommand);
  
  Serial.println("Started!");
}

void loop() {
  homey.loop();
}

int onMyCommand(int data) {
  return data * 5;
}

String onOtherCommand(float data) {
  return "Here is your float converted to string: "+String(data);
}
