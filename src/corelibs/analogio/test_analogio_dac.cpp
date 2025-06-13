/**
 * @file test_analogio_dac.cpp
 * @brief DAC Functionality Verification Tests
 *
 * @details This test suite verifies the proper operation of the DAC on the board.
 *          The tests send predetermined digital values to the DAC output pin (TEST_PIN_ANALOG_IO_DAC)
 *          via analogWrite() and then measure the corresponding analog voltage using the ADC at
 *          TEST_PIN_ANALOG_IO_DAC_INPUT.
 *
 *          The expected ADC reading is computed using a scaled interpolation between two limits:
 *
 *              expected_value = (MAX_ADC - MIN_ADC) * (written_value / READ_RESOLUTION) + MIN_ADC
 *
 *          Where:
 *              - READ_RESOLUTION is the full-scale digital value for the DAC (e.g., the maximum for a 10-bit resolution).
 *              - MAX_ADC represents the ADC reading corresponding to the DAC's maximum output voltage,
 *                which is defined by the macro ANALOG_MAXIMUM.
 *              - MIN_ADC represents the ADC reading corresponding to the DAC's minimum output voltage,
 *                which is defined by the macro ANALOG_MINIMUM.
 *
 *          This approach generalizes the test across various hardware configurations by replacing
 *          fixed numeric values with descriptive macros.
 *
 *          The test suite includes the following cases:
 *              - Full-scale test: Verifies output when the DAC is driven to its full digital range.
 *              - Half-scale tests (VDD/2): Verifies output when the DAC is set to half of its digital range,
 *                tested under different resolutions.
 *              - One-third scale test (VDD/3): Verifies output when the DAC is driven to one-third of its full-scale value.
 */

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// defines
#define ANALOG_MAXIMUM 2.5
#define ANALOG_MINIMUM 0.3
#define READ_RESOLUTION 1023

// tolerance = gain error, see data sheet
#define TOLERANCE_BELOW 0.065
#define TOLERANCE_ABOVE 0.03

// New helper macros
#define MAX_ADC ((int)((ANALOG_MAXIMUM/3.3)*READ_RESOLUTION))
#define MIN_ADC ((int)((ANALOG_MINIMUM/3.3)*READ_RESOLUTION))

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
 * @brief Helper function for half-scale DAC test.
 *
 * For a given resolution 'res', this function:
 *  - Sets the DAC resolution,
 *  - Writes a half-scale value,
 *  - Computes expected ADC reading based on:
 *         expected = (MAX_ADC - MIN_ADC) * (max_dac/2)/max_dac + MIN_ADC
 */
static void dac_test_for_resolution_divider(uint16_t res, uint16_t divider)
{
    analogWriteResolution(res);
    uint16_t max_dac = (1 << res) - 1;
    int write_value = max_dac / divider;  // Half scale
    
    analogWrite(TEST_PIN_ANALOG_IO_DAC, write_value);
    analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    delay(500);
    
    int expected_value = (MAX_ADC - MIN_ADC) * write_value / max_dac + MIN_ADC;
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DAC_INPUT);
    
    char msg[128];
    sprintf(msg, "%u-bit half-scale test failed: expected %d, got %d", res, expected_value, adc_value);
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), msg);
}

/**
 * @brief Test for DAC half-scale (VDD/2) using 8-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_8_bit)
{
    dac_test_for_resolution_divider(8, 1);
    dac_test_for_resolution_divider(8, 2);
    dac_test_for_resolution_divider(8, 3);
}

/**
 * @brief Test for DAC half-scale (VDD/2) using 10-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_10_bit)
{
    dac_test_for_resolution_divider(10, 1);
    dac_test_for_resolution_divider(10, 2);
    dac_test_for_resolution_divider(10, 3);

}

/**
 * @brief Test for DAC half-scale (VDD/2) using 12-bit resolution.
 */
TEST_IFX(analogio_dac, test_dac_write_and_read_value_12_bit)
{
    dac_test_for_resolution_divider(12, 1);
    dac_test_for_resolution_divider(12, 2);
    dac_test_for_resolution_divider(12, 3);
}

/**
 * @brief Bundle all tests for this test group.
 */
TEST_GROUP_RUNNER(analogio_dac)
{
    analogio_dac_suite_setup();
    
#ifdef TEST_PIN_ANALOG_IO_DAC
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_8_bit);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_10_bit);
    RUN_TEST_CASE(analogio_dac, test_dac_write_and_read_value_12_bit);
#endif

    analogio_dac_suite_teardown();
}