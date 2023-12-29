/**
 * @file presetLoader.h
 * @author Oskars Putans (o.putaans@gmail.com)
 * @brief This file contains a function for setting up
 * commands from the preset file. This file is relatively
 * unfinished, as it is not used in the current version.
 * It is left here for future development.
 */

#include "configuration.h"
#include "scheduler.h"
#include <Arduino.h>
#pragma once


/// @brief Schedule the commands from the preset file.
/// @param data The reference to the DataBuffer containing the preset file struct.
static void sendOnceCommand(DataBuffer& data) {
	cfg::PresetFile& presetFile = data.get<cfg::PresetFile>();
	//Serial.println("Sending command");
	Serial.swap();
	for(int i = 0; i < presetFile.run_once_count; i++) {
		cfg::Command& command = presetFile.run_once[i];
		Serial.print(command.command);
		Serial.print(presetFile.serial.EOL);
		int k;
		for(k = 1; k < 3 && presetFile.serial.EOL[k] != 0; k++);
		char finalEOL = presetFile.serial.EOL[k-1];
		if(command.expect_response) {
			String response = Serial.readStringUntil(finalEOL);
		}
	}
	Serial.swap();
}

/// @brief Schedule the commands to be repeated from the preset file.
/// @param data Reference to the DataBuffer containing the preset file struct.
static void sendRepeatCommand(DataBuffer& data) {
	cfg::PresetFile& presetFile = data.get<cfg::PresetFile>();
	//Serial.println("Sending command");
	Serial.swap();
	for(int i = 0; i < presetFile.run_scheduled_count; i++) {
		cfg::Command& command = presetFile.run_scheduled[i];
		Serial.print(command.command);
		Serial.print(presetFile.serial.EOL);
		int k;
		for(k = 1; k < 3 && presetFile.serial.EOL[k] != 0; k++);
		char finalEOL = presetFile.serial.EOL[k-1];
		if(command.expect_response) {
			String response = Serial.readStringUntil(finalEOL);
		}
	}
	Serial.swap();
}

/// @brief Set up the preset commands.
/// @param scheduler Reference to the scheduler.
/// @param presetFile Reference to the preset file struct.
void setUpPresetCommands(Scheduler& scheduler, cfg::PresetFile& presetFile) {
	// set up preset commands
	scheduler.schedule<cfg::PresetFile>(sendOnceCommand, 0, presetFile);
	scheduler.schedule<cfg::PresetFile>(sendRepeatCommand, 0, presetFile);
}