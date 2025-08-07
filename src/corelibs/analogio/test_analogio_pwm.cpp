/**
 * @brief test_analogio_pwm.cpp
 *
 * @details This test is used to verify the functionality of the Analog IO module.
 * only one board is needed with PWM_PIN_OUTPUT (TEST_PIN_DIGITAL_IO_OUTPUT) pin connected to 
 * PWM_PIN_FEEDBACK (TEST_PIN_DIGITAL_IO_INPUT) pin for the test cases to work as expected.
 * 
 */

// std includes

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// project includes

// defines
#define TRACE_OUTPUT
#define TOLERANCE_DUTY_CYCLE 5.0f           // 5% tolerance for duty cycle
#define TOLERANCE_FREQUENCY 10.0f           // 10 Hz tolerance for frequency
#define PWM_PIN_OUTPUT      TEST_PIN_DIGITAL_IO_OUTPUT
#define PWM_PIN_FEEDBACK    TEST_PIN_DIGITAL_IO_INPUT

volatile uint32_t start_time = 0;            // Timestamp of the last rising edge
volatile uint32_t high_time = 0;             // Duration of the HIGH period
volatile uint32_t low_time = 0;              // Duration of the LOW period
volatile bool high_measured = false;         // Flag to indicate a full PWM cycle has been measured

// variables
volatile float measured_duty_cycle_percentage = 0; 
volatile uint32_t measured_frequency_hz = 0; 

/**
 * @brief Interrupt handler for the PWM feedback pin.
 * This function is triggered on both rising and falling edges of the PWM signal.
 */
void feedback_interrupt_handler() {
    uint32_t current_time = micros(); // Get the current timestamp

    // Check the current state of the feedback pin
    if (digitalRead(TEST_PIN_DIGITAL_IO_INPUT) == HIGH) {
        // Rising edge detected
        if (start_time > 0) { // Ensure this is not the first edge
            low_time = current_time - start_time; // Measure LOW time (previous cycle)
        }
        
        start_time = current_time; // Update start_time for HIGH period
    } else {
        // Falling edge detected
        high_time = current_time - start_time; // Measure HIGH time
        start_time = current_time;             // Update start_time for LOW period
       
        // Indicate that we have captured a full PWM cycle (HIGH + LOW)
        high_measured = true;
    }
}

/**
 * @brief Function to process the measured timing and calculate duty cycle and frequency.
 */
void feedback_measurement_handler() {
    uint32_t t_period;
 
    // Wait until a full PWM cycle has been measured
    while (!high_measured);

    // Disable interrupts temporarily to avoid race conditions
    noInterrupts();

    // Calculate the total period (HIGH + LOW)
    t_period = high_time + low_time;

    // Calculate duty cycle and frequency
    measured_duty_cycle_percentage = ((float)high_time / t_period) * 100.0f;
    measured_frequency_hz = 1000000 / t_period; // Convert period to frequency

    // Reset the measurement flag
    high_measured = false;

    // Re-enable interrupts
    interrupts();

}

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void analogio_pwm_suite_setup() {

    pinMode(PWM_PIN_FEEDBACK, INPUT);
    attachInterrupt(digitalPinToInterrupt(PWM_PIN_FEEDBACK), feedback_interrupt_handler, CHANGE);
}


/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void analogio_pwm_suite_teardown() {
    detachInterrupt(digitalPinToInterrupt(PWM_PIN_FEEDBACK));
}

// Define test group name
TEST_GROUP(analogio_pwm);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(analogio_pwm) { 
    interrupts();
    // Reset the measurement variables
    start_time = 0;
    high_time = 0;
    low_time = 0;
    high_measured = false;
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(analogio_pwm) {

}

/**
 * @brief Verify SetAnalog frequency. 
 */

TEST_IFX(analogio_pwm, test_analog_set_fz)
{   
    //Set the frequency followed by a analogWrite
    analogWriteResolution(16); 
    setAnalogWriteFrequency(PWM_PIN_OUTPUT, 100); 
    delay(1000); 
    analogWrite(PWM_PIN_OUTPUT, 32767); 
    delay(1000); 
    feedback_measurement_handler();
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, 100, measured_frequency_hz);

    const float frequency[] = {1, 50, 5000, 50000};

    for(size_t i=0; i< sizeof(frequency)/sizeof(frequency[i]); i++)
    {
        // Set different frequencies and verify the output
        setAnalogWriteFrequency(PWM_PIN_OUTPUT, frequency[i]);
        delay(1000); // Wait for the signal to stabilize
        feedback_measurement_handler();
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, frequency[i], measured_frequency_hz);
    }

    setAnalogWriteFrequency(PWM_PIN_OUTPUT, PWM_FREQUENCY_HZ); //set back to default fz
    delay(1000);
}

