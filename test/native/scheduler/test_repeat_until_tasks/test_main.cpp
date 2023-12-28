#include <unity.h>
#include "scheduler.h"

Scheduler scheduler;

int callCounter = 0;

void setUp() {
	scheduler.clearTasks();
	callCounter = 0;
}

void tearDown() {
	scheduler.clearTasks();
	uint8_t errFlags = DataBuffer::getErrFlags();
	DataBuffer::clearErrFlags();
	TEST_ASSERT_EQUAL(0, errFlags);
}

auto function = [](void){callCounter++;};
struct Data {
	int a;
	int b;
};

auto dataFunction = [](DataBuffer& data){
	data.get<Data>().a++; 
	callCounter++;
};



void test_single() {
	scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(2, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(3, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3, callCounter);
	for(int i = 0; i<50;i++) scheduler.update(5000);
	TEST_ASSERT_EQUAL(11, callCounter);
}

void test_multiple() {
	scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
	scheduler.scheduleRepeatUntil(function, 100, 4100, 5000);
	scheduler.scheduleRepeatUntil(function, 100, 4200, 5000);
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i<50;i++) scheduler.update(5000);
	TEST_ASSERT_EQUAL(30, callCounter);
}

void test_end_early() {
	scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
	scheduler.scheduleRepeatUntil(function, 100, 4100, 5000);
	scheduler.scheduleRepeatUntil(function, 100, 4200, 5000);
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i<50;i++) scheduler.update(5001);
	TEST_ASSERT_EQUAL(0, callCounter);

}


void test_data() {
	scheduler.scheduleRepeatUntil<Data>(dataFunction, 100, 4000, 5000, std::move(Data{1,2}));
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(2, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(3, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3, callCounter);
}

void test_clear() {
	scheduler.scheduleRepeatUntil<Data>(dataFunction, 100, 4000, 5000, std::move(Data{1,2}));
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(2, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(3, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3, callCounter);
	scheduler.clearTasks();
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3, callCounter);
}

void test_multiple_data() {
	scheduler.scheduleRepeatUntil<Data>(dataFunction, 100, 4000, 5000, Data{1,2});
	scheduler.scheduleRepeatUntil<Data>(dataFunction, 100, 4100, 5000, Data{1,2});
	scheduler.scheduleRepeatUntil<Data>(dataFunction, 100, 4200, 5000, Data{1,2});
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(1, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(3, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(6, callCounter);
}

void test_fill() {
	for (int i = 0; i < SCHEDULER_SIZE; i++) {
		scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
	}
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE*2, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE*3, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE*3, callCounter);
	for(int i = 0; i<50;i++) scheduler.update(5000);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE*11, callCounter);
}

void test_fill_multiple() {
	const int LOOPS = SCHEDULER_SIZE/3;
	for (int i = 0; i < LOOPS; i++) {
		scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4100, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4200, 5000);
	}
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(LOOPS, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(3*LOOPS, callCounter);
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(6*LOOPS, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(6*LOOPS, callCounter);
}

void test_fill_clear() {
	const int LOOPS = SCHEDULER_SIZE/3;
	for (int i = 0; i < LOOPS; i++) {
		scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4100, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4200, 5000);
	}
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(2000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(3000);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(4000);
	TEST_ASSERT_EQUAL(LOOPS, callCounter);
	scheduler.update(4100);
	TEST_ASSERT_EQUAL(3*LOOPS, callCounter);
	scheduler.clearTasks();
	scheduler.update(4200);
	TEST_ASSERT_EQUAL(3*LOOPS, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3*LOOPS, callCounter);
	scheduler.update(4210);
	TEST_ASSERT_EQUAL(3*LOOPS, callCounter);
	scheduler.update(5010);
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	callCounter = 0;
	for (int i = 0; i < LOOPS; i++) {
		scheduler.scheduleRepeatUntil(function, 100, 4000, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4100, 5000);
		scheduler.scheduleRepeatUntil(function, 100, 4200, 5000);
	}
	for (int i = 0; i<30;i++) scheduler.update(4210);
	TEST_ASSERT_EQUAL(6*LOOPS, callCounter);
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_single);
	RUN_TEST(test_multiple);
	RUN_TEST(test_end_early);
	RUN_TEST(test_data);
	RUN_TEST(test_clear);
	RUN_TEST(test_multiple_data);
	RUN_TEST(test_fill);
	RUN_TEST(test_fill_multiple);
	RUN_TEST(test_fill_clear);
	UNITY_END();
	return 0;
}