#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>

#define PIN_PUMP 2
#define PIN_FLOAT 3
#define PIN_SENSOR A0

byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 };

unsigned long previousMillis = 0;
uint8_t pumpTimer = 0;
bool previousFloatState = false;
int previousMoistureSensorValue = 0;

void setup() {
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_PUMP, INPUT_PULLUP);
  pinMode(PIN_SENSOR, INPUT);

  Serial.begin(115200);

  Serial.println("Starting ethernet..."); //If this is the last message you see appear in the serial monitor...
  Ethernet.begin(mac);

  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  Homey.begin("planter");
  Homey.addAction("pump", onPump);
  Homey.addAction("stop", onStop);
  Homey.addCondition("float", onFloatCondition);
}

void onPump() {
  pumpTimer = Homey.value.toInt(); //Set the timer

  if ( (pumpTimer<1) || (pumpTimer>30) ) {
    onStop(); //Stop the pump
    return Homey.returnError("Invalid value!");
  }

  if (!previousFloatState) return Homey.returnError("Tank empty!");

  Serial.print("Pumping for ");
  Serial.print(pumpTimer);
  Serial.println(" seconds...");

  if (pumpTimer>0) digitalWrite(2, HIGH); //Start the pump
}

void onStop() {
  pumpTimer = 0;
  digitalWrite(PIN_PUMP, LOW);
  Serial.println("Pump stopped manually.");
}

void loop() {
  Homey.loop();

  unsigned long currentMillis = millis();

  if (currentMillis-previousMillis >= 1000) { //Code in this if statement is run once every second
    previousMillis = currentMillis;

    if (pumpTimer>0) { //If the pump is active
      pumpTimer = pumpTimer - 1; //Decrease the pump timer
      if (pumpTimer < 1) { //once the timer reaches zero
        digitalWrite(PIN_PUMP, LOW); //Turn off the pump
        Serial.println("Pump stopped.");
      } else {
        Serial.print("Pumping for ");
        Serial.print(pumpTimer);
        Serial.println(" more seconds...");
      }
    }

    bool currentFloatState = digitalRead(PIN_FLOAT);

    if (previousFloatState != currentFloatState) {
      previousFloatState = currentFloatState;
      Homey.trigger("float", currentFloatState);
      if (!currentFloatState) { //Tank empty
        onStop();
      }
    }

    bool currentMoistureSensorValue = analogRead(PIN_SENSOR);

    if (previousMoistureSensorValue != currentMoistureSensorValue) {
      previousMoistureSensorValue = currentMoistureSensorValue;
      Homey.trigger("moisture", previousMoistureSensorValue);
    }
  }
}

void onFloatCondition() {
  return Homey.returnResult(digitalRead(3));
}

void onMoistureCondition() {
  int compareTo = Homey.value.toInt();
  return Homey.returnResult(previousMoistureSensorValue>=compareTo);
}
