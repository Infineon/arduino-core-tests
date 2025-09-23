/* test_onewire_DS18x20.cpp
 *
 * This test verifies if the OneWire sensor (e.g. in this case: DS18B20) is responding and providing valid data.
 * It includes tests for device presence, chip description, and data retrieval.
 */

// std includes
#include <OneWire.h>
#include "test_common_includes.h"

// OneWire instance and variables
static OneWire oneWire(TEST_PIN_ONEWIRE);
static uint8_t addr[8];
static uint8_t data[12];

// Test group definition
TEST_GROUP(onewire_DS18x20);

TEST_SETUP(onewire_DS18x20) {
    // Reset the OneWire bus
    oneWire.reset_search();
}

TEST_TEAR_DOWN(onewire_DS18x20) {
    // Cleanup if necessary
}

// Test case: Verify if a device is present on the OneWire bus
TEST(onewire_DS18x20, test_device_presence) {
    bool deviceFound = oneWire.search(addr);

    TEST_ASSERT_TRUE_MESSAGE(deviceFound, " - No device found on the OneWire bus");

    // Verify the CRC of the device address
    TEST_ASSERT_TRUE_MESSAGE(OneWire::crc8(addr, 7) == addr[7], " - Invalid CRC for device address");
}

// Test case: Verify if a chip description is returned
TEST(onewire_DS18x20, test_device_description) {
    bool deviceFound = oneWire.search(addr);

    TEST_ASSERT_TRUE_MESSAGE(deviceFound, " - No device found on the OneWire bus");

    TEST_ASSERT_TRUE_MESSAGE(
        (addr[0] == 0x28|| addr[0] == 0x10 || addr[0] == 0x22),
        " - No valid chip description found. Expected DS18B20 (0x28) or DS18S20 (0x10) or DS1822 (0x22)");
}

// Test case: Verify if data was returned (basic functionality)
TEST(onewire_DS18x20, test_data_returned) {
    bool deviceFound = oneWire.search(addr);
    TEST_ASSERT_TRUE_MESSAGE(deviceFound, " - No device found on the OneWire bus");

    // Start temperature conversion
    oneWire.reset();
    oneWire.select(addr);
    oneWire.write(0x44, 1); // Start conversion with parasite power on at the end

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
    bool allDataNonZero = true;
    for (uint8_t i = 0; i < 9; i++) {
        if (data[i] == 0) {
            allDataNonZero = false;
            break;
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(allDataNonZero, " - Not all data bytes are non-zero in the OneWire device scratchpad");

    TEST_ASSERT_TRUE_MESSAGE(OneWire::crc8(data, 8) == data[8], " - CRC check failed");

    // Decode temperature
    int16_t raw = (data[1] << 8) | data[0];
    if(addr[0] == 0x10) {
        raw = raw << 3; // 9 bit resolution
        if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } else{
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit
    }

    float celsius = (float)raw / 16.0;
    // Assert temperature is within expected range
    TEST_ASSERT_TRUE_MESSAGE(
    celsius > -50.0 && celsius < 50.0,
    " - Temperature out of expected range (-50°C to 50°C)"
    );
}

// Define test runner for the OneWire sensor test group
TEST_GROUP_RUNNER(onewire_DS18x20) {
    RUN_TEST_CASE(onewire_DS18x20, test_device_presence);
    RUN_TEST_CASE(onewire_DS18x20, test_device_description);
    RUN_TEST_CASE(onewire_DS18x20, test_data_returned);
}