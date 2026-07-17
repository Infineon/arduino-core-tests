/**
 * @brief Single-board sanity test for the BLE library scaffold and stack
 * lifecycle (BLE.begin() / BLE.poll() / BLE.end()).
 *
 * @details This is a happy-path smoke test only: it verifies that the BLE
 * stack can be initialized, that poll() can be called repeatedly without
 * error while the stack is up, and that the stack can be cleanly
 * de-initialized. It requires a single BLE-capable PSOC6 board
 * (CY8CPROTO-063-BLE) and does not need a peer device.
 */
#include "test_common_includes.h"

#include <BLE.h>

TEST_GROUP(bluetooth_single);

static TEST_SETUP(bluetooth_single)
{
}

static TEST_TEAR_DOWN(bluetooth_single)
{
    /* Make sure a failed test does not leave the stack running for the
     * next test case. */
    BLE.end();
}

TEST_IFX(bluetooth_single, begin_poll_end)
{
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NONE, BLE.lastError(),
                               "BLE.lastError() should be BLE_ERROR_NONE after a successful begin()");

    /* poll() must be callable repeatedly, from the sketch's task, without
     * crashing or blocking, while the stack is running. */
    unsigned long start = millis();
    while ((millis() - start) < 250) {
        BLE.poll();
        delay(10);
    }

    BLE.end();
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_NONE, BLE.lastError(),
                               "BLE.lastError() should be BLE_ERROR_NONE after a successful end()");
}

TEST_IFX(bluetooth_single, begin_twice_fails)
{
    TEST_ASSERT_TRUE_MESSAGE(BLE.begin(), "BLE.begin() failed");

    /* A second begin() while already active must fail, not crash or
     * silently re-initialize, and must report a matching last error. */
    TEST_ASSERT_FALSE_MESSAGE(BLE.begin(), "BLE.begin() should fail when already active");
    TEST_ASSERT_EQUAL_MESSAGE(BLE_ERROR_ALREADY_INITIALIZED, BLE.lastError(),
                               "BLE.lastError() should report BLE_ERROR_ALREADY_INITIALIZED");

    BLE.end();
}

TEST_GROUP_RUNNER(bluetooth_single)
{
    RUN_TEST_CASE(bluetooth_single, begin_poll_end);
    RUN_TEST_CASE(bluetooth_single, begin_twice_fails);
}
