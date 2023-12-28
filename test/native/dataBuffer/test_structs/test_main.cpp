
#define DATA_BUFFER_SIZE 2000
#include "dataBuffer.h"
#include <unity.h>
DataBuffer db;

void setUp(void) {
	db = DataBuffer();
}

void tearDown(void) {
	TEST_ASSERT_FALSE(db.isDataSet());
	uint8_t errFlags = DataBuffer::getErrFlags();
	DataBuffer::clearErrFlags();
	TEST_ASSERT_EQUAL(0, errFlags);
}

struct DestructorWatchdog {
	int alive_flag = 0;
	long long int data[10];
	DestructorWatchdog() {
		alive_flag = 0x69;
	}
	~DestructorWatchdog() {
		// We should always have a valid alive_flag
		// If we don't, then we've double deleted,
		// or we've deleted something that wasn't allocated
		// by us. Either way, we've got a problem.
		TEST_ASSERT_EQUAL(0x69, alive_flag);
		alive_flag = 0;
	}
};

void test_empty_buffer(void) {
	DataBuffer db;
	TEST_ASSERT_FALSE(db.isDataSet());
}

void test_dataBuffer_set_get(void) {
	DataBuffer db;
	db.set<DestructorWatchdog>(DestructorWatchdog());
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(0x69, db.get<DestructorWatchdog>().alive_flag);
	db.clear();
	TEST_ASSERT_FALSE(db.isDataSet());
	db.set<DestructorWatchdog>(DestructorWatchdog());
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(0x69, db.get<DestructorWatchdog>().alive_flag);
	db.clear();
	TEST_ASSERT_FALSE(db.isDataSet());
}

int main(int argc, char **argv) {
	UNITY_BEGIN();
	RUN_TEST(test_empty_buffer);
	RUN_TEST(test_dataBuffer_set_get);
	UNITY_END();
	return 0;
}
