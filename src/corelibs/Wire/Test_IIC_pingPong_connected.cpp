// std includes

// test includes
#include "Test_common_includes.h"

// project includes

// defines
#define TRACE_OUTPUT

// variables
const static uint8_t slaveAddress = 8U;
const static uint8_t slaveIncrement = 10U;
const static uint8_t masterIncrement = 1U;

const static uint8_t globalQuantityMax = 20U;

// TODO: Maximum value to request by master is 2, otherwise XMC_I2C_CH_MasterStart in line 253 of
// Wire.cpp hangs !
static uint8_t globalQuantity = 1U;

// static uint8_t slaveData[globalQuantityMax]  = { 0 };
static uint8_t masterData[globalQuantityMax] = {0};

volatile static uint8_t slaveData[globalQuantityMax] = {0};

volatile static uint8_t howManyCopy[globalQuantityMax] = {0};
volatile static uint8_t availCopy[globalQuantityMax] = {0};
volatile static uint8_t peekCopy[globalQuantityMax] = {0};

static TwoWire *master = nullptr;
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

    (void)slave->write((const uint8_t *)slaveData, globalQuantity);
}

// Method invoked before a test suite is run.
static void IIC_pingPong_connected_suiteSetup() {
    master = &Wire;
    master->begin();

    slave = &Wire1;
    slave->begin(slaveAddress);
    slave->onReceive(slaveReceiveEventIIC);
    slave->onRequest(slaveRequestEventIIC);
}

// Method invoked after a test suite is run.
static void IIC_pingPong_connected_suiteTearDown() {
    master->end();
    slave->end();
}

// define test group name
TEST_GROUP(IIC_pingPong_connected);
TEST_GROUP(IIC_pingPong_connectedInternal);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(IIC_pingPong_connectedInternal) {
    for (uint8_t count = 0; count < globalQuantity; ++count) {
        masterData[count] = 0;
        slaveData[count] = 0;
    }
}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(IIC_pingPong_connectedInternal) {}

static bool masterSend(const uint8_t *data, const uint8_t quantity) {
    master->beginTransmission(slaveAddress);

    TEST_ASSERT_EQUAL_UINT8(quantity, master->write(data, quantity));
    TEST_ASSERT_EQUAL_UINT8(
        0U, master->endTransmission()); // TODO: endTransmission() fails from time to time. Why ?

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

        printArray("\nslaveData", slaveData, globalQuantity);
        printArray("howManyCopy", howManyCopy, globalQuantity);
        printArray("availCopy", availCopy, globalQuantity);
        printArray("peekCopy", peekCopy, globalQuantity);
        Serial.flush();

#endif

        for (uint8_t i = 0; i < globalQuantity; ++i) {
            TEST_ASSERT_EQUAL_UINT8(globalQuantity, howManyCopy[i]);
            TEST_ASSERT_EQUAL_UINT8(availCopy[i], globalQuantity - i);

            peekCopy[i] += slaveIncrement;
        }

        TEST_ASSERT_EQUAL_UINT8_ARRAY(slaveData, peekCopy, globalQuantity);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(masterDataCopy, masterData, quantity);

        for (uint8_t i = 0; i < quantity; ++i) {
            masterData[i] += masterIncrement;
        }
    }
}

TEST_IFX(IIC_pingPong_connectedInternal, checkPingPong) {
    // TODO: Loop over different amounts of data, clock frequencies and other parameters !
    IIC_pingPong(1, 15);
    IIC_pingPong(2, 15);

    // IIC_pingPong(8, 15); // TODO: Why does any quantity larger than 2 not work ?
}

static TEST_GROUP_RUNNER(IIC_pingPong_connectedInternal) {
    RUN_TEST_CASE(IIC_pingPong_connectedInternal, checkPingPong);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(IIC_pingPong_connected) {
    IIC_pingPong_connected_suiteSetup();

    RUN_TEST_GROUP(IIC_pingPong_connectedInternal);

    IIC_pingPong_connected_suiteTearDown();
}


