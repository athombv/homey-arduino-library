#include <Homey.h>

/* PUBLIC FUNCTIONS */

HomeyClass::HomeyClass( uint16_t port )
: _tcpServer(port), _udpServer(), _master_host(0,0,0,0)
{
	_port = port;
	_deviceName = "";
	_deviceType = DTYPE_UNKNOWN;
	_master_port = 9999;
}

void HomeyClass::begin(const String& name, const String& type)
{
	_deviceName = name;
	_deviceType = type;
	_tcpServer.begin();
	_udpServer.begin(_port);
}

void HomeyClass::stop()
{
	#ifndef CAN_NOT_STOP_TCP
	_tcpServer.stop();
	#endif
	_udpServer.stop();
}

void HomeyClass::name(const String& name)
{
	_deviceName = name;
}

void HomeyClass::type(const String& type)
{
	_deviceType = type;
}

bool HomeyClass::onAction(const String& name, CallbackFunction fn)
{
	return on(name.c_str(), TYPE_ACTION, fn);
}

bool HomeyClass::onCondition(const String& name, CallbackFunction fn)
{	//Register a condition
	return on(name.c_str(), TYPE_CONDITION, fn);
}

bool HomeyClass::onCapability(const String& name, CallbackFunction fn)
{	//Register a capability
	return on(name.c_str(), TYPE_CAPABILITY, fn);
}

HomeyFunction* HomeyClass::findAction(const char* name)
{
	return find(name, TYPE_ACTION);
}

HomeyFunction* HomeyClass::findCondition(const char* name)
{
	return find(name, TYPE_CONDITION);
}

HomeyFunction* HomeyClass::findCapability(const char* name)
{
	return find(name, TYPE_CAPABILITY);
}

bool HomeyClass::removeAction(const char* name)
{
	return remove(name, TYPE_ACTION);
}

bool HomeyClass::removeCondition(const char* name)
{
	return remove(name, TYPE_CONDITION);
}

bool HomeyClass::removeCapability(const char* name)
{
	return remove(name, TYPE_CAPABILITY);
}

void HomeyClass::clear()
{
	while (remove("","")); //This deletes all endpoints
}

bool HomeyClass::trigger(const String& name)
{
	return _emit(name.c_str(), CTYPE_NULL, "\"\"", TYPE_TRIGGER);
}
bool HomeyClass::trigger(const String& name, const char* value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+String(value)+"\"", TYPE_TRIGGER);
}
bool HomeyClass::trigger(const String& name, const String& value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+value+"\"", TYPE_TRIGGER);
}
bool HomeyClass::trigger(const String& name, int value)
{
	return _emit(name.c_str(), CTYPE_INT, String(value), TYPE_TRIGGER);
}
bool HomeyClass::trigger(const String& name, float value)
{
	return _emit(name.c_str(), CTYPE_FLOAT, String(value), TYPE_TRIGGER);
}

bool HomeyClass::trigger(const String& name, double value)
{
	return _emit(name.c_str(), CTYPE_DOUBLE, String(value), TYPE_TRIGGER);
}

bool HomeyClass::trigger(const String& name, bool value)
{
	String str = BVAL_FALSE;
	if (value) str = BVAL_TRUE;
	return _emit(name.c_str(), CTYPE_BOOL, str, TYPE_TRIGGER);
}

bool HomeyClass::setCapabilityValue(const String& name)
{
	return _emit(name.c_str(), CTYPE_NULL, "\"\"", TYPE_CAPABILITY);
}
bool HomeyClass::setCapabilityValue(const String& name, const char* value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+String(value)+"\"", TYPE_CAPABILITY);
}
bool HomeyClass::setCapabilityValue(const String& name, const String& value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+value+"\"", TYPE_CAPABILITY);
}
bool HomeyClass::setCapabilityValue(const String& name, int value)
{
	return _emit(name.c_str(), CTYPE_INT, String(value), TYPE_CAPABILITY);
}
bool HomeyClass::setCapabilityValue(const String& name, float value)
{
	return _emit(name.c_str(), CTYPE_FLOAT, String(value), TYPE_CAPABILITY);
}

bool HomeyClass::setCapabilityValue(const String& name, double value)
{
	return _emit(name.c_str(), CTYPE_DOUBLE, String(value), TYPE_CAPABILITY);
}

