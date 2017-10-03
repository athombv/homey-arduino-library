#include <Homey.h>

/* PUBLIC FUNCTIONS */

HomeyClass::HomeyClass( uint16_t port )
: _tcpServer(port), _udpServer(), _master_host(0,0,0,0)
{	//Class constructor
	_port = port;
	_name = "";
	_master_port = 9999;
}

void HomeyClass::begin(const String& name)
{	//Start the servers
	_name = name;
	_tcpServer.begin();
	_udpServer.begin(_port);
}

void HomeyClass::stop()
{	//Stop the servers
	#ifndef CAN_NOT_STOP_TCP
	_tcpServer.stop();
	#endif
	_udpServer.stop();
}

void HomeyClass::name(const String& name)
{	//Set the device identifier
	_name = name;
}

void HomeyClass::loop()
{	//Handle incoming connections
	while (handleUdp()) { yield(); }
	yield();
	while (handleTcp()) { yield(); }
}

bool HomeyClass::onAction(const String& name, ActionCallback fn)
{	//Register an action
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnVoid = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

bool HomeyClass::onCondition(const String& name, ConditionCallback fn)
{	//Register a condition
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnBool = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

bool HomeyClass::remove(const String& name)
{	//Removes an action or condition
	for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
		if (callbacks[i]) {
			uint8_t l = strnlen(callbacks[i]->name, ENDPOINT_MAX_SIZE)+1;
			if (strncmp(callbacks[i]->name,name.c_str(),l)==0) {
				DEBUG_PRINTLN("delete cb #"+String(i));
				delete callbacks[i];
				callbacks[i] = NULL;
				return true;
				break;
			}
		}
	}
	return false;
}

void HomeyClass::clear()
{	//Deletes all actions and conditions
	for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
		if (callbacks[i]) {
			delete callbacks[i];
			callbacks[i] = NULL;
		}
	}
}

bool HomeyClass::emit(const String& name)
{	//Emits a trigger to Homey
	return emit(name.c_str(), "null", "\"\"");
}

bool HomeyClass::emitText(const String& name, const String& value)
{	//Emits a trigger with a string argument to Homey
	return emit(name.c_str(), "String", "\""+value+"\"");
}
bool HomeyClass::emitNumber(const String& name, int value)
{	//Emits a trigger with an integer argument to Homey
	return emit(name.c_str(), "Number", String(value));
}
bool HomeyClass::emitNumber(const String& name, float value)
{	//Emits a trigger with a float argument to Homey
	return emit(name.c_str(), "Number", String(value));
}

bool HomeyClass::emitBoolean(const String& name, bool value)
{	//Emits a trigger with a boolean argument to Homey
	String str = "false";
	if (value) str = "true";
	return emit(name.c_str(), "Boolean", str);
}

void HomeyClass::returnError(const String& error, bool hasFailed)
{	//Returns an error to the Homey flow
	lastError = error;
	failed = hasFailed;
}

/* PRIVATE FUNCTIONS */
bool HomeyClass::split(char* buffer, char*& a, char*& b, char separator, uint16_t size)
{	//Helper function that splits a buffer into separate parts
	a = buffer; //Set to start of buffer
	b = &buffer[size-1]; //Set to end of buffer
	
	for (uint8_t i = 0; i<size; i++) { //Find separator in buffer
		if (buffer[i]==0) break; //Stop when at end-of-string
		if (buffer[i]==separator) { //If current character is separator
			buffer[i]=0; //Replace with end-of-string
			b = &buffer[i+1]; //Adjust pointer of second half
			return true; //Successfully split the buffer
		}
	}
	return false; //Separator not found in buffer
}

