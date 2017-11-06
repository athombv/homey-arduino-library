 #include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>
#include <HomeyRc.h>

byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 };

unsigned long previousMillis = 0; 
const unsigned long interval = 500; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting ethernet...");
  Ethernet.begin(mac);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  Homey.begin("RCtest");

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

