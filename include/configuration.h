
#include <ArduinoJson.h>
#include <stdio.h>

/*
Preset file format:
{
	“serial”: {
		“baud_rate”: 9600,
		“byte_size”: 8,
		“parity”: “None”,
		“stop_bits”: 1,
		“EOL”: “\r”
	},
	“run_once”:[
		{“command”: “*IDN?\r”, “expect_response”: true}
	],
	“run_scheduled”:[
		{“command”: “READ?\r”, “expect_response”: true}
	],
	“task_schedule”:{
		“period”: 5,
		“offset”: 1699867392,
	},
	“http_client”: {
		“url”: “http://myserver.com/api/add”,
		“experiment_id”: “EX2023-12-5”,
		“experiment_description”: ”Test experiment”,
		“access_token”: “password123”,
		“check_certs”: false
	},
}
*/
namespace cfg {
	const int COMMAND_LENGTH = 80;
	struct Command {
		char command[COMMAND_LENGTH] = ""; // 79 chars + null terminator
		bool expect_response;
	};
	const int PRESET_ONCE_COUNT = 10;
	const int PRESET_SCHEDULED_COUNT = 5;
	struct PresetFile {
		struct Serial {
			uint32_t baud_rate = 9600;
			uint8_t byte_size = 8;
			uint8_t parity = 0;
			uint8_t stop_bits = 2;
			char EOL[3] = ""; // up to 2 chars + null terminator
		} serial;
		uint8_t run_once_count = 0;
		Command run_once[PRESET_ONCE_COUNT]; // up to 20 commands to run once
		uint8_t run_scheduled_count = 0;
		Command run_scheduled[PRESET_SCHEDULED_COUNT]; // up to 20 commands to run periodically
		struct TaskSchedule {
			uint32_t period = 1; // in seconds
			uint32_t offset = 1; // in seconds, since 1970-01-01 00:00:00
		} task_schedule;
		struct HttpClient {
			char url[128] = ""; // up to 127 chars + null terminator
			char experiment_id[32] = ""; // up to 31 chars + null terminator
			char experiment_description[128] = ""; // up to 127 chars + null terminator
			char access_token[32] = "";// up to 31 chars + null terminator
			bool check_certs = false; // weather to check server certificates
		} http_client;
	};

	int loadPresetFileFromJSON(PresetFile &preset_file, const JsonDocument &jsonDocument);
	int savePresetFileToJSON(const PresetFile &preset_file, JsonDocument &jsonDocument);

	/*
	Network configuration file format:
	{
		“mdns”: “rscpi”,
		“type”: “static”,
		“static”: {
			“ip”: “192.168.0.5”,
			“mask”: “255.255.255.0”,
			“gateway”: “192.168.0.1”,
			“sntp”: [
				“1.1.1.1”,
				“sntp.local”,
			]
		}
	}
	*/

	enum class NetworkType {
	DHCP,
	STATIC
	};

	struct NetworkConfigFile {
	char mdns[32] = ""; // up to 31 chars + null terminator
	NetworkType type;
	struct Static {
		char ip[16] = ""; // up to 15 chars + null terminator
		char mask[16] = ""; // up to 15 chars + null terminator
		char gateway[16] = ""; // up to 15 chars + null terminator
		char sntp[3][64] = {"", "", ""}; // up to 63 chars + null terminator
	} static_config;
	};

	int loadNetworkConfigFileFromJSON(NetworkConfigFile &network_config_file, const JsonDocument &jsonDocument);
	int saveNetworkConfigFileToJSON(const NetworkConfigFile &network_config_file, JsonDocument &jsonDocument);

	/*
	Server configuration file format:
	{
		“https_enabled”: true,
		“username”: “admin”,
		“pass”: “huwfi4892hworbg3oqgb93q5bophftg”
	}
	*/

	struct ServerConfigFile {
	bool https_enabled;
	char username[32] = ""; // up to 31 chars + null terminator
	char pass[32] = ""; // up to 31 chars + null terminator
	};

	int loadServerConfigFileFromJSON(ServerConfigFile &server_config_file, const JsonDocument &jsonDocument);
	int saveServerConfigFileToJSON(const ServerConfigFile &server_config_file, JsonDocument &jsonDocument);

	/*
	user configuration file format:
	{
		“username”: “admin”,
		“pass”: “huwfi4892hworbg3oqgb93q5bophftg”
	}
	*/

	struct UserConfigFile {
	char username[32] = ""; // up to 31 chars + null terminator
	char pass[32] = ""; // up to 31 chars + null terminator
	};

