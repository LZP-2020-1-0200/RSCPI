#include "configuration.h"
#include <unity.h>

void setUp(void)
{
  	// set stuff up here
}

void tearDown(void)
{
  	// clean stuff up here
}


void setup()
{
	// NOTE!!! Wait for >2 secs
	// if board doesn't support software reset via Serial.DTR/RTS
	delay(2000);


	UNITY_BEGIN(); // IMPORTANT LINE!
	TEST_ASSERT_EQUAL(1, 1);
	UNITY_END();
}

void loop()
{
	// Do nothing
}