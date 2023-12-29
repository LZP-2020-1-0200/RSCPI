/**
 * @file serverHandlers.cpp
 * @author Oskars Putans (o.putaans@gmail.com)
 * @brief This file contains the functions for handling the webserver.
 */

#include "serverHandlers.h"
ESP8266WebServer server(80);
ESP8266WebServerSecure serverSecure(443);

extern Scheduler scheduler; // defined in ./main.cpp

String acc="";

/// @brief Function for handling the root path.
/// @param server reference to the server.
template <typename WSBase>
void handleRoot(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server) {
	//Serial.println("Handling root");
	time_t now = time(nullptr);
	struct tm* timeinfo = localtime(&now);
	char buffer[26];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
	server.send(200, "text/plain", buffer);
}

/// @brief Function for handling the not found path.
template <typename WSBase>
void handleNotFound(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server) {
	//Serial.println("Handle not found");
	server.send(404, "text/plain", "Not found");
}

/// @brief Function for handling the /exec path.
/// @param server reference to the server.
/// @details The /exec path is used to execute commands on the microcontroller.
/// The command is sent to the microcontroller as a JSON object.
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
	
}

/// @brief Function for setting up the webserver.
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
	serverSecure.on("/exec", [](){
		//Serial.println("Handling exec from serverSecure");
		handleExec(serverSecure);
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


	server.begin();
	serverSecure.begin();
}