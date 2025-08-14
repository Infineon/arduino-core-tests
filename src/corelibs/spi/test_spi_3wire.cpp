/* test_spi_3wire.cpp
 *
 * This test is used to verify if the TLE5012 sensor is responding using 3-wire SPI communication.
 */

// std includes
#include <tlx5012-arduino.hpp>
#include "test_common_includes.h"

// Use the tle5012 namespace
using namespace tle5012;

// TLE5012 sensor instance and variables
static Tle5012Ino Tle5012Sensor = Tle5012Ino();
static errorTypes checkError;

// Test group definition
TEST_GROUP(spi_3wire);

TEST_SETUP(spi_3wire) {
    // Initialize the TLE5012 sensor

    checkError = Tle5012Sensor.begin();
}

TEST_TEAR_DOWN(spi_3wire) {
    // Cleanup if necessary
}

// Test case: Verify if the sensor is responding
TEST(spi_3wire, test_sensor_response) {
    TEST_ASSERT_EQUAL(tle5012::NO_ERROR, checkError); // Check if initialization was successful
}


// Test case: Request data from the sensor using 3-wire SPI
TEST(spi_3wire, test_3wire_request) {
    double command = 0.0; // Example command to request data from the sensor
    int response = 0;

    // Use the sendReceiveSPI function to perform 3-wire SPI communication
    response = Tle5012Sensor.getAngleValue(command);
    
    // Verify the response (this is an example; adjust based on expected behavior)
    TEST_ASSERT_NOT_EQUAL(1, response); // Ensure the response is not an error
    TEST_ASSERT_TRUE(response >= -180.0 && response <= 180.0); // Ensure the angle is within a valid range
}

// Test case: Request temperature data from the sensor
TEST(spi_3wire, test_temperature_request) {
    double temperature = 0.0;
    int result = 0;

    // Use the getTemperature function to request temperature data from the sensor
    result = Tle5012Sensor.getTemperature(temperature);

    Serial.print(temperature);
    // Verify the response
    TEST_ASSERT_EQUAL(tle5012::NO_ERROR, result); // Ensure the request was successful
    TEST_ASSERT_TRUE(temperature >= -40.0 && temperature <= 150.0); // Ensure the temperature is within a valid range
}

// Define test runner for the SPI 3-wire test group
TEST_GROUP_RUNNER(spi_3wire) {
    RUN_TEST_CASE(spi_3wire, test_sensor_response);
    RUN_TEST_CASE(spi_3wire, test_3wire_request);
    RUN_TEST_CASE(spi_3wire, test_temperature_request);
}