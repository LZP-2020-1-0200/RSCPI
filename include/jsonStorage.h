/**
 * @file jsonStorage.h
 * @author Oskars Putans (o.putaans@gmail.com)
 * @brief This file contains functions for storing and loading data from the LittleFS filesystem.
 */

#include <ArduinoJson.h>
#include <LittleFS.h>

namespace loc {

	/// @brief The path to the battery configuration file.
	const char* battery = "/battery.json";

	/// @brief The path to the user configuration file.
	const char* user = "/user.json";

	/// @brief The path to the http server configuration file.
	const char* http_server = "/http_server.json";

	/// @brief The path to the network configuration file.
	const char* network = "/network.json";

	/// @brief The path to the system configuration file.
	const char* system = "/system.json";

	/// @brief The path to the presets directory.
	const char* preset_dir = "/presets";

	/// @brief Saves the json data to the file.
	/// @param filename The name of the file to save the data to.
	/// @param data The json data to save.
	/// @return 0 on success, -1 on failure.
	int saveData(const char* filename, const JsonDocument& data) {
		// Open file for writing
		File file = LittleFS.open(filename, "w");
		if (!file) {
			// Failed to open file for writing
			//Serial.println(F("Failed to create file"));
			return -1;
		}
		serializeJson(data, file);
		file.close();
		return 0;
		
	}

	/**
	 * @brief Loads the json data from the specified file.
	 * 
	 * @param filename The name of the file to load the data from.
	 * @param data Reference to the JsonDocument to load the data into.
	 * @return 0 on success, -1 on failure.
	 */
	int loadData(const char* filename, JsonDocument& data) {
		// Open file for reading
		File file = LittleFS.open(filename, "r");
		if (!file) {
			// Failed to open file for reading
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
