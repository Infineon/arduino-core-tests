/**
 * @brief test_tone_no_tone.cpp
 *
 * @details This test verifies the functionality of the tone and noTone APIs.
 * The test requires two pins: 
 * - TONE_PIN_OUTPUT (output pin for tone generation).
 * - TONE_PIN_FEEDBACK (connected to TONE_PIN_OUTPUT as an input for verification).
 */

// Includes
#include "test_common_includes.h"
#include "test_config.h"

// Defines
#define TRACE_OUTPUT
#define TOLERANCE_FREQUENCY_PERCENTAGE 5                 
#define TOLERANCE_DURATION_PERCENTAGE 5                    
#define TONE_PIN_OUTPUT TEST_PIN_DIGITAL_IO_OUTPUT  
#define TONE_PIN_FEEDBACK TEST_PIN_DIGITAL_IO_INPUT 

// Variables
volatile uint32_t current_time = 0;         // Timestamp of the last rising/falling edge
volatile uint32_t start_time = 0;           // Timestamp of the last rising edge
volatile uint32_t start_tone = 0;           // Timestamp of the last rising edge
volatile uint32_t high_time = 0;            // Duration of the HIGH period
volatile uint32_t low_time = 0;             // Duration of the LOW period
volatile bool tone_measured = false;        // Flag to indicate a complete tone cycle has been measured

volatile float measured_frequency_hz = 0;  // Measured frequency
volatile unsigned long measured_duration_ms = 0; // Measured duration

void reset_tone() {
    start_time = 0;
    high_time = 0;
    low_time = 0;
    tone_measured = false;
    measured_frequency_hz = 0;
    measured_duration_ms = 0;
    start_tone = 0;
    current_time = 0;
 }

/**
 * @brief Interrupt handler for the tone feedback pin.
 * This function is triggered on both rising and falling edges of the tone signal.
 */
void feedback_interrupt_handler() {
    static int initial_pin_state = digitalRead(TONE_PIN_FEEDBACK); // Detect initial state of the pin
    current_time = micros(); // Get the current timestamp

    if(start_tone == 0) {
        start_tone = current_time;
    }

    // Check the state of the pin to detect rising or falling edges
    if (digitalRead(TONE_PIN_FEEDBACK) == HIGH) {
        // Rising edge detected
        if (initial_pin_state == LOW) { // Ensure this is after a LOW state
            low_time = current_time - start_time; // Measure LOW time
            measured_duration_ms = MICROS_TO_MILLISECONDS(current_time - start_tone);
        }
        start_time = current_time; // Update start_time for HIGH period
        initial_pin_state = HIGH; // Update the state
    } else {
        // Falling edge detected
        if (initial_pin_state == HIGH) { // Ensure this is after a HIGH state
            high_time = current_time - start_time; // Measure HIGH time
            measured_duration_ms = MICROS_TO_MILLISECONDS(current_time - start_tone);
        }
        start_time = current_time; // Update start_time for LOW period
        initial_pin_state = LOW; // Update the state
    }
    // Check if both HIGH and LOW times have been captured
    if (high_time > 0 && low_time > 0) {
        tone_measured = true; // Indicate a full cycle has been captured
    }
}



/**
 * @brief Function to process the measured timing and calculate frequency.
 * This function waits for a complete PWM cycle and calculates the frequency.
 */
void feedback_measurement_handler_frequency() {
    uint32_t t_period;

    // Wait until a full PWM cycle has been measured
    while (!tone_measured);

    // Disable interrupts temporarily to avoid race conditions
    noInterrupts();

    // Calculate the total period (HIGH + LOW)
    t_period = high_time + low_time;

    measured_frequency_hz = 1000000.0f / t_period; // Convert period to frequency (in Hz)
    tone_measured = false;

    // Re-enable interrupts
    interrupts();
}

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void tone_no_tone_suite_setup() {
    pinMode(TONE_PIN_FEEDBACK, INPUT);
    attachInterrupt(digitalPinToInterrupt(TONE_PIN_FEEDBACK), feedback_interrupt_handler, CHANGE);
}

/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void tone_no_tone_suite_teardown() {
    reset_tone();
    detachInterrupt(digitalPinToInterrupt(TONE_PIN_FEEDBACK));
}

