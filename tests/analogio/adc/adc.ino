/**
 * @brief ADC tests using the default analog reference.
 *
 * Checks reference-voltage and half-supply voltage-divider inputs at the
 * default resolution and at 8, 10, and 12 bits, plus a grounded input.
 * Readings must be within TOLERANCE counts of their expected values.
 * Each test is enabled by its corresponding TEST_PIN_ANALOG_IO_* definition.
 */

#include "config.h"
#include "ottu.hpp"

#define TOLERANCE 100

/**
 * @brief Validates if the actual ADC value is within the acceptable range of
 * the expected value.
 *
 * @param expected_value The expected ADC value.
 * @param actual_value The ADC value read by the system.
 * @return true if the actual value is within the acceptable range, false
 * otherwise.
 */
static bool validate_adc_raw_value(int expected_value, int actual_value) {
    return (expected_value - TOLERANCE) < actual_value &&
           (expected_value + TOLERANCE) > actual_value;
}

#ifdef TEST_PIN_ANALOG_IO_VREF

OTTUNITY_TEST(read_default_vdda_vref_pin) {
    analogReference(DEFAULT);
    int adc_value = analogRead(
        TEST_PIN_ANALOG_IO_VREF); // Make sure that you have selected the
                                  // correct ADC pin for your board
    int expected_value = TEST_ADC_MAX_VALUE;
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
                             "ADC value is out of the acceptable range");

    const int resolutions[] = {8, 10, 12};
    const int num_resolutions = sizeof(resolutions) / sizeof(resolutions[0]);

    for (int i = 0; i < num_resolutions; i++) {
        int resolution = resolutions[i];
        analogReadResolution(resolution);

        adc_value = analogRead(TEST_PIN_ANALOG_IO_VREF);

        int expected_value = (1 << resolution) - 1; // 2^resolution - 1
        TEST_ASSERT_TRUE_MESSAGE(
            validate_adc_raw_value(expected_value, adc_value),
            "ADC value is out of the acceptable range");
    }
}

#endif // TEST_PIN_ANALOG_IO_VREF

#ifdef TEST_PIN_ANALOG_IO_DIVIDER

OTTUNITY_TEST(read_default_vdda_divider_pin) {
    analogReference(DEFAULT);
    int adc_value = analogRead(
        TEST_PIN_ANALOG_IO_DIVIDER); // Make sure that you have selected the
                                     // correct ADC pin for your board
    int expected_value =
        TEST_ADC_MAX_VALUE /
        2; // Assuming a voltage divider that halves the input voltage
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
                             "ADC value is out of the acceptable range");

    const int resolutions[] = {8, 10, 12};
    const int num_resolutions = sizeof(resolutions) / sizeof(resolutions[0]);

    for (int i = 0; i < num_resolutions; i++) {
        int resolution = resolutions[i];
        analogReadResolution(resolution);

        adc_value = analogRead(TEST_PIN_ANALOG_IO_DIVIDER);

        int expected_value = (1 << resolution) - 1; // 2^resolution - 1
        expected_value /= 2;

        TEST_ASSERT_TRUE_MESSAGE(
            validate_adc_raw_value(expected_value, adc_value),
            "ADC value is out of the acceptable range");
    }
}

#endif // TEST_PIN_ANALOG_IO_DIVIDER

#ifdef TEST_PIN_ANALOG_IO_GND

OTTUNITY_TEST(read_default_gnd_pin) {
    analogReference(DEFAULT);
    int adc_value = analogRead(TEST_PIN_ANALOG_IO_GND);
    int expected_value = 0;
    TEST_ASSERT_TRUE_MESSAGE(validate_adc_raw_value(expected_value, adc_value),
                             "ADC value is out of the acceptable range");
}

#endif // TEST_PIN_ANALOG_IO_GND

OTTUNITY_RUN();
