
#pragma once
#include <utility>
#include "dataBuffer.h"

// hashes will reach TASK_RUN_COUNT_LOOPOVER*SCHEDULER_SIZE
// before they start to repeat
// Increase this value if tasks swap out too quickly
#ifndef TASK_RUN_COUNT_LOOPOVER
#define TASK_RUN_COUNT_LOOPOVER 200
#endif


enum class TaskType {
	Once,
	Repeat,
	RepeatUntil
};

struct Task {
	void (*functionWithBuffer)(DataBuffer& data);
	void (*function)(void);
	TaskType type;
	unsigned long startTimestamp;
	unsigned long period;
	unsigned long endTimestamp;
	long lastIndex;
	DataBuffer data;
	long runCount = 0;

	void run() {
		if (functionWithBuffer != nullptr) {
			functionWithBuffer(data);
		}
		if (function != nullptr) {
			function();
		}
	}
	bool clear() {
		
		functionWithBuffer = nullptr;
		function = nullptr;
		if(data.isDataSet()) data.clear();
		return !data.isDataSet();
	}

	bool isSet() const {
		return functionWithBuffer != nullptr || function != nullptr;
	}

	void updateTask(void (*function)(void), 
			unsigned long startTimestamp, 
			unsigned long period, 
			unsigned long endTimestamp) {
		//Serial.println("updateTask with endTimestamp");
		this->functionWithBuffer = nullptr;
		this->function = function;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->endTimestamp = endTimestamp;
		this->lastIndex = -1;
		this->type = TaskType::RepeatUntil;
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	void updateTask(void (*function)(void), 
			unsigned long startTimestamp, 
			unsigned long period) {
		//Serial.println("updateTask repeat");
		this->function = function;
		this->functionWithBuffer = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->endTimestamp = 0;
		this->lastIndex = -1;
		this->type = TaskType::Repeat;
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	void updateTask(void (*function)(void), 
			unsigned long startTimestamp) {
		//Serial.println("updateTask once");
		this->function = function;
		this->functionWithBuffer = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = 0;
		this->endTimestamp = 0;
		this->lastIndex = -1;
		this->type = TaskType::Once;
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp, 
			unsigned long period, 
			unsigned long endTimestamp, 
			const typename std::remove_reference<T>& data) {
		//Serial.println("updateTask with endTimestamp and copied data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->endTimestamp = endTimestamp;
		this->lastIndex = -1;
		this->type = TaskType::RepeatUntil;
		this->data.set<T>(data);
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}
	

	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp, 
			unsigned long period, 
			unsigned long endTimestamp, 
			typename std::remove_reference<T>::type&& data) {
		//Serial.println("updateTask with endTimestamp and moved data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->endTimestamp = endTimestamp;
		this->lastIndex = -1;
		this->type = TaskType::RepeatUntil;
		this->data.set<T>(data);
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp, 
			unsigned long period, 
			const typename std::remove_reference<T>::type& data) {
		//Serial.println("updateTask repeat with copied data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->lastIndex = -1;
		this->type = TaskType::Repeat;
		this->data.set<T>(data);
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp, 
			unsigned long period,
			typename std::remove_reference<T>::type&& data) {
		//Serial.println("updateTask repeat with moved data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->period = period;
		this->lastIndex = -1;
		this->type = TaskType::Repeat;
		this->data.set<T>(data);
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}


	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp, 
			const typename std::remove_reference<T>::type& data) {
		//Serial.println("updateTask once with copied data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->lastIndex = -1;
		this->type = TaskType::Once;
		this->data.set<T>(std::move(data));
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
	}

	template <typename T>
	void updateTask(void (*function)(DataBuffer&), 
			unsigned long startTimestamp,
			typename std::remove_reference<T>::type&& data) {
		//Serial.println("updateTask once with moved data");
		this->functionWithBuffer = function;
		this->function = nullptr;
		this->startTimestamp = startTimestamp;
		this->lastIndex = -1;
		this->type = TaskType::Once;
		this->data.set<T>(std::move(data));
		runCount = (runCount + 1) % TASK_RUN_COUNT_LOOPOVER;
		//Serial.println("updateTask once with moved data end");
	}
};