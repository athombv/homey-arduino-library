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
	yield();
}

bool HomeyClass::onAction(const String& name, CallbackFunction fn)
{	//Register an action
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

bool HomeyClass::on(const String& name, const String& type, CallbackFunction fn)
{	//Register a custom type
	return on(name.c_str(), type.c_str(), fn);
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

void HomeyClass::clear()
{	//Deletes all actions and conditions
	while (remove("","")); //This deletes the whole api...
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

void HomeyClass::returnIndex()
{	//Returns the api index to the Homey app
	responseCode = 1; //(hack, returns actual index elsewhere)
	response = "";
	responseType = "null";
}

void HomeyClass::returnNothing()
{	//Returns an empty result to the Homey app
	responseCode = 200; //Success
	response = "";
	responseType = "null";
}

void HomeyClass::returnError(const String& error, uint16_t code)
{	//Returns an error to the Homey app
	responseCode = code;
	response = "\""+error+"\"";
	responseType = "Error";
}

void HomeyClass::returnText(const String& result)
{	//Returns a string result to the Homey app
	responseCode = 200; //Success
	response = "\""+result+"\"";
	responseType = "String";
}

void HomeyClass::returnBoolean(bool result)
{	//Returns a boolean result to the Homey app
	responseCode = 200; //Success
	if (result) {
		response = "true";
	} else {
		response = "false";
	}	
	responseType = "Boolean";
}

void HomeyClass::returnNumber(int result)
{	//Returns a number result to the Homey app
	responseCode = 200; //Success
	response = String(result);
	responseType = "Number";
}

void HomeyClass::returnNumber(float result)
{	//Returns a floating point number result to the Homey app
	responseCode = 200; //Success
	response = String(result);
	responseType = "Number";
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

void HomeyClass::runCallback(HomeyFunction* cb, const String& argument) {
	value = argument; //Make argument accessible for the callback
	returnNothing(); //This sets the initial response (nothing)
	if (cb->callback!=NULL) {
		cb->callback();
	} else {
		returnError("Callback not found?", 501);
	}
}

String HomeyClass::descCallback(HomeyFunction* cb) {	
	String desc = "\""+String(cb->name)+"\":\"" + String(cb->type) + "\"";
	return desc;
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
		
		//DEBUG_PRINTLN("Host: "+host);
		//DEBUG_PRINT("Port: ");
		//DEBUG_PRINTLN(arg_p);
		
		if ((host=="") || (port<1) || (!_master_host.fromString(host) || (!success))) {
			returnError("invalid argument", 400);
			return;
		}
		_master_port = port;
		returnBoolean(true);
		DEBUG_PRINTLN("Master set to "+host+":"+String(port));
	} else {
		char* endpointCompareTo = endpoint+1; //Remove first character
		char* type;
		char* name;
		
		split(endpointCompareTo, type, name, '/', ENDPOINT_MAX_SIZE);
		
		HomeyFunction* cb = find(name, type);
		
		/*for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
			if (callbacks[i]) {
				uint8_t l = strnlen(callbacks[i]->name, ENDPOINT_MAX_SIZE)+1;
				if (strncmp(endpointCompareTo,callbacks[i]->name, l)==0) {
					cb = callbacks[i];
					break;
				}
			}
		}*/
		
		if (cb==NULL) {
			returnError("unknown call", 404);
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
		
		split(buffer, endpoint, argument, '?', ENDPOINT_MAX_SIZE+ARGUMENT_MAX_SIZE);
						
		handleRequest(endpoint, argument);

		_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
		if (responseCode==1) { //Return the index
			_udpServer.print("{\"id\":\"");
			_udpServer.print( _name);
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
			_udpServer.print(responseType);
			_udpServer.print("\",\"result\":");
			if (response=="") response = "\"\"";
			_udpServer.print(response);
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
				returnError("Could not parse request", 400);
			} else {
				if (_request.postArgs.length()>0) {
					DEBUG_PRINTLN("Handled as post request");
					handleRequest(_request.endpoint, _request.postArgs.c_str());
				} else {
					DEBUG_PRINTLN("Handled as get request");
					handleRequest(_request.endpoint, _request.getArgs.c_str());
				}
				
				if (responseCode==1) {
					sendIndex = true;
					responseCode = 200;
				}
			}
			
			const char* desc;
			if (responseCode==400) {
				desc = "Bad Request";
			} else if (responseCode==404) {
				desc = "Not Found";
			} else if (responseCode==500) {
				desc = "Internal Server Error";
			} else if (responseCode==501) {
				desc = "Not Implemented";
			} else {
				responseCode = 200;
				desc = "OK";
			}
			
			client.print("HTTP/1.1 ");
			client.print(responseCode);
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
				client.print(responseType);
				client.print("\",\"result\":");
				if (response=="") response = "\"\"";
				client.print(response);
				client.print("}");
			}
		}
		client.stop();
		yield();
		return true;
	}
	return false;
}

bool HomeyClass::on(const char* name, const char* type, CallbackFunction cb) {
  char* newType = copyCharArray(type, MAX_TYPE_LENGTH);
  if (newType==NULL) {
	  DEBUG_PRINTLN("Alloc error for type");
	  return false;
  }
  char* newName = copyCharArray(name, MAX_NAME_LENGTH);
  if (newName==NULL) {
	  DEBUG_PRINTLN("Alloc error for name");
	  free(newType);
	  return false;
  }
  
  HomeyFunction *newFunction = new HomeyFunction(newName, newType, cb);
  
  if (newFunction==NULL) {
	DEBUG_PRINTLN("Alloc error for object");
	free(newType);
	free(newName);
	return false;
  }
  
  if (firstHomeyFunction==NULL) {
	  firstHomeyFunction = newFunction;
	  DEBUG_PRINTLN("Created first.");
	  return true;
  }
  
  HomeyFunction *next = firstHomeyFunction;
  HomeyFunction *last = firstHomeyFunction;
  
  while (next!=NULL) {
	  last = next;
	  DEBUG_PRINT("List: '");
	  DEBUG_PRINT(last->name);
	  DEBUG_PRINT("' ('");
	  DEBUG_PRINT(last->type);
	  DEBUG_PRINTLN("')");
	  next = last->nextFunction;
  }
  
  newFunction->prevFunction = last; // <
  last->nextFunction = newFunction; // >
  
  
  DEBUG_PRINTLN("Created new.");
    
  return true;
}

/*HomeyFunction* HomeyClass::createEmptyCallback(const String& name) {
	HomeyFunction *cb = new HomeyFunction;
	name.toCharArray(cb->name, ENDPOINT_MAX_SIZE);
	
	for (uint8_t i = 0; i<strlen(cb->name)-1; i++) { //Replace spaces with underscores
		if (cb->name[i]==' ') cb->name[i] = '_';
	}
	
	cb->fnVoid = NULL;
	cb->fnBool = NULL;
	return cb;
}*/

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

char* HomeyClass::copyCharArray(const char* input, uint16_t maxlen)
{	//Allocate memory for a char array and copy input
	uint16_t size = strnlen(input, maxlen)+1;
	if (size>=maxlen) return NULL;
	char* output = (char*) malloc(size*sizeof(char));
	if (output==NULL) return NULL;
	strncpy(output, input, size);
	return output;
}

/* STRUCT CONSTRUCTORS */

HomeyFunction::HomeyFunction(char* newName, char* newType, CallbackFunction newCallback)
{	//Struct constructor
	prevFunction = NULL;
	nextFunction = NULL;
	type = newType;
	name = newName;
	callback = newCallback;
}

/* OBJECT CREATION */

HomeyClass Homey;
