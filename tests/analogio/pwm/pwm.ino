#include "config.h"
#include "ottu.hpp"
#include <Arduino.h>

#define TRACE_OUTPUT
#define TOLERANCE_DUTY_CYCLE 5.0f
#define TOLERANCE_FREQUENCY 10.0f
#define PWM_PIN_OUTPUT TEST_PIN_DIGITAL_IO_OUTPUT
#define PWM_PIN_FEEDBACK TEST_PIN_DIGITAL_IO_INPUT
#define MEASUREMENT_CYCLES 100
#define MEASUREMENT_TIMEOUT_MS 5000

volatile bool measurement_active = false;
volatile bool measurement_complete = false;
volatile bool rising_seen = false;
volatile bool falling_seen = false;
volatile uint32_t first_rising_time = 0;
volatile uint32_t last_rising_time = 0;
volatile uint32_t high_time = 0;
volatile uint32_t total_high_time = 0;
volatile uint32_t elapsed_time = 0;
volatile uint32_t measured_cycles = 0;

volatile float measured_duty_cycle_percentage = 0.0;
volatile float measured_frequency_hz = 0.0;

/**
 * @brief Interrupt handler for the PWM feedback pin.
 * This function is triggered on both rising and falling edges of the PWM
 * signal.
 */
void feedback_interrupt_handler() {
    if (!measurement_active) {
        return;
    }

    uint32_t current_time = micros();

    if (digitalRead(PWM_PIN_FEEDBACK) == HIGH) {
        if (rising_seen && falling_seen) {
            total_high_time += high_time;
            measured_cycles++;
            if (measured_cycles == MEASUREMENT_CYCLES) {
                elapsed_time = current_time - first_rising_time;
                measurement_active = false;
                measurement_complete = true;
                return;
            }
        } else {
            // Restart if a falling edge was missed.
            first_rising_time = current_time;
            total_high_time = 0;
            measured_cycles = 0;
        }
        last_rising_time = current_time;
        rising_seen = true;
        falling_seen = false;
    } else if (rising_seen && !falling_seen) {
        high_time = current_time - last_rising_time;
        falling_seen = true;
    }
}

/**
 * @brief Measure the average duty cycle and frequency over complete cycles.
 */
void feedback_measurement_handler() {
    noInterrupts();
    measurement_complete = false;
    rising_seen = false;
    falling_seen = false;
    total_high_time = 0;
    measured_cycles = 0;
    measurement_active = true;
    interrupts();

    uint32_t started_at = millis();
    while (!measurement_complete &&
           (uint32_t)(millis() - started_at) < MEASUREMENT_TIMEOUT_MS) {
    }

    noInterrupts();
    measurement_active = false;
    bool complete = measurement_complete;
    uint32_t period_sum = elapsed_time;
    uint32_t high_sum = total_high_time;
    interrupts();

    TEST_ASSERT_TRUE_MESSAGE(complete, "Timed out waiting for PWM feedback");
    TEST_ASSERT_TRUE_MESSAGE(period_sum > 0, "Invalid PWM measurement period");

    measured_duty_cycle_percentage = (float)high_sum * 100.0f / period_sum;
    measured_frequency_hz = (float)MEASUREMENT_CYCLES * 1000000.0f / period_sum;
}

OTTUNITY_TEST_SETUP() {
    pinMode(PWM_PIN_FEEDBACK, INPUT);
    attachInterrupt(digitalPinToInterrupt(PWM_PIN_FEEDBACK),
                    feedback_interrupt_handler, CHANGE);

    interrupts();
#if defined(EXTENDED_FEATURES)
    setAnalogWriteFrequency(PWM_PIN_OUTPUT,
                            PWM_FREQUENCY_HZ); // set back to default fz
#endif
    delay(1000);
}

OTTUNITY_TEST_TEAR_DOWN() {
    detachInterrupt(digitalPinToInterrupt(PWM_PIN_FEEDBACK));
}

// OTTUNITY_GROUP(pwm);

OTTUNITY_TEST(set_fz) {
    // Set the frequency followed by a analogWrite
    analogWriteResolution(16);
#if defined(EXTENDED_FEATURES)
    setAnalogWriteFrequency(PWM_PIN_OUTPUT, 100);
#endif
    delay(1000);
    // 32767 for 50% duty cycle
    analogWrite(PWM_PIN_OUTPUT, 32767);
    delay(1000);
    feedback_measurement_handler();
#if defined(EXTENDED_FEATURES)
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, 100, measured_frequency_hz);
#endif
    TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, 500, measured_frequency_hz);