bool HomeyClass::setCapabilityValue(const String& name, bool value)
{
	String str = BVAL_FALSE;
	if (value) str = BVAL_TRUE;
	return _emit(name.c_str(), CTYPE_BOOL, str, TYPE_CAPABILITY);
}

bool HomeyClass::emit(const String& name)
{
	return _emit(name.c_str(), CTYPE_NULL, "\"\"", TYPE_RAW);
}
bool HomeyClass::emit(const String& name, const char* value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+String(value)+"\"", TYPE_RAW);
}
bool HomeyClass::emit(const String& name, const String& value)
{
	return _emit(name.c_str(), CTYPE_STRING, "\""+value+"\"", TYPE_RAW);
}
bool HomeyClass::emit(const String& name, int value)
{
	return _emit(name.c_str(), CTYPE_INT, String(value), TYPE_RAW);
}
bool HomeyClass::emit(const String& name, float value)
{
	return _emit(name.c_str(), CTYPE_FLOAT, String(value), TYPE_RAW);
}

bool HomeyClass::emit(const String& name, double value)
{
	return _emit(name.c_str(), CTYPE_FLOAT, String(value), TYPE_RAW);
}

bool HomeyClass::emit(const String& name, bool value)
{
	String str = BVAL_FALSE;
	if (value) str = BVAL_TRUE;
	return _emit(name.c_str(), CTYPE_BOOL, str, TYPE_RAW);
}

void HomeyClass::returnIndex()
{
	_response.code = 1; //(hack, returns actual index elsewhere)
	_response.response = "";
	_response.type = CTYPE_NULL;
}

void HomeyClass::returnNothing()
{
	_response.code = 200; //Success
	_response.response = "";
	_response.type = CTYPE_NULL;
}

void HomeyClass::returnError(const String& error, uint16_t code)
{
	_response.code = code;
	_response.response = "\""+error+"\"";
	_response.type = "Error";
}

void HomeyClass::returnResult(const char* result)
{
	_response.code = 200; //Success
	_response.response = "\""+String(result)+"\"";
	_response.type = CTYPE_STRING;
}

void HomeyClass::returnResult(const String& result)
{
	_response.code = 200; //Success
	_response.response = "\""+result+"\"";
	_response.type = CTYPE_STRING;
}

void HomeyClass::returnResult(bool result)
{
	_response.code = 200; //Success
	if (result) {
		_response.response = BVAL_TRUE;
	} else {
		_response.response = BVAL_FALSE;
	}	
	_response.type = "Boolean";
}

void HomeyClass::returnResult(int result)
{
	_response.code = 200; //Success
	_response.response = String(result);
	_response.type = CTYPE_INT;
}

void HomeyClass::returnResult(float result)
{
	_response.code = 200; //Success
	_response.response = String(result);
	_response.type = CTYPE_FLOAT;
}

void HomeyClass::returnResult(double result)
{
	_response.code = 200; //Success
	_response.response = String(result);
	_response.type = CTYPE_DOUBLE;
}

void HomeyClass::loop()
{
	while (handleUdp()) { yield(); }
	yield();
	while (handleTcp()) { yield(); }
	yield();
}

/* PRIVATE FUNCTIONS */

