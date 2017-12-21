 #include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>
#include <HomeyRemoteConfiguration.h>

//Device settings
byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 };
#define ID "Change me"

//No need to change anything below this line
unsigned long previousMillis = 0;
const unsigned long interval = 200; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ethernet...");
  Ethernet.begin(mac);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

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
