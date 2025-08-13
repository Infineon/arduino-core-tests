/**
 * @brief test_pulse_board1.cpp
 *
 * @details This test suite verifies the functionality of the `pulseIn` and `pulseInLong` APIs.
 *      The test requires TEST_PIN_PULSE of board1 connected to the TEST_PIN_PULSE board2 and additionally 
 *      TEST_PIN_SYNC_IO of board1 connected to the TEST_PIN_PULSE board2 inorder to synchronize the pulse 
 *      generation and measurement.
 * 
 * @note The board1 test shall be started first before the board2 tests for proper synchronization.
 *       The board1 test will generate the pulses and board2 will test PulseIn APIs.
 */

// Includes
#include "test_common_includes.h"
#include "test_config.h"

// Defines

// Variables
volatile unsigned long measured_pulse_us = 0;       

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void pulse_board1_suite_setup() {
    pinMode(TEST_PIN_PULSE, OUTPUT);
    digitalWrite(TEST_PIN_PULSE, LOW);

    pinMode(TEST_PIN_SYNC_IO, INPUT_PULLDOWN);
}

/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void pulse_board1_suite_teardown() {
    
}

// Define test group name
TEST_GROUP(pulse_board1);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
TEST_SETUP(pulse_board1) {
    
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
TEST_TEAR_DOWN(pulse_board1) {
    digitalWrite(TEST_PIN_PULSE, LOW);
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}

/**
 * @brief Test case: Measure HIGH pulse duration.
 */
TEST_IFX(pulse_board1, test_high_pulse_duration) {
    const unsigned long test_duration_us = 1000; // 1 ms pulse

    
    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}

    delay(100);

    // Generate a HIGH pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}

/**
 * @brief Test case: Measure LOW pulse duration.
 */
TEST_IFX(pulse_board1, test_low_pulse_duration) {
    const unsigned long test_duration_us = 2000; // 2 ms pulse

    // Generate a LOW pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(1000); // Add a HIGH pulse before LOW

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}
    delay(100);

    digitalWrite(TEST_PIN_PULSE, LOW);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, HIGH);


    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}
    digitalWrite(TEST_PIN_PULSE, LOW);

}

/**
 * @brief Test case: Measure HIGH pulse duration.
 */
TEST_IFX(pulse_board1, test_high_pulse_long_duration) {
    const unsigned long test_duration_us = 1000; // 1 ms pulse

    
    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}

    delay(100);

    // Generate a HIGH pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}

/**
 * @brief Test case: Measure LOW pulse duration.
 */
TEST_IFX(pulse_board1, test_low_pulse_long_duration) {
    const unsigned long test_duration_us = 2000; // 2 ms pulse

    // Generate a LOW pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(1000); // Add a HIGH pulse before LOW

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}
    delay(100);

    digitalWrite(TEST_PIN_PULSE, LOW);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, HIGH);


    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}

/**
 * @brief Test case: Timeout when no pulse is detected.
 */
TEST_IFX(pulse_board1, test_pulse_timeout) {

    // Ensure no pulse is generated
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}
    delay(100);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}

/**
 * @brief Test case: Very short pulse duration.
 */
TEST_IFX(pulse_board1, test_short_pulse_duration) {
    const unsigned long test_duration_us = 10; // 10 microseconds pulse

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}
    delay(100);

    // Generate a very short HIGH pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}

/**
 * @brief Test case: Very long pulse duration.
 */
TEST_IFX(pulse_board1, test_long_pulse_duration) {
    const unsigned long test_duration_us = 3000000; // 3s pulse

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {}
    delay(100);

    // Generate a long HIGH pulse
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH) {}

}


/**
 * @brief Test case: Overlapping pulses.
 */
TEST_IFX(pulse_board1, test_overlapping_pulses) {
    const unsigned long test_duration_us = 1000; // 1 ms pulse
    const unsigned long overlap_delay_us = 500; // Overlap delay

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW);
    delay(100);

    // Generate overlapping pulses
    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    delayMicroseconds(overlap_delay_us); // Overlap period

    digitalWrite(TEST_PIN_PULSE, HIGH);
    delayMicroseconds(test_duration_us);
    digitalWrite(TEST_PIN_PULSE, LOW);

    while (digitalRead(TEST_PIN_SYNC_IO) == HIGH);

}



/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(pulse_board1) {
    pulse_board1_suite_setup();

    RUN_TEST_CASE(pulse_board1, test_high_pulse_duration);
    RUN_TEST_CASE(pulse_board1, test_low_pulse_duration);
    RUN_TEST_CASE(pulse_board1, test_high_pulse_long_duration);
    RUN_TEST_CASE(pulse_board1, test_low_pulse_long_duration);
    RUN_TEST_CASE(pulse_board1, test_pulse_timeout);
    RUN_TEST_CASE(pulse_board1, test_short_pulse_duration);
    RUN_TEST_CASE(pulse_board1, test_long_pulse_duration);
    RUN_TEST_CASE(pulse_board1, test_overlapping_pulses);

    pulse_board1_suite_teardown();
}

