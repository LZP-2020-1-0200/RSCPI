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

void test_save_load_preset() {
	PresetFile preset_file;

	strcpy(preset_file.http_client.access_token, "access_token");
	strcpy(preset_file.http_client.url, "http://random.lol");
	strcpy(preset_file.http_client.experiment_id, "experiment_id");
	strcpy(preset_file.http_client.experiment_description, "experiment_description");
	preset_file.http_client.check_certs = true;

	preset_file.run_once_count = 2;
	preset_file.run_once[0] = Command{"command1", true};
	preset_file.run_once[1] = Command{"command2", false};
	TEST_ASSERT_EQUAL_STRING("command1", preset_file.run_once[0].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_once[0].expect_response);
	TEST_ASSERT_EQUAL_STRING("command2", preset_file.run_once[1].command);
	TEST_ASSERT_EQUAL(false, preset_file.run_once[1].expect_response);

	preset_file.run_scheduled_count = 2;
	preset_file.run_scheduled[0] = Command{"command3", true};
	preset_file.run_scheduled[1] = Command{"command4", false};
	TEST_ASSERT_EQUAL_STRING("command3", preset_file.run_scheduled[0].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_scheduled[0].expect_response);

	preset_file.serial.baud_rate = 9600;
	preset_file.serial.byte_size = 8;
	preset_file.serial.parity = 0;
	preset_file.serial.stop_bits = 1;
	strcpy(preset_file.serial.EOL, "\r");

	preset_file.task_schedule.period = 5;
	preset_file.task_schedule.offset = 1699867392;

	TEST_ASSERT_EQUAL(0, savePresetFileToJSON(preset_file, jsonDocument));
	PresetFile preset_file2;
	TEST_ASSERT_EQUAL(0, loadPresetFileFromJSON(preset_file2, jsonDocument));
	
	TEST_ASSERT_EQUAL_STRING(preset_file.http_client.access_token, preset_file2.http_client.access_token);
	TEST_ASSERT_EQUAL_STRING(preset_file.http_client.url, preset_file2.http_client.url);
	TEST_ASSERT_EQUAL_STRING(preset_file.http_client.experiment_id, preset_file2.http_client.experiment_id);
	TEST_ASSERT_EQUAL_STRING(preset_file.http_client.experiment_description, preset_file2.http_client.experiment_description);
	TEST_ASSERT_EQUAL(preset_file.http_client.check_certs, preset_file2.http_client.check_certs);
	TEST_ASSERT_EQUAL_STRING(preset_file.run_once[0].command, preset_file2.run_once[0].command);
	TEST_ASSERT_EQUAL(preset_file.run_once[0].expect_response, preset_file2.run_once[0].expect_response);
	TEST_ASSERT_EQUAL_STRING(preset_file.run_once[1].command, preset_file2.run_once[1].command);
	TEST_ASSERT_EQUAL(preset_file.run_once[1].expect_response, preset_file2.run_once[1].expect_response);
	TEST_ASSERT_EQUAL_STRING(preset_file.run_scheduled[0].command, preset_file2.run_scheduled[0].command);
	TEST_ASSERT_EQUAL(preset_file.run_scheduled[0].expect_response, preset_file2.run_scheduled[0].expect_response);
	TEST_ASSERT_EQUAL_STRING(preset_file.run_scheduled[1].command, preset_file2.run_scheduled[1].command);
	TEST_ASSERT_EQUAL(preset_file.run_scheduled[1].expect_response, preset_file2.run_scheduled[1].expect_response);
	TEST_ASSERT_EQUAL(preset_file.serial.baud_rate, preset_file2.serial.baud_rate);
	TEST_ASSERT_EQUAL(preset_file.serial.byte_size, preset_file2.serial.byte_size);
	TEST_ASSERT_EQUAL(preset_file.serial.parity, preset_file2.serial.parity);
	TEST_ASSERT_EQUAL(preset_file.serial.stop_bits, preset_file2.serial.stop_bits);
	TEST_ASSERT_EQUAL_STRING(preset_file.serial.EOL, preset_file2.serial.EOL);
	TEST_ASSERT_EQUAL(preset_file.task_schedule.period, preset_file2.task_schedule.period);
	TEST_ASSERT_EQUAL(preset_file.task_schedule.offset, preset_file2.task_schedule.offset);
}



