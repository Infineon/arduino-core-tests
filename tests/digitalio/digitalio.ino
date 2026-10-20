/** 
 * This test is used to verify the functionality of the Digital IO module.
 * only one board is needed with TEST_PIN_DIGITAL_IO_OUTPUT pin connected to 
 * TEST_PIN_DIGITAL_IO_INPUT pin for the test cases to work as expected.
 */

#include "ottu.hpp"

/* Include default board configuration */
#if !defined(TEST_PIN_DIGITAL_IO_OUTPUT) || \
    !defined(TEST_PIN_DIGITAL_IO_INPUT)
#include "config.h"
#endif

const uint8_t INVALID_PIN = 255; 

volatile bool interrupt_triggered = false;

void test_pin_interrupt_handler() {
    interrupt_triggered = true;
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT mode.
 * 
 * @note: Assumption is made that TEST_PIN_DIGITAL_IO_OUTPUT is connected to TEST_PIN_DIGITAL_IO_INPUT
 *       for the test cases to work as expected.
 */
OTTUNITY_TEST(read_write_input_normal)
{
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT);

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to HIGH");

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify when input pin is set to INPUT_PULLUP mode.
 * 
 * @note: Assumption is made that TEST_PIN_DIGITAL_IO_OUTPUT is connected to TEST_PIN_DIGITAL_IO_INPUT
 *       for the test cases to work as expected.
 */
OTTUNITY_TEST(read_write_input_pullup) {
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT_OPENDRAIN);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT_PULLUP);

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW");

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // set output pin to HIGH ie, floating state
#if defined(ARDUINO_ARCH_XMC)   
    delay(1); //To stablize the floating voltage need small delay in XMC board.
#endif
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to HIGH when output is floating and input is pullup");
}

/**
 * @brief This test is to verify when input pin is set to INPUT_PULLDOWN mode.
 * 
 * @note: Assumption is made that TEST_PIN_DIGITAL_IO_OUTPUT is connected to TEST_PIN_DIGITAL_IO_INPUT
 *       for the test cases to work as expected.
 */
OTTUNITY_TEST(read_write_input_pulldown) {
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT_OPENDRAIN);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT_PULLDOWN);

    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW initially");

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);// set output pin to HIGH ie, floating state
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be LOW when output is floating and input is pulldown");

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when output pin is set to OPENDRAIN mode.
 * 
 * @note: Assumption is made that TEST_PIN_DIGITAL_IO_OUTPUT is connected to TEST_PIN_DIGITAL_IO_INPUT
 *       for the test cases to work as expected.
 */
OTTUNITY_TEST(read_write_output_opendrain) {
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT_OPENDRAIN);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT);

    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW initially");

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); 
    // Skip assert as it may not be set to HIGH due to open-drain configuration

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_PIN_DIGITAL_IO_INPUT), "Input Pin should be set to LOW");
}

/**
 * @brief Test invalid pin number handling in pinMode
*/
OTTUNITY_TEST(mode_invalid_pin) {
    pinMode(INVALID_PIN, OUTPUT);
    // No assertion as pinMode doesn't return a value, but ensure no crash
}

/**
 * @brief Test invalid pin mode
*/
OTTUNITY_TEST(mode_invalid_mode) {
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, 255);
    // No assertion as pinMode doesn't return a value, but ensure no crash
}

/**
 * @brief Test invalid pin number handling in digitalWrite
*/
OTTUNITY_TEST(write_invalid_pin) {
    digitalWrite(INVALID_PIN, HIGH);
    // No assertion as digitalWrite doesn't return a value, but ensure no crash
}

/**
 * @brief Test invalid pin number handling in digitalRead
*/
OTTUNITY_TEST(read_invalid_pin) {
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(INVALID_PIN), "Should return LOW for invalid pin");
}

/**
 * @brief Test switching pinmode from OUTPUT to INPUT_PULLUP. 
 * Here an interrupt is attached to the input pin to check if it gets triggered on state change.
 * This is to ensure that when the init pin value remains unchanged, underlying reconfigure function is used and there is no state change.
 * 
 *  @note: Assumption is made that TEST_PIN_DIGITAL_IO_OUTPUT is connected to TEST_PIN_DIGITAL_IO_INPUT
 *       for the test cases to work as expected.
 */

OTTUNITY_TEST(switch_input_output) {    
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT);
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);

    interrupt_triggered = false;
    attachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT), test_pin_interrupt_handler, CHANGE);

    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, INPUT_PULLUP);

    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not have been triggered on state change");
}

OTTUNITY_RUN();