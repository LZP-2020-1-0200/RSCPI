
#include "configuration.h"
#include "scheduler.h"
#include <Arduino.h>
#pragma once


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

void setUpPresetCommands(Scheduler& scheduler, cfg::PresetFile& presetFile) {
	// set up preset commands
	scheduler.schedule<cfg::PresetFile>(sendOnceCommand, 0, presetFile);
	scheduler.schedule<cfg::PresetFile>(sendRepeatCommand, 0, presetFile);
}