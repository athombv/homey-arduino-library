/*
 * Homeyduino firmware for Sonoff Basic
 * 
 * WARNING:
 * Never try to program a Sonoff which is connected to the mains! Failure to
 * disconnect mains before connecting the usb to serial converter will cause
 * damage to you and your equipment.
 * 
 * Always power the Sonoff using an external 3.3v source while programming.
 * 
 */

// -- CONFIGURATION --------------------------------------------------------------

/* Comment out (//) lines to disable the feature */

#define BUTTON_SWITCHES_OUTPUT //Have the button toggle the relay directly
                               //(if disabled the button will only send a trigger)

#define LED_SHOWS_OUTPUT_STATE //Have the led show the state of the relay
                               //(if disabled led can be controlled using an action)

//--------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Homey.h>

//GPIO map
#define PIN_BUTTON  0
#define PIN_LED     13
#define PIN_RELAY   12

WiFiManager wifiManager;

bool state = false;
bool previousButtonState = false;
unsigned long previousMillis = 0;
const unsigned long interval = 100; //Interval in milliseconds

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BUTTON, INPUT); //Set button pin to input
  pinMode(PIN_LED, OUTPUT);   //Set led pin to output
  pinMode(PIN_RELAY, OUTPUT); //Set relay pin to output
  
  digitalWrite(PIN_LED, LOW); //Turn led on
  digitalWrite(PIN_RELAY, LOW); //Turn output off
  
  String deviceName = "sonoff-" + String(ESP.getChipId()); //Generate device name based on ID

  Serial.println("wifiManager...");
  wifiManager.autoConnect(deviceName.c_str(), ""); //Start wifiManager
  Serial.println("Connected!");
  
  Homey.begin(deviceName, "sonoff"); //Start Homeyduino
  Homey.setClass("socket");
  Homey.addCapability("onoff", setState);
  Homey.addAction("output", setState);
  #ifndef LED_SHOWS_OUTPUT_STATE
    Homey.addAction("led", setLed);
  #endif
  Homey.addCondition("state", getState);

  digitalWrite(PIN_LED, HIGH); //Turn led off
}

void loop() {
  Homey.loop();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    button();
  }
}

void setState() {
  state = Homey.value.toInt();
  applyState();
}

void setLed() {
  bool led = Homey.value.toInt();
  digitalWrite(PIN_LED, !led); //Write to led, output is inverted
}

void applyState() {
  digitalWrite(PIN_RELAY, state);
  #ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
  #endif
  Serial.println("applyState(): new state is "+String(state));
  Homey.setCapabilityValue("onoff", state);
  Homey.trigger("state", state);
}

void getState() {
  Serial.println("getState(): state is "+String(state));
  return Homey.returnResult(state);
}

void button() {
  bool currentButtonState = !digitalRead(PIN_BUTTON);
  if (currentButtonState!=previousButtonState) {
    currentButtonState = previousButtonState;
    Serial.println("button(): button is "+String(currentButtonState));
    Homey.trigger("button", currentButtonState);
    #ifdef BUTTON_SWITCHES_OUTPUT
      Serial.println("button(): toggle output");
      if (currentButtonState) {
        state = !state;
        applyState();
      }
    #endif
  }
}

