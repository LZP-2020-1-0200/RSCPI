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

void test_save_load_user() {
	UserConfigFile user;
	strcpy(user.pass, "huwfi4892hworbg3oqgb93q5bophftg");
	strcpy(user.username, "loop");
	TEST_ASSERT_EQUAL(0, saveUserConfigFileToJSON(user, jsonDocument));
	UserConfigFile user2;
	TEST_ASSERT_EQUAL(0, loadUserConfigFileFromJSON(user2, jsonDocument));
	
	TEST_ASSERT_EQUAL_STRING("huwfi4892hworbg3oqgb93q5bophftg", user2.pass);
	TEST_ASSERT_EQUAL_STRING("loop", user2.username);
}



void test_deserialize_to_user() {
	UserConfigFile user;
	const char* json = "{\"username\": \"admin\",\"pass\": \"huwfi4892hworbg3oqgb93q5bophftg\"}";
	auto err = deserializeJson(jsonDocument, json, DeserializationOption::NestingLimit(10));
	if(err) {
		TEST_FAIL_MESSAGE(err.c_str());
		TEST_ABORT();
	}
	TEST_ASSERT_EQUAL(0, loadUserConfigFileFromJSON(user, jsonDocument));
	TEST_ASSERT_EQUAL_STRING("huwfi4892hworbg3oqgb93q5bophftg", user.pass);
	TEST_ASSERT_EQUAL_STRING("admin", user.username);
}


int main() {
	UNITY_BEGIN();
	RUN_TEST(test_save_load_user);
	RUN_TEST(test_deserialize_to_user);
	
	UNITY_END();
	return 0;
}