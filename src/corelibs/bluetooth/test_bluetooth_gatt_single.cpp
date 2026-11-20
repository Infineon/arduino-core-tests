/**
 * @brief Single-board coverage of the BLE GATT object model
 * (BLEService / BLECharacteristic / BLEDescriptor / BLEDevice) and the
 * BLEClass error paths that are not otherwise exercised by
 * test_bluetooth_single.cpp or the two-board connected tests.
 *
 * @details These tests only touch the local, in-memory data model and
 * BLEClass bookkeeping (service/characteristic capacity limits, default
 * (unconnected/undiscovered) state, and calling API methods that require
 * begin() before it has been called). They require a single BLE-capable
 * board (CY8CPROTO-063-BLE running this repo's BLE library, or an Arduino
 * board running ArduinoBLE) and do not need a peer device or an active
 * radio connection. Anything specific to this repo's BLE library
 * (BLE_ERROR_* codes, MAX_CHARACTERISTICS/MAX_SERVICES limits, BLEDescriptor)
 * is skipped on ArduinoBLE, which does not expose those.
 */
#include "test_common_includes.h"

#if defined(ARDUINO_ARCH_RENESAS)
#include <ArduinoBLE.h>
#else
#include <BLE.h>
#endif

TEST_GROUP(bluetooth_gatt_single);

static TEST_SETUP(bluetooth_gatt_single)
{
}

static TEST_TEAR_DOWN(bluetooth_gatt_single)
{
    /* Make sure a failed test does not leave the stack running for the
     * next test case. */
    BLE.end();
}

TEST_IFX(bluetooth_gatt_single, characteristicConstructionAndValueAccess)
{
    static const uint8_t kInitial[] = {0x01U, 0x02U, 0x03U};
    BLECharacteristic characteristic("2A37", BLERead | BLEWrite | BLENotify, sizeof(kInitial));

    TEST_ASSERT_EQUAL_STRING_MESSAGE("2A37", characteristic.uuid(), "uuid() should return the constructor's UUID");
    TEST_ASSERT_EQUAL_MESSAGE(BLERead | BLEWrite | BLENotify, characteristic.properties(),
                               "properties() should return the constructor's property bits");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(kInitial), characteristic.valueSize(),
                               "valueSize() should return the constructor's valueSize");
    TEST_ASSERT_EQUAL_MESSAGE(0, characteristic.valueLength(),
                               "A freshly constructed characteristic should have no value bytes stored");
    TEST_ASSERT_FALSE_MESSAGE(characteristic.isRemote(), "A locally-declared characteristic should not be remote");
    TEST_ASSERT_FALSE_MESSAGE(characteristic.written(), "written() should be false before any write occurs");
    TEST_ASSERT_FALSE_MESSAGE(characteristic.subscribed(), "subscribed() should be false before any subscribe occurs");

    TEST_ASSERT_TRUE_MESSAGE(characteristic.writeValue(kInitial, sizeof(kInitial)), "writeValue() should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(kInitial), characteristic.valueLength(),
                               "valueLength() should match the number of bytes written");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(kInitial, characteristic.value(), sizeof(kInitial),
                                           "value() should return the bytes most recently written");

    /* A write larger than valueSize() must fail and leave the value unchanged. */
    uint8_t oversized[sizeof(kInitial) + 1] = {0};
    TEST_ASSERT_FALSE_MESSAGE(characteristic.writeValue(oversized, sizeof(oversized)),
                               "writeValue() with length > valueSize() should fail");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(kInitial, characteristic.value(), sizeof(kInitial),
                                           "A failed oversized writeValue() must not change the stored value");

    /* String overload. */
    TEST_ASSERT_TRUE_MESSAGE(characteristic.writeValue("hi"), "writeValue(const char*) should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(2, characteristic.valueLength(), "valueLength() should match the string length written");

    /* readValue() on a local (non-remote) characteristic copies from the local buffer. */
    uint8_t readBuf[8] = {0};
    int copied = characteristic.readValue(readBuf, sizeof(readBuf));
    TEST_ASSERT_EQUAL_MESSAGE(2, copied, "readValue() should return the number of bytes copied");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE('h', readBuf[0], "readValue() should copy the current value bytes");
}

