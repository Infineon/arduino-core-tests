/** 
 * @name    test_spi_connected2_slavepingpong.cpp
 *
 * @brief   This test is used to verify the functionality of the SPI transfer functions.
 *          2 boards are needed with SPI pins of master board connected to SPI pins of slave board.
 *          Also a sync pin is used to synchronize the transfer between master and slave.
 * 
 * @note    These tests are expected to be executed after slave tests are executed.
 *          Make sure no SPI data is being transferred before executing these tests which
 *          may interfere with the test results.
 **/

// std includes

// test includes
#include "test_common_includes.h"
#include "pins_arduino.h"
#include "test_config.h"

// project includes
#include <SPI.h>

const uint8_t MAX_BUFFER_SIZE = 20;
const uint8_t MAX_TEST_ITERATION = 10;

// variables
static uint8_t testTransmitBuff[MAX_BUFFER_SIZE] = {0};
static uint8_t expectedReceiveBuff[MAX_BUFFER_SIZE] = {0};

SPIClass *spi_slave = &SPI1; 

// Method invoked before a test suite is run.
static void spi_connected2_slavepingpong_suite_setup() {
    pinMode(TEST_PIN_SYNC_IO, INPUT_PULLDOWN);
    spi_slave->begin();
}

// Method invoked after a test suite is run.
static void spi_connected2_slavepingpong_suite_teardown() {
    spi_slave->end();
}

// Test group definition
TEST_GROUP(spi_connected2_slavepingpong);

TEST_SETUP(spi_connected2_slavepingpong) {
}

TEST_TEAR_DOWN(spi_connected2_slavepingpong) {
}


/**
 * @brief    This test transfers bytes in a loop specified by MAX_TEST_ITERATION to the master.
 * 
 * @details  Slave increments its previously transmitted byte and sends it to the master.
 *           The master increments the previously transmitted byte from master
 *           and sends it to the slave.
 * 
 * @note     Slave initailly waits for the master to pull the sync pin high.
 */
TEST_IFX(spi_connected2_slavepingpong, test_ping_pong_transfer_byte) {
    uint8_t testTranceiveByte = 0xCC;
    uint8_t testReceiveByte = 0xAA;
    uint8_t expectedReceiveByte = testReceiveByte;
    
    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {
        // Wait for the master to pull the sync pin high
    }

    for (uint8_t i = 1; i < MAX_TEST_ITERATION + 1; i++) {
        testReceiveByte = spi_slave->transfer(testTranceiveByte);
        testTranceiveByte++;

        TEST_ASSERT_EQUAL_UINT8_MESSAGE(expectedReceiveByte, testReceiveByte, "SPI Slave PingPong byte transfer failed");

        expectedReceiveByte = testReceiveByte + 1; // master increments the previously transmitted byte from master
    }
}

/**
 * @brief    This test transfers words in a loop specified by MAX_TEST_ITERATION to the master.
 * 
 * @details  Slave increments its previously transmitted word and sends it to the master.
 *           The master increments the previously transmitted word from master
 *           and sends it to the slave.
 * 
 * @note     Slave initailly waits for the master to pull the sync pin high.
 */
TEST_IFX(spi_connected2_slavepingpong, test_ping_pong_transfer_word) {
    uint16_t testTranceiveWord = 0x5522;
    uint16_t testReceiveWord = 0x4433; // first word read from master
    uint16_t expectedReceiveWord = testReceiveWord;
    
    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {
        // Wait for the master to pull the sync pin high
    }

    for (uint8_t i = 1; i < MAX_TEST_ITERATION + 1; i++) {
        testReceiveWord = spi_slave->transfer16(testTranceiveWord);
        testTranceiveWord++;

        TEST_ASSERT_EQUAL_UINT16_MESSAGE(expectedReceiveWord, testReceiveWord, "SPI Slave PingPong word transfer failed");

        expectedReceiveWord = testReceiveWord + 1; // master increments the previously transmitted word from master
    }
}

/**
 * @brief    This test transfers a buffer of data specified by MAX_BUFFER_SIZE to the master.
 * 
 * @note     Slave initailly waits for the master to pull the sync pin high.
 */
TEST_IFX(spi_connected2_slavepingpong, test_ping_pong_transfer_buffer) {
    for (uint8_t i = 0; i < MAX_BUFFER_SIZE; i++) {
        testTransmitBuff[i] = 0xBB + i;
        expectedReceiveBuff[i] = 0xAA + i;
    }

    while (digitalRead(TEST_PIN_SYNC_IO) == LOW) {
        // Wait for the master to pull the sync pin high
    }

    spi_slave->transfer(testTransmitBuff, MAX_BUFFER_SIZE);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedReceiveBuff, testTransmitBuff, MAX_BUFFER_SIZE, "SPI Master PingPong transfer buffer failed");
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(spi_connected2_slavepingpong) {
    spi_connected2_slavepingpong_suite_setup();

    RUN_TEST_CASE(spi_connected2_slavepingpong, test_ping_pong_transfer_byte);
    RUN_TEST_CASE(spi_connected2_slavepingpong, test_ping_pong_transfer_word);
    RUN_TEST_CASE(spi_connected2_slavepingpong, test_ping_pong_transfer_buffer);

    spi_connected2_slavepingpong_suite_teardown();
}
