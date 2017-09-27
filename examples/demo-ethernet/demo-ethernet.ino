#include <SPI.h>
#include <Ethernet.h>
#include <homey.h>

byte mac[] = { 0xFE, 0xED, 0xDE, 0xAD, 0xBE, 0xEF };

//Objects
Homey homey("my_sensor");

//Arduino functions
void setup() {
  Serial.begin(115200);  
  Ethernet.begin(mac);

  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  
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

