/**
 * @file certificates.h
 * This file contains the setServerCertAndKey function, which is used to set the server certificate and key.
 * The certificate and key are stored in the filesystem, and are loaded from there.
 * The certificate and key are used to secure the webserver.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>

void setServerCertAndKey(BearSSL::ESP8266WebServerSecure &server);