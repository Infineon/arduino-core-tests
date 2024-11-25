/* test_can_single.cpp
 *
 * This test is used to verify the functionality of the CAN library.
 * only one board is needed with out any other connections.
 */

// std includes

// test includes
#include "test_common_includes.h"

// project includes

// defines
#define TRACE_OUTPUT

// variables

// Method invoked by Unity before a test suite is run
void CAN_suiteSetUp() {
}

// Method invoked by Unity after a test suite is run
void CAN_suiteTearDown() {
}

// variables used in the tests below that have to be accessed in the setup and tear down methods
extern CANXMC CAN;

// test includes that may require dut

// define test group name
TEST_GROUP(can_single);
TEST_GROUP(can_single_internal);

// Setup method called before every individual test defined for this test group
static TEST_SETUP(can_single_internal) {
}

// Tear down method called before every individual test defined for this test group
static TEST_TEAR_DOWN(can_single_internal) {
}

// Functionality not - yet - supported
TEST_IFX(can_single_internal, checkUnsupportedFunctionality) {
    TEST_ASSERT_FALSE(CAN.observe());
    // TEST_ASSERT_TRUE(CAN.packetRtr() ); Remote frame not tested
}


// Test case for CAN_msg_tx initialization
TEST_IFX(can_single_internal, testCanMsgTxInitialization)  {
    // Get the Tx message object
    XMC_CAN_MO_t *txMessage = CAN.getTxMessage();

    // Verify that the Tx message object is correctly initialized
    TEST_ASSERT_NOT_NULL(txMessage);
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_FRAME_TYPE_STANDARD_11BITS,
                             txMessage->can_id_mode); // Standard 11-bit identifier
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_ARBITRATION_MODE_ORDER_BASED_PRIO_1, txMessage->can_priority);
    TEST_ASSERT_EQUAL_UINT32(0x1FFFFFFFU, txMessage->can_id_mask);
    TEST_ASSERT_EQUAL_UINT32(0U, txMessage->can_ide_mask); // send both standard and extended frames
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_MO_TYPE_TRANSMSGOBJ, txMessage->can_mo_type);
}

// Test case for CAN_msg_rx initialization
TEST_IFX(can_single_internal, test_CAN_msg_rx_initialization) {
    // Get the Rx message object
    XMC_CAN_MO_t *rxMessage = CAN.getRxMessage();

    // Verify that the Rx message object is correctly initialized
    TEST_ASSERT_NOT_NULL(rxMessage);
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_FRAME_TYPE_STANDARD_11BITS,
                             rxMessage->can_id_mode); // Standard 11-bit identifier
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_ARBITRATION_MODE_ORDER_BASED_PRIO_1, rxMessage->can_priority);
    TEST_ASSERT_EQUAL_UINT32(0x000, rxMessage->can_id_mask);
    TEST_ASSERT_EQUAL_UINT32(0U,
                             rxMessage->can_ide_mask); // receive both standard and extended frames
    TEST_ASSERT_EQUAL_UINT32(XMC_CAN_MO_TYPE_RECMSGOBJ, rxMessage->can_mo_type);
}

// Test case for CAN begin function with different baud rates
TEST_IFX(can_single_internal, test_CAN_initialization_with_baudrate_125k) {
    int result = CAN.begin(125000);
    TEST_ASSERT_EQUAL(1, result);
}

TEST_IFX(can_single_internal, test_CAN_initialization_with_baudrate_250k) {
    int result = CAN.begin(250000);
    TEST_ASSERT_EQUAL(1, result);
}

TEST_IFX(can_single_internal, test_CAN_initialization_with_baudrate_500k) {
    int result = CAN.begin(500000);
    TEST_ASSERT_EQUAL(1, result);
}

TEST_IFX(can_single_internal, test_CAN_initialization_with_baudrate_1M) {
    int result = CAN.begin(1000000);
    TEST_ASSERT_EQUAL(1, result);
}

TEST_IFX(can_single_internal, test_CAN_beginPacket) {
    int result = CAN.beginPacket(0x123); // Assuming 0x123 is the identifier
    TEST_ASSERT_EQUAL(1, result);
}

TEST_IFX(can_single_internal, test_CAN_write) {
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    size_t result = CAN.write(data, sizeof(data));
    TEST_ASSERT_EQUAL(sizeof(data), result);
}

TEST_IFX(can_single_internal, test_CAN_endPacket) {
    int result = CAN.endPacket();
    TEST_ASSERT_EQUAL(1, result);
}

TEST_GROUP_RUNNER(can_single_internal) {
    RUN_TEST_CASE(can_single_internal, checkUnsupportedFunctionality);
    RUN_TEST_CASE(can_single_internal, testCanMsgTxInitialization);
    RUN_TEST_CASE(can_single_internal, test_CAN_msg_rx_initialization);
    RUN_TEST_CASE(can_single_internal, test_CAN_initialization_with_baudrate_125k);
    RUN_TEST_CASE(can_single_internal, test_CAN_initialization_with_baudrate_250k);
    RUN_TEST_CASE(can_single_internal, test_CAN_initialization_with_baudrate_1M);
    RUN_TEST_CASE(can_single_internal, test_CAN_initialization_with_baudrate_500k);
    RUN_TEST_CASE(can_single_internal, test_CAN_beginPacket);
    RUN_TEST_CASE(can_single_internal, test_CAN_write);
    RUN_TEST_CASE(can_single_internal, test_CAN_endPacket);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(can_single) {
    CAN_suiteSetUp();
    
    RUN_TEST_GROUP(can_single_internal);

    CAN_suiteTearDown();
}
