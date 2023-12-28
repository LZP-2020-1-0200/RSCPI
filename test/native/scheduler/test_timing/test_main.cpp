#include <unity.h>
#include "scheduler.h"

Scheduler scheduler;

int callCounter = 0;

void setUp() {
	scheduler.clearTasks();
	callCounter = 0;
}

void tearDown() {
	uint8_t errFlags = DataBuffer::getErrFlags();
	DataBuffer::clearErrFlags();
	TEST_ASSERT_EQUAL(0, errFlags);
}

void test_no_tasks() {
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(100);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(500);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(1000);
	TEST_ASSERT_EQUAL(0, callCounter);
}


void test_schedule_now() {
	int id = scheduler.schedule([](void){callCounter++;}, 53);
	TEST_ASSERT_NOT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(23);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(54);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(2345);
	TEST_ASSERT_EQUAL(1, callCounter);
}

void test_schedule_multiple() {
	scheduler.schedule([](void){callCounter |= 0b00000001;}, 854);
	scheduler.schedule([](void){callCounter |= 0b00000100;}, 242);
	scheduler.schedule([](void){callCounter |= 0b00100000;}, 447);
	scheduler.schedule([](void){callCounter |= 0b10000000;}, 102);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(100);
	TEST_ASSERT_EQUAL(0b00000000, callCounter);
	scheduler.update(500);
	TEST_ASSERT_EQUAL(0b10100100, callCounter);
	scheduler.update(1000);
	TEST_ASSERT_EQUAL(0b10100101, callCounter);
}

void test_repeat_task() {
	scheduler.scheduleRepeat([](void){callCounter++;}, 100, 4000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(5000);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(5001);
	TEST_ASSERT_EQUAL(2, callCounter);
	for(int i = 0; i<40; i++) {
		scheduler.update(5003);
	}
	TEST_ASSERT_EQUAL(11, callCounter);
}

void test_clear_tasks() {
	auto func = [](void){callCounter++;};
	scheduler.schedule(func, 100);
	scheduler.schedule(func, 200);
	scheduler.schedule(func, 300);
	scheduler.scheduleRepeat(func, 400, 0);
	scheduler.scheduleRepeatUntil(func, 400, 0, 0);
	scheduler.schedule(func, 400);
	TEST_ASSERT_EQUAL(6, scheduler.getTaskCount());
	scheduler.clearTasks();
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}



int main() {
	UNITY_BEGIN();
	RUN_TEST(test_no_tasks);
	RUN_TEST(test_schedule_now);
	RUN_TEST(test_schedule_multiple);
	RUN_TEST(test_repeat_task);
	RUN_TEST(test_clear_tasks);
	UNITY_END();
	return 0;
}