bool HomeyClass::parseRequest(CLIENT_TYPE* client) {
	DEBUG_PRINTLN("-----");
	memset(_request.endpoint, 0, ENDPOINT_MAX_SIZE);
	_request.getArgs = "";
	_request.postArgs = "";
	
	char buffer[HEADER_MAX_SIZE] = {0};
	
	uint8_t to = REQUEST_TIMEOUT;
	while (client->available()<10) {
		delay(1);
		//DEBUG_PRINTLN("waiting... ("+String(to)+")");
		to--;
		if (to<1) break;
	}
	
	DEBUG_PRINT("timeout: ");
	DEBUG_PRINTLN(to);
	
	//Read first header into buffer
	for (uint16_t i = 0; i<HEADER_MAX_SIZE; i++) {
		if (!client->connected()) break;
		if (!client->available()) break;
		char c = client->read();
		if (c=='\n') break;
		if (c!='\r') buffer[i] = c;
	}
	
	DEBUG_PRINT("First header: ");
	DEBUG_PRINTLN(buffer);
	
	char* requestType;
	char* request;
	char* restOfBuffer;
	
	split(buffer, requestType, restOfBuffer, ' ', HEADER_MAX_SIZE);
	split(restOfBuffer, request, restOfBuffer, ' ', HEADER_MAX_SIZE - strlen(requestType));
	
	bool postRequest = false;
	
	if (strncmp(requestType, "GET", 3)==0) {
		DEBUG_PRINTLN("GET REQUEST");
	} else if (strncmp(requestType, "POST", 4)==0) {
		DEBUG_PRINTLN("POST REQUEST");
		postRequest = true;
	} else {
		DEBUG_PRINTLN("INVALID REQUEST");
		DEBUG_PRINT(">");
		DEBUG_PRINT(requestType);
		DEBUG_PRINTLN("<");
		return false;
	}
		
	char* endpoint;
	char* argument;
	
	split(request, endpoint, argument, '?', REQUEST_MAX_SIZE);
	
	memcpy(_request.endpoint, endpoint, strnlen(endpoint, ENDPOINT_MAX_SIZE));
	
	_request.getArgs = argument;
	
	DEBUG_PRINTLN("-----");
	
	DEBUG_PRINT("Endpoint: ");
	DEBUG_PRINTLN(endpoint);
	
	DEBUG_PRINT("Argument: ");
	DEBUG_PRINTLN(argument);
	
	DEBUG_PRINTLN("-----");
	
	//Then skip over the other headers
	bool emptyLine = true;
	while(client->connected() && client->available()) {
		char c = client->read();
		if (c=='\r') continue;
		if (c=='\n') {
			if (emptyLine) {
				break; //End-of-headers
			} else {
				//Just a header
				emptyLine = true;
				DEBUG_PRINTLN();
				continue;
			}
		}
		emptyLine = false;
		DEBUG_PRINT(c);
	}
	
	DEBUG_PRINTLN("-----");
	
	if (postRequest) { //Read POST data
		while (client->connected() && client->available()) {
			_request.postArgs += (char) client->read();
		}
		DEBUG_PRINT("Post arguments: ");
		DEBUG_PRINTLN(_request.postArgs);
	}
	DEBUG_PRINTLN("-----");
	return true;
}

void HomeyClass::runCallback(CommandCallback* cb, const String& argument) {
	failed = false;	
	lastError = "";
	value = argument;
	webResponseCode = 200;
	webResponseText = "";
	if (cb->fnVoid) {
		cb->fnVoid();
		webResponseText = "true";
	} else if (cb->fnBool) {
		bool result = cb->fnBool();
		if (result) {
			webResponseText = "true";
		} else {
			webResponseText = "false";
		}
	} else {
		webResponseCode = 501;
		lastError = "Callback not found?";
	}
	
	if (failed) webResponseCode = 500;
}

String HomeyClass::descCallback(CommandCallback* cb) {	
	String desc = "\""+String(cb->name)+"\":\"";
	if (cb->fnBool) {
		desc += "Boolean";
	} else {
		desc += "null";
	}
	desc += "\"";
	return desc;
}

void HomeyClass::handleRequest(const char* endpoint, const char* argument) {
	if (endpoint[0]==0) {
		webResponseCode = 400;
		lastError = "invalid request";
		webResponseText = "";
	} else if (strncmp(endpoint,"/\0",2)==0) {
		webResponseCode = 1;
	} else if (strncmp(endpoint,"/cfg/reg",8)==0) {
		
		char buffer[ARGUMENT_MAX_SIZE] = {0};
		strncpy(buffer, argument, ARGUMENT_MAX_SIZE);
		
		char* arg_h;
		char* arg_p;
		
		bool success = split(buffer, arg_h, arg_p, ':', ARGUMENT_MAX_SIZE);
				
		String host = arg_h;
		uint16_t port = atoi(arg_p);
		
		DEBUG_PRINTLN("Host: "+host);
		DEBUG_PRINT("Port: ");
		DEBUG_PRINTLN(arg_p);
		
		if ((host=="") || (port<1) || (!_master_host.fromString(host) || (!success))) {
			webResponseCode = 400;
			lastError = "invalid argument";
			webResponseText = "false";
			return;
		}
		_master_port = port;
		webResponseCode = 200;
		lastError = "";
		webResponseText = "true";
		//DEBUG_PRINTLN("Master set to "+host+":"+String(port));
	} else {
		const char* endpointCompareTo = endpoint+1; //Remove first character
				
		CommandCallback* cb = NULL;		
		for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
			if (callbacks[i]) {
				uint8_t l = strnlen(callbacks[i]->name, ENDPOINT_MAX_SIZE)+1;
				if (strncmp(endpointCompareTo,callbacks[i]->name, l)==0) {
					cb = callbacks[i];
					break;
				}
			}
		}
		
		if (cb==NULL) {
			webResponseCode = 404;
			webResponseText = "";
			lastError = "unknown call";
		} else {
			runCallback(cb, String(argument));
		}
	}
}

