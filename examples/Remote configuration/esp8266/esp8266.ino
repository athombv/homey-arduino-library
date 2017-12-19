#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>
#include <HomeyRemoteConfiguration.h>

unsigned long previousMillis = 0;
const unsigned long interval = 500; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  WiFi.begin("<ssid>", "<password>");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Homey.begin("Wemos D1 mini");

  rcEnable(); //Enable remote configuration

  Serial.println("READY.");
}

void loop() {
  Homey.loop();

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    rcTriggerRun(); //Run RC triggers
  }
}
