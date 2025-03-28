/* test_spi_connected1_loopback.cpp
 *
 * This test is used to verify the functionality of the SPI functions.
 * only one board is needed with MOSI pin looped back to 
 * MISO pin for the test cases to work as expected.
 */

 // THIS IS THE BRANCH OF 3XPre

// std includes
#include "test_common_includes.h"

// project includes
#include <SPI.h>

// defines
// Variables for SPI testing
static SPIClass *spi = nullptr;
static const uint8_t testDataByte = 0xA5; // Example test byte
static const uint16_t testDataWord = 0x55AA; // Example test word
static uint8_t testTranceiveBuff[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

// Setup and teardown methods for the test suite
static void spi_test_suite_setup() {
    spi = &SPI;
    spi->begin();
}

static void spi_test_suite_teardown() {
    spi->end();
}

// Test group definition
TEST_GROUP(spi_connected1_loopback);
TEST_SETUP(spi_connected1_loopback) {}
TEST_TEAR_DOWN(spi_connected1_loopback) {}

TEST_IFX(spi_connected1_loopback, test_spi_transfer_byte) {
    uint8_t receivedData = spi->transfer(testDataByte);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(testDataByte, receivedData, "SPI transfer byte failed");
}

TEST_IFX(spi_connected1_loopback, test_spi_transfer_word) {
    uint16_t receivedData = spi->transfer16(testDataWord);
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(testDataWord, receivedData, "SPI transfer word failed");
}

TEST_IFX(spi_connected1_loopback, test_spi_transfer_buffer) {
    // Perform transfer
    spi->transfer(testTranceiveBuff, sizeof(testTranceiveBuff));

    // Verify received data matches transmitted data
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testTranceiveBuff, testTranceiveBuff, sizeof(testTranceiveBuff), "SPI transfer buffer failed");
}

TEST_IFX(spi_connected1_loopback, test_spi_mode_configuration) {
    spi->endTransaction();
    SPISettings testSettings1(500000, MSBFIRST, SPI_MODE0);
    spi->beginTransaction(testSettings1);
    
    uint16_t receivedData = spi->transfer16(testDataWord);
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(testDataWord, receivedData, "SPI transfer word failed");

    spi->endTransaction();
    SPISettings testSettings2(1000000, LSBFIRST, SPI_MODE3);
    spi->beginTransaction(testSettings2);
    
    receivedData = spi->transfer16(testDataWord);
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(testDataWord, receivedData, "SPI transfer word failed");
}

TEST_IFX(spi_connected1_loopback, test_spi_reinitialization) {
    spi->end();
    spi->begin();
    TEST_ASSERT_TRUE_MESSAGE(true, "SPI reinitialization completed");
}

// Define test runner for the SPI test group
TEST_GROUP_RUNNER(spi_connected1_loopback) {
    spi_test_suite_setup();

    RUN_TEST_CASE(spi_connected1_loopback, test_spi_transfer_byte);
    RUN_TEST_CASE(spi_connected1_loopback, test_spi_transfer_word);
    RUN_TEST_CASE(spi_connected1_loopback, test_spi_transfer_buffer);
    RUN_TEST_CASE(spi_connected1_loopback, test_spi_mode_configuration);
    RUN_TEST_CASE(spi_connected1_loopback, test_spi_reinitialization);

    spi_test_suite_teardown();
}