/**
 * @file scheduler.h
 * @author Oskars Putans (o.putaans@gmail.com)
 * @brief This file contains the Scheduler class, which is used to schedule tasks.
 * 
 * This file depends on the Task class, which is defined in task.h.
 * This file is microcontroller independent, so it can be used in a native environment for testing.
 */


#pragma once
#include <utility>
#include "dataBuffer.h"
#include "task.h"
// add a define to use this file in a native environment for logging
#ifdef NATIVE_TEST
	#include <stdio.h>
#endif

#ifndef SCHEDULER_SIZE
#define SCHEDULER_SIZE 25
#endif



/// @brief General error codes for the scheduler.
enum SCH_ERR {
	SCH_ERR_NONE = 0b10000001,
	SCH_ERR_BAD_TEARDOWN = 0b10000010,
	SCH_ERR_BAD_TASK_HASH = 0b10000100,
	SCH_ERR_TASK_ALREADY_KILLED = 0b10001000,
	SCH_ERR_NO_SPACE = 0b10010000,
	SCH_ERR_UNRECOGNIZED_TASK_TYPE = 0b10100000
};

/// @brief The Scheduler class is used to schedule tasks.
/// The Scheduler class is used to schedule tasks.
/// The Scheduler class is microcontroller independent, so it can be used in a native environment.
class Scheduler {
private:
	/// @brief The array of tasks.
	Task taskList[SCHEDULER_SIZE];
public:
	Scheduler() {
		for(unsigned int i = 0; i < SCHEDULER_SIZE; i++) {
			taskList[i].functionWithBuffer = nullptr;
			taskList[i].runCount = 0;
		}
	}

	/// @brief Checks if the task should be run, and runs it if necessary.
	/// @param time The current time.
	/// @return Error code.
	uint16_t update(unsigned long long time) {
		uint16_t errCode = 0;
		// check scheduled tasks
		for(unsigned int i = 0; i < SCHEDULER_SIZE; i++) {
			Task& task = taskList[i];
			if (!task.isSet()) continue;

			// Tasks to be run once
			if (task.type == TaskType::Once) {
				if (task.startTimestamp + task.period < time) {
					task.run();
					// handle teardown and clearing of data
					if(!task.clear()) {
						// if the data is not cleared, the teardown function was not formed correctly
						#ifdef NATIVE_TEST
							printf("Tear down function was not formed correctly\n");
						#endif
						errCode |= SCH_ERR_BAD_TEARDOWN;
					}
				}
				continue;
			}

			// Tasks to be run repeatedly
			if(task.type == TaskType::Repeat) {
				const long timeSinceStart = ((long long)(time) - (long long)task.startTimestamp);
				const long repeatIndex = timeSinceStart/(long long)task.period;
				if (repeatIndex > task.lastIndex) {
					task.lastIndex++;
					task.run();
				}
				continue;
			} 
			
			// Tasks to be run until a certain time
			if(task.type == TaskType::RepeatUntil) {
				const long timeSinceStart = ((long long)(time) - (long long)task.startTimestamp);
				const long repeatIndex = timeSinceStart/(long long)task.period;
				// check if the task should be removed before executing it
				if (task.endTimestamp < time) {
					if(!task.clear()){
						// if the data is not cleared, the teardown function was not formed correctly
						#ifdef NATIVE_TEST
							printf("Tear down function was not formed correctly\n");
						#endif
						errCode |= SCH_ERR_BAD_TEARDOWN;
					}
					continue;
				}
				if (repeatIndex > task.lastIndex) {
					task.lastIndex++;
					task.run();
				}
				continue;
			}
			
			// if the task type is not recognized, return an error
			errCode |= SCH_ERR_UNRECOGNIZED_TASK_TYPE;
			continue;

		}
		return errCode;
	}

	/// @brief Fetches the task hash for the task at the specified index.
	/// @param i index of the task.
	/// @return The task hash.
	int getTaskHash(unsigned int i) {
		return taskList[i].runCount*SCHEDULER_SIZE + i;
	}

