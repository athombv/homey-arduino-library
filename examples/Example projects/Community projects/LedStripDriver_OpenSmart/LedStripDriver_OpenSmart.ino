/*
   Homey Arduino library
   Usage example for use with Wemos D1 and Led Strip Driver (open-smart)

   Homey library: https://github.com/athombv/homey-arduino-library
   Driver for P9813 chip: ChainableRGB: https://github.com/pjpmarques/ChainableLED

   Hardware:
   - WeMos D1 mini V2
   - RGB LED Strip Driver Module Shield
   - Wemos ralais shield (optional)
   
*/

#include <ESP8266WiFi.h>        //ESP8266 library
#include <WiFiClient.h>         //ESP8266 library
#include <Homey.h>              //Athom Homey library
#include <ChainableLED.h>       //P9813 driver library

//configuration LED driver
#define NUM_LEDS  1   //number of leds (chips) in series
#define LED_PIN_C D3  //clock pin
#define LED_PIN_D D4  //data pin
#define RELAY_PIN D1 //relay shield

boolean ledOnOff = false;
float ledHue = 0;
float ledSaturation = 0;
float ledBrightness = 0;

ChainableLED leds(LED_PIN_C, LED_PIN_D, NUM_LEDS);

IPAddress ip;

//Arduino functions
void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to wifi...");

  //Connect to network
  WiFi.begin("your SSID", "your password");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(200); 
    Serial.print(".");
  }
  //Print IP address
  ip = WiFi.localIP();
  Serial.print("IP address: ");
  Serial.println(ip.toString());
    
  //Start Homey library
  Homey.begin("Arduino LedStrip");
  Homey.setClass("light");
  Homey.addCapability("onoff", setLightOnoff);                    //boolean 
  Homey.addCapability("dim", setLightDim);                        //number 0.00 - 1.00
  Homey.addCapability("light_hue", setLightHue);                  //number 0.00 - 1.00
  Homey.addCapability("light_saturation", setLightSaturation);    //number 0.00 - 1.00

  //initialise LED driver library
  leds.init();

  //define relais pin
  pinMode(RELAY_PIN, OUTPUT);
  
  Homey.setCapabilityValue("onoff", ledOnOff); //Set initial value
  Homey.setCapabilityValue("dim", ledBrightness); //Set initial value
  Homey.setCapabilityValue("light_hue", ledHue); //Set initial value
  Homey.setCapabilityValue("light_saturation", ledSaturation); //Set initial value

  Serial.println("Setup completed");
}

void loop() {
  Homey.loop();
}

void SendColorToLeds(){
  float tmpHue = mapfloat(ledHue, 0.00, 1.00, 1.00, 0.00);  
  float tmpBrightMax = mapfloat(ledSaturation, 0.00, 1.00, 1.00, 0.50);
  float tmpBrightness = mapfloat(ledBrightness, 0.00, 1.00, 0.00, tmpBrightMax);
  
  if(ledOnOff){
    for (byte i=0; i<NUM_LEDS; i++){
      leds.setColorHSB(i, tmpHue, ledSaturation, tmpBrightness); //led, hue, saturation, brightness);
      digitalWrite(RELAY_PIN,LOW);
    }
  }else{
    for (byte i=0; i<NUM_LEDS; i++){
      leds.setColorHSB(i, tmpHue, ledSaturation, 0.00); //led, hue, saturation, brightness);
      digitalWrite(RELAY_PIN,HIGH);
    }
  } 
}

//Callbacks which handle requests from Homey:

void setLightOnoff( void ) {
  ledOnOff = Homey.value.toInt(); //boolean
  SendColorToLeds();
}
void setLightHue( void ) {
  ledHue = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
}
void setLightDim( void ) {
  ledBrightness = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
}
void setLightSaturation( void ) {
  ledSaturation = Homey.value.toFloat(); //number 0.00 - 1.00
  SendColorToLeds();
}

//The default map function is only for int and long variables, so we define our own:

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
