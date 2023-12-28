#include "dataBuffer.h"
#include <unity.h>

/*
	This test file is for testing the DataBuffer class.
	This is the best place to test error flags, as primitives
	do not take up heap space, and thus cannot be deleted.
	This means that we can test all error flags without
	needing to worry about memory leaks.
*/

DataBuffer db;

void setUp(void) {
	db = DataBuffer();
}

void tearDown(void) {
	TEST_ASSERT_FALSE(db.isDataSet());
	TEST_ASSERT_EQUAL(0, DataBuffer::getErrFlags());
}

void test_dataBuffer_set_get(void) {
	db.set<int>(123);
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(123, db.get<int>());
	db.clear();
}

void test_dataBuffer_clear(void) {
	db.set<int>(123);
	TEST_ASSERT_TRUE(db.isDataSet());
	db.clear();
	TEST_ASSERT_FALSE(db.isDataSet());
}

// Here we test weather the error flag is set when we try to clear a buffer
// It should be set if the buffer is not set
// It should not be set if the buffer is set
void test_dataBuffer_clear_not_set(void) {
	TEST_ASSERT_FALSE(db.isDataSet());
	TEST_ASSERT_FALSE(DataBuffer::getErrFlags() & DataBuffer::DOUBLE_CLEAR_ERR);
	db.clear();
	TEST_ASSERT_FALSE(db.isDataSet());
	TEST_ASSERT_TRUE(DataBuffer::getErrFlags() & DataBuffer::DOUBLE_CLEAR_ERR);
	DataBuffer::clearErrFlags();
}

void test_dataBuffer_double_set(void) {
	db.set<int>(123);
	TEST_ASSERT_TRUE(db.isDataSet());
	db.set<int>(456);
	TEST_ASSERT_TRUE(DataBuffer::getErrFlags() & DataBuffer::DOUBLE_SET_ERR);
	db.clear();
	DataBuffer::clearErrFlags();
}

void test_destructor_without_clear(void) {
	// specifically testing the destructor here
	// The local scope ensures that the destructor is called
	// before the last assert.
	{
		DataBuffer local = DataBuffer();
		local.set<int>(123);
		TEST_ASSERT_TRUE(local.isDataSet());
	}
	TEST_ASSERT_TRUE(DataBuffer::getErrFlags() & DataBuffer::DESTRUCTOR_WITHOUT_CLEAR_ERR);
	DataBuffer::clearErrFlags();
}

void test_repeat_set(void) {
	db.set<int>(123);
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(123, db.get<int>());
	db.clear();
	db.set<int>(456);
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(456, db.get<int>());
	db.clear();
	db.set<int>(789);
	TEST_ASSERT_TRUE(db.isDataSet());
	TEST_ASSERT_EQUAL_INT(789, db.get<int>());
	db.clear();
	TEST_ASSERT_FALSE(db.isDataSet());
}



int main(int argc, char **argv) {
	UNITY_BEGIN();
	RUN_TEST(test_dataBuffer_set_get);
	RUN_TEST(test_dataBuffer_clear);
	RUN_TEST(test_dataBuffer_clear_not_set);
	RUN_TEST(test_repeat_set);
	RUN_TEST(test_dataBuffer_double_set);
	RUN_TEST(test_destructor_without_clear);
	UNITY_END();
	return 0;
}
