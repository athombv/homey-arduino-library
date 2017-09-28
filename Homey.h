#ifndef _HOMEY_H_
#define _HOMEY_H_

#include <Arduino.h>

//Should you want to use the old ws5100 ethernet shield then uncomment the following line

//#define HOMEY_USE_ETHERNET_V1

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
#else
	#include <Ethernet2.h>
	#include <EthernetUdp2.h>
	#define CLIENT_TYPE EthernetClient
	#define UDP_SERVER_TYPE EthernetUDP
	#define TCP_SERVER_TYPE EthernetServer
	#define MAXCALLBACKS 10
#endif

//SETTINGS
#define DEBUG_ENABLE
//--------

#define DEBUG_PRINTER Serial

#ifdef DEBUG_ENABLE
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

//typedef String (*CommandCallbackStringType)(void);
//typedef int (*CommandCallbackIntType)(void);
//typedef float (*CommandCallbackFloatType)(void);
typedef void (*CommandCallbackVoidType)(void);
typedef bool (*CommandCallbackBoolType)(void);

#define NAME_LEN 16

struct CommandCallback {
	char name[NAME_LEN];
	//CommandCallbackStringType fnString;
	//CommandCallbackIntType fnInt;
	//CommandCallbackFloatType fnFloat;
	CommandCallbackVoidType fnVoid;
	CommandCallbackBoolType fnBool;
};

struct WebResponse {
	uint16_t code;
	const char* result;
};

struct WebRequest {
	String endpoint;
	String getArgs;
	String postArgs;
};

class HomeyClass {
	public:
		HomeyClass( uint16_t port = 46639 ); //46639 = HOMEY
		
		void begin(const String& name);
		void stop();
		void loop();
		
		void name(const String& name);
		
		//bool on(const String& name, CommandCallbackStringType fn);
		//bool on(const String& name, CommandCallbackIntType fn);
		//bool on(const String& name, CommandCallbackFloatType fn);
		//bool on(const String& name, CommandCallbackVoidType fn);
		//bool on(const String& name, CommandCallbackBoolType fn);
		
		bool onAction(const String& name, CommandCallbackVoidType fn);
		bool onCondition(const String& name, CommandCallbackBoolType fn);
		
		bool del(const String& name);
		//void clear();
		
		String value;
		bool failed;
		
		bool emitText(const String& name, const String& value);
		bool emitNumber(const String& name, int value);
		bool emitNumber(const String& name, float value);
		bool emitBoolean(const String& name, bool value);
		bool emit(const String& name);
		
	private:
		TCP_SERVER_TYPE _tcpServer;
		UDP_SERVER_TYPE _udpServer;
		
		String _name;
		
		CommandCallback *callbacks[MAXCALLBACKS];
		
		uint16_t _port;
				
		WebRequest _request;
		
		String webResponseText;
		uint16_t webResponseCode;
				
		bool on(CommandCallback *cb);
		
		String getToken(const String& data, char separator, int index);
		void parseRequest(CLIENT_TYPE* client);	
		
		void runCallback(CommandCallback* cb, const String& argument);
		String createJsonIndex(uint16_t part, uint8_t lc);
		String descCallback(CommandCallback* cb);
		void handleRequest(const char* endpoint, const String& argument);
		
		void handleTcp();
		bool handleUdp();
		
		CommandCallback* createEmptyCallback(const String& name);

		IPAddress _master_host;
		uint16_t _master_port;
		
		bool emit(const char* name, const char* type, const String& value);
		
		
};

extern HomeyClass Homey;

#endif
