#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>
#include <HomeyRemoteConfiguration.h>

//Device settings
#define SSID "<your wifi name>"
#define PASS "<your wifi password>"
#define ID "Change me"

//No need to change anything below this line
unsigned long previousMillis = 0;
const unsigned long interval = 200; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Homey.begin(ID);

  rcEnable(); //Enable remote configuration

  Serial.println("ready");
}

void loop() {
  Homey.loop();

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    rcTriggerRun(); //Run RC triggers
  }
}
