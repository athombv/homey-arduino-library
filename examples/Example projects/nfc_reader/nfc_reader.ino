#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>
#include <SPI.h>
#include "MFRC522.h"

/* PIN CONFIG */
#define MFRC522_RESET D3
#define MFRC522_SS    D8

/* OBJECTS */
MFRC522 mfrc522(MFRC522_SS, MFRC522_RESET);

/* GLOBAL VARIABLES */

unsigned long previousMillis = 0;
const unsigned long interval = 100; //Interval in milliseconds
String oldCardId = "";
uint8_t timeout = 0;

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

//Arduino functions
void setup() {
  Serial.begin(115200);
  Homey.begin("RFID");
  Homey.setClass("sensor");
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  wifi();
  Homey.loop();

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    if ( mfrc522.PICC_IsNewCardPresent() ) {
      mfrc522.PICC_ReadCardSerial();
      String cardId = "";
      for (uint8_t i = 0; i<mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i]<0x10) cardId += "0";
        cardId += String(mfrc522.uid.uidByte[i], HEX);
      }
      if (cardId!=oldCardId) { //Check if we didn't already see this card
        oldCardId = cardId; //Store the card id
        Serial.println(cardId); //Print the card id to the serial monitor
        Homey.trigger("card", cardId); //Send the card id to Homey
        timeout=50; //Allow the same card to be seen again after 5 seconds (50*100ms)
      }
    } else {
      if (timeout>0) {
        timeout--;
        if (timeout==0) oldCardId=""; //Remove previous card ID
      }
    }
  }
}