void test_deserialize_to_preset() {
	PresetFile preset_file;
	const char* json = "{\"http_client\":{\"access_token\":\"lol\",\"url\":\"https://datoriki.lv\",\"experiment_id\":\"dsga\",\"experiment_description\":\"something_lol\",\"check_certs\":true},\"run_once\":[{\"command\":\"TST1\",\"expect_response\":true},{\"command\":\"TST2\",\"expect_response\":false},{\"command\":\"TST3\",\"expect_response\":true}],\"run_scheduled\":[{\"command\":\"TSE1\",\"expect_response\":true},{\"command\":\"TSE2\",\"expect_response\":false},{\"command\":\"TSE3\",\"expect_response\":true}],\"serial\":{\"baud_rate\":10000,\"byte_size\":8,\"parity\":0,\"EOL\":\"\\n\\r\",\"stop_bits\":0},\"task_schedule\":{\"period\":9600,\"offset\":8}}";
	auto err = deserializeJson(jsonDocument, json, DeserializationOption::NestingLimit(10));
	if(err) {
		TEST_FAIL_MESSAGE(err.c_str());
		TEST_ABORT();
	}
	TEST_ASSERT_EQUAL(0, loadPresetFileFromJSON(preset_file, jsonDocument));
	TEST_ASSERT_EQUAL_STRING("lol", preset_file.http_client.access_token);
	TEST_ASSERT_EQUAL_STRING("https://datoriki.lv", preset_file.http_client.url);
	TEST_ASSERT_EQUAL_STRING("dsga", preset_file.http_client.experiment_id);
	TEST_ASSERT_EQUAL_STRING("something_lol", preset_file.http_client.experiment_description);
	TEST_ASSERT_EQUAL(true, preset_file.http_client.check_certs);
	
	TEST_ASSERT_EQUAL_MESSAGE( 10000, preset_file.serial.baud_rate, "Baud error");
	TEST_ASSERT_EQUAL(8, preset_file.serial.byte_size);
	TEST_ASSERT_EQUAL_MESSAGE(0, preset_file.serial.parity, "Parity error");
	TEST_ASSERT_EQUAL_MESSAGE(0, preset_file.serial.stop_bits, "Stop bit error");
	TEST_ASSERT_EQUAL_STRING("\n\r", preset_file.serial.EOL);
	TEST_ASSERT_EQUAL(9600, preset_file.task_schedule.period);
	TEST_ASSERT_EQUAL(8, preset_file.task_schedule.offset);

	TEST_ASSERT_EQUAL_STRING("TST1", preset_file.run_once[0].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_once[0].expect_response);
	TEST_ASSERT_EQUAL_STRING("TST2", preset_file.run_once[1].command);
	TEST_ASSERT_EQUAL(false, preset_file.run_once[1].expect_response);
	TEST_ASSERT_EQUAL_STRING("TST3", preset_file.run_once[2].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_once[2].expect_response);
	TEST_ASSERT_EQUAL_STRING("TSE1", preset_file.run_scheduled[0].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_scheduled[0].expect_response);
	TEST_ASSERT_EQUAL_STRING("TSE2", preset_file.run_scheduled[1].command);
	TEST_ASSERT_EQUAL(false, preset_file.run_scheduled[1].expect_response);
	TEST_ASSERT_EQUAL_STRING("TSE3", preset_file.run_scheduled[2].command);
	TEST_ASSERT_EQUAL(true, preset_file.run_scheduled[2].expect_response);
}


int main() {
	UNITY_BEGIN();
	RUN_TEST(test_save_load_preset);
	RUN_TEST(test_deserialize_to_preset);
	UNITY_END();
	return 0;
}