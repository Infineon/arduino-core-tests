/** 
 * @name    test_spi_connected2_masterpingpong.cpp
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
#include "test_config.h"

// project includes
#include <SPI.h>
#ifndef SPI_TRANSFER_DELAY
#define SPI_TRANSFER_DELAY 500
#endif
const uint8_t MAX_BUFFER_SIZE = 20;
const uint8_t MAX_TEST_ITERATION = 10;

// variables
static uint8_t testTransmitBuff[MAX_BUFFER_SIZE+1] = {0};
static uint8_t expectedReceiveBuff[MAX_BUFFER_SIZE] = {0};

SPIClass *spi_master = &SPI;

// Method invoked before a test suite is run.
static void spi_connected2_masterpingpong_suite_setup() {
    pinMode(TEST_PIN_SPI_SSEL, OUTPUT);
    pinMode(TEST_PIN_SYNC_IO, OUTPUT);

    digitalWrite(TEST_PIN_SPI_SSEL, HIGH);
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
    spi_master->begin();
}

// Method invoked after a test suite is run.
static void spi_connected2_masterpingpong_suite_teardown() {
    digitalWrite(TEST_PIN_SPI_SSEL, HIGH);
    digitalWrite(TEST_PIN_SYNC_IO, LOW);
    spi_master->end();
}

// Test group definition
TEST_GROUP(spi_connected2_masterpingpong);

TEST_SETUP(spi_connected2_masterpingpong) {

}
TEST_TEAR_DOWN(spi_connected2_masterpingpong) {
    
}


/**
 * @brief    This test transfers bytes in a loop specified by MAX_TEST_ITERATION from master to slave.
 * 
 * @details  The first byte received from slave is dummy and ignored.
 *           From the second byte onwards, the slave increments its previously
 *           transmitted byte from slave and sends it to the master.
 *           The master increments the previously transmitted byte from master
 *           and sends it to the slave.
 * 
 * @note     The sync pin is pulled high before the transfer starts and pulled low after the transfer ends.
 *           The test will stop when the sync pin is pulled low.
 */
TEST_IFX(spi_connected2_masterpingpong, test_ping_pong_transfer_byte) {

    uint8_t testTransmitByte = 0xAA;
    uint8_t testReceiveByte = 0xCC;
    uint8_t expectedReceiveByte = testReceiveByte;

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    digitalWrite(TEST_PIN_SPI_SSEL, LOW);
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    testReceiveByte =  spi_master->transfer(testTransmitByte); // first byte received is dummy and ignored
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    digitalWrite(TEST_PIN_SPI_SSEL, HIGH);

    for (uint8_t i = 1; i < MAX_TEST_ITERATION; i++) {   
        testTransmitByte++;
        
        digitalWrite(TEST_PIN_SPI_SSEL, LOW);
        delayMicroseconds(SPI_TRANSFER_DELAY_US);
        testReceiveByte = spi_master->transfer(testTransmitByte);
        delayMicroseconds(SPI_TRANSFER_DELAY_US);
        digitalWrite(TEST_PIN_SPI_SSEL, HIGH);

        TEST_ASSERT_EQUAL_UINT8_MESSAGE(expectedReceiveByte, testReceiveByte, "SPI Master PingPong transfer byte failed");

        expectedReceiveByte = testReceiveByte + 1; // slave increments the previously transmitted byte from slave
    }

    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}


/**
 * @brief    This test transfers words in a loop specified by MAX_TEST_ITERATION from master to slave.
 * 
 * @details  The first byte received from slave is dummy and ignored.
 *           From the second word onwards, the slave increments its previously
 *           transmitted word from slave and sends it to the master.
 *           The master increments the previously transmitted word from master
 *           and sends it to the slave.
 * 
 * @note     The sync pin is pulled high before the transfer starts and pulled low after the transfer ends.
 *           The test will stop when the sync pin is pulled low.
 */
TEST_IFX(spi_connected2_masterpingpong, test_ping_pong_transfer_word) {

    uint16_t testTransmitWord = 0x4433;
    uint16_t testReceiveWord = 0x5522; // first word read from slave
    uint16_t expectedReceiveWord = testReceiveWord;
    uint16_t result = 0;

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    digitalWrite(TEST_PIN_SPI_SSEL, LOW);
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    testReceiveWord =  spi_master->transfer16(testTransmitWord); // first byte received is dummy and ignored
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    digitalWrite(TEST_PIN_SPI_SSEL, HIGH);


    for (uint8_t i = 1; i < MAX_TEST_ITERATION; i++) {
        testTransmitWord++;
        result = testReceiveWord << 8;

        digitalWrite(TEST_PIN_SPI_SSEL, LOW);
        delayMicroseconds(SPI_TRANSFER_DELAY_US);
        testReceiveWord = spi_master->transfer16(testTransmitWord);
        delayMicroseconds(SPI_TRANSFER_DELAY_US);
        digitalWrite(TEST_PIN_SPI_SSEL, HIGH);

        result = result | testReceiveWord >> 8; // combine the two bytes to form a 16-bit word

        TEST_ASSERT_EQUAL_UINT16_MESSAGE(expectedReceiveWord, result, "SPI Master PingPong transfer word failed");

        expectedReceiveWord = result + 1; // slave increments the previously transmitted word from slave
    }

    digitalWrite(TEST_PIN_SYNC_IO, LOW);
}


/**
 * @brief    This test transfers buffer of data specified by MAX_BUFFER_SIZE from master to slave.
 * 
 * @details  The first byte received from slave is dummy and ignored.
 *           From the second byte onwards, the data received from the slave is valid.
 *           In order to receive all the data from slave, the master sends MAX_BUFFER_SIZE+1 bytes to the slave.
 * 
 * @note     The sync pin is pulled high before the transfer starts and pulled low after the transfer ends.
 *           The test will stop when the sync pin is pulled low.
 */
TEST_IFX(spi_connected2_masterpingpong, test_ping_pong_transfer_buffer) {

    for (uint8_t i = 0; i < MAX_BUFFER_SIZE; i++) {
        testTransmitBuff[i] = 0xAA + i;
        expectedReceiveBuff[i] = 0xBB + i;
    }

    digitalWrite(TEST_PIN_SYNC_IO, HIGH);

    digitalWrite(TEST_PIN_SPI_SSEL, LOW);
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    spi_master->transfer(testTransmitBuff, MAX_BUFFER_SIZE+1); 
    delayMicroseconds(SPI_TRANSFER_DELAY_US);
    digitalWrite(TEST_PIN_SPI_SSEL, HIGH);

    digitalWrite(TEST_PIN_SYNC_IO, LOW);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expectedReceiveBuff, testTransmitBuff+1, MAX_BUFFER_SIZE, "SPI Master PingPong transfer word failed");
}


// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(spi_connected2_masterpingpong) {
    spi_connected2_masterpingpong_suite_setup();

    RUN_TEST_CASE(spi_connected2_masterpingpong, test_ping_pong_transfer_byte);
    RUN_TEST_CASE(spi_connected2_masterpingpong, test_ping_pong_transfer_word);
    RUN_TEST_CASE(spi_connected2_masterpingpong, test_ping_pong_transfer_buffer);

    spi_connected2_masterpingpong_suite_teardown();

}
