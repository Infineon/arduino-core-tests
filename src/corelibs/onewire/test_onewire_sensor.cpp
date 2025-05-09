/* test_onewire_sensor.cpp
 *
 * This test verifies if the OneWire sensor (e.g. in this case: DS18B20) is responding and providing valid data.
 * It includes tests for device presence, chip description, and data retrieval.
 */

// std includes
#include <OneWire.h>
#include "test_common_includes.h"

// OneWire instance and variables
static OneWire oneWire(5); // Pin 5 is used for the OneWire bus
static uint8_t addr[8];
static uint8_t data[12];

// Test group definition
TEST_GROUP(onewire_sensor);

TEST_SETUP(onewire_sensor) {
    // Reset the OneWire bus
    oneWire.reset_search();
}

TEST_TEAR_DOWN(onewire_sensor) {
    // Cleanup if necessary
}

// Test case: Verify if a device is present on the OneWire bus
TEST(onewire_sensor, test_device_presence) {
    bool deviceFound = oneWire.search(addr);

    // Print result and pass/fail message
    if (deviceFound) {
        Serial.println(" - Device Found");
    } else {
        TEST_FAIL_MESSAGE(" - No device found on the OneWire bus.");
    }

    // Verify the CRC of the device address
    TEST_ASSERT_TRUE_MESSAGE(OneWire::crc8(addr, 7) == addr[7], " - Invalid CRC for device address");
}

// Test case: Verify if a chip description is returned
TEST(onewire_sensor, test_device_description) {
    bool deviceFound = oneWire.search(addr);
    TEST_ASSERT_TRUE_MESSAGE(deviceFound, " - No device found on the OneWire bus");

    // Check if the first byte of the address corresponds to a known chip family
    if (addr[0] == 0x10 || addr[0] == 0x28 || addr[0] == 0x22) {
        Serial.println(" - Chip description found.");
    } else {
        TEST_FAIL_MESSAGE(" - No valid chip description found.");
    }
}

// Test case: Verify if data was returned (basic functionality)
TEST(onewire_sensor, test_data_returned) {
    bool deviceFound = oneWire.search(addr);
    TEST_ASSERT_TRUE_MESSAGE(deviceFound, " - No device found on the OneWire bus");

    // Start temperature conversion
    oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0x44, 1); // Start conversion with parasite power on

    // Wait for conversion to complete
    delay(1000);

    // Read the scratchpad
    oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0xBE); // Read Scratchpad

    for (uint8_t i = 0; i < 9; i++) {
        data[i] = oneWire.read();
    }

    // Verify that data was returned (non-zero values in the scratchpad)
    bool dataReturned = false;
    for (uint8_t i = 0; i < 9; i++) {
        if (data[i] != 0) {
            dataReturned = true;
            break;
        }
    }

    // Print result and pass/fail message
    if (dataReturned) {
        Serial.println(" - Data received");
    } else {
        TEST_FAIL_MESSAGE(" - No valid data returned from the sensor.");
    }
}

// Define test runner for the OneWire sensor test group
TEST_GROUP_RUNNER(onewire_sensor) {
    RUN_TEST_CASE(onewire_sensor, test_device_presence);
    RUN_TEST_CASE(onewire_sensor, test_device_description);
    RUN_TEST_CASE(onewire_sensor, test_data_returned);
}