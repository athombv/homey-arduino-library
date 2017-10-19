/*
   Homey Arduino library
   Usage example for use with ESP32

   Most of the code in this file comes from the ethernet usage examples included with Arduino
   Also the code at https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay has been included to
   show you how to read sensors and emit triggers without using the delay function.

*/

#include <WiFi.h>
#include <WiFiClient.h>

#include <Homey.h>

//Global variables used for the blink without delay example
unsigned long previousMillis = 0;
const unsigned long interval = 500; //Interval in milliseconds

//Arduino functions
void setup() {
  //Enable serial port
  Serial.begin(115200);

  //Connect to network
  WiFi.begin("<YOUR SSID>", "<YOUR PASSWORD>");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  //Print IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //Start Homey library
  Homey.begin("esp32example");
  /* Note:
     The name of each Arduino on the network needs to be unique
     since it will be used as the unique identifier for your device.
     The name is stored as a String and can thus be as long as you
     desire it to be.
  */

  //Register an example action
  Homey.addAction("myaction", onExampleAction);

  //Register an example condition
  Homey.addCondition("mycondition", onExampleCondition);

  /* Note:
   *  Names of actions and conditions can be at most 16 characters long.
   *  Names of both actions and conditions have to be unique on this device,
   *  which means that there can not be a condition and an action with the
   *  same name on the same device.
   *  
   *  An action is a function which returnes 'void'. While a condition is a function
   *  which returns a boolean ('bool').
   */

  Serial.println("Started.");
}

void loop() {
  //Handle incoming connections
  Homey.loop();
  /* Note:
   *  The Homey.loop(); function needs to be called as often as possible.
   *  Failing to do so will cause connection problems and instability.
   *  Avoid using the delay function at all times. Instead please use the
   *  method explaind on the following page on the Arduino website:
   *  https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
   */

  //This is the 'blink without delay' code
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    //(This code will be executed every <interval> milliseconds.)

    //Emit a trigger to Homey
    bool success = Homey.trigger("mytrigger", "Hello world");
    /* Note:
     *  The first argument to the emit function is the name of the trigger  
     *  this name has to match the name used in the flow editor
     *  
     *  The second argument to the emit function is the argument.
     *  An argument can be one of the following:
     *     - a string
     *     - an integer (int)
     *     - a floating point number (float or double),
     *     - a boolean (bool)
     *     - nothing (void)
     *  
     *  Make sure to select the right type of flow card to match the type
     *  of argument sent to Homey.
     *  For a string argument the "text" flowcard is used.
     *  For an integer or floating point number the "number" flowcard is used.
     *  For the boolean argument the "boolean" flowcard is used.
     *  And when no argument is supplied the flowcard without argument is used.
     *  
     */

    //And print the result to the serial terminal
    Serial.print("Result: ");
    if (success) {
      Serial.println("success");
    } else {
      Serial.println("failure");
    }
    /* Note:
     *  At first you will see "Result: failure" a lot in the serial monitor
     *  This is expected behaviour when the device has not yet been paired with 
     *  a Homey.
     */
    
  }
}

//An example action
void onExampleAction() {
  //Read the argument sent from the homey flow
  String value = Homey.value;

  //And print it to the serial terminal
  Serial.print("Example action: ");
  Serial.println(value);

  /* Note:
   *  
   *  The argument will always be received as a String.
   *  If you sent a number or boolean from homey then you can convert
   *  the value into the type you want as follows:
   *  
   *   - Integer number: "int value = Homey.value.toInt();"
   *   - Floating point number: "float value = Homey.value.toFloat();"
   *   - Boolean: "bool value = Homey.value.toInt();"
   *   - String: "String value = Homey.value;"
   *  
   * In case something goes wrong while executing your action
   * you can return an error to the Homey flow by calling
   * Homey.returnError("<message>");
   */
}

//An example condition
void onExampleCondition() {
  //Read the argument sent from the homey flow
  int value = Homey.value.toInt();

  //And print it to the serial terminal
  Serial.print("Example condition: ");
  Serial.println(value);

  bool result = false;

  if (value > 5) {
    result = true;
  }

  //Return the result to the Homey flow
  return Homey.returnResult(result);
}