	/// @brief Schedules a task to be run once.
	int schedule(void (*func)(void), unsigned long startTimestamp) {
		//Serial.println("schedule");
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask(func, startTimestamp);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	/// @brief Schedules a task to be run once with data.
	template <typename T>
	int schedule(void (*func)(DataBuffer&), unsigned long startTimestamp, typename std::remove_reference<T>::type&& data) {
		//Serial.println("schedule with moved data");
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask<T>(func, startTimestamp, std::move(data));
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	/// @brief Schedules a task to be run once with data.
	template <typename T>
	int schedule(void (*func)(DataBuffer&), unsigned long startTimestamp, const typename std::remove_reference<T>::type& data) {
		//Serial.println("schedule with copied data");
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask<T>(func, startTimestamp, data);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}
	
	/// @brief Clears all tasks, clearing the data and calling the teardown function in the process.
	void clearTasks() {
		for(unsigned int i = 0; i< SCHEDULER_SIZE; i++) {
			taskList[i].clear();
		}
	}

	int scheduleRepeat(void (*func)(void), unsigned long period, unsigned long startTimestamp) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask(func, startTimestamp, period);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	template <typename T>
	int scheduleRepeat(void (*func)(DataBuffer&), unsigned long period, unsigned long startTimestamp, typename std::remove_reference<T>::type&& data) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask<T>(func, startTimestamp, period, std::move(data));
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	
	// overwrites a repeat task
	template <typename T>
	int scheduleRepeat(void (*func)(DataBuffer&), unsigned long period, unsigned long startTimestamp, const typename std::remove_reference<T>::type& data) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask(func, startTimestamp, period, data);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return SCH_ERR_NO_SPACE;
	}
	
	int scheduleRepeatUntil(void (*func)(void), unsigned long period, unsigned long startTimestamp, unsigned long endTimestamp) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask(func, startTimestamp, period, endTimestamp);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	template <typename T>
	int scheduleRepeatUntil(void (*func)(DataBuffer&), unsigned long period, unsigned long startTimestamp, unsigned long endTimestamp, typename std::remove_reference<T>::type&& data) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask<T>(func, startTimestamp, period, endTimestamp, std::move(data));
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return -1;
	}

	
	template <typename T>
	int scheduleRepeatUntil(void (*func)(DataBuffer&), unsigned long period, unsigned long startTimestamp, unsigned long endTimestamp, const typename std::remove_reference<T>::type& data) {
		for(unsigned int i = 0; i< SCHEDULER_SIZE;i++) {
			if (!taskList[i].isSet()) {
				taskList[i].updateTask(func, startTimestamp, period, endTimestamp, data);
				return getTaskHash(i);
			}
		}
		// since there wasn't any space for the task, return an error
		return SCH_ERR_NO_SPACE;
	}
	

	const Task* getTasks() const {
		return taskList;
	}

	unsigned int getTaskCount() const {
		unsigned int count = 0;
		for(unsigned int i = 0; i< SCHEDULER_SIZE; i++) {
			if (taskList[i].functionWithBuffer != nullptr || taskList[i].function != nullptr) {
				count++;
			}
		}
		return count;
	}

	int killTask(int taskHash) {
		const int index = taskHash % SCHEDULER_SIZE;
		if (taskList[index].functionWithBuffer == nullptr && taskList[index].function == nullptr) {
			return SCH_ERR_TASK_ALREADY_KILLED;
		}
		// check if the task hash is correct, if not, the task was already killed
		if(getTaskHash(index) != taskHash) {
			return SCH_ERR_BAD_TASK_HASH;
		}
		if(!taskList[index].clear()) {
			// if the data is not cleared, the teardown function was not formed correctly
			#ifdef NATIVE_TEST
				printf("Tear down function was not formed correctly\n");
			#endif
			return SCH_ERR_BAD_TEARDOWN;
		}
		return 0;
	}
};