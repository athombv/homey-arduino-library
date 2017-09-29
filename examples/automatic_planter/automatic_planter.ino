/*
    Homeyduino example 1: The automatic planter

    Connections:
      - A pump is connected to pin 2 via a relay module or a mosfet
      - A float sensor is connected to pin A3. This sensor has a digital output (closed when not floating, open when floating)
      - Two water sensors connected to pins A0 and A1. These sensors have an analog output.
*/

#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>

//Pin definitions
#define PIN_PUMP      2   //Connected to mosfet / pump (digital output)
#define PIN_FLOAT     A3  //Connected to the float (digital input)
#define HUM_SENSOR_1  A0  //Connected to the first water sensor (analog input)
#define HUM_SENSOR_2  A1  //Connected to the second water sensor (analog input)

//Ethernet shield MAC address
byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 }; //(This address has to be unique on your network)

//Global variables used for the blink without delay example
unsigned long previousMillis = 0;   //The value of millis() the last time the sensor update routine was executed
const unsigned long interval = 200; //Interval in milliseconds between sensor updates

//Global variables used for keeping track of sensors
int pump_timer = 0;                 //Amount of sensor routine ticks before pump needs to be switched off
bool previousFloatState = false;    //The previous value of the float sensor
int previousSensor1Value = 0;       //The previous value of the first water sensor
int previousSensor2Value = 0;       //The previous value of the second water sensor

//Arduino functions
void setup() {
  //Set pin modes
  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_FLOAT, INPUT_PULLUP); //We're using the internal pullup of the atmega

  //Enable serial port
  Serial.begin(115200);

  //Connect to network
  Serial.println("Starting ethernet..."); //If this is the last message you see appear in the serial monitor...
  Ethernet.begin(mac);                    //... then you should check the network connection!

  //Print IP address
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  //Start Homey library
  Homey.begin("Plantenbak");

  //Register actions
  Homey.onAction("pump_on", onPumpOnAction);    //Turns the pump on
  Homey.onAction("pump_off", onPumpOffAction);  //Turns the pump off

  //Register conditions
  Homey.onCondition("s1_has_water", onSensor1HasWaterCondition);  //Checks the value of water sensor 1 against a supplied value
  Homey.onCondition("s2_has_water", onSensor2HasWaterCondition);  //Checks the value of water sensor 2 against a supplied value
  Homey.onCondition("has_water", onHasWaterCondition);            //Checks the flotter state. Returns true when there is water in the water tank

  previousFloatState = digitalRead(PIN_FLOAT); //Make sure not to trigger after boot

  Serial.println("Setup completed.");
}

void loop() {
  Homey.loop(); //Handle incoming connections

  //This is the 'blink without delay' code
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    bool currentFloatState = digitalRead(PIN_FLOAT); //Read the float sensor

    if (previousFloatState != currentFloatState) { //If changed
      previousFloatState = currentFloatState;
      Homey.emitBoolean("tank_level_changed", currentFloatState); //Emit change event
      if (!currentFloatState) { //If the tank has become empty stop the pump
        setPumpState(false);
      }
    }

    if (pump_timer) {
      pump_timer--;
      if (pump_timer == 0) {
        setPumpState(false);
      }
    }

    int currentSensor1Value = analogRead(HUM_SENSOR_1);
    if (currentSensor1Value != previousSensor1Value) {
      previousSensor1Value = currentSensor1Value;
      Homey.emitNumber("s1_water", currentSensor1Value);
    }

    int currentSensor2Value = analogRead(HUM_SENSOR_2);
    if (currentSensor2Value != previousSensor2Value) {
      previousSensor2Value = currentSensor2Value;
      Homey.emitNumber("s2_water", currentSensor2Value);
    }

  }
}

void setPumpState(bool state) {
  Homey.emitBoolean("pump_state_changed", state);
  digitalWrite(PIN_PUMP, state);
}

//Action: pump ON
void onPumpOnAction() {
  if (!digitalRead(PIN_FLOAT)) { //Check water float first
    return Homey.returnError("Could not start pump. Tank is empty!");
  }
  pump_timer = Homey.value.toInt();
  if (pump_timer == 0 || pump_timer > 180) { //Also check if input from flow is valid
    pump_timer = 0; //Make sure that the countdown doesn't start
    return Homey.returnError("Invalid pump timeout. Must be between 0 and 180");
  }
  pump_timer = (pump_timer * 1000) / interval; //Convert pump_timer from seconds into amount of routine ticks
  setPumpState(true); //Turn the pump on
}

//Action: pump OFF
void onPumpOffAction() {
  pump_timer = 0; //Stop the countdown
  setPumpState(false); //And turn the pump off
}

//Condition: Sensor 1 has enough water
bool onSensor1HasWaterCondition() {
  int threshold = Homey.value.toInt();  //Read the value received from Homey
  int value = analogRead(HUM_SENSOR_1); //Read the sensor value
  return value >= threshold;            //Return the boolean result of the comparison
}

//Condition: Sensor 2 has enough water
bool onSensor2HasWaterCondition() {
  int threshold = Homey.value.toInt();  //Read the value received from Homey
  int value = analogRead(HUM_SENSOR_2); //Read the sensor value
  return value >= threshold;            //Return the boolean result of the comparison
}

//Condition: there is enough water in the tank
bool onHasWaterCondition() {
  return digitalRead(PIN_FLOAT);        //Return the state of the float sensor
}
