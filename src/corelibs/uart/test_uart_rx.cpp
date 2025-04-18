/**
 * @brief This test focuses on the verification the RX functionality of the UART
 * communication. 
 * 
 * @details This test is paired with the "test_uart_tx.cpp"s test, which
 * running on a second board will implement TX functionality.
 * 
 * Ultimately, the instance also writes back to the other other to verify the
 * full duplex capability of the UART communication.
 * 
 * An additional pin is used to synchronize the start of the test. The other 
 * end will notify that the test will start by a rising edge on that pin.
 * 
 * The following steps are performed:
 * - wait for the synchronization signal
 * - begin the UART communication
 * - wait and read one byte
 * - wait and peek one byte
 * - wait and read multiple bytes
 * - wait and read in a loop
 * - wait for data sent using Print class
 * - read a longer string than the writable buffer
 * - check that no data is available
 * - write a reply back to the other end
 * - end the UART communication
 * 
 * This test is meant as a happy path verification of the API functions 
 * of the UART class RX mode. 
 * 
 * Exception and corner/edge cases are not covered in this test. 
 * 
 * @note This test must be run BEFORE the "test_uart_tx.cpp" test. 
 */

# include "test_common_includes.h"

/**
 * The baudrate can be passed in the command line for 
 * the test.
 */
#ifndef UART_TEST_BAUDRATE
#define UART_TEST_BAUDRATE 115200
#endif

TEST_GROUP(uart_rx);

static TEST_SETUP(uart_rx) {

}
static TEST_TEAR_DOWN(uart_rx) {

}

/**
 * @note Without this synchronization the transient state
 * of the TX UART bus lines during initialization 
 * can trigger already the receive interrupt on this UART end.
 * And that can lead to potential reception of invalid data.
 */
TEST_IFX(uart_rx, wait_for_synch_signal) {
    pinMode(TEST_SYNC_INPUT_OUTPUT, INPUT_PULLDOWN);
    while (digitalRead(TEST_SYNC_INPUT_OUTPUT) == LOW) {}
}

TEST_IFX(uart_rx, begin) {
    Serial1.begin(UART_TEST_BAUDRATE);
    TEST_ASSERT_TRUE(Serial1);
}

TEST_IFX(uart_rx, wait_and_read_one_byte) {
    uint8_t expected_data = 0x55;

    /* Block until data is available */
    while(Serial1.available() == 0) {}

    uint8_t received_data = Serial1.read();
    TEST_ASSERT_EQUAL(expected_data, received_data);
}

TEST_IFX(uart_rx, wait_and_peek) {
    uint8_t expected_data = 0x11;

    /* Block until data is available */
    while(Serial1.available() == 0) {}

    uint8_t peeked_data = Serial1.peek();
    TEST_ASSERT_EQUAL(expected_data, peeked_data);
}

TEST_IFX(uart_rx, wait_and_read_multi_bytes) {
    uint8_t expected_data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    /* Wait for the expected data to be available */
    while(Serial1.available() < (int)sizeof(expected_data)) {}
    
    uint8_t read_bytes[sizeof(expected_data)] = {0};
    uint8_t received_data = Serial1.readBytes(read_bytes, sizeof(expected_data));

    TEST_ASSERT_EQUAL(sizeof(expected_data), received_data);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_data, read_bytes, sizeof(expected_data));
}

TEST_IFX(uart_rx, wait_and_read_in_loop) {
    char expected_data = 'A';
    uint8_t expected_num_rx_bytes = 60;

    /* Wait for the expected data to be available */
    while(Serial1.available() < expected_num_rx_bytes) {}

    while(expected_num_rx_bytes) {
        char received_data = Serial1.read();
        TEST_ASSERT_EQUAL(expected_data, received_data);
        expected_data++;
        expected_num_rx_bytes--;
    }

    TEST_ASSERT_EQUAL(0, expected_num_rx_bytes);
}