// Define test group name
TEST_GROUP(tone_no_tone);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(tone_no_tone) {
    interrupts();
    // Reset the measurement variables
    
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(tone_no_tone) {
    reset_tone();
}

/**
 * @brief Verify tone functionality for various frequencies without duration.
 */
TEST_IFX(tone_no_tone, test_tone_without_duration) {
    const unsigned int test_frequencies_hz[] = {500, 1000, 2000}; 

    for (size_t i = 0; i < sizeof(test_frequencies_hz) / sizeof(test_frequencies_hz[0]); i++) {
        reset_tone();

        tone(TONE_PIN_OUTPUT, test_frequencies_hz[i]);

        delay(100); // Wait for the tone to stabilize

        feedback_measurement_handler_frequency();

        float tolerance = TOLERANCE_FREQUENCY_PERCENTAGE * test_frequencies_hz[i] / 100.0f;
        
        TEST_ASSERT_FLOAT_WITHIN(tolerance, test_frequencies_hz[i], measured_frequency_hz);

        noTone(TONE_PIN_OUTPUT); // Stop the tone
    }
}

/**
 * @brief Verify if the frequency changes correctly on same pin without calling notone.
 */
TEST_IFX(tone_no_tone, test_tone_change_frequency) {
    const unsigned int test_frequencies_hz[] = {500, 1000, 2000}; 

    for (size_t i = 0; i < sizeof(test_frequencies_hz) / sizeof(test_frequencies_hz[0]); i++) {
        reset_tone();
        tone(TONE_PIN_OUTPUT, test_frequencies_hz[i]);
        delay(100); // Wait for the tone to stabilize

        feedback_measurement_handler_frequency();
        float tolerance = TOLERANCE_FREQUENCY_PERCENTAGE * test_frequencies_hz[i] / 100.0f;
        TEST_ASSERT_FLOAT_WITHIN(tolerance, test_frequencies_hz[i], measured_frequency_hz);
    }

    noTone(TONE_PIN_OUTPUT); // Stop the tone
}

/**
 * @brief Verify tone functionality for various durations with specified frequency.
 */
TEST_IFX(tone_no_tone, test_tone_with_duration) {
    const unsigned long test_durations_ms[] = {500, 1000, 2000}; 

    for (size_t i = 0; i < sizeof(test_durations_ms) / sizeof(test_durations_ms[0]); i++) {
        reset_tone();

        tone(TONE_PIN_OUTPUT, 2000, test_durations_ms[i]);
        delay(test_durations_ms[i] + 100); // wait for the tone duration to elapse

        float tolerance = TOLERANCE_DURATION_PERCENTAGE * test_durations_ms[i] / 100.0f;
        TEST_ASSERT_FLOAT_WITHIN(tolerance, test_durations_ms[i], measured_duration_ms);
    }
        
}

/**
 * @brief Verify noTone functionality stops the tone immediately.
 */
TEST_IFX(tone_no_tone, test_no_tone) {
    tone(TONE_PIN_OUTPUT, 1000);
    delay(100); 
    noTone(TONE_PIN_OUTPUT); 
    delay(1000); 

    PinStatus pin_state = digitalRead(TONE_PIN_FEEDBACK);
    delay(100); 
    TEST_ASSERT_TRUE_MESSAGE((pin_state == digitalRead(TONE_PIN_FEEDBACK)), "Tone should be stopped after calling noTone.");
    delay(100); 
    TEST_ASSERT_TRUE_MESSAGE((pin_state == digitalRead(TONE_PIN_FEEDBACK)), "Tone should be stopped after calling noTone.");
}

/**
 * @brief Verify tone functionality for various frequency and same duration but before 
 *        the duration elapses.
 *        The tone is expected to change frequency without stopping with duration specified.
 */
TEST_IFX(tone_no_tone, test_tone_overlap_frequency) {
 #if defined(ARDUINO_ARCH_XMC)
    const unsigned int test_frequencies_hz[] = {35, 50, 39}; //Tested with XMC 4700 minimum frequency working from 35Hz
#else
    const unsigned int test_frequencies_hz[] = {5, 10, 20}; 
#endif
    const unsigned int array_size = sizeof(test_frequencies_hz) / sizeof(test_frequencies_hz[0]);
    const unsigned int tone_duration_ms = 1000;
    const unsigned int delay_introduced_ms = 300;

    for (size_t i = 0; i < array_size; i++) {
        reset_tone();
        tone(TONE_PIN_OUTPUT, test_frequencies_hz[i], tone_duration_ms);

        delay(delay_introduced_ms); // delay to change the frequency before duration elapses

        feedback_measurement_handler_frequency();
        float tolerance_freq = TOLERANCE_FREQUENCY_PERCENTAGE * test_frequencies_hz[i] / 100.0f;
        TEST_ASSERT_FLOAT_WITHIN(tolerance_freq, test_frequencies_hz[i], measured_frequency_hz);
    }

    delay(tone_duration_ms);
        
}

/**
 * @brief Verify that calling tone again with different pin does not affect the output.
 */
TEST_IFX(tone_no_tone, test_tone_second_call) {
    const unsigned int test_frequencies_hz = 1000;
    
    tone(TONE_PIN_OUTPUT, test_frequencies_hz); 
    delay(500); 
    tone(TEST_PIN_SYNC_IO, test_frequencies_hz); // no effect on the second call

    feedback_measurement_handler_frequency();
    float tolerance = TOLERANCE_FREQUENCY_PERCENTAGE * test_frequencies_hz / 100.0f; 
    TEST_ASSERT_FLOAT_WITHIN(tolerance, test_frequencies_hz, measured_frequency_hz);

    noTone(TONE_PIN_OUTPUT); // Stop the tone
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(tone_no_tone) {
    tone_no_tone_suite_setup();

    RUN_TEST_CASE(tone_no_tone, test_tone_without_duration);
    RUN_TEST_CASE(tone_no_tone, test_tone_change_frequency);
    RUN_TEST_CASE(tone_no_tone, test_tone_with_duration);
    RUN_TEST_CASE(tone_no_tone, test_no_tone);
    RUN_TEST_CASE(tone_no_tone, test_tone_overlap_frequency);
    RUN_TEST_CASE(tone_no_tone, test_tone_second_call);

    tone_no_tone_suite_teardown();
}
