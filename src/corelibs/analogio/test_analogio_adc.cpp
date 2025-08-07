/**
 * @brief test_analogio_adc.cpp
 *
 * @details This test is used to verify the functionality of the Analog IO module.
 * only one board is needed with 
 * 
 * TEST_PIN_ANALOG_IO_VREF pin connected to VDDA,
 * TEST_PIN_ANALOG_IO_DIVIDER pin connected to voltage divider,
 * TEST_PIN_ANALOG_IO_GND pin connected to Ground.
 * Depending on the pins availability, the test can be run on any board.
 * 
 * 
 * Voltage divider circuit would look like below:
 * 
 *      VDDA
 *      |
 *      [R1]   <-- Resistor R1
 *      |
 *      +-------> A1 (Mid-Point connected to Analog Input pin TEST_PIN_ANALOG_IO_DIVIDER)
 *      |
 *      [R2]   <-- Resistor R2
 *      |
 *      GND (0V)
 * 
 */

// std includes

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// project includes

// defines
#define TRACE_OUTPUT
#define TOLERANCE 100

// variables

/**
 * @brief Validates if the actual ADC value is within the acceptable range of the expected value.
 *
 * @param expected_value The expected ADC value.
 * @param actual_value The ADC value read by the system.
 * @return true if the actual value is within the acceptable range, false otherwise.
 */
static bool validate_adc_raw_value(int expected_value, int actual_value) {
    return (expected_value - TOLERANCE) < actual_value && (expected_value + TOLERANCE) > actual_value;
}

/**
 * @brief Suite setup function, runs before test suite execution begins.
 */
static void analogio_adc_suite_setup() {
    
}


/**
 * @brief Suite teardown function, runs after test suite execution is complete.
 */
static void analogio_adc_suite_teardown() {
    
}

// Define test group name
TEST_GROUP(analogio_adc);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(analogio_adc) { 
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(analogio_adc) {
    analogReadResolution(TEST_ADC_RESOLUTION);
}

#ifdef TEST_PIN_ANALOG_IO_VREF 

/**
 * @brief Verify ADC value for the DEFAULT volatage reference on the pin that is connected to VDDA with various resolution values
 */
TEST_IFX(analogio_adc, test_adc_read_default_vdda_vref_pin)
{
    analogReference(DEFAULT); 
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_VREF);
    int expected_value = ADC_MAX_VALUE;
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), "ADC value is not within the expected range for default resolution");

    const int resolutions[] = {8, 10, 12}; 
    const int num_resolutions = sizeof(resolutions) / sizeof(resolutions[0]);
    
    for (int i = 0; i < num_resolutions; i++)
    {
        int resolution = resolutions[i];
        analogReadResolution(resolution);
        
        adc_value = analogRead(TEST_PIN_ANALOG_IO_VREF); // Perform ADC read
        
        int expected_value = (1 << resolution) - 1; // 2^resolution - 1
        TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), "ADC value does not match expected resolution range");
    }
}

#endif // TEST_PIN_ANALOG_IO_VREF

#ifdef TEST_PIN_ANALOG_IO_DIVIDER 

/**
 * @brief Verify ADC value for the DEFAULT volatage reference on the pin that is connected to voltage divider with various resolution values.
 */
TEST_IFX(analogio_adc, test_adc_read_default_vdda_divider_pin)
{
    analogReference(DEFAULT); // Configure reference to VDDA
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_DIVIDER);
    int expected_value = ADC_MAX_VALUE / 2;
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), "ADC value is not within the expected range for default resolution");

    const int resolutions[] = {8, 10, 12}; 
    const int num_resolutions = sizeof(resolutions) / sizeof(resolutions[0]);
    
    for (int i = 0; i < num_resolutions; i++)
    {
        int resolution = resolutions[i];
        analogReadResolution(resolution);
        
        adc_value = analogRead(TEST_PIN_ANALOG_IO_DIVIDER);
        
        int expected_value = (1 << resolution) - 1; // 2^resolution - 1
        expected_value = expected_value / 2;

        TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), "ADC value does not match expected resolution range");
    }
}

#endif // TEST_PIN_ANALOG_IO_DIVIDER

#ifdef TEST_PIN_ANALOG_IO_GND 

/**
 * @brief Verify ADC value for the DEFAULT volatage reference on the pin that is connected to ground.
 */
TEST_IFX(analogio_adc, test_adc_read_default_gnd_pin)
{
    analogReference(DEFAULT); 
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_GND);
    int expected_value = 0;
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value), "ADC value is not within the expected range");
}

#endif // TEST_PIN_ANALOG_IO_GND 


/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(analogio_adc)
{
    analogio_adc_suite_setup();

#ifdef TEST_PIN_ANALOG_IO_VREF 
    RUN_TEST_CASE(analogio_adc, test_adc_read_default_vdda_vref_pin);
#endif

#ifdef TEST_PIN_ANALOG_IO_DIVIDER
    RUN_TEST_CASE(analogio_adc, test_adc_read_default_vdda_divider_pin);
#endif

#ifdef TEST_PIN_ANALOG_IO_GND
    RUN_TEST_CASE(analogio_adc, test_adc_read_default_gnd_pin);
#endif

    analogio_adc_suite_teardown();
}
