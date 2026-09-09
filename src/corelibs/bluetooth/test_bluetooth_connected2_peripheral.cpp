/* test_bluetooth_connected2_peripheral.cpp
 *
 * Two-board hardware test for the BLE library, peripheral role.
 * ONLY WORKS WHEN THE OTHER BOARD HAS BEEN FLASHED
 * test_bluetooth_connected2_central.cpp.
 *
 * Advertises a GATT service with a writable "command" characteristic and a
 * read/notify "counter" characteristic, then walks through the flows
 * exercised by the central side: connection, GATT discovery (implicit,
 * performed by the central), a characteristic write detected on this side,
 * a characteristic read served to the central, and a notification sent to
 * a subscribed central.
 *
 * Modeled after the small-test-case-per-API-call style used by the WiFi
 * connected tests (see test_wifi_ap.cpp / test_wifi_sta.cpp): each step of
 * the interaction with the central is its own TEST_IFX case sharing state
 * (via file-scope statics) with the steps around it, run in order by
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
static const uint8_t kExpectedCommandValue = 0x01U;

static const unsigned long kConnectTimeoutMs = 30000UL;
static const unsigned long kWriteTimeoutMs = 15000UL;
static const unsigned long kSubscribeTimeoutMs = 15000UL;
static const unsigned long kUnsubscribeTimeoutMs = 15000UL;

static BLEService gattService(kServiceUuid);
static BLECharacteristic commandCharacteristic(kCommandCharUuid, BLERead | BLEWrite);
static BLECharacteristic counterCharacteristic(kCounterCharUuid, BLERead | BLENotify);
static bool gattServiceConfigured = false;

// Shared across test cases: the central discovered/connected to over the course of the group.
static BLEDevice central;
static uint8_t counterValue = kInitialCounterValue;

// define test group name
TEST_GROUP(bluetooth_connected2_peripheral);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(bluetooth_connected2_peripheral) {}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(bluetooth_connected2_peripheral) {}

// Exercises: BLE.begin().
TEST_IFX(bluetooth_connected2_peripheral, begin) {
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");
}

// Exercises: BLEService::addCharacteristic(), BLE.addService().
TEST_IFX(bluetooth_connected2_peripheral, buildAndRegisterGattService) {
    if (!gattServiceConfigured) {
        TEST_ASSERT_TRUE_MESSAGE(gattService.addCharacteristic(commandCharacteristic),
                                  "addCharacteristic() for the command characteristic failed");
        TEST_ASSERT_TRUE_MESSAGE(gattService.addCharacteristic(counterCharacteristic),
                                  "addCharacteristic() for the counter characteristic failed");
        gattServiceConfigured = true;
    }
    TEST_ASSERT_EQUAL_MESSAGE(2, gattService.characteristicCount(),
                               "Service should have both the command and counter characteristics");

    TEST_ASSERT_TRUE_MESSAGE(BLE.addService(gattService), "BLE.addService() failed");
}

// Exercises: BLECharacteristic::writeValue() on locally-declared characteristics.
TEST_IFX(bluetooth_connected2_peripheral, writeInitialCharacteristicValues) {
    uint8_t commandInit = 0;
    counterValue = kInitialCounterValue;

    TEST_ASSERT_TRUE_MESSAGE(commandCharacteristic.writeValue(&commandInit, sizeof(commandInit)),
                              "writeValue() on command characteristic failed");
    TEST_ASSERT_TRUE_MESSAGE(counterCharacteristic.writeValue(&counterValue, sizeof(counterValue)),
                              "writeValue() on counter characteristic failed");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(kInitialCounterValue, counterCharacteristic.value()[0],
                                     "Counter characteristic value should match what was just written");
}

// Exercises: BLE.advertise() (peripheral advertises its service).
TEST_IFX(bluetooth_connected2_peripheral, advertise) {
    TEST_ASSERT_TRUE_MESSAGE(BLE.advertise("PSOC6BluetoothTest", gattService.uuid()),
                              "BLE.advertise() failed");
}

// Exercises: BLE.poll(), BLE.central(), BLEDevice::hasAddress(), BLEDevice::connected()
// (central discovers and connects to us).
TEST_IFX(bluetooth_connected2_peripheral, waitForCentralConnection) {
    unsigned long start = millis();
    while ((millis() - start) < kConnectTimeoutMs) {
        BLE.poll();
        central = BLE.central();
        if (central.hasAddress()) {
            break;
        }
        delay(10);
    }
    TEST_ASSERT_TRUE_MESSAGE(central.hasAddress(), "No central connected within timeout");

#ifdef TRACE_OUTPUT
    Serial.print("Connected to central: ");
    Serial.println(central.address());
#endif

    TEST_ASSERT_TRUE_MESSAGE(central.connected(), "central.connected() should be true right after connecting");
}

// Exercises: BLECharacteristic::written(), BLECharacteristic::value()
// (central writes a value read by the peripheral).
TEST_IFX(bluetooth_connected2_peripheral, waitForCommandWrite) {
    unsigned long start = millis();
    bool commandWritten = false;
    while ((millis() - start) < kWriteTimeoutMs) {
        BLE.poll();
        if (commandCharacteristic.written()) {
            commandWritten = true;
            break;
        }
        delay(10);
    }
    TEST_ASSERT_TRUE_MESSAGE(commandWritten, "Command characteristic was not written by central within timeout");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(kExpectedCommandValue, commandCharacteristic.value()[0],
                                     "Command characteristic value did not match what the central wrote");
}

// Exercises: BLECharacteristic::subscribed() becoming true (central subscribes to notifications).
TEST_IFX(bluetooth_connected2_peripheral, waitForSubscribe) {
    unsigned long start = millis();
    bool subscribed = false;
    while ((millis() - start) < kSubscribeTimeoutMs) {
        BLE.poll();
        if (counterCharacteristic.subscribed()) {
            subscribed = true;
            break;
        }
        delay(10);
    }
    TEST_ASSERT_TRUE_MESSAGE(subscribed, "Central did not subscribe to the counter characteristic within timeout");
}

// Exercises: BLECharacteristic::writeValue() triggering a notification to a subscribed central.
TEST_IFX(bluetooth_connected2_peripheral, sendCounterNotification) {
    ++counterValue;
    TEST_ASSERT_TRUE_MESSAGE(counterCharacteristic.writeValue(&counterValue, sizeof(counterValue)),
                              "writeValue() on counter characteristic failed");

#ifdef TRACE_OUTPUT
    Serial.print("Notified counter: ");
    Serial.println(counterValue);
#endif

    TEST_ASSERT_TRUE_MESSAGE(central.connected(), "Central disconnected before the notification was sent");
}

// Exercises: BLECharacteristic::subscribed() becoming false (central unsubscribes).
TEST_IFX(bluetooth_connected2_peripheral, waitForUnsubscribe) {
    unsigned long start = millis();
    while ((millis() - start) < kUnsubscribeTimeoutMs) {
        BLE.poll();
        if (!counterCharacteristic.subscribed()) {
            break;
        }
        delay(10);
    }
    TEST_ASSERT_FALSE_MESSAGE(counterCharacteristic.subscribed(),
                              "Central unsubscribe was not reflected by subscribed()");
}

// Exercises: BLE.stopAdvertise(), BLEDevice::disconnect() (peripheral side), BLE.end().
TEST_IFX(bluetooth_connected2_peripheral, stopAdvertiseAndEnd) {
    BLE.stopAdvertise();

    if (central.connected()) {
        central.disconnect();
    }

    BLE.end();
}

static TEST_GROUP_RUNNER(bluetooth_connected2_peripheral) {
    RUN_TEST_CASE(bluetooth_connected2_peripheral, begin);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, buildAndRegisterGattService);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, writeInitialCharacteristicValues);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, advertise);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, waitForCentralConnection);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, waitForCommandWrite);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, waitForSubscribe);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, sendCounterNotification);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, waitForUnsubscribe);
    RUN_TEST_CASE(bluetooth_connected2_peripheral, stopAdvertiseAndEnd);
}
