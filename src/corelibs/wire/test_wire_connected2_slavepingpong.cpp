// std includes

// test includes
#include "test_common_includes.h"

// project includes
#include <Wire.h>

// defines

// variables
const static uint8_t slaveAddress = 8U;
const static uint8_t slaveIncrement = 10U;

const static uint8_t globalQuantityMax = 20U;

static uint8_t globalQuantity = 8U;

volatile static uint8_t slaveData[globalQuantityMax] = {0};

volatile static uint8_t howManyCopy[globalQuantityMax] = {0};
volatile static uint8_t availCopy[globalQuantityMax] = {0};
volatile static uint8_t peekCopy[globalQuantityMax] = {0};

volatile static uint8_t bytesWrittenCopy = 0;

static TwoWire *slave = nullptr;

static void slaveReceiveEventIIC(int howMany) {
    uint8_t count = 0;

    while (slave->available() > 0) {
        howManyCopy[count] = howMany;
        availCopy[count] = slave->available();
        peekCopy[count] = slave->peek();
        slaveData[count] = slave->read();
        ++count;
    }
}

static void slaveRequestEventIIC() {
    uint8_t count = 0;

    while (count < globalQuantity) {
        slaveData[count] += slaveIncrement;
        ++count;
    }

    bytesWrittenCopy = slave->write((const uint8_t *)slaveData, globalQuantity);
}

// Method invoked before a test suite is run.
static void wire_connected2_slavepingpong_suite_setup() {
    slave = &Wire;

    slave->begin(slaveAddress);
    slave->onReceive(slaveReceiveEventIIC);
    slave->onRequest(slaveRequestEventIIC);
}

// Method invoked after a test suite is run.
static void wire_connected2_slavepingpong_suite_teardown() {
    // slave->end(); // TODO: Why can end() not be called here without disrupting the execution ?
}

// define test group name
TEST_GROUP(wire_connected2_slavepingpong);
TEST_GROUP(wire_connected2_slavepingpong_internal);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(wire_connected2_slavepingpong_internal) {}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(wire_connected2_slavepingpong_internal) {}

TEST_IFX(wire_connected2_slavepingpong_internal, checkPingPong) {
#ifdef TRACE_OUTPUT

    printArray("\nslaveData", slaveData, globalQuantity);
    printArray("howManyCopy", howManyCopy, globalQuantity);
    printArray("availCopy", availCopy, globalQuantity);
    printArray("peekCopy", peekCopy, globalQuantity);
    Serial.flush();

    Serial.print("bytesWrittenCopy : ");
    Serial.println(bytesWrittenCopy);
    Serial.flush();

#endif

    for (uint8_t i = 0; i < globalQuantity; ++i) {
        TEST_ASSERT_EQUAL_UINT8(globalQuantity, howManyCopy[i]);
        TEST_ASSERT_EQUAL_UINT8(availCopy[i], globalQuantity - i);

        peekCopy[i] += slaveIncrement;
    }

    TEST_ASSERT_EQUAL_UINT8_ARRAY(slaveData, peekCopy, globalQuantity);

    TEST_ASSERT_EQUAL_UINT8(globalQuantity, bytesWrittenCopy);
}

static TEST_GROUP_RUNNER(wire_connected2_slavepingpong_internal) {
    RUN_TEST_CASE(wire_connected2_slavepingpong_internal, checkPingPong);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(wire_connected2_slavepingpong) {
    wire_connected2_slavepingpong_suite_setup();

    RUN_TEST_GROUP(wire_connected2_slavepingpong_internal);

    wire_connected2_slavepingpong_suite_teardown();
}