TEST_IFX(bluetooth_gatt_single, serviceAddCharacteristicAndLookup)
{
    BLEService service("180D");
    BLECharacteristic characteristicA("2A37", BLERead);
    BLECharacteristic characteristicB("2A38", BLEWrite);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("180D", service.uuid(), "uuid() should return the constructor's UUID");
    TEST_ASSERT_EQUAL_MESSAGE(0, service.characteristicCount(), "A freshly constructed service should have no characteristics");
    TEST_ASSERT_NULL_MESSAGE(service.characteristic(0), "characteristic(index) should be nullptr when out of range");
    TEST_ASSERT_NULL_MESSAGE(service.characteristic("2A37"), "characteristic(uuid) should be nullptr when not yet added");

    TEST_ASSERT_TRUE_MESSAGE(service.addCharacteristic(characteristicA), "addCharacteristic() should succeed");
    TEST_ASSERT_TRUE_MESSAGE(service.addCharacteristic(characteristicB), "addCharacteristic() should succeed");
    TEST_ASSERT_EQUAL_MESSAGE(2, service.characteristicCount(), "characteristicCount() should reflect the added characteristics");

    TEST_ASSERT_EQUAL_MESSAGE(&characteristicA, service.characteristic(0),
                               "characteristic(0) should return the first characteristic added");
    TEST_ASSERT_EQUAL_MESSAGE(&characteristicB, service.characteristic("2A38"),
                               "characteristic(uuid) should find a characteristic by UUID");
    /* UUID lookup is documented as case-insensitive. */
    TEST_ASSERT_EQUAL_MESSAGE(&characteristicB, service.characteristic("2a38"),
                               "characteristic(uuid) lookup should be case-insensitive");
}

#if !defined(ARDUINO_ARCH_RENESAS)
TEST_IFX(bluetooth_gatt_single, serviceRejectsCharacteristicsBeyondCapacity)
{
    /* BLECharacteristic's copy constructor is deleted, so these must be
     * declared as separate named objects (not a copy-initialized array)
     * and referenced through pointers. */
    BLEService service("180D");
    BLECharacteristic c0("2A01", BLERead);
    BLECharacteristic c1("2A02", BLERead);
    BLECharacteristic c2("2A03", BLERead);
    BLECharacteristic c3("2A04", BLERead);
    BLECharacteristic c4("2A05", BLERead);
    BLECharacteristic c5("2A06", BLERead);
    BLECharacteristic c6("2A07", BLERead);
    BLECharacteristic c7("2A08", BLERead);
    BLECharacteristic c8("2A09", BLERead);
    BLECharacteristic *characteristics[] = {&c0, &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8};
    static_assert(sizeof(characteristics) / sizeof(characteristics[0]) == BLEService::MAX_CHARACTERISTICS + 1,
                  "This test needs exactly one more characteristic than the service can hold");

    for (int i = 0; i < BLEService::MAX_CHARACTERISTICS; ++i) {
        TEST_ASSERT_TRUE_MESSAGE(service.addCharacteristic(*characteristics[i]),
                                  "addCharacteristic() should succeed up to MAX_CHARACTERISTICS");
    }
    TEST_ASSERT_FALSE_MESSAGE(service.addCharacteristic(*characteristics[BLEService::MAX_CHARACTERISTICS]),
                               "addCharacteristic() should fail once MAX_CHARACTERISTICS is reached");
    TEST_ASSERT_EQUAL_MESSAGE(BLEService::MAX_CHARACTERISTICS, service.characteristicCount(),
                               "characteristicCount() should not grow past MAX_CHARACTERISTICS");
}

TEST_IFX(bluetooth_gatt_single, descriptorConstructionAndValueAccess)
{
    static const uint8_t kCccdInitial[] = {0x00U, 0x00U};
    BLEDescriptor descriptor("2902", kCccdInitial, sizeof(kCccdInitial));

    TEST_ASSERT_EQUAL_STRING_MESSAGE("2902", descriptor.uuid(), "uuid() should return the constructor's UUID");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(kCccdInitial), descriptor.valueLength(),
                               "valueLength() should match the constructor's valueLength");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(kCccdInitial, descriptor.value(), sizeof(kCccdInitial),
                                           "value() should return the constructor's initial value bytes");
}
#endif /* !defined(ARDUINO_ARCH_RENESAS) */

TEST_IFX(bluetooth_gatt_single, defaultConstructedDeviceIsInvalid)
{
    BLEDevice device;

    TEST_ASSERT_FALSE_MESSAGE(device.hasAddress(), "A default-constructed BLEDevice should have no address");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", device.address(), "address() should be empty for a default-constructed BLEDevice");
    TEST_ASSERT_FALSE_MESSAGE(device.hasLocalName(), "A default-constructed BLEDevice should have no local name");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", device.localName(),
                                      "localName() should be empty for a default-constructed BLEDevice");
    TEST_ASSERT_EQUAL_MESSAGE(0, device.rssi(), "rssi() should be 0 for a default-constructed BLEDevice");
    TEST_ASSERT_EQUAL_MESSAGE(0, device.advertisedServiceUuidCount(),
                               "advertisedServiceUuidCount() should be 0 for a default-constructed BLEDevice");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("", device.advertisedServiceUuid(0),
                                     "advertisedServiceUuid(index) should be empty when no UUIDs are available");
    TEST_ASSERT_FALSE_MESSAGE(device.connected(), "A default-constructed BLEDevice should not be connected");
    TEST_ASSERT_FALSE_MESSAGE(device.hasAdvertisedServiceUuid("180D"),
                               "hasAdvertisedServiceUuid() should be false for a default-constructed BLEDevice");

    /* disconnect() on a peer that isn't the current connection is documented
     * as a safe no-op that returns true. */
    TEST_ASSERT_TRUE_MESSAGE(device.disconnect(), "disconnect() on an unconnected/invalid device should be a no-op success");

    /* connect() on a device with no address must fail, not crash. */
    TEST_ASSERT_FALSE_MESSAGE(device.connect(), "connect() on a device with no address should fail");
}

