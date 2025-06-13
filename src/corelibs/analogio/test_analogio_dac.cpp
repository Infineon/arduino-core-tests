/**
 * @brief test_analogio_dac.cpp
 *
 * @details This test verifies the DAC functionality.
 * It writes different values via analogWrite() on the DAC output pin (TEST_PIN_ANALOG_IO_DAC, i.e., P14.8)
 * and then reads the voltage using the ADC from TEST_PIN_ANALOG_IO_DAC_INPUT.
 * The tests assert that the ADC reading is within tolerance compared to the expected value computed by:
 *    expected_value = (775 - 93) * written_value / 1023 + 93
 * The maximum output voltage of the dac is 2.5V, but V_DD is 3.3V. The upper bound is therefore: (2.5 / 3.3) * 1023 = 775
*  The minimum output voltage of the dac is 0.3V. Therefore, the lower bound is: (0.3 / 3.3) * 1023 = 93
 */

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// defines
#define TOLERANCE_BELOW 0.065
#define TOLERANCE_ABOVE 0.03

/**
 * @brief Validates if the actual ADC value is within the acceptable range of the expected value.
 *
 * @param expected_value The expected ADC value.
 * @param actual_value The ADC value read by the system.
 * @return true if the actual value is within the acceptable range, false otherwise.
 */
static bool validate_adc_raw_value(int expected_value, int actual_value) {
    return (expected_value * (1 - TOLERANCE_BELOW)) < actual_value &&
           (expected_value * (1 + TOLERANCE_ABOVE)) > actual_value;
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
    
    // Calculate expected ADC value using the following formula: 775 is highest resolution, 93 is lowest resolution --> needed a way to compare
    int expected_value = (775 - 93) * write_value / 1023 + 93;
    
    // Read the voltage via the ADC from the DAC input pin (for now A0)
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    /*
    // Print the expected and actual ADC values
    Serial.print("Test Full Scale - Expected ADC value: ");
    Serial.print(expected_value);
    Serial.print(" - Actual ADC value: ");
    Serial.println(adc_value);
    */

    // Assert that the measured ADC value is within tolerance of the expected value.
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
        "Full-scale DAC output not received as expected");
}

/**
 * @brief Helper function for half-scale DAC test.
 *
 * For a given resolution 'res', this function:
 *  - Sets the DAC resolution,
 *  - Writes a half-scale value,
 *  - Computes expected ADC reading based on:
 *         expected = (max_adc - min_adc) * (max_dac/2)/max_dac + min_adc
 *    where max_adc = (2.5/3.3)*max_dac and min_adc = (0.3/3.3)*max_dac.
 */
static void half_dac_test_for_resolution(uint16_t res)
{
    analogWriteResolution(res);
    uint16_t max_dac = (1 << res) - 1;
    int write_value = max_dac / 2;  // Half scale
    
    analogWrite(TEST_PIN_ANALOG_IO_DAC, write_value);
    analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    delay(500);
    
    int expected_value = (775 - 93) * write_value / max_dac + 93;
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    char msg[128];
    sprintf(msg, "%u-bit half-scale test failed: expected %d, got %d", res, expected_value, adc_value);
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), msg);
}

/**
 * @brief Test for DAC half-scale (VDD/2) using 8-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_half_8_bit)
{
    half_dac_test_for_resolution(8);
}

/**
 * @brief Test for DAC half-scale (VDD/2) using 10-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_half_10_bit)
{
    half_dac_test_for_resolution(10);
}

/**
 * @brief Test for DAC half-scale (VDD/2) using 12-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_half_12_bit)
{
    half_dac_test_for_resolution(12);
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
    
    /*
    Serial.print("Test One-Third Scale - Expected ADC value: ");
    Serial.print(expected_value);
    Serial.print(" - Actual ADC value: ");
    Serial.println(adc_value);
    */

    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
        "One-third scale DAC output not received as expected");
}

/**
 * @brief Bundle all tests for this test group.
 */
TEST_GROUP_RUNNER(analogio_dac)
{
    analogio_dac_suite_setup();
    
#ifdef TEST_PIN_ANALOG_IO_DAC
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_full);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_half_8_bit);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_half_10_bit);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_half_12_bit);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_onethird);
#endif

    analogio_dac_suite_teardown();
}