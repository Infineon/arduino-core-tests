/**
 * @brief This test focuses on the verification the TX functionality of the UART
 * communication. 
 * 
 * @details This test is paired with the "test_uart_rx.cpp"s test, which
 * running on a second board will implement RX functionality.
 * 
 * Ultimately, the instance also wait for a reply back from the other other to 
 * verify the full duplex capability of the UART communication.
 * 
 * An additional pin is used to synchronize the start of the test. The other 
 * end wait for rising edge on the pin to start running the test.
 * 
 * The following steps are performed:
 * - notify the other end that the test will start
 * - begin the UART communication
 * - write one byte
 * - write multiple bytes
 * - print in a loop
 * - print a end of line
 * - check the availableForWrite
 * - write a longer string than the writable buffer
 * - read the reply back from the other end
 * - end the UART communication
 * 
 * This test is meant as a happy path verification of the API functions 
 * of the UART class TX mode. 
 * 
 * Exception and corner/edge cases are not covered in this test. 
 * 
 * @note This test must be run AFTER the "test_uart_rx.cpp" test. 
 */

# include "test_common_includes.h"

/**
 * The baudrate can be passed in the command line for 
 * the test.
 */
#ifndef UART_TEST_BAUDRATE
#define UART_TEST_BAUDRATE 115200
#endif

TEST_GROUP(uart_tx);

static void uart_tx_suite_setup() {
    pinMode(TEST_PIN_SYNC_IO, OUTPUT);
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}

static void uart_tx_suite_teardown() {
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}

static TEST_SETUP(uart_tx) {

}
static TEST_TEAR_DOWN(uart_tx) {

}

/**
 * @note Without this synchronization the transient state
 * of the UART bus lines during initialization 
 * can trigger already the receive interrupt on the other UART end.
 * And that can lead to potential reception of invalid data.
 */
TEST_IFX(uart_tx, notify_readiness_to_uart_rx) {
    digitalWrite(TEST_PIN_SYNC_IO, HIGH);
    /* Let some time for the other to initialize 
    and be ready for the communication. */
    delay(500);
}

TEST_IFX(uart_tx, not_ready) {
    TEST_ASSERT_FALSE(Serial1);
}

TEST_IFX(uart_tx, begin) {
    Serial1.begin(UART_TEST_BAUDRATE);
    /* No assertion. Just checking that this calls
    does not crash. */
    /* Evaluating bool operator */
    TEST_ASSERT_TRUE(Serial1);
}

TEST_IFX(uart_tx, write_one_byte) {
    uint8_t data = 0x55;

    size_t bytes_written = Serial1.write(data);
    TEST_ASSERT_EQUAL(1, bytes_written);
}

TEST_IFX(uart_tx, write_bytes) {
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    size_t bytes_written = Serial1.write(data, sizeof(data));
    TEST_ASSERT_EQUAL(sizeof(data), bytes_written);
}

/**
 * @note This test intends to check if there is any overflow 
 * when sending a data in a loop continuously.
 */
TEST_IFX(uart_tx, write_in_loop) {
    char value = 'A';
    uint8_t num_bytes_to_write = 60;
    size_t bytes_written = 0;

    for(uint8_t i = 0; i < num_bytes_to_write; i++) {
        bytes_written += Serial1.write(value);
        value++;
    }

    TEST_ASSERT_EQUAL(num_bytes_to_write, bytes_written);
}

/**
 * @note This tests checks an example of the inherited 
 * from the Print class functionality. These tests are not meant 
 * coverage of the Stream and Print class inherited functions.  
 */
TEST_IFX(uart_tx, println) {
    size_t bytes_written = Serial1.println();
    TEST_ASSERT_EQUAL(2, bytes_written);
}

TEST_IFX(uart_tx, availableForWrite) {  
    #if defined(ARDUINO_ARCH_PSOC6)
    /**
     * The internal tx_buffer of cyhal_uart library
     * is at least 128 for CY8CKIT-062S2-AI.
     * TODO: Review this value when integrating new boards.
     */
    /* The value 128 is the max value. 
       Only when sufficient time has elapsed after writing.*/
    uint8_t expected_writable = 128;
    #elif defined(ARDUINO_ARCH_XMC)
    uint8_t expected_writable = 1;
    #else
    /* Inherited implementation */
    uint8_t expected_writable = 0;
    #endif

    TEST_ASSERT_EQUAL(expected_writable, Serial1.availableForWrite());
}

/**
 * @note The message cannot be longer than the rx_buffer size of the class. 
 *       This depends on the core implementation.
 */
TEST_IFX(uart_tx, write_longer_than_writable) {
    #if defined(ARDUINO_ARCH_PSOC6) || defined (ARDUINO_ARCH_XMC)
    char msg[] = "This is a very long string that is meant to be longer "
                 "than the writable buffer of the UART. "
                 "The Serial class API provides the mechanisms for "
                 "user level flow control by availableForWrite(), "
                 "flush() and the return of the actual bytes written. "
                 "Still, for some cores, the experience is that only "
                 "after a few calls to Serial.print() or Serial.println() "
                 "with small amount of bytes, the TX buffer seems to "
                 "overflow and the Serial output becomes corrupted.";
    #else
    char msg[] = "This is a (not so) long string example. Adjust length as per core.";
    #endif

    size_t bytes_written = Serial1.write(msg, strlen(msg));
    TEST_ASSERT_EQUAL(strlen(msg), bytes_written);
}

TEST_IFX(uart_tx, read_reply) {
    char expected_msg[] = "All went well so far. We can close now. Bye. Thanks!";

    /* Wait for the expected data to be available.
    A timeout is added to avoid an infinite loop. 
    That will prevent the teardown to deinit the synch mechanism.
    And recurrent restart of the test with RX can fail (as the signal
    will remain high)
    */
    unsigned long time_start_ms = millis();
    unsigned long timeout_ms = 3000; 
    while(Serial1.available() < (int)strlen(expected_msg)) {
        if(millis() - time_start_ms >= timeout_ms) {
            TEST_FAIL_MESSAGE("Timeout waiting for reply from UART RX");
            return;
        }
    }
    
    char received_msg[sizeof(expected_msg)] = {0};
    size_t received_msg_len = Serial1.readBytes(received_msg, strlen(expected_msg));

    TEST_ASSERT_EQUAL(strlen(expected_msg), received_msg_len);
    TEST_ASSERT_EQUAL_STRING(expected_msg, received_msg);
}

TEST_IFX(uart_tx, end) {
    Serial1.end();
    TEST_ASSERT_FALSE(Serial1);
    /* No assertion. Just checking that this calls
    does not crash. */
}

TEST_GROUP_RUNNER(uart_tx) {
    uart_tx_suite_setup();
    RUN_TEST_CASE(uart_tx, notify_readiness_to_uart_rx);
    RUN_TEST_CASE(uart_tx, not_ready);
    RUN_TEST_CASE(uart_tx, begin);
    RUN_TEST_CASE(uart_tx, write_one_byte);
    RUN_TEST_CASE(uart_tx, write_bytes);
    RUN_TEST_CASE(uart_tx, write_in_loop);
    RUN_TEST_CASE(uart_tx, println);
    RUN_TEST_CASE(uart_tx, availableForWrite);
    RUN_TEST_CASE(uart_tx, write_longer_than_writable);
    RUN_TEST_CASE(uart_tx, read_reply);
    RUN_TEST_CASE(uart_tx, end);
    uart_tx_suite_teardown();
}
