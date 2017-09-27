#include <Homey.h>

HomeyClass::HomeyClass( uint16_t port )
: _tcpServer(port), _udpServer(), _master_host(0,0,0,0)
{
	_port = port;
	_name = "";
	_master_port = 9999;
}

void HomeyClass::begin(const String& name) {
	_name = name;
	_tcpServer.begin();
	_udpServer.begin(_port);
}

void HomeyClass::stop() {
	_udpServer.stop();
	//clear();
}

void HomeyClass::name(const String& name) {
	_name = name;
}

String HomeyClass::getToken(const String& data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

#if 0
void HomeyClass::parseRequest(CLIENT_TYPE* client) {
	
	_request.endpoint = "";
	_request.getArgs = "";
	_request.postArgs = "";
	
	if ((!client->connected())||(!client->available())) return;
	
	char requestType = client->read(); //Read first character in request
	
	uint8_t skip;
	if ((requestType == 'P') || (requestType == 'p')) {
		//Post request
		skip = 4;
	} else if ((requestType == 'G') || (requestType == 'g')) {
		//Get request
		skip = 3;
	} else {
		return;
	}
	
	uint8_t skipped = 0;
	while (client->connected()&&client->available()&&(skipped<skip)) {//Skip until after 'GET ' or 'POST '
		client->read();
		skipped++;
	}
	
	bool noArgs = false;
	
	while (client->connected()&&client->available()) {//Read into endpoint until '?'
		char c = client->read();
		if (c==' ') {
			noArgs = true;
			break;
		}
		if (c=='?') break;
		_request.endpoint += c;
	}
	
	if (!noArgs ) {
		while (client->connected()&&client->available()) {//Read into getArgs until ' '
			char c = client->read();
			if (c==' ') break;
			_request.getArgs += c;
		}
	}
	
	while (client->connected()&&client->available()) { //Skip rest of line
		char c = client->read();
		if (c=='\n') break;
	}
	
	if ((!client->connected())||(!client->available())) return;
	
	bool emptyLine = true;
	while (client->connected()&&client->available()){ //Skip rest of headers
		char c = client->read();
		if (c=='\r') continue;
		if (c=='\n') {
			if (emptyLine) {
				break; //End of headers
			} else {
				emptyLine = true; //New line, it could be empty...
			}
		} else {
			emptyLine = false;
		}
	}
	
	while (client->connected() && client->available()) {
		_request.postArgs += (char) client->read();
	}
}

#else
#if 0
void HomeyClass::parseRequest(CLIENT_TYPE* client) {
	bool newHeader = true;
	bool isPost = false;
	uint8_t timeout = 20;
	
	uint16_t pos = 0;
	uint8_t part = 0;
	
	_request.endpoint = "";
	_request.getArgs = "";
	
	while(client->connected()) {
		if (client->available()) {
			char c = client->read();
			if (newHeader && (c=='\n')) {
				//Serial.println("end of headers");
				break; //End of headers	
			} else if (newHeader) {
				//Serial.println("new header");
				newHeader = false;
				if ((c == 'P') || (c == 'G') || (c == 'p') || (c == 'g')) {
					if ((c == 'P') || (c == 'p')) isPost = true;
					pos = 1;
					//Serial.println("is get or post");
				} else {
					//Serial.println("ignoring header");
				}
			} else if (c=='\n') {
				newHeader = true;
				Serial.println("eoh");
			} else if (pos > 0) {
				if (((!isPost)&&(pos < 4))||((isPost)&&(pos < 6))) {
					pos++;
					//Serial.println("ignoring "+c);
				} else if (c != ' ') {
					if (c=='?') {
						part = 1;
					} else if (part==1) {
						_request.getArgs += c;
						//Serial.println("getArgs: "+_request.getArgs);
					} else if (part==0) {
						_request.endpoint += c;
						//Serial.println("endpoint: "+_request.endpoint);
					}
				} else {
					part = 2;
					//Done (wait for \n to appear...)
				}
			}
	  } else {
			timeout--;
			if (timeout<1) break;
		}
	}
	
	if (isPost) { //Read POST data
		//Serial.println("reading post data");
		while (client->connected() && client->available()) {
			_request.postArgs += (char) client->read();
		}
	}
}

#else
void HomeyClass::parseRequest(CLIENT_TYPE* client) {
	String currentHeader = "";
	String path = "";
	_request.postArgs = "";
	
	bool isPost = false;
	uint8_t timeout = 20;
	while(client->connected()) {
		if (client->available()) {
			char c = client->read();
			if (c=='\n') {
			if (currentHeader=="") break; //We've read all the headers and can continue
			String title = getToken(currentHeader, ' ', 0);
			if ((title=="GET")||(title=="POST")) {
				path = getToken(currentHeader, ' ', 1);
				if (title=="POST") isPost = true;
			}/* else {
				DEBUG_PRINTLN("ignored header: "+currentHeader);
			}*/
			currentHeader = "";
			} else if (c!='\r') {
			currentHeader = currentHeader + c;
			}
		} else {
			timeout--;
			if (timeout<1) {
			//DEBUG_PRINTLN("timeout");
			break;
			}
			delay(100);
		}
	}
	
	if (isPost) { //Read POST data
		while (client->connected() && client->available()) {
			_request.postArgs += (char) client->read();
		}
	}
	
	_request.endpoint = getToken(path, '?', 0);
	_request.getArgs = getToken(path, '?', 1);
}
#endif
#endif

void HomeyClass::runCallback(CommandCallback* cb, const String& argument) {
	failed = false;	
	value = argument;
	webResponseCode = 200;
	/*if (cb->fnString) {
		webResponseText = "\""+cb->fnString()+"\"";
	} else if (cb->fnInt) {
		webResponseText = String(cb->fnInt());
	} else if (cb->fnFloat) {
		webResponseText = String(cb->fnFloat());
	} else*/ if (cb->fnVoid) {
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
		webResponseText = "rCbErr";
	}
	
	if (failed) webResponseCode = 500;
		
	if (webResponseCode == 200) {
		webResponseText = "{\"result\":"+webResponseText+"}";
	} else {
		webResponseText = "{\"error\":"+webResponseText+"}";
	}
}

String HomeyClass::descCallback(CommandCallback* cb) {	
	String desc = "\""+String(cb->name)+"\":\"";
	/*if (cb->fnString) {
		desc += "String";
	} else if ((cb->fnInt)||(cb->fnFloat)) {
		desc += "Number";
	} else*/ if (cb->fnBool) {
		desc += "Boolean";
	} else {
		desc += "null";
	}
	desc += "\"";
	return desc;
}

String HomeyClass::createJsonIndex(uint16_t part, uint8_t lc){
	if (part==0) return "{\"id\":\""+_name+"\",\"master\":{\"host\":\""+String(_master_host[0])+"."+String(_master_host[1])+"."+String(_master_host[2])+"."+String(_master_host[3])+"\", \"port\":"+String(_master_port)+"},\"api\":{";
	part--;
	if (part<MAXCALLBACKS) {
		if (callbacks[part]) {
			String d = ",";
			if (lc<2) d = "";
			return d + descCallback(callbacks[part]);
		} else {
			return "~"; //Skip empty line
		}
	} else if (part==MAXCALLBACKS) {
		return "}}";
	} else {
		return "";
	}
}

//void HomeyClass::handleRequest(const String& endpoint, String argument, WebResponse* response) {
void HomeyClass::handleRequest(const char* endpoint, const String& argument) {
	//if (endpoint=="") {
	if (endpoint[0]==0) {
		webResponseCode = 400;
		webResponseText = "{\"error\":\"invalid request\"}";
	//} else if (endpoint=="/") {
	} else if (strncmp(endpoint,"/\0",2)==0) {
		webResponseCode = 1;
	//} else if (endpoint=="/cfg/reg") {
	} else if (strncmp(endpoint,"/cfg/reg",8)==0) {
		String host = getToken(argument,':',0);
		uint16_t port = getToken(argument,':',1).toInt();
		if ((host=="") || (port<1) || (!_master_host.fromString(host))) {
			webResponseCode = 400;
			webResponseText = "{\"error\":\"invalid argument\"}";
			return;
		}
		_master_port = port;
		webResponseCode = 200;
		webResponseText = "{\"result\":\"ok\"}";
		//DEBUG_PRINTLN("Master set to "+host+":"+String(port));
	} else {
		//endpoint.remove(0,1); //Remove "/" from start of string
		
		//const char* epp = endpoint.c_str();
		const char* endpointCompareTo = endpoint+1; //Remove first character
				
		CommandCallback* cb = NULL;		
		for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
			if (callbacks[i]) {
				uint8_t l = strnlen(callbacks[i]->name, NAME_LEN)+1;
				if (strncmp(endpointCompareTo,callbacks[i]->name, l)==0) {
					cb = callbacks[i];
					break;
				}
			}
		}
		
		if (cb==NULL) {
			webResponseCode = 404;
			webResponseText = "{\"error\":\"not found\"}";
		} else {
			runCallback(cb, argument);
		}
	}
}

