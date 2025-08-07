/**
 * @brief test_pulse_board2.cpp
 *
 * @details This test suite verifies the functionality of the `pulseIn` and `pulseInLong` APIs.
 *      The test requires TEST_PIN_PULSE of board1 connected to the TEST_PIN_PULSE board2 and additionally 
 *      TEST_PIN_SYNC_IO of board1 connected to the  TEST_PIN_SYNC_IO board2 inorder to synchronize the pulse 
 *      generation and measurement.
 * 
 * @note The board1 test shall be started first before the board2 tests for proper synchronization.
 *       The board1 test will generate the pulses and board2 will test PulseIn APIs.
 */

// Includes
#include "test_common_includes.h"
#include "test_config.h"

// Defines  
#define TOLERANCE_DURATION_PERCENTAGE 10              
#define TIMEOUT_MICROSECONDS 10000000 // 10 second timeout for pulse measurement 

// Variables
volatile unsigned long measured_pulse_us = 0;        // Measured pulse duration in microseconds

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void pulse_board2_suite_setup() {
    pinMode(TEST_PIN_PULSE, INPUT);
    
    pinMode(TEST_PIN_SYNC_IO, OUTPUT);
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}

/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void pulse_board2_suite_teardown() {
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}

// Define test group name
TEST_GROUP(pulse_board2);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
TEST_SETUP(pulse_board2) {
    delay(100); // Allow time for setup to stabilize
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
TEST_TEAR_DOWN(pulse_board2) {
}

/**
 * @brief Test case: Measure HIGH pulse duration.
 */
TEST_IFX(pulse_board2, test_high_pulse_duration) {
    const unsigned long test_duration_us = 1000; // 1 ms pulse

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure the pulse duration
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, HIGH);

    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    // Validate the pulse duration within tolerance
    float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_duration_us / 100.0f;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_duration_us, measured_pulse_us);  

}

/**
 * @brief Test case: Measure LOW pulse duration.
 */
TEST_IFX(pulse_board2, test_low_pulse_duration) {
    const unsigned long test_duration_us = 2000; // 2 ms pulse

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure the LOW pulse duration
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, LOW, TIMEOUT_MICROSECONDS);
    
    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_duration_us / 100.0f;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_duration_us, measured_pulse_us);
}


/**
 * @brief Test case: Timeout when no pulse is detected.
 */
TEST_IFX(pulse_board2, test_pulse_timeout) {
    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure pulse duration (expect timeout)
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, HIGH, TIMEOUT_MICROSECONDS);
   
    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    // Validate that no pulse is detected
    TEST_ASSERT_EQUAL(0, measured_pulse_us);
}

/**
 * @brief Test case: Very short pulse duration.
 */
TEST_IFX(pulse_board2, test_short_pulse_duration) {
    const unsigned long test_duration_us = 10; // 10 microseconds pulse

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure the pulse duration
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, HIGH, TIMEOUT_MICROSECONDS);
    
    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_duration_us / 100.0f;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_duration_us, measured_pulse_us);
}

/**
 * @brief Test case: Very long pulse duration.
 */
TEST_IFX(pulse_board2, test_long_pulse_duration) {
    const unsigned long test_duration_us = 3000000; // 3s pulse

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure the pulse duration
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, HIGH, TIMEOUT_MICROSECONDS);
    
    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_duration_us / 100.0f;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_duration_us, measured_pulse_us);
}

/**
 * @brief Test case: Overlapping pulses.
 */
TEST_IFX(pulse_board2, test_overlapping_pulses) {
    const unsigned long test_duration_us = 1000; // 1 ms pulse

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    // Measure the first pulse duration
    measured_pulse_us = pulseIn(TEST_PIN_PULSE, HIGH, TIMEOUT_MICROSECONDS);

    digitalWrite(TEST_PIN_SYNC_IO, LOW);
    
    float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_duration_us / 100.0f;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_duration_us, measured_pulse_us);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(pulse_board2) {
    pulse_board2_suite_setup();

    RUN_TEST_CASE(pulse_board2, test_high_pulse_duration);
    RUN_TEST_CASE(pulse_board2, test_low_pulse_duration);
    RUN_TEST_CASE(pulse_board2, test_pulse_timeout);
    RUN_TEST_CASE(pulse_board2, test_short_pulse_duration);
    RUN_TEST_CASE(pulse_board2, test_long_pulse_duration);
    RUN_TEST_CASE(pulse_board2, test_overlapping_pulses);

    pulse_board2_suite_teardown();
}