bool HomeyClass::handleUdp() {
	int packetSize = _udpServer.parsePacket();
	if (packetSize) {
		if (packetSize>=ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE) {
			_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
			const char err[] = "{\"error\":\"packet too large\"}";
			_udpServer.write((uint8_t*) err, strlen(err));
			_udpServer.endPacket();
			return true;
		}
		
		char buffer[ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE] = {0};
		_udpServer.read(buffer, ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE-1);
		
		char* endpoint;
		char* argument;
		
		bool success = split(buffer, endpoint, argument, '?', ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE);
						
		handleRequest(endpoint, argument);

		_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
		if (webResponseCode==1) {
			_udpServer.print("{\"id\":\"");
			_udpServer.print( _name);
			_udpServer.print("\",\"master\":{\"host\":\"");
			_udpServer.print(_master_host);
			_udpServer.print("\", \"port\":");
			_udpServer.print(_master_port);
			_udpServer.print("},\"api\":{");
			bool first = true;
			for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
				if (callbacks[i]!=NULL) {
					if (!first) _udpServer.print(',');
					first = false;
					_udpServer.print(descCallback(callbacks[i]));
				}
			}
			_udpServer.print("}}");
			
		} else {
			_udpServer.print("{\"error\":\"");
			_udpServer.print(lastError);
			_udpServer.print("\",\"result\":");
			if (webResponseText=="") webResponseText = "\"\"";
			_udpServer.print(webResponseText);
			_udpServer.print("}");
		}
		_udpServer.endPacket();
		return true;
	}
	return false; //Handle TCP next
}

bool HomeyClass::handleTcp() {
	CLIENT_TYPE client = _tcpServer.available();
	if (client) {
		bool valid = parseRequest(&client);
		if (client.connected()) {
			bool sendIndex = false;
			if (!valid) {
				webResponseCode = 400;
				webResponseText = "";
				lastError = "Could not parse request";
			} else {
				if (_request.postArgs.length()>0) {
					DEBUG_PRINTLN("Handled as post request");
					handleRequest(_request.endpoint, _request.postArgs.c_str());
				} else {
					DEBUG_PRINTLN("Handled as get request");
					handleRequest(_request.endpoint, _request.getArgs.c_str());
				}
				
				if (webResponseCode==1) {
					sendIndex = true;
					webResponseCode = 200;
				}
			}
			
			const char* desc;
			if (webResponseCode==400) {
				desc = "Bad Request";
			} else if (webResponseCode==404) {
				desc = "Not Found";
			} else if (webResponseCode==500) {
				desc = "Internal Server Error";
			} else if (webResponseCode==501) {
				desc = "Not Implemented";
			} else {
				webResponseCode = 200;
				desc = "OK";
			}
			
			client.print("HTTP/1.1 ");
			client.print(webResponseCode);
			client.print(' ');
			client.println(desc);
			client.println("Content-Type: application/json");
			client.println("Connection: close");
			client.println();
						
			if (sendIndex) {			
				client.print("{\"id\":\"");
				client.print( _name);
				client.print("\",\"master\":{\"host\":\"");
				client.print(_master_host);
				client.print("\", \"port\":");
				client.print(_master_port);
				client.print("},\"api\":{");
				bool first = true;
				for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
					if (callbacks[i]!=NULL) {
						if (!first) client.print(',');
						first = false;
						client.print(descCallback(callbacks[i]));
					}
				}
				client.print("}}");
			} else {
				client.print("{\"error\":\"");
				client.print(lastError);
				client.print("\",\"result\":");
				if (webResponseText=="") webResponseText = "\"\"";
				client.print(webResponseText);
				client.print("}");
			}
		}
		client.stop();
		yield();
		return true;
	}
	return false;
}

bool HomeyClass::on(CommandCallback *cb) {
  for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
      if (callbacks[i]==NULL) {
          callbacks[i] = cb;
          return true;
          break;
      }
  }
  return false;
}

CommandCallback* HomeyClass::createEmptyCallback(const String& name) {
	CommandCallback *cb = new CommandCallback;
	name.toCharArray(cb->name, ENDPOINT_MAX_SIZE);
	
	for (uint8_t i = 0; i<strlen(cb->name)-1; i++) { //Replace spaces with underscores
		if (cb->name[i]==' ') cb->name[i] = '_';
	}
	
	cb->fnVoid = NULL;
	cb->fnBool = NULL;
	return cb;
}

bool HomeyClass::emit(const char* name, const char* type, const String& triggerValue) {
	yield();
	if (_master_host[0]==0) return false; //Master IP not set: this function is doomed to fail so exit
	CLIENT_TYPE client;
	if (client.connect(_master_host, _master_port)) {
		client.print("POST /trigger/");
		client.print(name);
		client.println(" HTTP/1.1");
		
		//client.println("User-Agent: Arduino/1.0");
		client.println("Content-Type: application/json");
		
		client.print("Content-Length: ");
		//client.println(postData.length());
		client.println(12+9+2+strlen(type)+triggerValue.length());
		
		client.println("Connection: close");
		client.println();
		//client.println(postData);
		client.print("{\"argument\":"); //12
		client.print(triggerValue);
		client.print(",\"type\":\""); //9
		client.print(type);
		client.println("\"}"); //2
		
		//FOR ESP8266 ONLY
		uint8_t timeout = 100;
		while (client.available()==0) {
			delay(1);
			timeout--;
			if (timeout<1) break;
		}
				
		String response = "";
		
		while (client.available()>0) {
			char c = client.read();
			response += c;
		}
		
		//DEBUG_PRINT("Trigger response: ");
		//DEBUG_PRINTLN(response);
		
		client.stop();
		yield();
		return true;
	}
	yield();
	return false;
}

HomeyClass Homey;