/**
 * @brief Verify PWM duty cycle for various percentages (25%, 50%, 75%) with default 8-bit resolution.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_8_bit_resolution)
{
    analogWriteResolution(5); // Any value lesser than or equal to 8 will be considered as 8-bit resolution

    // Array of expected duty cycle percentages and their corresponding analogWrite values (8-bit resolution, 0 to 255)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint8_t analog_write_values[] = {64, 127, 191};

    for (size_t i = 0; i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]); i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i], measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ, measured_frequency_hz);
    } 
}

/**
 * @brief Verify PWM duty cycle for various percentages (25%, 50%, 75%) with 10-bit resolution.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_10_bit_resolution)
{
    analogWriteResolution(10); 

    // Array of expected duty cycle percentages and their corresponding analogWrite values (10-bit resolution, 0 to 1023)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {256, 511, 767};

    for (size_t i = 0; i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]); i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i], measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ, measured_frequency_hz);
    } 
}

/**
 * @brief Verify PWM duty cycle for various percentages (25%, 50%, 75%) with 12-bit resolution.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_12_bit_resolution)
{
    analogWriteResolution(12); 

    // Array of expected duty cycle percentages and their corresponding analogWrite values (12-bit resolution, 0 to 4095)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {1024, 2047, 3071};

    for (size_t i = 0; i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]); i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i], measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ, measured_frequency_hz);
    } 
}

/**
 * @brief Verify PWM duty cycle for various percentages (25%, 50%, 75%) with 16-bit resolution.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_16_bit_resolution)
{
    analogWriteResolution(16); 
   

    // Array of expected duty cycle percentages and their corresponding analogWrite values (16-bit resolution, 0 to 65535)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {16384, 32767, 49151};

    for (size_t i = 0; i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]); i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i], measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ, measured_frequency_hz);
    } 
}

/**
 * @brief Verify PWM 100% duty cycle.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_100_percentage_dutycycle)
{
    analogWriteResolution(16);
    analogWrite(PWM_PIN_OUTPUT, 65535);  
    delay(1000); // Wait for the signal to stabilize

    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(PWM_PIN_FEEDBACK), "PWM output should be HIGH when 100 percentage duty cycle is set");
    // Check once again if the PWM pin is still HIGH
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(PWM_PIN_FEEDBACK), "PWM output should be HIGH when 100 percentage duty cycle is set");
}

/**
 * @brief Verify PWM 0% duty cycle.
 */
TEST_IFX(analogio_pwm, test_analog_write_pwm_0_percentage_dutycycle)
{
    analogWriteResolution(8);
    analogWrite(PWM_PIN_OUTPUT, 0);  
    delay(1000); // Wait for the signal to stabilize

    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(PWM_PIN_FEEDBACK), "PWM output should be LOW when 0 percentage duty cycle is set");
    // Check once again if the PWM pin is still LOW
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(PWM_PIN_FEEDBACK), "PWM output should be LOW when 0 percentage duty cycle is set");
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(analogio_pwm)
{
    analogio_pwm_suite_setup();

    RUN_TEST_CASE(analogio_pwm, test_analog_set_fz);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_8_bit_resolution);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_10_bit_resolution);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_12_bit_resolution);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_16_bit_resolution);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_100_percentage_dutycycle);
    RUN_TEST_CASE(analogio_pwm, test_analog_write_pwm_0_percentage_dutycycle);

    analogio_pwm_suite_teardown();
}
