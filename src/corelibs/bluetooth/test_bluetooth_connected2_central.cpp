/* test_bluetooth_connected2_central.cpp
 *
 * Two-board hardware test for the BLE library, central role.
 * ONLY WORKS WHEN THE OTHER BOARD HAS BEEN FLASHED
 * test_bluetooth_connected2_peripheral.cpp.
 *
 * Scans for the peripheral's advertised service, connects, discovers its
 * GATT service/characteristics, writes the "command" characteristic,
 * reads back the "counter" characteristic (proving a value written by the
 * peripheral can be read), and subscribes to receive counter
 * notifications.
 *
 * Modeled after the small-test-case-per-API-call style used by the WiFi
 * connected tests (see test_wifi_ap.cpp / test_wifi_sta.cpp): each step of
 * the interaction with the peripheral is its own TEST_IFX case sharing
 * state (via file-scope statics) with the steps around it, run in order by
 * TEST_GROUP_RUNNER, so a failure at any one step is reported against that
 * specific API call rather than a single monolithic test.
 *
 * Requires two BLE-capable PSOC6 boards (CY8CPROTO-063-BLE).
 */

// std includes

// test includes
#include "test_common_includes.h"

// project includes
#include <BLE.h>

// defines
#define TRACE_OUTPUT

// variables
static const char *kServiceUuid = "1234";
static const char *kCommandCharUuid = "1235";
static const char *kCounterCharUuid = "1236";

static const uint8_t kInitialCounterValue = 0x2AU;
static const uint8_t kCommandValue = 0x01U;

static const unsigned long kScanStartTimeoutMs = 15000UL;
static const unsigned long kScanStartRetryDelayMs = 200UL;
static const unsigned long kScanTimeoutMs = 90000UL;
static const unsigned long kNotifyTimeoutMs = 30000UL;

// Shared across test cases: the peripheral discovered/connected to, and its
// discovered service/characteristics, over the course of the group.
static BLEDevice peripheral;
static BLEService *service = nullptr;
static BLECharacteristic *commandCharacteristic = nullptr;
static BLECharacteristic *counterCharacteristic = nullptr;

// define test group name
TEST_GROUP(bluetooth_connected2_central);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(bluetooth_connected2_central) {}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(bluetooth_connected2_central) {}

// Exercises: BLE.begin().
TEST_IFX(bluetooth_connected2_central, begin) {
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");
}

// Exercises: BLE.scan(). Retries on failure (e.g. the adapter can be briefly busy
// finishing teardown right after begin()) instead of a single one-shot attempt.
TEST_IFX(bluetooth_connected2_central, scan) {
    unsigned long start = millis();
    bool scanStarted = false;
    while ((millis() - start) < kScanStartTimeoutMs) {
        if (BLE.scan(kServiceUuid)) {
            scanStarted = true;
            break;
        }
        delay(kScanStartRetryDelayMs);
    }
    TEST_ASSERT_TRUE_MESSAGE(scanStarted, "BLE.scan() failed to start within timeout");
}

// Exercises: BLE.poll(), BLE.available(), BLEDevice::hasAddress()/address()/localName()/
// hasLocalName()/rssi()/advertisedServiceUuidCount()/hasAdvertisedServiceUuid(), BLE.stopScan()
// (peripheral advertises, central discovers it).
TEST_IFX(bluetooth_connected2_central, waitForAdvertisingPeripheral) {
    unsigned long start = millis();
    while ((millis() - start) < kScanTimeoutMs) {
        BLE.poll();
        peripheral = BLE.available();
        if (peripheral.hasAddress()) {
            break;
        }
        delay(10);
    }
    TEST_ASSERT_TRUE_MESSAGE(peripheral.hasAddress(), "No advertising peripheral discovered within timeout");

#ifdef TRACE_OUTPUT
    Serial.print("Discovered ");
    Serial.print(peripheral.address());
    Serial.print(" rssi=");
    Serial.println(peripheral.rssi());
#endif

    TEST_ASSERT_TRUE_MESSAGE(peripheral.hasAdvertisedServiceUuid(kServiceUuid),
                              "Discovered peripheral should advertise the expected service UUID");
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, peripheral.advertisedServiceUuidCount(),
                                      "Discovered peripheral should report at least one advertised service UUID");

    BLE.stopScan();
}

// Exercises: BLEDevice::connect(), BLEDevice::connected().
TEST_IFX(bluetooth_connected2_central, connectToPeripheral) {
#ifdef TRACE_OUTPUT
    Serial.print("Connecting to ");
    Serial.println(peripheral.address());
#endif

    TEST_ASSERT_TRUE_MESSAGE(peripheral.connect(), "peripheral.connect() failed");
    TEST_ASSERT_TRUE_MESSAGE(peripheral.connected(), "peripheral.connected() should be true right after connecting");
}

// Exercises: BLEDevice::discoverAttributes(), BLEDevice::serviceCount()/service(index).
TEST_IFX(bluetooth_connected2_central, discoverAttributes) {
    if (!peripheral.discoverAttributes()) {
        Serial.print("discoverAttributes() failed, error code: ");
        Serial.println(BLE.lastError());
        Serial.print("discovered services: ");
        Serial.println(peripheral.serviceCount());
        for (int i = 0; i < peripheral.serviceCount(); ++i) {
            BLEService *discoveredService = peripheral.service(i);
            Serial.print("service ");
            Serial.print(i);
            Serial.print(" characteristics: ");
            Serial.println(discoveredService == nullptr ? -1 : discoveredService->characteristicCount());
        }
        TEST_FAIL_MESSAGE("discoverAttributes() failed");
    }
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, peripheral.serviceCount(), "At least one service should be discovered");
}

