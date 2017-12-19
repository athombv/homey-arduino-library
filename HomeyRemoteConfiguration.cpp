#include "HomeyRemoteConfiguration.h"

rcTrigger* firstRcTrigger = NULL;

/* Remote configuration */
uint8_t rcMapPin(String pin)
{
	if (!isDigit(pin.charAt(0))) {
		//First character is NOT a digit
		if ((pin.charAt(0)=='A')||(pin.charAt(0)=='a')) {
			pin.remove(0,1); //Remove first character
			uint8_t p = pin.toInt();
			if (p>=NUM_ANALOG_INPUTS) {
				DEBUG_PRINTLN("invalid analog pin");
				Homey.returnError("invap");
				return 255;
			}
			return analog_input_map[p]; //Map Ax to corresponding digital pin number
		}
		#if defined(_VARIANT_ARDUINO_DUE_X_) //Hack for supporting the special pins on Arduino Due
			else if(pin=="DAC0") {
				return DAC0;
			} else if(pin=="DAC1") {
				return DAC1;
			} else if(pin=="CANRX") {
				return CANRX;
			} else if(pin=="CANTX") {
				return CANTX;
			}
		#endif
		else if ((pin.charAt(0)=='D')||(pin.charAt(0)=='d')) {
			pin.remove(0,1); //Remove first character
			uint8_t p = pin.toInt();
			if (p>=sizeof(digital_pin_map)) {
				DEBUG_PRINTLN("invalid mapped digital pin");
				Homey.returnError("invmap");
				return 255;
			}
			return digital_pin_map[p]; //Map Dx to corresponding digital pin number
		} else {
			DEBUG_PRINTLN("invalid pin type");
			Homey.returnError("invpt");
			return 255;
		}
	} else {
		uint8_t p = pin.toInt();
		if (p>=NUM_DIGITAL_PINS) {
			DEBUG_PRINTLN("invalid digital pin");
			Homey.returnError("invdp");
			return 255;
		}
		return p;
	}

	return 255;
}

/*bool rcSecurePinMode(const String& pin, uint8_t mode)
{
	uint8_t p = rcMapPin(pin);
	if (p==255) return false;

	pinMode(p, mode);
	return true;
}*/

void rcSecureDigitalWrite(const String& pin, bool state)
{
	uint8_t p = rcMapPin(pin);
	if (p==255) return;

	digitalWrite(p, state);
}

bool rcSecureDigitalRead(const String& pin)
{
	uint8_t p = rcMapPin(pin);
	if (p==255) return false;

	return digitalRead(p);
}

void rcSecureAnalogWrite(const String& pin, int state)
{
	uint8_t p = rcMapPin(pin);
	if (p==255) return;

#ifndef ARDUINO_ARCH_ESP32
	analogWrite(p, state);
#else
	DEBUG_PRINTLN("analog write is not supported on this platform");
#endif
}

int rcSecureAnalogRead(const String& pin)
{
	uint8_t p = rcMapPin(pin);
	if (p==255) return false;

	return analogRead(p);
}

void rcTriggerRegister(const String& name, uint8_t pin, bool analog)
{
	rcTrigger* searchItem = firstRcTrigger;
	rcTrigger* lastItem = firstRcTrigger;

	while (searchItem) {
		DEBUG_PRINT("In list ");
		DEBUG_PRINT(searchItem->name);
		DEBUG_PRINT(" ");
		DEBUG_PRINTLN(searchItem->pin);
		if (searchItem->pin==pin) {
			DEBUG_PRINTLN("REG ADD EXISTS");
			return; //Pin already registered, stop
		}
		lastItem= searchItem;
		searchItem = searchItem->next; //Go to next registration
	}

	rcTrigger* newItem = new rcTrigger;
	name.toCharArray(newItem->name, 3);
	newItem->pin = pin;
	newItem->analog = analog;
	newItem->prevState = digitalRead(pin);
	newItem->prev = lastItem;
	newItem->next = NULL;

	if (lastItem==NULL) {
		firstRcTrigger = newItem; //First item
		DEBUG_PRINTLN("REG ADD FIRST");
	} else {
		lastItem->next = newItem; //Add to list
		newItem->prev = lastItem; //Link to list
		DEBUG_PRINTLN("REG ADD NEXT");
	}

}

