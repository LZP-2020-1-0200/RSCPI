/**
 * @file battery.h
 * This file contains the Battery class, which is used to interface with the LTC2942 battery monitor IC.
 * The class is used to read and write the battery state, and to configure the battery monitor.
 * The class also contains functions to save and load the battery state to and from the filesystem.
 */

#include <LTC2942.h>
#include "configuration.h"
#include <ArduinoJson.h>
#include "jsonStorage.h"
#pragma once

/**
 * @brief The Battery class is used to interface with the LTC2942 battery monitor IC.
*/
class Battery {
	LTC2942 ltc;
	cfg::BatterySettings battery;
public:
	Battery(TwoWire& wire) : ltc() {
		ltc.begin(wire);
		ltc.configureALCC(0b10);
		this->loadState();
	}
	void loadState() {
		
		StaticJsonDocument<1000> jsonDocument;
		loc::loadData(loc::battery, jsonDocument);
		cfg::loadBatterySettingsFromJSON(battery, jsonDocument);
		ltc.setVoltageThresholds(battery.voltage_high, battery.voltage_low);
		ltc.setTemperatureThresholds(battery.temperature_high, battery.temperature_low);
		ltc.setBatteryCapacity(battery.capacity);
		ltc.setRawAccumulatedCharge(battery.charge);
	}

	void saveState() {
		cfg::BatterySettings battery;
		battery.charge = ltc.getRawAccumulatedCharge();
		StaticJsonDocument<1000> jsonDocument;
		cfg::saveBatterySettingsToJSON(battery, jsonDocument);
		loc::saveData(loc::battery, jsonDocument);
	}


	float getVoltage() {
		return ltc.getVoltage();
	}

	float getTemperature() {
		return ltc.getTemperature();
	}

	float getRemainingCapacity() {
		return ltc.getRemainingCapacity();
	}

	float getCharge() {
		return ltc.getRawAccumulatedCharge();
	}

	void setCharge(uint16_t charge) {
		ltc.setRawAccumulatedCharge(charge);
		this->saveState();
	}

	void setCapacity(uint16_t capacity) {
		ltc.setBatteryCapacity(capacity);
	}

	void setToFull() {
		ltc.setBatteryToFull();
	}

	void setVoltageThresholds(float high, float low) {
		ltc.setVoltageThresholds(high, low);
	}

	void setTemperatureThresholds(float high, float low) {
		ltc.setTemperatureThresholds(high, low);
	}


};