/* test_can_connected2_node2.cpp 
 *
 * This test is used to verify the functionality of the CAN library.
 * It will test the can communcation between two nodes. 
 * 2 boards must be used and connected the can_tx, can_rx, vcc and ground pins. (termination resistor is optional)
 * This board work as a receiver.
 *
 */

// std includes

// test includes
#include "test_common_includes.h"

// project includes

// defines
#define TRACE_OUTPUT
#define CAN_ID_1 0x123
#define CAN_ID_2 0x321

// variables

const static uint8_t node1Increment = 10;
const static uint8_t node2Increment = 1;
const static uint8_t canDataLengthMax = 8;

static uint8_t canDataLength = canDataLengthMax;
static uint8_t node2Data[canDataLengthMax] = {0};
static uint8_t receivedData[canDataLengthMax] = {0};

volatile bool newDataReceivedNode2 = false;

// test feature includes requiring the above defined variables

extern CANXMC CAN;

void receiveEventNode2(int packetSize) {
    uint8_t count = 0;
    while (CAN.available()) {
        receivedData[count++] = CAN.read();
    }
    newDataReceivedNode2 = true;
    canDataLength = packetSize;
}

// Method invoked before a test suite is run.
void can_connected_node2_suite_setup() {
    CAN.begin();
    CAN.filter(CAN_ID_1, 0x7FF); // Set filter to receive messages with CAN_ID_1
    CAN.onReceive(receiveEventNode2);
}

// Method invoked after a test suite is run.
void can_connected_node2_suite_teardown() { CAN.end(); }

// define test group name
TEST_GROUP(can_connected2_node2);
TEST_GROUP(can_connected2_node2_internal);

void processReceivedMessagesNode2() {
    if (newDataReceivedNode2) {
        // Process the received data
        for (uint8_t i = 0; i < canDataLength; ++i) {
            node2Data[i] = receivedData[i] + node2Increment;
        }

        // Send processed data back to Node1
        TEST_ASSERT_TRUE(CAN.beginPacket(CAN_ID_2));
        for (uint8_t i = 0; i < canDataLength; ++i) {
            TEST_ASSERT_EQUAL(1, CAN.write(node2Data[i]));
        }
        TEST_ASSERT_EQUAL(1, CAN.endPacket());

        // Clear flag
        newDataReceivedNode2 = false;

#ifdef TRACE_OUTPUT
        printArray("\nReceived Data", receivedData, canDataLength);
        printArray("Sent Data", node2Data, canDataLength);
#endif
    }
}

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(can_connected2_node2_internal) {}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(can_connected2_node2_internal) {}

TEST_IFX(can_connected2_node2_internal, checkPingPong) { processReceivedMessagesNode2(); }

static TEST_GROUP_RUNNER(can_connected2_node2_internal) {
    RUN_TEST_CASE(can_connected2_node2_internal, checkPingPong);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(can_connected2_node2) {
    can_connected_node2_suite_setup();

    RUN_TEST_GROUP(can_connected2_node2_internal);

    can_connected_node2_suite_teardown();
}
