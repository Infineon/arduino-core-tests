// std includes

// test includes
#include "test_common_includes.h"

// project includes

// defines
#define TRACE_OUTPUT

// Baud rate array
const static uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
const static uint8_t baudRatesCount = sizeof(baudRates) / sizeof(baudRates[0]);

const static uint8_t globalQuantityMax = 20U;
const static uint8_t dataLengths[] = {2, 4, 8, 12};
const static uint8_t dataLengthsCount = sizeof(dataLengths) / sizeof(dataLengths[0]);

volatile static uint8_t sendData[globalQuantityMax] = {0};
volatile static uint8_t receivedData[globalQuantityMax] = {0};

static HardwareSerial *uart = nullptr;

// Method invoked before a test suite is run.
static void uart_connected2_tx_suite_setup() {
    uart = &Serial1;
}

// Method invoked after a test suite is run.
static void uart_connected2_tx_suite_teardown() {
    uart->end(); // End UART communication
}

// define test group name
TEST_GROUP(uart_connected2_tx);
TEST_GROUP(uart_connected2_tx_internal);

// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(uart_connected2_tx_internal) {
}

// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(uart_connected2_tx_internal) {
}

bool uartSend(const uint8_t *data, const uint8_t quantity) {
    uint8_t bytesSent = uart->write(data, quantity);
    return bytesSent == quantity;
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

void checkPingPongForLength( uint8_t length) {

    // Initialize sendData
    for (uint8_t count = 0; count < length; ++count) {
        sendData[count] = count;
    }

#ifdef TRACE_OUTPUT
    printArray("\nMaster send", sendData, length);
#endif

    // Send data to slave
    TEST_ASSERT_TRUE(uartSend((const uint8_t*)sendData, length));  

    // Wait and receive data back from slave
    //delay(100); // Wait for the slave to process and respond
    uint8_t bytesToReceive = length;
    TEST_ASSERT_TRUE(uartReceive((uint8_t*)receivedData, bytesToReceive)); 

#ifdef TRACE_OUTPUT
    printArray("\nMaster received", receivedData, bytesToReceive);
#endif

    TEST_ASSERT_EQUAL_UINT8(bytesToReceive, length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)sendData, (const uint8_t*)receivedData, bytesToReceive);  // Explicitly cast to const uint8_t*


}

void checkPingPongForBaudRate (uint32_t baudRate){
        uart->begin(baudRate);

        // Wait for the slave's acknowledgment signal
        while (!uart->available() || uart->read() != 'A') {
            uart->write('H');
            Serial.println("Handshaking send");
            delay(1000);
        }
        for (uint8_t lengthIndex = 0; lengthIndex < dataLengthsCount; ++lengthIndex) {
            checkPingPongForLength(dataLengths[lengthIndex]);
        }
        uart->end();
    }

TEST_IFX(uart_connected2_tx_internal, checkPingPong_baudRate_9600) {
     checkPingPongForBaudRate(9600);
    }

TEST_IFX(uart_connected2_tx_internal, checkPingPong_baudRate_115200) {
    checkPingPongForBaudRate(115200);
    }

// Test case to check all serial functions at different baud rates
void checkAllSerialFunctionsForBaudRate(uint32_t baudRate) {
    uart->begin(baudRate);
    
    // Clear UART buffer after initializing UART
    while (uart->available()) {
        uart->read();
    }

        // Wait for the slave's acknowledgment signal
    while (!uart->available() || uart->read() != 'A') {
            uart->write('H');
            Serial.println("Handshaking send");
            delay(1000);
    }

    while (uart->available()) {
        uart->read();
    }
    // Test available, availableForWrite, peek, read, write, and flush functions
    TEST_ASSERT_EQUAL(0, uart->available());
    TEST_ASSERT_GREATER_THAN(0, uart->availableForWrite());
    TEST_ASSERT_EQUAL(-1, uart->peek());
    TEST_ASSERT_EQUAL(-1, uart->read());
    uart->flush();

    // Wait for the slave's acknowledgment signal
    while (!uart->available() || uart->read() != 'A') {
            uart->write('H');
            Serial.println("Handshaking send");
            delay(1000);
    }
    TEST_ASSERT_EQUAL(1, uart->write('A'));
    TEST_ASSERT_EQUAL('A', uart->peek());
    TEST_ASSERT_EQUAL(1, uart->available());
    TEST_ASSERT_EQUAL('A', uart->read());
    uart->end();
}

TEST_IFX(uart_connected2_tx_internal, checkAllSerialFunctions_baudRate_9600) {
    checkAllSerialFunctionsForBaudRate(9600);
}

TEST_IFX(uart_connected2_tx_internal, checkAllSerialFunctions_baudRate_115200) {
    checkAllSerialFunctionsForBaudRate(115200);
}

static TEST_GROUP_RUNNER(uart_connected2_tx_internal) {
    RUN_TEST_CASE(uart_connected2_tx_internal, checkPingPong_baudRate_9600);
    RUN_TEST_CASE(uart_connected2_tx_internal, checkPingPong_baudRate_115200);
    RUN_TEST_CASE(uart_connected2_tx_internal, checkAllSerialFunctions_baudRate_9600);
    RUN_TEST_CASE(uart_connected2_tx_internal, checkAllSerialFunctions_baudRate_115200);
}

// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(uart_connected2_tx) {
    uart_connected2_tx_suite_setup();
    RUN_TEST_GROUP(uart_connected2_tx_internal);
    uart_connected2_tx_suite_teardown();
}