	int loadUserConfigFileFromJSON(UserConfigFile &user_config_file, const JsonDocument &jsonDocument);
	int saveUserConfigFileToJSON(const UserConfigFile &user_config_file, JsonDocument &jsonDocument);

	int loadPresetFileFromJSON(PresetFile &preset_file, const JsonDocument &jsonDocument) {
		// serial
		if(jsonDocument.containsKey("serial")) {
			auto ser = jsonDocument["serial"];
			if(ser.containsKey("baud_rate")) preset_file.serial.baud_rate = ser["baud_rate"];
				else return -1;
			if(ser.containsKey("byte_size")) preset_file.serial.byte_size = ser["byte_size"];
				else return -1;
			if(ser.containsKey("parity")) preset_file.serial.parity = ser["parity"];
				else return -1;
			if(ser.containsKey("stop_bits")) preset_file.serial.stop_bits = ser["stop_bits"];
				else return -1;
			if(ser.containsKey("EOL")) strncpy(preset_file.serial.EOL, ser["EOL"], sizeof(preset_file.serial.EOL)-1);
				else return -1;
			preset_file.serial.EOL[sizeof(preset_file.serial.EOL) - 1] = '\0';
		} else return -1;

		// task schedule
		if(jsonDocument.containsKey("task_schedule")) {
			auto ts = jsonDocument["task_schedule"];
			if(ts.containsKey("period")) preset_file.task_schedule.period = ts["period"];
				else return -1;
			if(ts.containsKey("offset")) preset_file.task_schedule.offset = ts["offset"];
				else return -1;
		} else return -1;
		
		// http client
		if(jsonDocument.containsKey("http_client")){
			auto httpc = jsonDocument["http_client"];
			if(httpc.containsKey("url")) strncpy(preset_file.http_client.url, httpc["url"], sizeof(preset_file.http_client.url)-1);
				else return -1;
			preset_file.http_client.url[sizeof(preset_file.http_client.url) - 1] = '\0';
			if(httpc.containsKey("experiment_id")) strncpy(preset_file.http_client.experiment_id, httpc["experiment_id"], sizeof(preset_file.http_client.experiment_id)-1);
				else return -1;
			preset_file.http_client.experiment_id[sizeof(preset_file.http_client.experiment_id) - 1] = '\0';
			if(httpc.containsKey("experiment_description")) strncpy(preset_file.http_client.experiment_description, httpc["experiment_description"], sizeof(preset_file.http_client.experiment_description)-1);
				else return -1;
			preset_file.http_client.experiment_description[sizeof(preset_file.http_client.experiment_description) - 1] = '\0';
			if(httpc.containsKey("access_token")) strncpy(preset_file.http_client.access_token, httpc["access_token"], sizeof(preset_file.http_client.access_token)-1);
				else return -1;
			preset_file.http_client.access_token[sizeof(preset_file.http_client.access_token) - 1] = '\0';
			if(httpc.containsKey("access_token")) preset_file.http_client.check_certs = httpc["check_certs"];
				else return -1;
		} else return -1;
		
		// run once
		if(jsonDocument.containsKey("run_once")) {
			JsonArrayConst run_once = jsonDocument["run_once"].as<JsonArrayConst>();
			preset_file.run_once_count = run_once.size() > PRESET_ONCE_COUNT ? PRESET_ONCE_COUNT : run_once.size();
			unsigned int i;
			for(i=0;i<preset_file.run_once_count;i++) {
				auto cmd = run_once[i];
				if(cmd.containsKey("command")) strncpy(preset_file.run_once[i].command, cmd["command"].as<const char*>(), sizeof(preset_file.run_once[i].command)-1);
					else return -1;
				preset_file.run_once[i].command[sizeof(preset_file.run_once[i].command) - 1] = '\0';
				
				if(cmd.containsKey("expect_response")) preset_file.run_once[i].expect_response = cmd["expect_response"];
					else return -1;
			}
			if(i < PRESET_ONCE_COUNT) preset_file.run_once[i].command[0] = '\0';

		} else return -1;

		// run scheduled
		if(jsonDocument.containsKey("run_scheduled")) {
			JsonArrayConst run_scheduled = jsonDocument["run_scheduled"].as<JsonArrayConst>();
			preset_file.run_scheduled_count = run_scheduled.size() > PRESET_SCHEDULED_COUNT ? PRESET_SCHEDULED_COUNT : run_scheduled.size();
			unsigned int i;
			for(i=0;i<preset_file.run_scheduled_count;i++) {
				auto cmd = run_scheduled[i];
				if(cmd.containsKey("command")) strncpy(preset_file.run_scheduled[i].command, run_scheduled[i]["command"], sizeof(preset_file.run_scheduled[i].command)-1);
					else return -1;
				preset_file.run_scheduled[i].command[sizeof(preset_file.run_scheduled[i].command) - 1] = '\0';
				if(cmd.containsKey("expect_response")) preset_file.run_scheduled[i].expect_response = run_scheduled[i]["expect_response"];
					else return -1;
			}
			if(i < PRESET_SCHEDULED_COUNT) preset_file.run_scheduled[i].command[0] = '\0';
		} else return -1;
		return 0;
	}

