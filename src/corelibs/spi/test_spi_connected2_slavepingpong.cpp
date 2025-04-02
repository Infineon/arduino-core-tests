// std includes

// test includes
#include "test_common_includes.h"

// project includes
#include <SPI.h>

const uint8_t MAX_BUFFER = 11;
const uint8_t MAX_TEST_ITERATION = MAX_BUFFER-1;
// variables
static SPIClassPSOC *spi_slave = nullptr;
static uint8_t testTranceiveBuff[MAX_BUFFER] = {0};
static uint8_t expectedReceiveData[MAX_BUFFER] = {0};

// Method invoked before a test suite is run.
static void spi_connected2_slavepingpong_suite_setup() {
    // spi_slave = &SPIClassPSOC(SPI1_MOSI_PIN, SPI1_MISO_PIN, SPI1_SCK_PIN, TEST_SPI_SLAVE_SELECT, true);
    spi_slave = &SPI1;
    spi_slave->begin();
    pinMode(TEST_SYNC_INPUT_OUTPUT, INPUT_PULLDOWN);
}

// Method invoked after a test suite is run.
static void spi_connected2_slavepingpong_suite_teardown() {
    spi_slave->end();
}

// Test group definition
TEST_GROUP(spi_connected2_slavepingpong);
TEST_SETUP(spi_connected2_slavepingpong) {}
TEST_TEAR_DOWN(spi_connected2_slavepingpong) {}


/**
 * @brief    This test transfers number of bytes specified by MAX_TEST_ITERATION to the master.
 * 
 * @details  Slave increments its previously transmitted byte and sends it to the master.
 *           The master increments the previously transmitted byte from master
 *           and sends it to the slave.
 */
TEST_IFX(spi_connected2_slavepingpong, test_ping_pong_transfer_byte) {
    while (digitalRead(TEST_SYNC_INPUT_OUTPUT) == LOW) {
        // Wait for the master to pull the sync pin high
    }

    uint8_t testTranceiveByte = 0;
    uint8_t testReceiveByte = 0;
    uint8_t expectedReceiveByte = 0;

    for (uint8_t i = 1; i < MAX_TEST_ITERATION; i++) {
        expectedReceiveByte = testReceiveByte + 1; // master increments the previously transmitted byte from master
        testReceiveByte = spi_slave->transfer(testTranceiveByte);
        testTranceiveByte++;

        TEST_ASSERT_NOT_EQUAL_UINT8_MESSAGE(0xFF, testReceiveByte, "No data was received from SPI Master");

        if (i > 1) { // from second iteration onwards
            TEST_ASSERT_EQUAL_UINT8_MESSAGE(expectedReceiveByte, testReceiveByte, "SPI Slave PingPong byte transfer failed");
        }
    }
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(spi_connected2_slavepingpong) {
    spi_connected2_slavepingpong_suite_setup();

    RUN_TEST_CASE(spi_connected2_slavepingpong, test_ping_pong_transfer_byte);

    spi_connected2_slavepingpong_suite_teardown();
}