void rcTriggerRemove(uint8_t pin)
{
	rcTrigger* searchItem = firstRcTrigger;

	while (searchItem) {
		if (searchItem->pin==pin) {
			if (searchItem->next) {
				searchItem->next->prev = searchItem->prev;
				DEBUG_PRINTLN("Stitch: set n->p");
			}
			if (searchItem->prev) {
				searchItem->prev->next = searchItem->next;
				DEBUG_PRINTLN("Stitch: set p->n");
			} else {
				DEBUG_PRINTLN("Stitch: item has no prev");
			}
			if (firstRcTrigger==searchItem) {
				firstRcTrigger = searchItem->next;
				DEBUG_PRINTLN("Stitch: item was first");
			}
			delete searchItem;
			DEBUG_PRINTLN("REG DEL OK");
			return;
		}
		searchItem = searchItem->next; //Go to next registration
	}

	DEBUG_PRINTLN("REG DEL NOT FOUND");
}

void rcTriggerRun()
{
	DEBUG_PRINTLN("rcTriggerRun START");
	rcTrigger* item = firstRcTrigger;

	while (item) {
		DEBUG_PRINT("rcTriggerRun ITEM");
		DEBUG_PRINT(item->name);
		DEBUG_PRINT(" ");
		uint16_t state;
		if (item->analog) {
			state = analogRead(item->pin);
		} else {
			state =	digitalRead(item->pin);
		}
		DEBUG_PRINT(" = ");
		DEBUG_PRINTLN(state);
		if (state!=item->prevState) {
			item->prevState = state;
			if (item->analog) {
				Homey.trigger(String(item->name), (int) state);
			} else {
				Homey.trigger(String(item->name), (bool) state);
			}
		}
		item = item->next; //Go to next registration
	}

	DEBUG_PRINTLN("rcTriggerRun DONE");
}

void rcEndpointMode()
{
	uint16_t position = 0;
	for (;position<Homey.value.length();position++) {
		if (Homey.value.charAt(position)=='=') break;
	}

	uint8_t mode = 255;
	bool reg = false;
	bool trana = false;

	String modeStr = Homey.value.substring(position+1);

	if (modeStr == "di") mode = INPUT;
	if (modeStr == "dip") mode = INPUT_PULLUP;
	if (modeStr == "dit") {
		mode = INPUT;
		reg = true;
	}
	if (modeStr == "ditp") {
		mode = INPUT_PULLUP;
		reg = true;
	}
	if (modeStr == "do") mode = OUTPUT;
	if (modeStr == "ao") mode = OUTPUT;
	if (modeStr == "ai") mode = INPUT;
	if (modeStr == "aip") mode = INPUT_PULLUP;
	if (modeStr == "ait") {
		mode = INPUT;
		reg = true;
		trana = true;
	}
	if (modeStr == "aitp") {
		mode = INPUT_PULLUP;
		reg = true;
		trana = true;
	}

	if (mode==255) return Homey.returnError("invm");

	String name = Homey.value.substring(0,position);

	uint8_t pin = rcMapPin(name);

	if (pin==255) return Homey.returnError("invp");

	pinMode(pin, mode);

	if (reg) {
		rcTriggerRegister(name, pin, trana);
	} else {
		rcTriggerRemove(pin);
	}
}

void rcEndpointDigitalWrite()
{
	uint16_t position = 0;
	for (;position<Homey.value.length();position++) {
		if (Homey.value.charAt(position)=='=') break;
	}

	rcSecureDigitalWrite(Homey.value.substring(0,position), Homey.value.substring(position+1).toInt());
}

void rcEndpointDigitalRead()
{
	Homey.returnResult(rcSecureDigitalRead(Homey.value));
}

void rcEndpointAnalogWrite()
{
	uint16_t position = 0;
	for (;position<Homey.value.length();position++) {
		if (Homey.value.charAt(position)=='=') break;
	}

	rcSecureAnalogWrite(Homey.value.substring(0,position), Homey.value.substring(position+1).toInt());
}

void rcEndpointAnalogRead()
{
	Homey.returnResult(rcSecureAnalogRead(Homey.value));
}

void rcEnable()
{
	Homey.addRc("mode", rcEndpointMode);
	Homey.addRc("dwrite", rcEndpointDigitalWrite);
	Homey.addRc("dread", rcEndpointDigitalRead);
	Homey.addRc("awrite",rcEndpointAnalogWrite);
	Homey.addRc("aread", rcEndpointAnalogRead);
	Homey.rcEnabled = true;
}