	int savePresetFileToJSON(const PresetFile &preset_file, JsonDocument &jsonDocument){
		jsonDocument.clear();
		// serial
		JsonObject serial = jsonDocument.createNestedObject("serial");
		serial["baud_rate"] = preset_file.serial.baud_rate;
		serial["byte_size"] = preset_file.serial.byte_size;
		serial["parity"] = preset_file.serial.parity;
		serial["stop_bits"] = preset_file.serial.stop_bits;
		serial["EOL"] = preset_file.serial.EOL;

		// task schedule
		JsonObject task_schedule = jsonDocument.createNestedObject("task_schedule");
		task_schedule["period"] = preset_file.task_schedule.period;
		task_schedule["offset"] = preset_file.task_schedule.offset;

		// http client
		JsonObject http_client = jsonDocument.createNestedObject("http_client");
		http_client["url"] = preset_file.http_client.url;
		http_client["experiment_id"] = preset_file.http_client.experiment_id;
		http_client["experiment_description"] = preset_file.http_client.experiment_description;
		http_client["access_token"] = preset_file.http_client.access_token;
		http_client["check_certs"] = preset_file.http_client.check_certs;

		// run once
		JsonArray run_once = jsonDocument.createNestedArray("run_once");
		
		for(unsigned int i=0;i<preset_file.run_once_count && preset_file.run_once[i].command[0]!='\0';i++) {
			auto new_cmd = run_once.add();
			new_cmd["command"] = preset_file.run_once[i].command;
			new_cmd["expect_response"] = preset_file.run_once[i].expect_response;
		}
		

		// run scheduled
		JsonArray run_scheduled = jsonDocument.createNestedArray("run_scheduled");
		for(unsigned int i=0;i<preset_file.run_scheduled_count && preset_file.run_scheduled[i].command[0]!='\0';i++) {
			auto new_cmd = run_scheduled.add();
			new_cmd["command"] = preset_file.run_scheduled[i].command;
			new_cmd["expect_response"] = preset_file.run_scheduled[i].expect_response;
		}
		return 0;
	}

	int loadNetworkConfigFileFromJSON(NetworkConfigFile &network_config_file, const JsonDocument &jsonDocument) {
		if(jsonDocument.containsKey("mdns")) { 
			strncpy(network_config_file.mdns, jsonDocument["mdns"], sizeof(network_config_file.mdns));
		} else return -1;
		network_config_file.mdns[sizeof(network_config_file.mdns) - 1] = '\0';
		if (strcasecmp(jsonDocument["type"], "static") == 0) {
			network_config_file.type = NetworkType::STATIC;
			if (!jsonDocument.containsKey("static")) return -1;
			if (!jsonDocument["static"].containsKey("ip")) return -1;
			strncpy(network_config_file.static_config.ip, jsonDocument["static"]["ip"], sizeof(network_config_file.static_config.ip));
			network_config_file.static_config.ip[sizeof(network_config_file.static_config.ip) - 1] = '\0';

			if (!jsonDocument["static"].containsKey("mask")) return -1;
			strncpy(network_config_file.static_config.mask, jsonDocument["static"]["mask"], sizeof(network_config_file.static_config.mask));
			network_config_file.static_config.mask[sizeof(network_config_file.static_config.mask) - 1] = '\0';

			if (!jsonDocument["static"].containsKey("gateway")) return -1;
			strncpy(network_config_file.static_config.gateway, jsonDocument["static"]["gateway"], sizeof(network_config_file.static_config.gateway));
			network_config_file.static_config.gateway[sizeof(network_config_file.static_config.gateway) - 1] = '\0';

			if (!jsonDocument["static"].containsKey("sntp")) return -1;
			JsonArrayConst sntp = jsonDocument["static"]["sntp"];
			for(unsigned int i=0;i<sntp.size() && i<3;i++) {
				strncpy(network_config_file.static_config.sntp[i], sntp[i], sizeof(network_config_file.static_config.sntp[i]));
				network_config_file.static_config.sntp[i][sizeof(network_config_file.static_config.sntp[i]) - 1] = '\0';
			}
		} else {
			network_config_file.type = NetworkType::DHCP;
		}
		return 0;
	}

