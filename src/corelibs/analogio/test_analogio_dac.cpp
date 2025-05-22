/**
 * @brief test_analogio_dac.cpp
 *
 * @details This test verifies the DAC functionality.
 * It writes different values via analogWrite() on the DAC output pin (TEST_PIN_ANALOG_IO_DAC, i.e., P14.8)
 * and then reads the voltage using the ADC from TEST_PIN_ANALOG_IO_DAC_INPUT.
 * The tests assert that the ADC reading is within tolerance compared to the expected value computed by:
 *    expected_value = (775 - 93) * written_value / 1023 + 93
 */

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// defines
#define TOLERANCE_Below 0.065
#define TOLERANCE_Above 0.03

/**
 * @brief Validates if the actual ADC value is within the acceptable range of the expected value.
 *
 * @param expected_value The expected ADC value.
 * @param actual_value The ADC value read by the system.
 * @return true if the actual value is within the acceptable range, false otherwise.
 */
static bool validate_adc_raw_value(int expected_value, int actual_value) {
    return (expected_value * (1-TOLERANCE_Below)) < actual_value &&
           (expected_value * (1 + TOLERANCE_Above)) > actual_value;
}

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void analogio_dac_suite_setup() {
    // Configure the DAC output pin as an output
    pinMode(TEST_PIN_ANALOG_IO_DAC, OUTPUT);
}

/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void analogio_dac_suite_teardown() {
    // Additional teardown if needed.
}

// Define test group name
TEST_GROUP(analogio_dac);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(analogio_dac) { }

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(analogio_dac) { }

#ifdef TEST_PIN_ANALOG_IO_DAC  // Ensure the DAC output is defined

/**
 * @brief Test 1: Write full-scale (ADC_RESOLUTION) and read back.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_full)
{
    // Configure ADC and DAC resolution (10-bit)
    analogReference(DEFAULT);
    analogWriteResolution(10);
    
    int write_value = ADC_RESOLUTION;
    // Write the full-scale value (1023) to the DAC output pin (P14.8)
    analogWrite(TEST_PIN_ANALOG_IO_DAC, write_value);
    
    // Discard the first ADC reading to allow settling
    analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    delay(500);
    
    // Calculate the expected ADC value using the given formula
    int expected_value = (775 - 93) * write_value / 1023 + 93;
    
    // Read the voltage via the ADC from the DAC input pin (for now A0)
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    // Print the expected and actual ADC values
    Serial.print("Test Full Scale - Expected ADC value: ");
    Serial.print(expected_value);
    Serial.print(" - Actual ADC value: ");
    Serial.println(adc_value);
    
    // Assert that the measured ADC value is within tolerance of the expected value.
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
        "Full-scale DAC output not received as expected");
}

/**
 * @brief Test 2: Write half-scale (ADC_RESOLUTION/2) and read back.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_half)
{
    analogReference(DEFAULT);
    analogWriteResolution(10);
    
    int write_value = ADC_RESOLUTION / 2;
    // Write half-scale value to the DAC output pin
    analogWrite(TEST_PIN_ANALOG_IO_DAC, write_value);
    
    // Discard the first ADC reading
    analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    delay(500);
    
    int expected_value = (775 - 93) * write_value / 1023 + 93;
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    Serial.print("Test Half Scale - Expected ADC value: ");
    Serial.print(expected_value);
    Serial.print(" - Actual ADC value: ");
    Serial.println(adc_value);
    
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
        "Half-scale DAC output not received as expected");
}

/**
 * @brief Test 3: Write one-third scale (ADC_RESOLUTION/3) and read back.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_onethird)
{
    analogReference(DEFAULT);
    analogWriteResolution(10);
    
    int write_value = ADC_RESOLUTION / 3;
    // Write one-third scale value to the DAC output pin
    analogWrite(TEST_PIN_ANALOG_IO_DAC, write_value);
    
    // Discard the first ADC reading
    analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    delay(500);
    
    int expected_value = (775 - 93) * write_value / 1023 + 93;
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    Serial.print("Test One-Third Scale - Expected ADC value: ");
    Serial.print(expected_value);
    Serial.print(" - Actual ADC value: ");
    Serial.println(adc_value);
    
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
        "One-third scale DAC output not received as expected");
}

#endif // TEST_PIN_ANALOG_IO_DAC

/**
 * @brief Bundle all tests for this test group.
 */
TEST_GROUP_RUNNER(analogio_dac)
{
    analogio_dac_suite_setup();
    
#ifdef TEST_PIN_ANALOG_IO_DAC
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_full);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_half);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_onethird);
#endif

    analogio_dac_suite_teardown();
}