bool HomeyClass::split(char* buffer, char*& a, char*& b, char separator, uint16_t size)
{
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

char* HomeyClass::copyCharArray(const char* input, uint16_t maxlen)
{
	uint16_t size = strnlen(input, maxlen)+1;
	if (size>=maxlen) return NULL;
	char* output = (char*) malloc(size*sizeof(char));
	if (output==NULL) return NULL;
	strncpy(output, input, size);
	return output;
}

bool HomeyClass::parseHttpHeaders(CLIENT_TYPE* client) {
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

bool HomeyClass::on(const char* name, const char* type, CallbackFunction cb) {
	if (cb==NULL) {DEBUG_PRINTLN("Callback is null"); return false; }
	
	char* newType = copyCharArray(type, MAX_TYPE_LENGTH);
	if (newType==NULL) { DEBUG_PRINTLN("Alloc error for type"); return false; }
	
	char* newName = copyCharArray(name, MAX_NAME_LENGTH);
	if (newName==NULL) { DEBUG_PRINTLN("Alloc error for name"); free(newType); return false; }
	
	HomeyFunction *newFunction = new HomeyFunction(newName, newType, cb);
	if (newFunction==NULL) {
		DEBUG_PRINTLN("Alloc error for object");
		free(newType);
		free(newName);
		return false;
	}
	
	if (firstHomeyFunction==NULL) {
		firstHomeyFunction = newFunction;
	} else {
		HomeyFunction *next = firstHomeyFunction;
		HomeyFunction *last = firstHomeyFunction;
		while (next!=NULL) { last = next; next = last->nextFunction; } //Follow links until end of list
		newFunction->prevFunction = last; // <
		last->nextFunction = newFunction; // >  
	}
	return true;
}

bool HomeyClass::on(const String& name, const String& type, CallbackFunction fn)
{
	return on(name.c_str(), type.c_str(), fn);
}

HomeyFunction* HomeyClass::find(const char* name, const char* type)
{
	HomeyFunction *item = firstHomeyFunction;
	
	if ((name[0]==0)&&(type[0]==0)) return item; //Empty query returns first item (used for "clear" function)
	
	while (item!=NULL) {	 
		uint16_t typelen = strnlen(item->type,MAX_TYPE_LENGTH);
		uint16_t namelen = strnlen(item->name,MAX_NAME_LENGTH);
		if ((strncmp(item->type,type,typelen)==0)&&(strncmp(item->name,name,namelen)==0)) {
			return item;
		}
		item = item->nextFunction;
	}
	return NULL; //Not found
}

bool HomeyClass::remove(const char* name, const char* type)
{	//Removes an action or condition
	HomeyFunction* func = find(name, type);
	if (func==NULL) return false;
	//DEBUG_PRINTLN("Remove: found");
	if (func->nextFunction) {
		//DEBUG_PRINTLN("Remove: stitch next to prev");
		func->nextFunction->prevFunction = func->prevFunction;
	}
	if (func->prevFunction) {
		//DEBUG_PRINTLN("Remove: stitch prev to next");
		func->prevFunction->nextFunction = func->nextFunction;
	} else {
		//DEBUG_PRINTLN("Remove: is first function");
		firstHomeyFunction = func->nextFunction;
	}
	free(func->name); //de-allocate the char array
	free(func->type); //de-allocate the char array
	delete func; //de-allocate the struct itself
	return true;
}

void HomeyClass::handleRequest(char* endpoint, const char* argument) {
	if (endpoint[0]==0) {
		returnError("Invalid request", 400);
	} else if (strncmp(endpoint,"/\0",2)==0) {
		returnIndex();
	} else if (strncmp(endpoint,ENDPOINT_SET_MASTER,8)==0) {
		char buffer[ARGUMENT_MAX_SIZE] = {0};
		strncpy(buffer, argument, ARGUMENT_MAX_SIZE);
		
		char* arg_h;
		char* arg_p;
		
		bool success = split(buffer, arg_h, arg_p, ':', ARGUMENT_MAX_SIZE);
				
		String host = arg_h;
		uint16_t port = atoi(arg_p);
				
		if ((host=="") || (port<1) || (!_master_host.fromString(host) || (!success))) {
			returnError("invalid argument", 400);
			return;
		}
		_master_port = port;
		returnResult((bool) true);
		DEBUG_PRINTLN("Master set to "+host+":"+String(port));
	} else {
		char* endpointCompareTo = endpoint+1; //Remove first character
		char* type;
		char* name;
		
		split(endpointCompareTo, type, name, '/', ENDPOINT_MAX_SIZE);
		
		HomeyFunction* cb = find(name, type);
				
		if (cb==NULL) {
			returnError("unknown call", 404);
		} else {
			value = argument;
			returnNothing();
			cb->callback();
		}
	}
}

bool HomeyClass::handleTcp() {
	CLIENT_TYPE client = _tcpServer.available();
	if (client) {
		bool valid = parseHttpHeaders(&client);
		if (client.connected()) {
			bool sendIndex = false;
			if (!valid) {
				returnError("Could not parse request", 400);
			} else {
				if (_request.postArgs.length()>0) {
					DEBUG_PRINTLN("Handled as post request");
					handleRequest(_request.endpoint, _request.postArgs.c_str());
				} else {
					DEBUG_PRINTLN("Handled as get request");
					handleRequest(_request.endpoint, _request.getArgs.c_str());
				}
				
				if (_response.code==1) {
					sendIndex = true;
					_response.code = 200;
				}
			}
			
			const char* desc;
			if (_response.code==400) {
				desc = "Bad Request";
			} else if (_response.code==404) {
				desc = "Not Found";
			} else if (_response.code==500) {
				desc = "Internal Server Error";
			} else if (_response.code==501) {
				desc = "Not Implemented";
			} else {
				_response.code = 200;
				desc = "OK";
			}
			
			client.print("HTTP/1.1 ");
			client.print(_response.code);
			client.print(' ');
			client.println(desc);
			client.println("Content-Type: application/json");
			client.println("Connection: close");
			client.println();
						
			if (sendIndex) {			
				client.print("{\"id\":\"");
				client.print( _deviceName);
				client.print(",\"type\":\"");
				client.print( _deviceType);
				client.print("\",\"master\":{\"host\":\"");
				client.print(_master_host);
				client.print("\", \"port\":");
				client.print(_master_port);
				client.print("},\"api\":{");
				uint16_t i = 0;
				HomeyFunction *item = firstHomeyFunction;
				while (item!=NULL) {
					client.print("\"");
					client.print(i);
					client.print("\":{\"name\":\"");
					client.print(item->name);
					client.print("\", \"type\":\"");
					client.print(item->type);
					client.print("\"}");
					item = item->nextFunction;
					if (item!=NULL) client.print(',');
					i++;
				}
				client.print("}}");
			} else {
				client.print("{\"type\":\"");
				client.print(_response.type);
				client.print("\",\"result\":");
				if (_response.response=="") _response.response = "\"\"";
				client.print(_response.response);
				client.print("}");
			}
		}
		client.stop();
		yield();
		return true;
	}
	return false;
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
		
		split(buffer, endpoint, argument, '?', ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE);
						
		handleRequest(endpoint, argument);

		_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
		if (_response.code==1) { //Return the index
			_udpServer.print("{\"id\":\"");
			_udpServer.print( _deviceName);
			_udpServer.print(",\"type\":\"");
			_udpServer.print( _deviceType);
			_udpServer.print("\",\"master\":{\"host\":\"");
			_udpServer.print(_master_host);
			_udpServer.print("\", \"port\":");
			_udpServer.print(_master_port);
			_udpServer.print("},\"api\":{");
			uint16_t i = 0;
			HomeyFunction *item = firstHomeyFunction;
			while (item!=NULL) {
				_udpServer.print("\"");
				_udpServer.print(i);
				_udpServer.print("\":{\"name\":\"");
				_udpServer.print(item->name);
				_udpServer.print("\", \"type\":\"");
				_udpServer.print(item->type);
				_udpServer.print("\"}");
				item = item->nextFunction;
				if (item!=NULL) _udpServer.print(',');
				i++;
			}
			_udpServer.print("}}");
			
		} else {
			_udpServer.print("{\"type\":\"");
			_udpServer.print(_response.type);
			_udpServer.print("\",\"result\":");
			if (_response.response=="") _response.response = "\"\"";
			_udpServer.print(_response.response);
			_udpServer.print("}");
		}
		_udpServer.endPacket();
		return true;
	}
	return false; //Handle TCP next
}

bool HomeyClass::_emit(const char* name, const char* argType, const String& triggerValue, const char* evType) {
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
		client.println(9+12+12+2+strlen(evType)+strlen(argType)+triggerValue.length());
		
		client.println("Connection: close");
		client.println();
		client.print(",\"type\":\""); //9
		client.print(evType);
		client.print(",\"argType\":\""); //12
		client.print(argType);
		client.print("{\"argument\":"); //12
		client.print(triggerValue);
		client.println("\"}"); //2
		
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
		
		DEBUG_PRINT("Emit response: ");
		DEBUG_PRINTLN(response);
		
		client.stop();
		yield();
		return true;
	}
	yield();
	return false;
}

/* STRUCT CONSTRUCTORS */

HomeyFunction::HomeyFunction(char* newName, char* newType, CallbackFunction newCallback)
{
	prevFunction = NULL;
	nextFunction = NULL;
	type = newType;
	name = newName;
	callback = newCallback;
}

/* OBJECT CREATION */

HomeyClass Homey;
