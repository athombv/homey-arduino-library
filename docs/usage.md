# Usage & getting started

This guide will help you to get started with using the Homeyduino Arduino library and application.

## Installation
...

## The basics
Now that everything is installed and ready for use let's get started with the basics.

To be able to use the library into your Arduino sketch you have to include the header file of the Homey arduino library by adding ```#include <Homey.h>``` to the top of your Arduino sketch.

To actually use the library you will have to start the library by adding ```Homey.begin("devicename");``` to the `setup()` function of your sketch. It is important to keep in mind that the name entered here needs to be unique for all Homeyduino devices in your network. It is possible to use for example part of the MAC address in the name to make it unique. Another method would be to store the unique name of your device in the EEPROM storage available on your board. Examples for doing this are included with the library.

Once the library is started it will be able to receive communication from Homey, however to actually handle the received packets you have to add a call to ```Homey.loop();``` to the ```loop()``` function of your sketch. This function needs to be executed as often as possible, therefore use of Arduino's ```delay()``` function should be avoided at all cost. Alternative delay methods are described in later sections of this guide.

Depending on your target platform (Ethernet shield or ESP8266/ESP32) you will also need some code to establish a network connection, since that connection is required to allow your project to communicate with Homey.

When using an ethernet shield on an Arduino board you will have to include the ethernet library. Also you will need to define the MAC address that your device will use. A MAC address is a globally unique identifier used to identify ethernet devices. More information on how to get started with the ethernet shield can be found on the [Arduino website](https://www.arduino.cc/en/Guide/ArduinoEthernetShield).

Example showing minimal required code for using Homeyduino with the ethernet shield 2 on an Arduino board:
```cpp
#include <SPI.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <Homey.h>

byte mac[] = { 0x48, 0x6F, 0x6D, 0x65, 0x79, 0x00 };
const char* name = "example";

void setup() {
  Ethernet.begin(mac);
  Homey.begin(name);
}

void loop() {
  Homey.loop();
}
```

When using an ESP8266 based board with Homeyduino you will have to include the Wifi libraries and provide network credentials.

A minimal example for using Homeyduino with an ESP8266 based board:

```cpp
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>

const char* ssid = "example";
const char* password = "example";
const char* name = "example";

void setup() {
  WiFi.begin(ssid, password);
  Homey.begin(name);
}

void loop() {
  Homey.loop();
}
```
For ESP32 only the included libraries change:

```cpp
#include <WiFi.h>
#include <WiFiClient.h>
#include <Homey.h>

const char* ssid = "example";
const char* password = "example";
const char* name = "example";

void setup() {
  WiFi.begin(ssid, password);
  Homey.begin(name);
}

void loop() {
  Homey.loop();
}
```

To allow for easier testing we suggest you add code to utilise the ```Serial``` library to print a log to Arduino's serial monitor. More advanced getting-started sketches are included with the library and can be opened in the Arduino IDE by navigating to the  ```File > Examples > Homey``` menu.

### Using the library

Your device can be integrated with Homey flows by defining triggers, conditions and actions which will be available directly in your Homey flows. It is also possible to define capabilities. Using capabilities is a a bit more complicated than directly defining flow cards but it allows you to integrate your device with global flow cards and speech commands.

#### Triggers
Triggers are used to start a Homey flow. A trigger can be sent to homey by running ```Homey.trigger("example");``` or by running ```Homey.trigger("example", value);``` where value can be a ```String```, ```bool```, ```int```, ```float``` or ```double``` type.

Corresponding Homey flow cards are the ```Trigger``` for triggers that do not provide a value, ```Trigger [text]``` for triggers that provide a ```String``` value, ```Trigger [boolean]``` for triggers that provide a ```bool``` value,  and last ```Trigger [number]``` for triggers that provide an ```int```, ```float``` or ```double``` type.

Homey only knows of a triggers existence after it has seen the trigger at least once. As such any name can be entered into the trigger flowcards, but triggers are only added to the autocomplete after it has been seen.

Alternatively the ```All triggers``` flowcard can be used. This card triggers when any trigger is received. The name of the received trigger is available as a tag, as well as the value cast to text, number and boolean.


#### Conditions and actions

Conditions and actions can be registered by calling ```Homey.addAction("name", <function>);``` or ```Homey.addCondition("name", <function>);```.

Actions and conditions function almost the same way: when an action or condition card gets executed the Homey flow will call the function provided while adding the action or condition to your sketch.

From within the action or condition function you can return an error by calling ```return Homey.returnError("my error message");```. For an action if no error occurs then it it not needed to return a result. However a condition is expected to return a boolean result by calling ```Homey.returnResult(<bool>);```, to either continue the flow or to continue the else part of the flow.

#### Capabilities
If your device is of a certain class then you can set the class by calling ```Homey.setClass("<class>");``` from within ```setup()```. By default the class is set to "other", which means that none of the special features that certain classes provide are enabled.

Adding a capability is done by calling ```Homey.addCapability("name");``` or ```Homey.addCapability("name", <function>);```. Depending on the type of capability. For example the ```measure_temperature``` does not require a callback function since it can not be set by Homey.

Setting the value of a capability is done by calling ```Homey.setCapabilityValue("name", <value>);```. Where value can be of a ```String```, ```bool```, ```int```, ```float``` or ```double``` type, depending on the type of capability.

# Remote configurations

Instead of creating your own sketch it is also possible to configure your Arduino from within the Homey interface. For this to work you will need to flash your board with the provided remote-configuration sketch, which can be found in the examples menu.

After flashing the remote configuration sketch pins can be configured to one of the following modes:

| Mode                                   | Description                                                                                                                                                                     |
|----------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Digital input                          | A basic digital input pin, can be interacted with using the ```Digital input``` condition flowcard                                                                              |
| Digital input with pull-up             | Same as ```Digital input``` but with the built-in pull-up resistor enabled                                                                                                      |
| Digital input with trigger             | Same as ```Digital input``` but when the pin changes a trigger will be sent so that a flow can be triggerd with the Digital input trigger flowcard                              |
| Digital input with pull-up and trigger | Same as above but with both pull-up and trigger functionality enabled                                                                                                           |
| Digital output                         | A basic digital output, can be interacted with using the ```Digital output``` action flowcard                                                                                   |
| Analog input                           | A basic Analog input pin, can be interacted with using the ```Analog input``` condition flowcard                                                                                |
| Analog input with pull-up              | Same as ```Analog input``` but with the built-in pull-up resistor enabled                                                                                                       |
| Analog input with trigger              | Same as ```Analog input``` but when the pin changes a trigger will be sent so that a flow can be triggerd with the Analog input trigger flowcard                                |
| Analog input with pull-up and trigger  | Same as above but with both pull-up and trigger functionality enabled                                                                                                           |
| Analog output                          | An analog output, can be interacted with using the ```Analog output``` action flowcard. Depending on hardware capabilities this pin will be either a PWM output or a DAC output |
