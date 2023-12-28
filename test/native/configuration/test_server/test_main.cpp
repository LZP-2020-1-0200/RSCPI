#include "configuration.h"
#include <unity.h>
using namespace cfg;
StaticJsonDocument<8000> jsonDocument;

void setUp() {
	// set stuff up here
	jsonDocument.clear();
}

void tearDown() {
	// clean stuff up here
}

void test_save_load_server() {
	ServerConfigFile server;
	strcpy(server.pass, "huwfi4892hworbg3oqgb93q5bophftg");
	strcpy(server.username, "loop");
	server.https_enabled = false;
	TEST_ASSERT_EQUAL(0, saveServerConfigFileToJSON(server, jsonDocument));
	ServerConfigFile server2;
	TEST_ASSERT_EQUAL(0, loadServerConfigFileFromJSON(server2, jsonDocument));
	
	TEST_ASSERT_EQUAL_STRING("huwfi4892hworbg3oqgb93q5bophftg", server2.pass);
	TEST_ASSERT_EQUAL_STRING("loop", server2.username);
	TEST_ASSERT_EQUAL(false, server2.https_enabled);
}



void test_deserialize_to_server() {
	ServerConfigFile server;
	const char* json = "{\"username\": \"admin\",\"pass\": \"huwfi4892hworbg3oqgb93q5bophftg\", \"https_enabled\": false}";
	auto err = deserializeJson(jsonDocument, json, DeserializationOption::NestingLimit(10));
	if(err) {
		TEST_FAIL_MESSAGE(err.c_str());
		TEST_ABORT();
	}
	TEST_ASSERT_EQUAL(0, loadServerConfigFileFromJSON(server, jsonDocument));
	TEST_ASSERT_EQUAL_STRING("huwfi4892hworbg3oqgb93q5bophftg", server.pass);
	TEST_ASSERT_EQUAL_STRING("admin", server.username);
	TEST_ASSERT_EQUAL(false, server.https_enabled);
}


int main() {
	UNITY_BEGIN();
	RUN_TEST(test_deserialize_to_server);
	RUN_TEST(test_save_load_server);
	
	UNITY_END();
	return 0;
}