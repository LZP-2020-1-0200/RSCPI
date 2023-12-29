#include "configuration.h"
#include <unity.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "jsonStorage.h"

void setUp(void)
{
  	// set stuff up here
}

void tearDown(void)
{
  	// clean stuff up here
}


void setup()
{
	// NOTE!!! Wait for >2 secs
	// if board doesn't support software reset via Serial.DTR/RTS
	delay(2000);

	LittleFS.begin();

	UNITY_BEGIN(); // IMPORTANT LINE!
	
	StaticJsonDocument<200> doc;
	doc["test"] = "test";
	doc["test2"] = "test2";
	doc["test3"] = JsonArray();
	doc["test3"].add(1);
	doc["test3"].add(2);
	doc["test3"].add(3);
	loc::saveData("_test/test1.json", doc);

	doc.clear();
	TEST_ASSERT_EQUAL(doc.size(), 0);
	loc::loadData("_test/test1.json", doc);
	TEST_ASSERT_EQUAL(doc.size(), 3);
	TEST_ASSERT_EQUAL_STRING(doc["test"].as<const char*>(), "test");
	TEST_ASSERT_EQUAL_STRING(doc["test2"].as<const char*>(), "test2");
	TEST_ASSERT_EQUAL(doc["test3"][0].as<int>(), 1);
	TEST_ASSERT_EQUAL(doc["test3"][1].as<int>(), 2);
	TEST_ASSERT_EQUAL(doc["test3"][2].as<int>(), 3);

	UNITY_END();
}

void loop()
{
	// Do nothing
}