/**
 * @brief Single-board sanity test for the BLE stack lifecycle
 * (BLE.begin() / BLE.poll() / BLE.end()).
 *
 * @details This is a happy-path smoke test only: it verifies that the BLE
 * stack can be initialized, that poll() can be called repeatedly without
 * error while the stack is up, and that the stack can be cleanly
 * de-initialized. It requires a single BLE-capable board (CY8CPROTO-063-BLE
 * running this repo's BLE library, or an Arduino board running ArduinoBLE)
 * and does not need a peer device. lastError() and the "second begin() must
 * fail" behavior are specific to this repo's BLE library and are skipped on
 * ArduinoBLE, which has no last-error accessor and allows begin() again
 * while already active.
 */
#include "test_common_includes.h"

#if defined(ARDUINO_ARCH_RENESAS)
#include <ArduinoBLE.h>
#else
#include <BLE.h>
#endif

TEST_GROUP(bluetooth_stack_single);

static TEST_SETUP(bluetooth_stack_single)
{
}

static TEST_TEAR_DOWN(bluetooth_stack_single)
{
    /* Make sure a failed test does not leave the stack running for the
     * next test case. */
    BLE.end();
}

TEST_IFX(bluetooth_stack_single, begin_poll_end)
{
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");
#if !defined(ARDUINO_ARCH_RENESAS)
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NONE, BLE.lastError(),
                               "BLE.lastError() should be BLE_ERROR_NONE after a successful begin()");
#endif

    /* poll() must be callable repeatedly, from the sketch's task, without
     * crashing or blocking, while the stack is running. */
    unsigned long start = millis();
    while ((millis() - start) < 250) {
        BLE.poll();
        delay(10);
    }

    BLE.end();
#if !defined(ARDUINO_ARCH_RENESAS)
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NONE, BLE.lastError(),
                               "BLE.lastError() should be BLE_ERROR_NONE after a successful end()");
#endif
}

TEST_IFX(bluetooth_stack_single, begin_twice_fails)
{
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");

#if defined(ARDUINO_ARCH_RENESAS)
    /* ArduinoBLE has no re-init guard: a second begin() while already
     * active re-runs HCI init and is expected to succeed, not fail. */
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() should succeed again while already active");
#else
    /* A second begin() while already active must fail, not crash or
     * silently re-initialize, and must report a matching last error. */
    TEST_ASSERT_FALSE_MESSAGE(BLE.begin(), "BLE.begin() should fail when already active");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_ALREADY_INITIALIZED, BLE.lastError(),
                               "BLE.lastError() should report BLE_ERROR_ALREADY_INITIALIZED");
#endif

    BLE.end();
}

TEST_GROUP_RUNNER(bluetooth_stack_single)
{
    RUN_TEST_CASE(bluetooth_stack_single, begin_poll_end);
    RUN_TEST_CASE(bluetooth_stack_single, begin_twice_fails);
}
