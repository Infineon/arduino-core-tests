/* test_digitalio_single.cpp
 *
 * This test is used to verify the functionality of the Digital IO module.
 * only one board is needed with TEST_DIGITALIO_OUTPUT pin connected to 
 * TEST_DIGITALIO_INPUT pin for the test cases to work as expected.
 */

// std includes

// test includes
#include "test_common_includes.h"
#include "test_config.h"

// project includes

// defines
#define TRACE_OUTPUT

// variables

// Method invoked before a test suite is run.
static void digitalio_single_suite_setup() {
    
}


// Method invoked after a test suite is run.
static void digitalio_single_suite_teardown() {
    
}

// test defines
const uint8_t INVALID_PIN = 255; 


// Define test group name
TEST_GROUP(digitalio_single);
TEST_GROUP(digitalio_single_internal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(digitalio_single_internal) { 
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(digitalio_single_internal) {
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT mode.
 * 
 * @note: Assumption is made that TEST_DIGITALIO_OUTPUT is connected to TEST_DIGITALIO_INPUT
 *       for the test cases to work as expected.
 */
TEST_IFX(digitalio_single_internal, test_digitalio_read_write_input_normal)
{
    pinMode(TEST_DIGITALIO_OUTPUT, OUTPUT);
    pinMode(TEST_DIGITALIO_INPUT, INPUT);

    digitalWrite(TEST_DIGITALIO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITALIO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT_PULLUP mode.
 * 
 * @note: Assumption is made that TEST_DIGITALIO_OUTPUT is connected to TEST_DIGITALIO_INPUT
 *       for the test cases to work as expected.
 */
TEST_IFX(digitalio_single_internal, test_digitalio_read_write_input_pullup)
{
    pinMode(TEST_DIGITALIO_OUTPUT, OUTPUT);
    pinMode(TEST_DIGITALIO_INPUT, INPUT_PULLUP);

    digitalWrite(TEST_DIGITALIO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITALIO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT_PULLDOWN mode.
 * 
 * @note: Assumption is made that TEST_DIGITALIO_OUTPUT is connected to TEST_DIGITALIO_INPUT
 *       for the test cases to work as expected.
 */
TEST_IFX(digitalio_single_internal, test_digitalio_read_write_input_pulldown) 
{
    pinMode(TEST_DIGITALIO_OUTPUT, OUTPUT);
    pinMode(TEST_DIGITALIO_INPUT, INPUT_PULLDOWN);

    digitalWrite(TEST_DIGITALIO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITALIO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when output pin is set to OPENDRAIN mode.
 * 
 * @note: Assumption is made that TEST_DIGITALIO_OUTPUT is connected to TEST_DIGITALIO_INPUT
 *       for the test cases to work as expected.
 */
TEST_IFX(digitalio_single_internal, test_digitalio_read_write_output_opendrain)
{
    pinMode(TEST_DIGITALIO_OUTPUT, OUTPUT_OPENDRAIN);
    pinMode(TEST_DIGITALIO_INPUT, INPUT);

    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to LOW initially");

    digitalWrite(TEST_DIGITALIO_OUTPUT, HIGH); 
    // Skip assert as it may not be set to HIGH due to open-drain configuration
    digitalWrite(TEST_DIGITALIO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITALIO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(digitalio_single_internal)
{
    RUN_TEST_CASE(digitalio_single_internal, test_digitalio_read_write_input_normal);
    RUN_TEST_CASE(digitalio_single_internal, test_digitalio_read_write_input_pullup);
    RUN_TEST_CASE(digitalio_single_internal, test_digitalio_read_write_input_pulldown);
    RUN_TEST_CASE(digitalio_single_internal, test_digitalio_read_write_output_opendrain);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(digitalio_single)
{
    digitalio_single_suite_setup();

    RUN_TEST_GROUP(digitalio_single_internal);

    digitalio_single_suite_teardown();
}