	int saveNetworkConfigFileToJSON(const NetworkConfigFile &network_config_file, JsonDocument &jsonDocument){
		jsonDocument.clear();
		jsonDocument["mdns"] = network_config_file.mdns;
		if (network_config_file.type == NetworkType::STATIC) {
			jsonDocument["type"] = "static";
			JsonObject static_config = jsonDocument.createNestedObject("static");
			static_config["ip"] = network_config_file.static_config.ip;
			static_config["mask"] = network_config_file.static_config.mask;
			static_config["gateway"] = network_config_file.static_config.gateway;
			JsonArray sntp = static_config.createNestedArray("sntp");
			for(int i=0;i<3;i++) {
				sntp.add(network_config_file.static_config.sntp[i]);
			}
		} else {
			jsonDocument["type"] = "dhcp";
		}
		return 0;
	}

	int loadServerConfigFileFromJSON(ServerConfigFile &server_config_file, const JsonDocument &jsonDocument) {
		if (!jsonDocument.containsKey("https_enabled")) return -1;
		server_config_file.https_enabled = jsonDocument["https_enabled"];

		if (!jsonDocument.containsKey("username")) return -1;
		strncpy(server_config_file.username, jsonDocument["username"], sizeof(server_config_file.username));
		server_config_file.username[sizeof(server_config_file.username) - 1] = '\0';

		if (!jsonDocument.containsKey("pass")) return -1;
		strncpy(server_config_file.pass, jsonDocument["pass"], sizeof(server_config_file.pass));
		server_config_file.pass[sizeof(server_config_file.pass) - 1] = '\0';
		return 0;
	}

	int saveServerConfigFileToJSON(const ServerConfigFile &server_config_file, JsonDocument &jsonDocument){
		jsonDocument.clear();
		jsonDocument["https_enabled"] = server_config_file.https_enabled;
		jsonDocument["username"] = server_config_file.username;
		jsonDocument["pass"] = server_config_file.pass;
		return 0;
	}

	int loadUserConfigFileFromJSON(UserConfigFile &user_config_file, const JsonDocument &jsonDocument) {
		if(!jsonDocument.containsKey("pass")) return -1;
		strncpy(user_config_file.pass, jsonDocument["pass"], sizeof(user_config_file.pass));
		user_config_file.pass[sizeof(user_config_file.pass) - 1] = '\0';

		if(!jsonDocument.containsKey("username")) return -1;
		strncpy(user_config_file.username, jsonDocument["username"], sizeof(user_config_file.username));
		user_config_file.username[sizeof(user_config_file.username) - 1] = '\0';

		return 0;
	}

	int saveUserConfigFileToJSON(const UserConfigFile &user_config_file, JsonDocument &jsonDocument){
		jsonDocument.clear();
		jsonDocument["pass"] = user_config_file.pass;
		jsonDocument["username"] = user_config_file.username;

		return 0;
	}

	struct BatterySettings {
		float voltage_high;
		float voltage_low;
		uint16_t capacity;
		uint16_t charge;
		float temperature_high;
		float temperature_low;
	};

	int loadBatterySettingsFromJSON(BatterySettings &battery_settings, const JsonDocument &jsonDocument) {
		if(!jsonDocument.containsKey("voltage_high")) return -1;
		battery_settings.voltage_high = jsonDocument["voltage_high"];
		if(!jsonDocument.containsKey("voltage_low")) return -1;
		battery_settings.voltage_low = jsonDocument["voltage_low"];
		if(!jsonDocument.containsKey("capacity")) return -1;
		battery_settings.capacity = jsonDocument["capacity"];
		if(!jsonDocument.containsKey("charge")) return -1;
		battery_settings.charge = jsonDocument["charge"];
		if(!jsonDocument.containsKey("temperature_high")) return -1;
		battery_settings.temperature_high = jsonDocument["temperature_high"];
		if(!jsonDocument.containsKey("temperature_low")) return -1;
		battery_settings.temperature_low = jsonDocument["temperature_low"];
		return 0;
	}

	int saveBatterySettingsToJSON(const BatterySettings &battery_settings, JsonDocument &jsonDocument){
		jsonDocument.clear();
		jsonDocument["voltage_high"] = battery_settings.voltage_high;
		jsonDocument["voltage_low"] = battery_settings.voltage_low;
		jsonDocument["capacity"] = battery_settings.capacity;
		jsonDocument["charge"] = battery_settings.charge;
		jsonDocument["temperature_high"] = battery_settings.temperature_high;
		jsonDocument["temperature_low"] = battery_settings.temperature_low;
		return 0;
	}

}