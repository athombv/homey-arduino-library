#ifndef _HOMEY_H_
#define _HOMEY_H_

// Settings
//#define HOMEY_USE_ETHERNET_V1 //Uncomment when using a legacy ethernet shield
#define DEBUG_ENABLE //Uncomment to have the library print debug messages

// Advanced settings
#define DEBUG_PRINTER Serial //Which class to use for printing debug mesages

/* -------------- DO NOT EDIT ANYTHING BELOW THIS LINE!  -------------- */
/* (If you do you might break compatibility with the Homeyduino app...) */

//
#define ENDPOINT_MAX_SIZE 17 //16 + null
#define ARGUMENT_MAX_SIZE 65 //64 + null
#define REQUEST_MAX_SIZE ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE
#define HEADER_MAX_SIZE REQUEST_MAX_SIZE+16
#define REQUEST_TIMEOUT 100

#define MAX_NAME_LENGTH 32
#define MAX_TYPE_LENGTH 32

#define TYPE_ACTION "act"
#define TYPE_CONDITION "con"
#define TYPE_CAPABILITY "cap"
#define TYPE_SYSTEM "sys"

#define ENDPOINT_SET_MASTER "/sys/setmaster"

//Includes
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP8266)
	#include <ESP8266WiFi.h>
	#include <WiFiClient.h>
	#include <WiFiUdp.h>		
	#define CLIENT_TYPE WiFiClient
	#define UDP_SERVER_TYPE WiFiUDP
	#define TCP_SERVER_TYPE WiFiServer
	#define MAXCALLBACKS 10
#elif defined(ARDUINO_ARCH_ESP32)
	#include <WiFi.h>
	#include <WiFiUdp.h>
	#define CLIENT_TYPE WiFiClient
	#define UDP_SERVER_TYPE WiFiUDP
	#define TCP_SERVER_TYPE WiFiServer
	#define UDP_TX_PACKET_MAX_SIZE 1024
	#define MAXCALLBACKS 10
#elif defined(HOMEY_USE_ETHERNET_V1)
	#include <Ethernet.h>
	#include <EthernetUdp.h>
	#define CLIENT_TYPE EthernetClient
	#define UDP_SERVER_TYPE EthernetUDP
	#define TCP_SERVER_TYPE EthernetServer
	#define MAXCALLBACKS 10
	#define CAN_NOT_STOP_TCP
#else
	#include <Ethernet2.h>
	#include <EthernetUdp2.h>
	#define CLIENT_TYPE EthernetClient
	#define UDP_SERVER_TYPE EthernetUDP
	#define TCP_SERVER_TYPE EthernetServer
	#define MAXCALLBACKS 10
	#define CAN_NOT_STOP_TCP
#endif

//Debug function definition
#ifdef DEBUG_ENABLE
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

//Type definitions
//typedef void (*ActionCallback)(void);
//typedef bool (*ConditionCallback)(void);

typedef void (*CallbackFunction)(void);

struct HomeyFunction {
	HomeyFunction(char* newName, char* newType, CallbackFunction newCallback);
	HomeyFunction* prevFunction;	//Linked list (<)
	HomeyFunction* nextFunction;	//Linked list (>)
	char* type;				//Type
	char* name;				//Name
	CallbackFunction callback;		//Function pointer
};

struct WebResponse {
	uint16_t code;
	const char* result;
};

struct WebRequest {
	char endpoint[ENDPOINT_MAX_SIZE];
	String getArgs;
	String postArgs;
};

class HomeyClass {
	public:
		//Class constructor
		HomeyClass( uint16_t port = 46639 ); //46639 = HOMEY
		
		//Start the servers
		void begin(const String& name);
		
		//Stop the servers
		void stop();
		
		//Set the device identifier
		void name(const String& name);
		
		//Handle incoming connections
		void loop();
		
		//Register api calls
		bool onAction(const String& name, CallbackFunction fn);
		bool onCondition(const String& name, CallbackFunction fn);
		bool onCapability(const String& name, CallbackFunction fn);
		bool on(const String& name, const String& type, CallbackFunction fn);
		
		//Finding api calls
		HomeyFunction* findAction(const char* name);
		HomeyFunction* findCondition(const char* name);
		HomeyFunction* findCapability(const char* name);
		HomeyFunction* find(const char* name, const char* type);
		
		//Remove api calls
		bool remove(const char* name, const char* type);
		
		//Deletes all actions and conditions
		void clear();
		
		//Emits a trigger to Homey
		bool emit(const String& name);
		
		//Emit a trigger to Homey
		bool emitText(const String& name, const String& value);
		bool emitNumber(const String& name, int value);
		bool emitNumber(const String& name, float value);
		bool emitBoolean(const String& name, bool value);
		
		//Returns a result to Homey
		void returnIndex();
		void returnNothing();
		void returnError(const String& error, uint16_t code = 500);
		void returnText(const String& result);
		void returnBoolean(bool result);
		void returnNumber(int result);
		void returnNumber(float result);
		
		//Handle incoming TCP connections
		bool handleTcp();
		
		//Handle incoming UDP connections
		bool handleUdp();
		
		//The argument supplied by the Homey flow
		String value;
		
	private:
		//Helper function that splits a buffer separate parts
		bool split(char* buffer, char*& a, char*& b, char separator, uint16_t size);
		
		//The TCP server
		TCP_SERVER_TYPE _tcpServer;
		
		//The UDP server
		UDP_SERVER_TYPE _udpServer;
		
		//The listening port for incoming connections
		uint16_t _port;
		
		//The device identifier
		String _name;
				
		//The registered actions and conditions
		HomeyFunction *callbacks[MAXCALLBACKS];
		
		WebRequest _request;
		
		//Response
		String response;
		String responseType;
		uint16_t responseCode;
				
		bool on(const char* name, const char* type, CallbackFunction cb);		
		
		bool parseRequest(CLIENT_TYPE* client);	
		
		void runCallback(HomeyFunction* cb, const String& argument);
		String descCallback(HomeyFunction* cb);
		void handleRequest(char* endpoint, const char* argument);
		
		//HomeyFunction* createEmptyCallback(const String& name);

		IPAddress _master_host;
		uint16_t _master_port;
		
		bool emit(const char* name, const char* type, const String& value);
				
		//Linked list entrypoints
		HomeyFunction* firstHomeyFunction = NULL;
		
		//Helper functions
		char* copyCharArray(const char* input, uint16_t maxlen);
		
};

extern HomeyClass Homey;

#endif
