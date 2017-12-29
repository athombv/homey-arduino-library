/*
   Homey Arduino library
   Usage example for use with Wemos D1 and Led Strip Driver (open-smart)

   Homey library: https://github.com/athombv/homey-arduino-library
   Driver for P9813 chip: ChainableRGB: https://github.com/pjpmarques/ChainableLED

   Hardware:
   - WeMos D1 mini V2
   - RGB LED Strip Driver Module Shield
   
*/

#include <ESP8266WiFi.h>        //ESP8266 library
#include <WiFiClient.h>         //ESP8266 library
#include <Homey.h>              //Athom Homey library
#include <ChainableLED.h>       //P9813 driver library

//just for debugging to serial.print: use DEBUG_PRINT instead of Serial.print
//exclude Serial.print (x) for non debug like: #define DEBUG_PRINT(x) //Serial.piint(x)
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTLN(x)   Serial.println (x)


//configuration LED driver
#define NUM_LEDS  1   //number of leds (chips) in series
#define LED_PIN_C D3  //clock pin
#define LED_PIN_D D4  //data pin
boolean ledOnOff = false;
float ledHue = 0;
float ledSaturation = 0;
float ledBrightness = 0;

//define relais
#define RELAIS_PIN D2 //activate relais


ChainableLED leds(LED_PIN_C, LED_PIN_D, NUM_LEDS);

//Global variables used for the blink without delay example
unsigned long previousMillis = 0;
const unsigned long interval = 1000; //Interval in milliseconds

IPAddress ip;

//Arduino functions
void setup() {
  //Enable serial port
  Serial.begin(115200);
  DEBUG_PRINTLN("Connect to wifi");

  //Connect to network
  WiFi.begin("your ssid", "your password");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(200); 
    DEBUG_PRINT(".");
  }
  //Print IP address
  ip = WiFi.localIP();
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(ip.toString());
    
  //Start Homey library
  Homey.begin("Arduino LedStrip");
  Homey.setClass("light");
  Homey.addCapability("onoff", setLightOnoff);                    //boolean 
  Homey.addCapability("dim", setLightDim);                        //number 0.00 - 1.00
  Homey.addCapability("light_hue", setLightHeu);                  //number 0.00 - 1.00
  Homey.addCapability("light_saturation", setLightSaturation);    //number 0.00 - 1.00

  //set Led status back to Homey
  Homey.setCapabilityValue("onoff", ledOnOff); //this will not work, I know now why

  //initialise LED driver library
  leds.init();

  //relais define
  pinMode(RELAIS_PIN, OUTPUT);
  
  DEBUG_PRINTLN("Setup completed");
}

void loop() {
  //Handle incoming connections
  Homey.loop();

  //This is the 'blink without delay' code
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    //(This code will be executed every <interval> milliseconds.)

  }
}



void SendColorToLeds(){
  DEBUG_PRINT("SendColorToLeds: ");

  float tmpHue = mapfloat(ledHue, 0.00, 1.00, 1.00, 0.00);  //changing blue and green
  //saturation determind then maximum value of brightness
  float tmpBrightMax = mapfloat(ledSaturation, 0.00, 1.00, 1.00, 0.50);
  float tmpBrightness = mapfloat(ledBrightness, 0.00, 1.00, 0.00, tmpBrightMax);
  
  if(ledOnOff){
    for (byte i=0; i<NUM_LEDS; i++){
      leds.setColorHSB(i, tmpHue, ledSaturation, tmpBrightness); //led, hue, saturation, brightness);
      digitalWrite(RELAIS_PIN,LOW);
      DEBUG_PRINT(" setOn ");
    }
  }else{
    for (byte i=0; i<NUM_LEDS; i++){
      leds.setColorHSB(i, tmpHue, ledSaturation, 0.00); //led, hue, saturation, brightness);
      digitalWrite(RELAIS_PIN,HIGH);
      DEBUG_PRINT(" setOff ");
    }
  }

  DEBUG_PRINT("ledOnOff["); 
  DEBUG_PRINT(ledOnOff);
  DEBUG_PRINT("] ledHue["); 
  DEBUG_PRINT(tmpHue);
  DEBUG_PRINT("] ledSaturation["); 
  DEBUG_PRINT(ledSaturation);
  DEBUG_PRINT("] ledBrightness["); 
  DEBUG_PRINT(tmpBrightness);
  DEBUG_PRINTLN("]");
  
}

  
//handel Homey calls
void setLightOnoff( void ) {
  ledOnOff = Homey.value.toInt(); //boolean
  SendColorToLeds();
  return Homey.returnResult("OK"); //or just return;
  //Homey.returnError("Failed :-(");
}
void setLightHeu( void ) {
  ledHue = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
  return Homey.returnResult("OK"); //or just return;
  //Homey.returnError("Failed :-(");
}
void setLightDim( void ) {
  ledBrightness = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
  return Homey.returnResult("OK"); //or just return;
  //Homey.returnError("Failed :-(");
}
void setLightSaturation( void ) {
  ledSaturation = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
  return Homey.returnResult("OK"); //or just return;
  //Homey.returnError("Failed :-(");
}


//default map funtion is only for int and long
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


