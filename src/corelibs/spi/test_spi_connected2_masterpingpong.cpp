// std includes

// test includes
#include "test_common_includes.h"

// project includes
#include <SPI.h>

const uint8_t MAX_BUFFER = 10;
const uint8_t MAX_TEST_ITERATION = MAX_BUFFER;

// variables
static SPIClassPSOC *spi_master = nullptr;
static uint8_t testTranceiveBuff[MAX_BUFFER] = {0};
static uint8_t expectedReceiveData[MAX_BUFFER] = {0};


// Method invoked before a test suite is run.
static void spi_connected2_masterpingpong_suite_setup() {
    
    pinMode(TEST_SPI_SLAVE_SELECT, OUTPUT);
    pinMode(TEST_SYNC_INPUT_OUTPUT, OUTPUT);
    digitalWrite(TEST_SPI_SLAVE_SELECT, HIGH);
    digitalWrite(TEST_SYNC_INPUT_OUTPUT, LOW);

    spi_master = &SPI;
    spi_master->begin();
}

// Method invoked after a test suite is run.
static void spi_connected2_masterpingpong_suite_teardown() {
    digitalWrite(TEST_SPI_SLAVE_SELECT, HIGH);
    digitalWrite(TEST_SYNC_INPUT_OUTPUT, LOW);
    spi_master->end();
}

// Test group definition
TEST_GROUP(spi_connected2_masterpingpong);
TEST_SETUP(spi_connected2_masterpingpong) {}
TEST_TEAR_DOWN(spi_connected2_masterpingpong) {}


/**
 * @brief    This test transfers number of bytes specified by MAX_TEST_ITERATION to slave.
 * 
 * @details  The first byte received from slave is dummy and ignored.
 *           From the second byte onwards, the slave increments its previously
 *           transmitted byte from slave and sends it to the master.
 *           The master increments the previously transmitted byte from master
 *           and sends it to the slave.
 */
TEST_IFX(spi_connected2_masterpingpong, test_ping_pong_transfer_byte) {

    uint8_t testTranceiveByte = 0;
    uint8_t testReceiveByte = 0;
    uint8_t expectedReceiveByte = 0;

    digitalWrite(TEST_SYNC_INPUT_OUTPUT, HIGH);

    digitalWrite(TEST_SPI_SLAVE_SELECT, LOW);
    testReceiveByte =  spi_master->transfer(testTranceiveByte); // first byte received is dummy and ignored
    digitalWrite(TEST_SPI_SLAVE_SELECT, HIGH);

    for (uint8_t i = 1; i < MAX_TEST_ITERATION; i++) {
        expectedReceiveByte = testReceiveByte + 1; // slave increments the previously transmitted byte from slave
        testTranceiveByte++;
        
        digitalWrite(TEST_SPI_SLAVE_SELECT, LOW);
        testReceiveByte = spi_master->transfer(testTranceiveByte);
        digitalWrite(TEST_SPI_SLAVE_SELECT, HIGH);

        TEST_ASSERT_NOT_EQUAL_UINT8_MESSAGE(0xFF, testReceiveByte, "No data was received from SPI Slave");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(expectedReceiveByte, testReceiveByte, "SPI Master PingPong transfer byte failed");
    }

    digitalWrite(TEST_SYNC_INPUT_OUTPUT, LOW);
    

}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(spi_connected2_masterpingpong) {
    spi_connected2_masterpingpong_suite_setup();

    RUN_TEST_CASE(spi_connected2_masterpingpong, test_ping_pong_transfer_byte);

    spi_connected2_masterpingpong_suite_teardown();
}