/**
 * @note This tests checks an example of the inherited 
 * from the Print class functionality. These tests are not meant 
 * coverage of the Stream and Print class inherited functions.  
 */
TEST_IFX(uart_rx, wait_for_println) {
    char expected_data[] = "\r\n";

    /* Wait for the expected data to be available */
    while(Serial1.available() < (int)strlen(expected_data)) {}
    
    char read_bytes[sizeof(expected_data)] = {0};
    size_t received_data = Serial1.readBytes(read_bytes, strlen(expected_data));

    TEST_ASSERT_EQUAL(strlen(expected_data), received_data);
    TEST_ASSERT_EQUAL_STRING(expected_data, read_bytes);
}

/**
 * @note The expected message cannot be longer than the rx_buffer size. 
 *       This depends on the core implementation.
 */
TEST_IFX(uart_rx, read_longer_than_writable) {
    #if defined(ARDUINO_ARCH_PSOC6)
    char expected_msg[] = "This is a very long string that is meant to be longer "
                 "than the writable buffer of the UART. "
                 "The Serial class API provides the mechanisms for "
                 "user level flow control by availableForWrite(), "
                 "flush() and the return of the actual bytes written. "
                 "Still, for some cores, the experience is that only "
                 "after a few calls to Serial.print() or Serial.println() "
                 "with small amount of bytes, the TX buffer seems to "
                 "overflow and the Serial output becomes corrupted.";
    #else
    char expected_msg[] = "This is a (not so) long string example. Adjust length as per core.";
    #endif

    /* Wait for the expected data to be available */
    while(Serial1.available() < (int)strlen(expected_msg)) {}
    
    char received_msg[sizeof(expected_msg)] = {0};
    size_t received_msg_len = Serial1.readBytes(received_msg, strlen(expected_msg));

    TEST_ASSERT_EQUAL(strlen(expected_msg), received_msg_len);
    TEST_ASSERT_EQUAL_STRING(expected_msg, received_msg);
}

/**
 *  @note After reading all the available data these 
 * functions should return the values for empty rx buffer.
 */
TEST_IFX(uart_rx, check_no_available_read_peek) {

    TEST_ASSERT_EQUAL(0, Serial1.available());
    TEST_ASSERT_EQUAL(-1, Serial1.read());
    TEST_ASSERT_EQUAL(-1, Serial1.peek());
}

/**
 * @note This test is only to check the other half of the 
 * duplex communication. This end focuses on the RX functionality.
 */
TEST_IFX(uart_rx, write_reply_back) {
    char msg[] = "All went well so far. We can close now. Bye. Thanks!";

    size_t bytes_written = Serial1.write(msg, strlen(msg));
    TEST_ASSERT_EQUAL(strlen(msg), bytes_written);
}

TEST_IFX(uart_rx, end) {
    /* Wait for the other end to complete its reading operations
     to avoid read errors due to closing the bus too early.*/
    delay(300);
    Serial1.end();
    TEST_ASSERT_FALSE(Serial1);
    /* No assertion. Just checking that this calls
    does not crash. */
}

TEST_GROUP_RUNNER(uart_rx) {
    RUN_TEST_CASE(uart_rx, wait_for_synch_signal);
    RUN_TEST_CASE(uart_rx, begin);
    RUN_TEST_CASE(uart_rx, wait_and_read_one_byte);
    RUN_TEST_CASE(uart_rx, wait_and_peek);
    RUN_TEST_CASE(uart_rx, wait_and_read_multi_bytes);
    RUN_TEST_CASE(uart_rx, wait_and_read_in_loop);
    RUN_TEST_CASE(uart_rx, wait_for_println);
    RUN_TEST_CASE(uart_rx, read_longer_than_writable);
    RUN_TEST_CASE(uart_rx, check_no_available_read_peek);
    RUN_TEST_CASE(uart_rx, write_reply_back);
    RUN_TEST_CASE(uart_rx, end);
    /* Wait forever for now. */
    /* This allows to check the rx
    test manually. */
    while(true) {};
}

