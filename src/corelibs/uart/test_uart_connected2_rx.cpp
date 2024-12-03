// std includes

// test includes
#include "test_common_includes.h"
#include <stdint.h>

// project includes

// defines
#define TRACE_OUTPUT

const static uint8_t globalQuantityMax = 20U;
const static uint8_t dataLengths[] = {2, 4, 8, 12};
const static uint8_t dataLengthsCount = sizeof(dataLengths) / sizeof(dataLengths[0]);

volatile static uint8_t sendData[globalQuantityMax] = {0};
volatile static uint8_t receivedData[globalQuantityMax] = {0};

static HardwareSerial *uart = nullptr;

// Method invoked before a test suite is run.
static void uart_connected2_rx_suite_setup() {
    uart = &Serial1;
}

// Method invoked after a test suite is run.
static void uart_connected2_rx_suite_teardown() {
    uart->end(); // End UART communication
}

// define test group name
TEST_GROUP(uart_connected2_rx);
TEST_GROUP(uart_connected2_rx_internal);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(uart_connected2_rx_internal) {
    for (uint8_t count = 0; count < globalQuantityMax; ++count) {
        receivedData[count] = 0;
    }
}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(uart_connected2_rx_internal) {
    
}

bool uartReceive(uint8_t *data, uint8_t quantity, unsigned long timeout = 1000) {
    unsigned long startMillis = millis();
    uint8_t count = 0;
    while (count < quantity) {
        // Check for timeout
        if ((millis() - startMillis) >= timeout) {
            return false; // Timeout occurred
        }

        // Wait for data to be available
        if (uart->available() > 0) {
            data[count] = uart->read();
            ++count;
        }
    }
    return count == quantity;
}

bool uartSend(const uint8_t *data, const uint8_t quantity) {
    uint8_t bytesSent = uart->write(data, quantity);
    return bytesSent == quantity;
}

void checkPingPongForLength(uint8_t length) {
    // Receive data from master
    uint8_t bytesToReceive = length;
    bool result_mid = uartReceive((uint8_t*)receivedData, bytesToReceive);
   
#ifdef TRACE_OUTPUT
    printArray("\nSlave received", receivedData, bytesToReceive);
#endif
 TEST_ASSERT_TRUE(result_mid); 

    // Send the received data back to master
    TEST_ASSERT_TRUE(uartSend((const uint8_t*)receivedData, bytesToReceive));  

#ifdef TRACE_OUTPUT
    printArray("\nSlave send", receivedData, bytesToReceive);
#endif
 // End UART communication
}


void checkPingPongForBaudRate (uint32_t baudRate){
        uart->begin(baudRate);
        
        // Wait for handshake signal
        while (!uart->available() || uart->read() != 'H') {
            delay(100);
        }
        uart->write('A');

        for (uint8_t lengthIndex = 0; lengthIndex < dataLengthsCount; ++lengthIndex) {
            checkPingPongForLength(dataLengths[lengthIndex]);
        }
        delay(1000);
        uart->end();
}

TEST_IFX(uart_connected2_rx_internal, checkPingPong_baudRate_9600) {
    checkPingPongForBaudRate(9600);
}
TEST_IFX(uart_connected2_rx_internal, checkPingPong_baudRate_115200){
    checkPingPongForBaudRate(115200);
}

static TEST_GROUP_RUNNER(uart_connected2_rx_internal) {
    RUN_TEST_CASE(uart_connected2_rx_internal, checkPingPong_baudRate_9600);
    RUN_TEST_CASE(uart_connected2_rx_internal, checkPingPong_baudRate_115200);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(uart_connected2_rx) {
    uart_connected2_rx_suite_setup();
    RUN_TEST_GROUP(uart_connected2_rx_internal);
    uart_connected2_rx_suite_teardown();
}