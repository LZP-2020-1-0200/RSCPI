#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include "scheduler.h"
#pragma once

template <typename WSBase>
void handleRoot(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server);

template <typename WSBase>
void handleNotFound(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server);

template <typename WSBase>
void handleExec(esp8266webserver::ESP8266WebServerTemplate<WSBase> &server);

void serverSetup();

extern ESP8266WebServer server;
extern ESP8266WebServerSecure serverSecure;
extern Scheduler scheduler;
