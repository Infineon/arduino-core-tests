/* test_time_single.cpp
 *
 * This test is used to verify the functionality of the time module.
 * only one board is needed with out any other connections.
 */

// std includes

// test includes
#include "test_common_includes.h"
#include "time.h"

// project includes

// defines
#define TRACE_OUTPUT

// variables

// Method invoked before a test suite is run.
static void time_single_suite_setup() {
    
}


// Method invoked after a test suite is run.
static void time_single_suite_teardown() {
    
}

#define TOLERANCE_MS 10 // Tolerance for delay in milliseconds
#define TOLERANCE_US 100 // Tolerance for delay in microseconds

// Define test group name
TEST_GROUP(time_single);
TEST_GROUP(time_single_internal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(time_single_internal)
{ 
    // Setup code if needed
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(time_single_internal)
{
    // Tear down code if needed
}

/**
 * @brief Test the delay function.
 */
TEST_IFX(time_single_internal, testDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_ms = 1000; 

    start_time = millis();
    delay(expected_delay_ms);
    end_time = millis();

    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS, expected_delay_ms, actual_delay);
}

/**
 * @brief Test the delayMicroseconds function.
 */
TEST_IFX(time_single_internal, testDelayMicroseconds)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_us = 500; 

    start_time = micros();
    delayMicroseconds(expected_delay_us);
    end_time = micros();
    
    unsigned long actual_delay_us = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, expected_delay_us, actual_delay_us);
}

/**
 * @brief Test the delay function with smallest value of 1ms.
 */
TEST_IFX(time_single_internal, testSmallDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_ms = 1; 

    start_time = millis();
    delay(expected_delay_ms);
    end_time = millis();
    
    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS, expected_delay_ms, actual_delay);
}

/**
 * @brief Test the delayMicroseconds function with smallest value of 1 microsecond.
 */
TEST_IFX(time_single_internal, testSmallDelayMicroseconds) 
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_us = 1; 

    start_time = micros();
    delayMicroseconds(expected_delay_us);
    end_time = micros();
    
    unsigned long actual_delay_us = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, expected_delay_us, actual_delay_us);
}

/**
 * @brief Test the delay function for 10 seconds.
 */
TEST_IFX(time_single_internal, testLongDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_ms = 10000;

    start_time = millis();
    delay(expected_delay_ms);
    end_time = millis();
    
    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS * 10, expected_delay_ms, actual_delay);
}

/**
 * @brief print the return value of function millis.
 */
TEST_IFX(time_single_internal, testPrintMillis)
{
    unsigned long current_millis = millis();
    Serial.print("Current millis: ");
    Serial.println(current_millis);
}

/**
 * @brief print the return value of function micros.
 */
TEST_IFX(time_single_internal, testPrintMicros)
{
    unsigned long current_micros = micros();
    Serial.print("Current micros: ");
    Serial.println(current_micros);
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(time_single_internal)
{
    RUN_TEST_CASE(time_single_internal, testDelay);
    RUN_TEST_CASE(time_single_internal, testDelayMicroseconds);
    RUN_TEST_CASE(time_single_internal, testSmallDelay);
    RUN_TEST_CASE(time_single_internal, testSmallDelayMicroseconds);
    RUN_TEST_CASE(time_single_internal, testLongDelay);
    RUN_TEST_CASE(time_single_internal, testPrintMillis);
    RUN_TEST_CASE(time_single_internal, testPrintMicros);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(time_single)
{
    time_single_suite_setup();

    RUN_TEST_GROUP(time_single_internal);

    time_single_suite_teardown();
}

