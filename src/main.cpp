#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include <sntp.h>
#include "certificates.h"
#include "scheduler.h"
#include "battery.h"
#include <ArduinoJson.h>
#include "serverHandlers.h"
#include <LTC2942.h>


Scheduler scheduler;

const unsigned int fullCapacity = 240; // Maximum value is 5500 mAh

LTC2942 gauge(50); // Takes R_SENSE value (in milliohms) as constructor argument, can be omitted if using LTC2942-1

void setup() {
  Serial.begin(300, SerialConfig::SERIAL_8N1, SerialMode::SERIAL_FULL);
  
  Serial.flush();
  Serial.setTimeout(1000);
  Serial.println("Starting ESP8266");
  Serial.printf("SNTP update interval: %d\n", SNTP_UPDATE_DELAY);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin("NanoLab", "********");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Print the IP address
  Serial.println(WiFi.localIP());

  setServerCertAndKey(serverSecure);
  // Initialize the SNTP client
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "192.168.5.21");
  //sntp_setservername(1, "time.google.com");
  //sntp_setservername(2, "pool.ntp.org");
  sntp_init();
  // Set timezone to Eastern Standard Time
  setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
  tzset();
  serverSetup();

  Serial.println("HTTP server started");
  if (!MDNS.begin("rscpi")) {             // Start the mDNS responder for rscpi.local
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
  
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("https", "tcp", 443);
  
  //pinMode(D7, OUTPUT);
  //pinMode(D8, OUTPUT);
  //I2C_setup();
  Wire.begin();

  while (gauge.begin() == false) {
    Serial.println("Failed to detect LTC2941 or LTC2942!");
    delay(5000);
  }

  unsigned int model = gauge.getChipModel();
  Serial.print("Detected LTC");
  Serial.println(model);

  gauge.setBatteryCapacity(fullCapacity);
  gauge.setBatteryToFull(); // Sets accumulated charge registers to the maximum value
  gauge.setADCMode(ADC_MODE_SLEEP); // In sleep mode, voltage and temperature measurements will only take place when requested
  gauge.startMeasurement();
  Serial.flush();
  Serial.swap();
}
void loop() {
  
  server.handleClient();
  //Serial.printf("Looping");
  serverSecure.handleClient();
  //Serial.printf(" %lu\n", millis()/1000);
  digitalWrite(D7, (millis()/1000)%2);
  digitalWrite(D8, (1+millis()/1000)%2);
  MDNS.update();
  time_t now = time(nullptr);
  scheduler.update(now);
  
  // unsigned int raw = gauge.getRawAccumulatedCharge();
  // Serial.print(F("Raw Accumulated Charge: "));
  // Serial.println(raw, DEC);

  // float capacity = gauge.getRemainingCapacity();
  // Serial.print(F("Battery Capacity: "));
  // Serial.print(capacity, 3);
  // Serial.print(F(" / "));
  // Serial.print(fullCapacity, DEC);
  // Serial.println(F(" mAh"));

  // float voltage = gauge.getVoltage();
  // Serial.print(F("Voltage: "));
  // if (voltage >= 0) {
  //   Serial.print(voltage, 3);
  //   Serial.println(F(" V"));
  // } else {
  //   Serial.println(F("Not supported by LTC2941"));
  // }

  // float temperature = gauge.getTemperature();
  // Serial.print(F("Temperature: "));
  // if (temperature >= 0) {
  //   Serial.print(temperature, 2);
  //   Serial.println(F(" 'C"));
  // } else {
  //   Serial.println(F("Not supported by LTC2941"));
  // }

  //Serial.println();

  //delay(5000);
}
