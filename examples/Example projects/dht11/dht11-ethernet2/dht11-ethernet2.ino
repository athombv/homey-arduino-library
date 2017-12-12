/*
 * Example project:
 * Arduino with ethernet 2 shield connected to a DHT11 temperature and humidity sensor
 *
 */

#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>
#include <SimpleDHT.h>

#define PIN_DHT 2

SimpleDHT11 dht11;

byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 };

unsigned long previousMillis = 0;
const unsigned long interval = 10000; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ethernet...");
  Ethernet.begin(mac);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  Homey.begin("DHT11 sensor");
  Homey.setClass("sensor");
  Homey.addCapability("measure_humidity");
  Homey.addCapability("measure_temperature");
  Serial.println("Started.");
}

void loop() {
  Homey.loop();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    updateSensor();
  }
}

void updateSensor() {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(PIN_DHT, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);
    return;
  }

  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" H");

  Homey.setCapabilityValue("measure_temperature", (int) temperature);
  Homey.setCapabilityValue("measure_humidity", (int) humidity);
}
