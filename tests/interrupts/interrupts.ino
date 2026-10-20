/**
 * This test is used to verify the functionality of the GPIO Interrupts.
 * TEST_PIN_DIGITAL_IO_OUTPUT pin should be connected to TEST_PIN_DIGITAL_IO_INPUT pin
 * for the test cases to work as expected.
 */

#include "ottu.hpp"

/* Include default board configuration */
#if !defined(TEST_PIN_DIGITAL_IO_OUTPUT) || \
    !defined(TEST_PIN_DIGITAL_IO_INPUT)
#include "config.h"
#endif

volatile bool interrupt_triggered = false;

void interrupt_callback() {
    interrupt_triggered = true;
}

OTTUNITY_TEST_SETUP() { 
    pinMode(TEST_PIN_DIGITAL_IO_OUTPUT, OUTPUT);
    pinMode(TEST_PIN_DIGITAL_IO_INPUT, INPUT);
    interrupt_triggered = false;
}

OTTUNITY_TEST_TEAR_DOWN() {

}

/**
 * @brief Test attachInterrupt and detachInterrupt with a valid pin at rising edge.
 */
OTTUNITY_TEST(rising_edge) {
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
OTTUNITY_TEST(falling_edge) {
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
OTTUNITY_TEST(change_edge) {
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
OTTUNITY_TEST(invalid_mode) {
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
OTTUNITY_TEST(invalid_pin) {
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
OTTUNITY_TEST(digital_pin_to_interrupt) {
    #if defined(ARDUINO_ARCH_XMC)
    int interrupt_number = digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT);
    TEST_ASSERT_LESS_OR_EQUAL_INT(1, interrupt_number);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, interrupt_number);
    #else
    int interrupt_number = digitalPinToInterrupt(TEST_PIN_DIGITAL_IO_INPUT);
    TEST_ASSERT_EQUAL_MESSAGE(TEST_PIN_DIGITAL_IO_INPUT, interrupt_number, "Interrupt number should match the input pin");
    #endif
    int invalid_interrupt_number = digitalPinToInterrupt(255); // Use an invalid pin number
    TEST_ASSERT_EQUAL_MESSAGE(-1, invalid_interrupt_number, "Interrupt number should be -1 for invalid pin");
}

OTTUNITY_RUN();