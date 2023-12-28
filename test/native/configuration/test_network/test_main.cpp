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

void test_save_load_network() {
	NetworkConfigFile network;

	strcpy(network.mdns, "rscpii");
	network.type = NetworkType::STATIC;
	strcpy(network.static_config.ip, "192.168.100.100");
	strcpy(network.static_config.mask, "255.255.255.0");
	strcpy(network.static_config.sntp[0], "lol.date.com");
	strcpy(network.static_config.gateway, "192.168.0.1");

	TEST_ASSERT_EQUAL(0, saveNetworkConfigFileToJSON(network, jsonDocument));
	NetworkConfigFile network2;
	TEST_ASSERT_EQUAL(0, loadNetworkConfigFileFromJSON(network2, jsonDocument));
	
	TEST_ASSERT_EQUAL_STRING("192.168.100.100", network.static_config.ip);
	TEST_ASSERT_EQUAL_STRING("255.255.255.0", network.static_config.mask);
	TEST_ASSERT_EQUAL_STRING("lol.date.com", network.static_config.sntp[0]);
	TEST_ASSERT_EQUAL_STRING("192.168.0.1", network.static_config.gateway);
	TEST_ASSERT_EQUAL_STRING("rscpii", network.mdns);
	TEST_ASSERT_EQUAL(NetworkType::STATIC, network.type);
	
}



void test_deserialize_to_network_dhcp() {
	NetworkConfigFile network;
	const char* json = "{ \"mdns\": \"rscpi\", \"type\": \"dhcp\", \"static\": { \"ip\": \"192.168.0.5\", \"mask\": \"255.255.255.0\", \"gateway\": \"192.168.0.1\", \"sntp\": [ \"1.1.1.1\", \"sntp.local\" ] } }";
	auto err = deserializeJson(jsonDocument, json, DeserializationOption::NestingLimit(10));
	if(err) {
		TEST_FAIL_MESSAGE(err.c_str());
		TEST_ABORT();
	}
	TEST_ASSERT_EQUAL(0, loadNetworkConfigFileFromJSON(network, jsonDocument));


	TEST_ASSERT_EQUAL_STRING("rscpi", network.mdns);
	TEST_ASSERT_EQUAL(NetworkType::DHCP, network.type);
	
	
}

void test_deserialize_to_network_static() {
	NetworkConfigFile network;
	const char* json = "{ \"mdns\": \"rscpi\", \"type\": \"static\", \"static\": { \"ip\": \"192.168.0.5\", \"mask\": \"255.255.255.0\", \"gateway\": \"192.168.0.1\", \"sntp\": [ \"1.1.1.1\", \"sntp.local\" ] } }";
	auto err = deserializeJson(jsonDocument, json, DeserializationOption::NestingLimit(10));
	if(err) {
		TEST_FAIL_MESSAGE(err.c_str());
		TEST_ABORT();
	}
	TEST_ASSERT_EQUAL(0, loadNetworkConfigFileFromJSON(network, jsonDocument));


	TEST_ASSERT_EQUAL_STRING("rscpi", network.mdns);
	TEST_ASSERT_EQUAL(NetworkType::STATIC, network.type);
	TEST_ASSERT_EQUAL_STRING("192.168.0.5", network.static_config.ip);
	TEST_ASSERT_EQUAL_STRING("255.255.255.0", network.static_config.mask);
	TEST_ASSERT_EQUAL_STRING("192.168.0.1", network.static_config.gateway);
	TEST_ASSERT_EQUAL_STRING("1.1.1.1", network.static_config.sntp[0]);
	TEST_ASSERT_EQUAL_STRING("sntp.local", network.static_config.sntp[1]);
	
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_save_load_network);
	RUN_TEST(test_deserialize_to_network_dhcp);
	RUN_TEST(test_deserialize_to_network_static);
	UNITY_END();
	return 0;
}