#if defined(EXTENDED_FEATURES)
    // Frequency array in test_config.h
    for (size_t i = 0;
         i < sizeof(test_pwm_frequencies) / sizeof(test_pwm_frequencies[i]);
         i++) {
        // Set different frequencies and verify the output
        setAnalogWriteFrequency(PWM_PIN_OUTPUT, test_pwm_frequencies[i]);
        delay(1000); // Wait for the signal to stabilize
        feedback_measurement_handler();
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, test_pwm_frequencies[i],
                                 measured_frequency_hz);
    }
#endif
}

OTTUNITY_TEST(write_pwm_8_bit_resolution) {
    analogWriteResolution(8); // Any value lesser than or equal to 8 will be
                              // considered as 8-bit resolution

    // Array of expected duty cycle percentages and their corresponding
    // analogWrite values (8-bit resolution, 0 to 255)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint8_t analog_write_values[] = {64, 127, 191};

    for (size_t i = 0;
         i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]);
         i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i],
                                 measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ,
                                 measured_frequency_hz);
    }
}

OTTUNITY_TEST(write_pwm_10_bit_resolution) {
    analogWriteResolution(10);

    // Array of expected duty cycle percentages and their corresponding
    // analogWrite values (10-bit resolution, 0 to 1023)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {256, 511, 767};

    for (size_t i = 0;
         i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]);
         i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i],
                                 measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ,
                                 measured_frequency_hz);
    }
}

OTTUNITY_TEST(write_pwm_12_bit_resolution) {
    analogWriteResolution(12);

    // Array of expected duty cycle percentages and their corresponding
    // analogWrite values (12-bit resolution, 0 to 4095)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {1024, 2047, 3071};

    for (size_t i = 0;
         i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]);
         i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i],
                                 measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ,
                                 measured_frequency_hz);
    }
}

OTTUNITY_TEST(write_pwm_16_bit_resolution) {
    analogWriteResolution(16);

    // Array of expected duty cycle percentages and their corresponding
    // analogWrite values (16-bit resolution, 0 to 65535)
    const float expected_duty_cycles[] = {25.0f, 50.0f, 75.0f};
    const uint16_t analog_write_values[] = {16384, 32767, 49151};

    for (size_t i = 0;
         i < sizeof(expected_duty_cycles) / sizeof(expected_duty_cycles[0]);
         i++) {
        analogWrite(PWM_PIN_OUTPUT, analog_write_values[i]);

        delay(1000); // Wait for the signal to stabilize

        feedback_measurement_handler();

        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_DUTY_CYCLE, expected_duty_cycles[i],
                                 measured_duty_cycle_percentage);
        TEST_ASSERT_FLOAT_WITHIN(TOLERANCE_FREQUENCY, PWM_FREQUENCY_HZ,
                                 measured_frequency_hz);
    }
}

OTTUNITY_TEST(write_pwm_100_percentage_dutycycle) {
    analogWriteResolution(16);
    analogWrite(PWM_PIN_OUTPUT, 65535);
    delay(1000); // Wait for the signal to stabilize

    TEST_ASSERT_EQUAL_MESSAGE(
        HIGH, digitalRead(PWM_PIN_FEEDBACK),
        "PWM output should be HIGH when 100 percentage duty cycle is set");
    // Check once again if the PWM pin is still HIGH
    TEST_ASSERT_EQUAL_MESSAGE(
        HIGH, digitalRead(PWM_PIN_FEEDBACK),
        "PWM output should be HIGH when 100 percentage duty cycle is set");
}

OTTUNITY_TEST(write_pwm_0_percentage_dutycycle) {
    analogWriteResolution(8);
    analogWrite(PWM_PIN_OUTPUT, 0);
    delay(1000); // Wait for the signal to stabilize

    TEST_ASSERT_EQUAL_MESSAGE(
        LOW, digitalRead(PWM_PIN_FEEDBACK),
        "PWM output should be LOW when 0 percentage duty cycle is set");
    // Check once again if the PWM pin is still LOW
    TEST_ASSERT_EQUAL_MESSAGE(
        LOW, digitalRead(PWM_PIN_FEEDBACK),
        "PWM output should be LOW when 0 percentage duty cycle is set");
}

OTTUNITY_RUN();