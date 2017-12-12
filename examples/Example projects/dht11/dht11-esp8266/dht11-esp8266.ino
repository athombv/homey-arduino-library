/*
 * Example project:
 * NodeMCU board connected to a DHT11 temperature and humidity sensor
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>
#include <SimpleDHT.h>

#define PIN_DHT D5

SimpleDHT11 dht11;

unsigned long previousMillis = 0;
const unsigned long interval = 10000; //Interval in milliseconds

void wifi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin("<SSID>", "<PASSWORD>");
    uint8_t timeout = 30;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if (timeout<1) break;
    }
    if (WiFi.status() == WL_CONNECTED) {
      //Print IP address
      Serial.print("Connected to WiFi! (");
      Serial.print(WiFi.localIP());
      Serial.println(")");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Homey.begin("DHT11 sensor");
  Homey.setClass("sensor");
  Homey.addCapability("measure_humidity");
  Homey.addCapability("measure_temperature");
}

void loop() {
  wifi();
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
