#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>

//Pins
#define LED_RED     9
#define LED_YELLOW  8
#define LED_GREEN   7
#define LED_BLUE    6

#define BUTTON_RED    5
#define BUTTON_YELLOW 4
#define BUTTON_GREEN  3
#define BUTTON_BLUE   2

//Settings
byte mac[] = { 0x58, 0x6F, 0x6D, 0x63, 0x29, 0x00 };
const unsigned long interval = 20;

//Global variables
String patternQueue = "";
bool patternState = false;

unsigned long previousMillis = 0;

int patternInterval = 10;
int patternIntervalPosition = 0;

bool prevKnop1 = false;
bool prevKnop2 = false;
bool prevKnop3 = false;
bool prevKnop4 = false;

//Functions
void setup() {
  //Configure GPIO
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);

  //Configure serial debug interface
  Serial.begin(115200);

  //Start ethernet
  Serial.println("Starting ethernet...");
  Ethernet.begin(mac);
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());

  //Start Homeyduino
  Homey.begin("SimonSays");
  Homey.addAction("showPattern", showPattern);
}

void showPattern() {
  //this is the callback function called by Homeyduino whenever the "showPattern"
  //action is executed in a Homey flow. This function just queues the pattern, so
  //that it can be displayed later.
  patternQueue = Homey.value;
}

void runPatternStep() {
  //This function reads the first character on the pattern queue and lights up
  //the LED corresponding to the character on the queue. The next time it is
  //run the patternState variable being set causes the function to turn off
  //all leds, while removing the first character of the queue.
  if (patternQueue.length()>0) {
    if (patternState) {
      patternQueue.remove(0,1);
      patternState = false;
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
    } else {
      char color = patternQueue.charAt(0);
      Serial.println(patternQueue);
      if (color=='R') {
        digitalWrite(LED_RED, HIGH);
      } else if (color=='Y') {
        digitalWrite(LED_YELLOW, HIGH);
      } else if (color=='G') {
        digitalWrite(LED_GREEN, HIGH);
      } else if (color=='B') {
        digitalWrite(LED_BLUE, HIGH);
      }
      patternState = true;
    }
  }
}

void loop() {
  Homey.loop();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    patternIntervalPosition++;
    if (patternIntervalPosition>=patternInterval) {
      runPatternStep();
      patternIntervalPosition = 0;
    }
    readButtons();
  }
}

void readButtons() {
  bool knop1 = !digitalRead(BUTTON_RED);
  bool knop2 = !digitalRead(BUTTON_YELLOW);
  bool knop3 = !digitalRead(BUTTON_GREEN);
  bool knop4 = !digitalRead(BUTTON_BLUE);
  if (knop1!=prevKnop1) {
    prevKnop1 = knop1;
    Homey.trigger("knop1", knop1);
    digitalWrite(LED_RED, knop1);

  }
  if (knop2!=prevKnop2) {
    prevKnop2 = knop2;
    Homey.trigger("knop2", knop2);
     digitalWrite(LED_YELLOW, knop2);
  }
  if (knop3!=prevKnop3) {
    prevKnop3 = knop3;
    Homey.trigger("knop3", knop3);
    digitalWrite(LED_GREEN, knop3);
  }
  if (knop4!=prevKnop4) {
    prevKnop4 = knop4;
    Homey.trigger("knop4", knop4);
    digitalWrite(LED_BLUE, knop4);
  }
}