// Exercises: BLEDevice::service(uuid), BLEService::characteristic(uuid),
// BLECharacteristic::uuid()/properties()/isRemote().
TEST_IFX(bluetooth_connected2_central, lookupServiceAndCharacteristics) {
    service = peripheral.service(kServiceUuid);
    TEST_ASSERT_NOT_NULL_MESSAGE(service, "Expected service not found on peripheral");

    commandCharacteristic = service->characteristic(kCommandCharUuid);
    counterCharacteristic = service->characteristic(kCounterCharUuid);
    TEST_ASSERT_NOT_NULL_MESSAGE(commandCharacteristic, "Command characteristic not found on peripheral");
    TEST_ASSERT_NOT_NULL_MESSAGE(counterCharacteristic, "Counter characteristic not found on peripheral");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(kCommandCharUuid, commandCharacteristic->uuid(),
                                      "Command characteristic uuid() should match the discovered UUID");
    TEST_ASSERT_TRUE_MESSAGE(commandCharacteristic->isRemote(),
                              "A characteristic discovered via discoverAttributes() should be remote");
    TEST_ASSERT_TRUE_MESSAGE((commandCharacteristic->properties() & BLEWrite) != 0,
                              "Command characteristic should report the BLEWrite property");
    TEST_ASSERT_TRUE_MESSAGE((counterCharacteristic->properties() & BLENotify) != 0,
                              "Counter characteristic should report the BLENotify property");
}

// Exercises: BLECharacteristic::readValue() (central reads a value written by the peripheral).
TEST_IFX(bluetooth_connected2_central, readCounterCharacteristic) {
    uint8_t counterReadBuf[1] = {0};
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, counterCharacteristic->readValue(counterReadBuf, sizeof(counterReadBuf)),
                                      "readValue() on counter characteristic failed");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(kInitialCounterValue, counterReadBuf[0],
                                     "Counter characteristic did not read back the peripheral's initial value");

#ifdef TRACE_OUTPUT
    Serial.print("Read back counter characteristic: ");
    Serial.println(counterReadBuf[0]);
#endif
}

// Exercises: BLECharacteristic::writeValue() (central writes a value read by the peripheral).
TEST_IFX(bluetooth_connected2_central, writeCommandCharacteristic) {
    if (!commandCharacteristic->writeValue(&kCommandValue, sizeof(kCommandValue))) {
        Serial.print("writeValue() failed, error code: ");
        Serial.println(BLE.lastError());
        TEST_FAIL_MESSAGE("writeValue() on command characteristic failed");
    }
}

// Exercises: BLECharacteristic::subscribe().
TEST_IFX(bluetooth_connected2_central, subscribeToCounter) {
    TEST_ASSERT_TRUE_MESSAGE(counterCharacteristic->subscribe(), "subscribe() on counter characteristic failed");
}

// Exercises: BLE.poll(), BLECharacteristic::valueUpdated()/value() (central receives a
// notification after subscribing).
TEST_IFX(bluetooth_connected2_central, waitForCounterNotification) {
    unsigned long start = millis();
    bool notified = false;
    while ((millis() - start) < kNotifyTimeoutMs) {
        BLE.poll();
        if (counterCharacteristic->valueUpdated()) {
            notified = true;
            break;
        }
        delay(10);
    }
    TEST_ASSERT_TRUE_MESSAGE(notified, "No counter notification received within timeout");
    TEST_ASSERT_GREATER_THAN_UINT8_MESSAGE(kInitialCounterValue, counterCharacteristic->value()[0],
                                            "Notified counter value did not increase past the initial value");

#ifdef TRACE_OUTPUT
    Serial.print("Counter notification: ");
    Serial.println(counterCharacteristic->value()[0]);
#endif
}

// Exercises: BLECharacteristic::unsubscribe().
TEST_IFX(bluetooth_connected2_central, unsubscribeFromCounter) {
    TEST_ASSERT_TRUE_MESSAGE(counterCharacteristic->unsubscribe(),
                             "unsubscribe() on counter characteristic failed");
}

// Exercises: BLEDevice::disconnect()/connected() (central side), BLE.end().
TEST_IFX(bluetooth_connected2_central, disconnectAndEnd) {
    TEST_ASSERT_TRUE_MESSAGE(peripheral.disconnect(), "peripheral.disconnect() failed");
    TEST_ASSERT_FALSE_MESSAGE(peripheral.connected(), "peripheral.connected() should be false after disconnect()");

    BLE.end();
}

TEST_GROUP_RUNNER(bluetooth_connected2_central) {
    RUN_TEST_CASE(bluetooth_connected2_central, begin);
    RUN_TEST_CASE(bluetooth_connected2_central, scan);
    RUN_TEST_CASE(bluetooth_connected2_central, waitForAdvertisingPeripheral);
    RUN_TEST_CASE(bluetooth_connected2_central, connectToPeripheral);
    RUN_TEST_CASE(bluetooth_connected2_central, discoverAttributes);
    RUN_TEST_CASE(bluetooth_connected2_central, lookupServiceAndCharacteristics);
    RUN_TEST_CASE(bluetooth_connected2_central, readCounterCharacteristic);
    RUN_TEST_CASE(bluetooth_connected2_central, writeCommandCharacteristic);
    RUN_TEST_CASE(bluetooth_connected2_central, subscribeToCounter);
    RUN_TEST_CASE(bluetooth_connected2_central, waitForCounterNotification);
    RUN_TEST_CASE(bluetooth_connected2_central, unsubscribeFromCounter);
    RUN_TEST_CASE(bluetooth_connected2_central, disconnectAndEnd);
}
