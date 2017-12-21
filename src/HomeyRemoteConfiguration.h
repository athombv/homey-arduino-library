#ifndef __HOMEY_RC__
	#define __HOMEY_RC__

	#include "Homey.h"

	//Remote pin control functions
	uint8_t rcMapPin(String pin);
	//bool rcSecurePinMode(const String& pin, uint8_t state);
	void rcSecureDigitalWrite(const String& pin, bool state);
	bool rcSecureDigitalRead(const String& pin);
	void rcSecureAnalogWrite(const String& pin, int state);
	int rcSecureAnalogRead(const String& pin);

	//Remote trigger functions
	struct rcTrigger {
		char name[3];
		uint8_t pin;			//Arduino pin number
		bool analog;			//Analog or digital input?
		uint16_t prevState; //Previous state
		rcTrigger* prev;	//Linked list (<)
		rcTrigger* next;	//Linked list (>)
	};

	void rcTriggerRegister(const String& name, uint8_t pin, bool analog);
	void rcTriggerRemove(uint8_t pin);
	void rcTriggerRun();

	//Remote control endpoints
	void rcEndpointMode();
	void rcEndpointDigitalWrite();
	void rcEndpointDigitalRead();
	void rcEndpointAnalogWrite();
	void rcEndpointAnalogRead();
	void rcEnable();	//Enable remote control functions
#endif
