#include <ArduinoJson.h>
#include <LittleFS.h>

namespace loc {
	const char* battery = "/battery.json";
	const char* user = "/user.json";
	const char* http_server = "/http_server.json";
	const char* network = "/network.json";
	const char* system = "/system.json";
	const char* preset_dir = "/presets";

	int saveData(const char* filename, const JsonDocument& data) {
		// Open file for writing
		File file = LittleFS.open(filename, "w");
		if (!file) {
			//Serial.println(F("Failed to create file"));
			return -1;
		}
		serializeJson(data, file);
		file.close();
		return 0;
		
	}

	int loadData(const char* filename, JsonDocument& data) {
		// Open file for reading
		File file = LittleFS.open(filename, "r");
		if (!file) {
			//Serial.println(F("Failed to read file"));
			return -1;
		}
		
		// Deserialize the JSON document
		DeserializationError error = deserializeJson(data, file);
		if (error) {
			//Serial.println(F("Failed to parse file"));
			return -1;
		}
		
		file.close();
		return 0;
	}
}