bool HomeyClass::handleUdp() {
	int packetSize = _udpServer.parsePacket();
	if (packetSize) {
		if (packetSize>=UDP_TX_PACKET_MAX_SIZE) {
			_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
			const char err[] = "{\"error\":\"packet too large\"}";
			_udpServer.write((uint8_t*) err, strlen(err));
			_udpServer.endPacket();
			return true;
		}
		char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
		
		_udpServer.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		int pstrlen = strnlen(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		
		if (packetSize!=pstrlen+1) {
			_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
			const char err[] = "{\"error\":\"pkt not null term\"}";
			_udpServer.write((uint8_t*) err, strlen(err));
			_udpServer.endPacket();
			return true;
		}
		
		String endpoint = getToken(String(packetBuffer), '?', 0);
		String argument = getToken(String(packetBuffer), '?', 1);
		
		_request.endpoint = endpoint;
		_request.getArgs = argument;
		_request.postArgs = "";
		
		handleRequest(endpoint.c_str(), argument);

		_udpServer.beginPacket(_udpServer.remoteIP(), _udpServer.remotePort());
		if (webResponseCode==1) {
			bool parsing = true;
			uint16_t i = 0;
			uint8_t lc = 0;
			while(parsing) {
				String line = createJsonIndex(i, lc);
				if ((line!="~")&&(line!="")) {
					_udpServer.write((uint8_t*) line.c_str(), line.length());
					lc++;
				}
				if (line=="") {
					parsing = false;
				}
				i++;
			}
		} else {
			_udpServer.write((uint8_t*) webResponseText.c_str(), webResponseText.length());
		}
		_udpServer.endPacket();
		return true;
	}
	return false; //Handle TCP next
}

void HomeyClass::handleTcp() {
	CLIENT_TYPE client = _tcpServer.available();
	if (client) {
		parseRequest(&client);
		if (client.connected()) {			
			String argument = _request.getArgs;
			if (_request.postArgs.length()>0) argument = _request.postArgs;
			handleRequest(_request.endpoint.c_str(), argument);
			
			bool sendIndex = false;
			if (webResponseCode==1) {
				sendIndex = true;
				webResponseCode = 200;
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
				bool parsing = true;
				uint16_t i = 0;
				uint8_t lc = 0;
				while(parsing) {
					String line = createJsonIndex(i, lc);
					//Serial.println(String(i)+"("+String(lc)+"): "+line);
					if ((line!="~")&&(line!="")) {
						client.println(line);
						lc++;
					}
					if (line=="") {
						parsing = false;
					}
					i++;
				}
			} else {
				client.println(webResponseText);
			}
		}
		client.stop();
		yield();
	}
}

void HomeyClass::loop() {
	for (uint8_t i = 0; i<5; i++) {
		if (!handleUdp()) {
			yield();
			handleTcp();
		}
		yield();
	}
}

bool HomeyClass::del(const String& name) {
  for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
    if (callbacks[i]) {
		uint8_t l = strnlen(callbacks[i]->name, NAME_LEN)+1;
		if (strncmp(callbacks[i]->name,name.c_str(),l)==0) {
			//DEBUG_PRINTLN("delete cb #"+String(i));
			delete callbacks[i];
			callbacks[i] = NULL;
			return true;
			break;
		}
	}
  }
  return false;
}