TEST_IFX(bluetooth_gatt_single, classDefaultsBeforeBegin)
{
    /* These queries must be safe to call even before/without BLE.begin(). */
    TEST_ASSERT_FALSE_MESSAGE(BLE.connected(), "BLE.connected() should be false before begin()");
    TEST_ASSERT_FALSE_MESSAGE(BLE.central().hasAddress(), "BLE.central() should be an invalid device before begin()");
    TEST_ASSERT_FALSE_MESSAGE(BLE.available().hasAddress(), "BLE.available() should be an invalid device before begin()");

    /* Safe no-ops even when never started. */
    BLE.stopAdvertise();
    BLE.stopScan();
    BLE.poll();
    BLE.end();
}

#if !defined(ARDUINO_ARCH_RENESAS)
TEST_IFX(bluetooth_gatt_single, apiCallsBeforeBeginFailWithNotInitialized)
{
    BLEService service("180D");

    TEST_ASSERT_FALSE_MESSAGE(BLE.advertise("PSOC6BluetoothTest"), "advertise() should fail before begin()");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NOT_INITIALIZED, BLE.lastError(),
                               "lastError() should report BLE_ERROR_NOT_INITIALIZED after advertise() before begin()");

    TEST_ASSERT_FALSE_MESSAGE(BLE.scan(), "scan() should fail before begin()");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NOT_INITIALIZED, BLE.lastError(),
                               "lastError() should report BLE_ERROR_NOT_INITIALIZED after scan() before begin()");
}

TEST_IFX(bluetooth_gatt_single, addServiceRejectsBeyondCapacity)
{
    /* BLEClass::MAX_SERVICES is private (unlike BLEService::MAX_CHARACTERISTICS),
     * so this local constant must be kept in sync with its value in BLE.h.
     * BLEService's copy constructor is also deleted, so these must be
     * declared as separate named objects (not a copy-initialized array)
     * and referenced through pointers. */
    static const int kMaxBleServices = 4;

    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");

    BLEService s0("1801");
    BLEService s1("1802");
    BLEService s2("1803");
    BLEService s3("1804");
    BLEService s4("1805");
    BLEService *services[] = {&s0, &s1, &s2, &s3, &s4};
    static_assert(sizeof(services) / sizeof(services[0]) == kMaxBleServices + 1,
                  "This test needs exactly one more service than BLEClass can hold");

    for (int i = 0; i < kMaxBleServices; ++i) {
        TEST_ASSERT_TRUE_MESSAGE(BLE.addService(*services[i]), "addService() should succeed up to MAX_SERVICES");
    }
    TEST_ASSERT_FALSE_MESSAGE(BLE.addService(*services[kMaxBleServices]),
                               "addService() should fail once MAX_SERVICES is reached");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_TOO_MANY_SERVICES, BLE.lastError(),
                               "lastError() should report BLE_ERROR_TOO_MANY_SERVICES");

    BLE.end();
}
#endif /* !defined(ARDUINO_ARCH_RENESAS) */

TEST_GROUP_RUNNER(bluetooth_gatt_single)
{
    RUN_TEST_CASE(bluetooth_gatt_single, characteristicConstructionAndValueAccess);
    RUN_TEST_CASE(bluetooth_gatt_single, serviceAddCharacteristicAndLookup);
#if !defined(ARDUINO_ARCH_RENESAS)
    RUN_TEST_CASE(bluetooth_gatt_single, serviceRejectsCharacteristicsBeyondCapacity);
    RUN_TEST_CASE(bluetooth_gatt_single, descriptorConstructionAndValueAccess);
#endif
    RUN_TEST_CASE(bluetooth_gatt_single, defaultConstructedDeviceIsInvalid);
    RUN_TEST_CASE(bluetooth_gatt_single, classDefaultsBeforeBegin);
#if !defined(ARDUINO_ARCH_RENESAS)
    RUN_TEST_CASE(bluetooth_gatt_single, apiCallsBeforeBeginFailWithNotInitialized);
    RUN_TEST_CASE(bluetooth_gatt_single, addServiceRejectsBeyondCapacity);
#endif
}
