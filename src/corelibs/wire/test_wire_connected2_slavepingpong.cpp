// std includes

// test includes
#include "test_common_includes.h"

// project includes
#include <Wire.h>

// defines
#define TRACE_OUTPUT

// variables
const static uint8_t slaveAddress = 8U;
const static uint8_t slaveIncrement = 10U;
const static uint8_t masterIncrement = 1U;

const static uint8_t globalQuantityMax = 20U;

static uint8_t globalQuantity = 8U;

static uint8_t masterData[globalQuantityMax] = {0};

static TwoWire *master = nullptr;


// Method invoked before a test suite is run.
static void wire_connected2_slavepingpong_suite_setup() {
    master = &Wire;

    master->begin();
}

// Method invoked after a test suite is run.
static void wire_connected2_slavepingpong_suite_teardown() { master->end(); }

// define test group name
TEST_GROUP(wire_connected2_slavepingpong);
TEST_GROUP(wire_connected2_slavepingpong_internal);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(wire_connected2_slavepingpong_internal) {
    for (uint8_t count = 0; count < globalQuantity; ++count) {
        masterData[count] = 0;
    }
}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(wire_connected2_slavepingpong_internal) {}

static bool masterSend(const uint8_t *data, const uint8_t quantity) {
    master->beginTransmission(slaveAddress);

    TEST_ASSERT_EQUAL_UINT8(quantity, master->write(data, quantity));
    TEST_ASSERT_EQUAL_UINT8(0U, master->endTransmission()); // TODO: Fails from time to time !

    return true;
}

static bool masterReceive(uint8_t *data, uint8_t &quantity) {
    uint8_t count = 0;
    uint8_t locQuantity = quantity;

    TEST_ASSERT_EQUAL_UINT8(quantity, master->requestFrom(slaveAddress, quantity));
    TEST_ASSERT_EQUAL_UINT8(quantity, master->available());

    while (master->available() > 0) {
        uint8_t peekValue = master->peek();
        data[count] = master->read();

        TEST_ASSERT_EQUAL_UINT8(peekValue, data[count]);

        ++count;
    }

    TEST_ASSERT_EQUAL_UINT8(quantity, count);

    quantity = count;

    return quantity == locQuantity;
}

static void IIC_pingPong(uint8_t quantity, uint8_t iterations) {
    globalQuantity = quantity;

    uint8_t masterDataCopy[quantity];

    for (uint8_t loop = 0; loop < iterations; ++loop) {
        TEST_ASSERT_TRUE(masterSend(masterData, quantity));

#ifdef TRACE_OUTPUT

        printArray("\nMaster send", masterData, quantity);

#endif

        for (uint8_t i = 0; i < quantity; ++i) {
            masterDataCopy[i] = masterData[i] + slaveIncrement;
        }

        TEST_ASSERT_TRUE(masterReceive(masterData, quantity));

#ifdef TRACE_OUTPUT

        printArray("\nMaster received", masterData, quantity);

#endif

        TEST_ASSERT_EQUAL_UINT8_ARRAY(masterDataCopy, masterData, quantity);

        for (uint8_t i = 0; i < quantity; ++i) {
            masterData[i] += masterIncrement;
        }
    }
}

TEST_IFX(wire_connected2_slavepingpong_internal, checkPingPong) {
    // TODO: Loop over different amounts of data, clock frequencies and other parameters !
    // Must set variable globalQuantity identically in slave and master ! This can be done by
    // communicating that value from master to slave, thereby allowing for a more dynamic behaviour.
    // IIC_pingPong(1, 15);
    // IIC_pingPong(2, 15);
    IIC_pingPong(8, 15);
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