/*void HomeyClass::clear() {
  for (uint8_t i = 0; i<MAXCALLBACKS; i++) {
    if (callbacks[i]) {
		delete callbacks[i];
		callbacks[i] = NULL;
	}
  }
}*/

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
	name.toCharArray(cb->name, NAME_LEN);
	//cb->fnString = NULL;
	//cb->fnInt = NULL;
	//cb->fnFloat = NULL;
	cb->fnVoid = NULL;
	cb->fnBool = NULL;
	return cb;
}

/*bool HomeyClass::on(const String& name, CommandCallbackStringType fn) {
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnString = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

bool HomeyClass::on(const String& name, CommandCallbackIntType fn) {
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnInt = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

bool HomeyClass::on(const String& name, CommandCallbackFloatType fn) {
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnFloat = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}*/

//bool HomeyClass::on(const String& name, CommandCallbackVoidType fn) {
bool HomeyClass::onAction(const String& name, CommandCallbackVoidType fn) {
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnVoid = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

//bool HomeyClass::on(const String& name, CommandCallbackBoolType fn) {
bool HomeyClass::onCondition(const String& name, CommandCallbackBoolType fn) {
	CommandCallback *cb = createEmptyCallback(name);
	cb->fnBool = fn;
	if (on(cb)) return true;
	delete cb;
	return false;
}

bool HomeyClass::emit(const char* name, const char* type, const String& triggerValue) {
	yield();
	if (_master_host[0]==0) return false; //Master IP not set: this function is doomed to fail so exit
	CLIENT_TYPE client;
	//String postData = "{\"argument\":"+triggerValue+",\"type\":\""+type+"\"}";
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
		
		Serial.print("Trigger response time left");
		Serial.println(timeout);
		
		String response = "";
		
		while (client.available()>0) {
			char c = client.read();
			response += c;
		}
		
		Serial.print("Trigger response: ");
		Serial.println(response);
		
		client.stop();
		yield();
		return true;
	}
	yield();
	return false;
}

bool HomeyClass::emitText(const String& name, const String& value) {
	return emit(name.c_str(), "String", "\""+value+"\"");
}
bool HomeyClass::emitNumber(const String& name, int value) {
	return emit(name.c_str(), "Number", String(value));
}
bool HomeyClass::emitNumber(const String& name, float value) {
	return emit(name.c_str(), "Number", String(value));
}

bool HomeyClass::emitBoolean(const String& name, bool value) {
	String str = "false";
	if (value) str = "true";
	return emit(name.c_str(), "Boolean", str);
}

bool HomeyClass::emit(const String& name) {
	return emit(name.c_str(), "null", "");
}

HomeyClass Homey;
