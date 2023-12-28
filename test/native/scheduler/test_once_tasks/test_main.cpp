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

void test_once_no_data() {
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	int id = scheduler.schedule([](void){callCounter++;}, 53);
	TEST_ASSERT_NOT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(23);
	TEST_ASSERT_EQUAL(1, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(54);
	TEST_ASSERT_EQUAL(0, DataBuffer::getErrFlags());
	TEST_ASSERT_EQUAL(1, callCounter);
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	scheduler.update(2345);
	TEST_ASSERT_EQUAL(1, callCounter);
}

void test_once_ref_data() {
	struct Data {
		int a;
		int b;
	};
	Data data;
	data.a = 5;
	data.b = 6;
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	int id = scheduler.schedule<Data>([](DataBuffer& data){callCounter++;}, 53, std::move(data));
	TEST_ASSERT_NOT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(23);
	TEST_ASSERT_EQUAL(1, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(54);
	TEST_ASSERT_EQUAL(1, callCounter);
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_move_data() {
	struct Data {
		int a;
		int b;
	};
	Data data;
	data.a = 5;
	data.b = 6;
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	int id = scheduler.schedule<Data>([](DataBuffer& data){
			callCounter++;
			Data& d = data.get<Data>();
			TEST_ASSERT_EQUAL(5, d.a);
			TEST_ASSERT_EQUAL(6, d.b);
			d.a = 7;

		}, 53, std::move(data));
	TEST_ASSERT_NOT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(23);
	TEST_ASSERT_EQUAL(1, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	scheduler.update(54);
	TEST_ASSERT_EQUAL(1, callCounter);
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_fill_multiple() {
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](void){callCounter++;};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		int id = scheduler.schedule(func, 100+i*200);
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule(func, 100);
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_fill_repeatedly() {
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](void){callCounter++;};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		int id = scheduler.schedule(func, 100+i*200);
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule(func, 100);
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-1, scheduler.getTaskCount());
		scheduler.schedule(func, 100+i*200+50000);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200+50000);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_fill_ref_data() {
	struct Data
	{
		int a;
		int b;
	};
	
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](DataBuffer& dat){
		dat.get<Data>().a++;
	};
	Data data {2, 3};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		int id = scheduler.schedule<Data>(func, 100+i*200, std::move(data));
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule<Data>(func, 100, std::move(data));
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_fill_ref_repeatedly(){
	struct Data
	{
		int a;
		int b;
	};
	
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](DataBuffer& dat){
		dat.get<Data>().a++;
	};
	Data data {2, 3};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		int id = scheduler.schedule<Data>(func, 100+i*200, std::move(data));
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule<Data>(func, 100, std::move(data));
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-1, scheduler.getTaskCount());
		scheduler.schedule<Data>(func, 100+i*200+50000, std::move(data));
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200+50000);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());

}

void test_once_fill_mov_multiple() {
	struct Data
	{
		int a;
		int b;
	};
	
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](DataBuffer&){callCounter++;};
	auto funcWB = [](void){callCounter++;};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		Data data {2, 3};
		int id = scheduler.schedule<Data>(func, 100+i*200, std::move(data));
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule(funcWB, 100);
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
}

void test_once_fill_mov_repeatedly() {
	struct Data
	{
		int a;
		int b;
	};
	
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	auto func = [](DataBuffer&){callCounter++;};
	auto funcWB = [](void){callCounter++;};
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		Data data {2, 3};
		int id = scheduler.schedule<Data>(func, 100+i*200, std::move(data));
		TEST_ASSERT_NOT_EQUAL(-1, id);
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	int id = scheduler.schedule(funcWB, 100);
	TEST_ASSERT_EQUAL(-1, id);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	scheduler.update(23);
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(0, callCounter);
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-1, scheduler.getTaskCount());
		scheduler.schedule<Data>(func, 100+i*200+50000, std::move(Data{2,3}));
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE, scheduler.getTaskCount());
	for(int i = 0; i< SCHEDULER_SIZE; i++) {
		scheduler.update(101+i*200+50000);
		TEST_ASSERT_EQUAL(SCHEDULER_SIZE-i-1, scheduler.getTaskCount());
	}
	TEST_ASSERT_EQUAL(0, scheduler.getTaskCount());
	TEST_ASSERT_EQUAL(SCHEDULER_SIZE*2, callCounter);

}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_once_no_data);
	RUN_TEST(test_once_ref_data);
	RUN_TEST(test_once_move_data);
	RUN_TEST(test_once_fill_multiple);
	RUN_TEST(test_once_fill_repeatedly);
	RUN_TEST(test_once_fill_ref_data);
	RUN_TEST(test_once_fill_ref_repeatedly);
	RUN_TEST(test_once_fill_mov_multiple);
	RUN_TEST(test_once_fill_mov_repeatedly);
	UNITY_END();
	return 0;
}