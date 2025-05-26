/* test_interrupts_single.cpp
 *
 * This test is used to verify the functionality of the GPIO Interrupts.
 * TEST_PIN_DIGITAL_IO_OUTPUT pin should be connected to TEST_PIN_DIGITAL_IO_INPUT pin
 * for the test cases to work as expected.
 */

#include "test_common_includes.h"
#include "test_config.h"

// Defines
#define TRACE_OUTPUT
// Variables
volatile bool interrupt_triggered = false;

// Method invoked before a test suite is run.
static void gpio_interrupts_single_suite_setup() {
    
}

// Method invoked after a test suite is run.
static void gpio_interrupts_single_suite_teardown() {
    
}

// Test group name
TEST_GROUP(gpio_interrupts_single);
TEST_GROUP(gpio_interrupts_single_internal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(gpio_interrupts_single_internal)
{ 
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT);
    interrupt_triggered = false;
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(gpio_interrupts_single_internal) {
}

/**
 * @brief Interrupt callback function.
 */
void interrupt_callback() {
    interrupt_triggered = true;
}

/**
 * @brief Test attachInterrupt and detachInterrupt with a valid pin at rising edge.
 */
TEST_IFX(gpio_interrupts_single_internal, test_attach_interrupt_rising_edge)
{
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // initial state

    attachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT), interrupt_callback, RISING);

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // triggers interrupt
    delay(100);
    TEST_ASSERT_TRUE_MESSAGE(interrupt_triggered, "Interrupt should be triggered");

    // detach interrupt and test
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // initial state

    detachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT));

    interrupt_triggered = false;
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);
    delay(100);
    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not be triggered after detach");
}

/**
 * @brief Test attachInterrupt and detachInterrupt with a valid pin at falling edge.
 */
TEST_IFX(gpio_interrupts_single_internal, test_attach_interrupt_falling_edge)
{
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // initial state
    attachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT), interrupt_callback, FALLING);

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // triggers interrupt
    delay(100);
    TEST_ASSERT_TRUE_MESSAGE(interrupt_triggered, "Interrupt should be triggered");

    // detach interrupt and test
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // initial state

    detachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT));

    interrupt_triggered = false;
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);
    delay(100);
    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not be triggered after detach");
}

/**
 * @brief Test attachInterrupt and detachInterrupt with a valid pin at both edge.
 */
TEST_IFX(gpio_interrupts_single_internal, test_attach_interrupt_change_edge)
{
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // initial state

    attachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT), interrupt_callback, CHANGE);

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // triggers interrupt
    delay(100);
    TEST_ASSERT_TRUE_MESSAGE(interrupt_triggered, "Interrupt should be triggered");

    interrupt_triggered = false;
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // triggers interrupt
    delay(100);
    TEST_ASSERT_TRUE_MESSAGE(interrupt_triggered, "Interrupt should be triggered");

    interrupt_triggered = false;
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); // triggers interrupt
    delay(100);
    TEST_ASSERT_TRUE_MESSAGE(interrupt_triggered, "Interrupt should be triggered");

    // detach interrupt and test
    interrupt_triggered = false;
    detachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT));

    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);
    delay(100);
    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not be triggered after detach");
}

/**
 * @brief Test attachInterrupt and detachInterrupt with a valid pin at both edge.
 */
TEST_IFX(gpio_interrupts_single_internal, test_attach_interrupt_invalid_mode)
{
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); // initial state
    attachInterrupt(digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT), interrupt_callback, (PinStatus) 255);
    interrupt_triggered = false;
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH); 
    delay(100);
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW); 
    delay(100);
    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not be triggered when invalid mode is set");
}

/**
 * @brief Test attachInterrupt with an invalid pin.
 */
TEST_IFX(gpio_interrupts_single_internal, test_attach_interrupt_invalid_pin)
{
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, LOW);

    attachInterrupt(digitalPinToInterrupt(255), interrupt_callback, RISING); // Use an invalid pin number

    // Trigger the interrupt by writing HIGH to the output pin
    digitalWrite(TEST_PIN_DIGITAL_IO_OUTPUT, HIGH);

    // Small delay to ensure the interrupt is processed
    delay(100);

    TEST_ASSERT_FALSE_MESSAGE(interrupt_triggered, "Interrupt should not be triggered for invalid pin");
}


/**
 * @brief Test digitalPinToInterrupt function.
 */
TEST_IFX(gpio_interrupts_single_internal, test_digital_pin_to_interrupt)
{
    #if defined(ARDUINO_ARCH_XMC)
    int interrupt_number = digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT);
    TEST_ASSERT_LESS_OR_EQUAL_INT(1, interrupt_number);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, interrupt_number);
    int invalid_interrupt_number = digitalPinToInterrupt(255); // Use an invalid pin number
    TEST_ASSERT_EQUAL_MESSAGE(-1, invalid_interrupt_number, "Interrupt number should be -1 for invalid pin");
    #else
    int interrupt_number = digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT);
    TEST_ASSERT_EQUAL_MESSAGE(TEST_PIN_DIGITAL_IO_INPUT, interrupt_number, "Interrupt number should match the input pin");

    int invalid_interrupt_number = digitalPinToInterrupt(255); // Use an invalid pin number
    TEST_ASSERT_EQUAL_MESSAGE(-1, invalid_interrupt_number, "Interrupt number should be -1 for invalid pin");
    #endif
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(gpio_interrupts_single_internal)
{
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_attach_interrupt_rising_edge);
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_attach_interrupt_falling_edge);
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_attach_interrupt_change_edge);
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_attach_interrupt_invalid_mode);
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_attach_interrupt_invalid_pin);
    RUN_TEST_CASE(gpio_interrupts_single_internal, test_digital_pin_to_interrupt);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(gpio_interrupts_single)
{
    gpio_interrupts_single_suite_setup();

    RUN_TEST_GROUP(gpio_interrupts_single_internal);

    gpio_interrupts_single_suite_teardown();
}
