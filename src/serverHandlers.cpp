#include "serverHandlers.h"
ESP8266WebServer server(80);
ESP8266WebServerSecure serverSecure(443);

extern Scheduler scheduler; // defined in ./main.cpp

String acc="";


template <typename WSBase>
void handleRoot(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server) {
	//Serial.println("Handling root");
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	char buffer[26];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
	server.send(200, "text/plain", buffer);
}

template <typename WSBase>
void handleNotFound(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server) {
	//Serial.println("Handle not found");
	server.send(404, "text/plain", "Not found");
}

template <typename WSBase>
void handleExec(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server) {
	//Serial.println("Handle exec");
	String input = server.arg("plain");
	StaticJsonDocument<200> doc;
	deserializeJson(doc, input);
	if(doc.isNull()) {
		//Serial.println("Failed to parse JSON");
		server.send(400, "text/plain", "Failed to parse JSON");
		return;
	}
	if(!doc.containsKey("command")) {
		//Serial.println("No command key");
		server.send(400, "text/plain", "No command key");
		return;
	}
	if(!doc["command"].is<const char*>()) {
		//Serial.println("Command is not a string");
		server.send(400, "text/plain", "Command is not a string");
		return;
	}
	if(!doc.containsKey("expect_response")) {
		//Serial.println("No expect_response key");
		server.send(400, "text/plain", "No expect_response key");
		return;
	}
	if(!doc["expect_response"].is<bool>()) {
		//Serial.println("expect_response is not a boolean");
		server.send(400, "text/plain", "expect_response is not a boolean");
		return;
	}
	String cmd = String(doc["command"].as<const char*>());
	Serial.print(cmd);
	Serial.flush();
	if(doc["expect_response"].as<bool>()) {
		auto currentTimeout = Serial.getTimeout();
		if(doc.containsKey("response_timeout")){
			if(doc["response_timeout"].is<unsigned int>()) {
				Serial.setTimeout(doc["response_timeout"].as<unsigned int>());
			} else {
				server.send(400, "text/plain", "response_timeout is not an unsigned integer");
				return;
			}
		}


		char EOL = '\n';
		if(doc.containsKey("response_EOL")) {
			if(doc["response_EOL"].is<const char*>()) {
				if(strlen(doc["response_EOL"].as<const char*>()) != 1) {
					server.send(400, "text/plain", "response_EOL is not a single character");
					return;
				}
				EOL = doc["response_EOL"].as<const char*>()[0];
			} else {
				server.send(400, "text/plain", "response_EOL is not a single character");
				return;
			}
		}
		String response = Serial.readStringUntil(EOL);
		Serial.setTimeout(currentTimeout);
		if(response.length() == 0) {
			server.send(400, "text/plain", "No response");
			return;
		}
		server.send(200, "text/plain", response);
	} else {
		server.send(200, "text/plain", "");
	}
	/*
	struct DATA {
		String command;
		bool expect_response;
		esp8266webserver::ESP8266WebServerTemplate<WSBase> &server
	};

	scheduler.schedule<DATA>([](DataBuffer& dataBuf){
		DATA& data = dataBuf.get<DATA>();
		//Serial.swap();
		Serial.print(data.command);
		//Serial.swap();
		if(data.expect_response) {
			String response = Serial.readStringUntil('\n');
			data.server.send(200, "text/plain", response);
			data.server.
		} else {
			data.server.send(200, "text/plain", "");
		}
	}, 0, std::move(DATA{cmd, doc["expect_response"].as<bool>(), server}));
	server.send(200, "text/plain", "Executed");
	*/
}

void serverSetup() {
		
	server.on("/", [](){
		//Serial.println("Handling root from server");
		handleRoot(server);
	});
	serverSecure.on("/", [](){
		//Serial.println("Handling root from serverSecure");
		handleRoot(serverSecure);
	});

	server.on("/exec", [](){
		//Serial.println("Handling exec from server");
		handleExec(server);
	});

	server.on("/read", [](){
		//Serial.println("Handling read from server");
		auto currentTimeout = Serial.getTimeout();
		Serial.setTimeout(1000);
		String response = Serial.readStringUntil('\n');
		server.send(200, "text/plain", response);
		Serial.setTimeout(currentTimeout);
	});

	server.onNotFound([](){
		//Serial.println("Handling not found from server");
		handleNotFound(server);
	});
	serverSecure.onNotFound([](){
		//Serial.println("Handling not found from serverSecure");
		handleNotFound(serverSecure);
	});

	server.on("/debug", HTTP_GET, []() {
		Serial.setTimeout(1000);
		Serial.write("*IDN?\n");
	
		delay(2000);
		
		char response[100];
		int awaitCounter = 0;
		while(!Serial.available() && awaitCounter++ < 10){
			delay(1000);
			acc.concat("Waiting for response\n");
		}
		if(awaitCounter >= 10) {
			server.send(400, "text/plain", acc);
			return;
		}
		Serial.readBytesUntil('\n', response, 100);
		acc.concat(response);
		if(acc.length()>800) acc=acc.substring(acc.length()-800);
		delay(2000);
		server.send(200, "text/plain", acc);
	});

	server.begin();
	serverSecure.begin();
}