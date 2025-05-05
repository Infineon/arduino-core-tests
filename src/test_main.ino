// test includes
#include "test_common_includes.h"


/***
     * Uses flags / defines to determine which groups of tests should be added to this test such that multiple tests CAN be executed in a single run.
     * Use the '--build-property option of the 'arduino_compile' target to add the flags by defining the respective targets in the makefile.
     * makefile : --build-property "compiler.cpp.extra_flags=\"-D<TEST_SPECIFIER>=1\"" build
*/
void RunAllTests(void)
{

#ifdef TEST_TIME_SINGLE

    RUN_TEST_GROUP(time_single);

#endif

#ifdef TEST_DIGITALIO_SINGLE

    RUN_TEST_GROUP(digitalio_single);

#endif

#ifdef TEST_ANALOGIO_ADC

    RUN_TEST_GROUP(analogio_adc);

#endif

#ifdef TEST_ANALOGIO_PWM

    RUN_TEST_GROUP(analogio_pwm);

#endif

#ifdef TEST_INTERRUPTS_SINGLE

    RUN_TEST_GROUP(gpio_interrupts_single);

#endif

#ifdef TEST_UART_CONNECTED2_TX

    RUN_TEST_GROUP(uart_connected2_tx);

#endif


#ifdef TEST_UART_CONNECTED2_RX

    RUN_TEST_GROUP(uart_connected2_rx);

#endif

#ifdef TEST_UART_RX

    RUN_TEST_GROUP(uart_rx);

#endif

#ifdef TEST_UART_TX

    RUN_TEST_GROUP(uart_tx);

#endif

// CAN\test_can_single.cpp
#ifdef TEST_CAN_SINGLE

    RUN_TEST_GROUP(can_single);

#endif

// CAN\test_can_connected2_node1.cpp
#ifdef TEST_CAN_CONNECTED2_NODE1

    RUN_TEST_GROUP(can_connected2_node1);

#endif

// CAN with 2 boards connections node 2, receiver
#ifdef TEST_CAN_CONNECTED2_NODE2

    RUN_TEST_GROUP(can_connected2_node2);

#endif


// IIC with connections
#ifdef TEST_WIRE_CONNECTED1_PINGPONG

    RUN_TEST_GROUP(wire_connected1_pingpong);

#endif


// IIC with connections
#ifdef TEST_WIRE_CONNECTED2_MASTERPINGPONG

    RUN_TEST_GROUP(wire_connected2_masterpingpong);

#endif


// IIC with connections
#ifdef TEST_WIRE_CONNECTED2_SLAVEPINGPONG

    RUN_TEST_GROUP(wire_connected2_slavepingpong);

#endif

#ifdef TEST_WIFI_STA

    RUN_TEST_GROUP(wifi_sta);

#endif

#ifdef TEST_WIFI_AP

    RUN_TEST_GROUP(wifi_ap);

#endif

#ifdef TEST_WIFI_CLIENT

    RUN_TEST_GROUP(wifi_client);

#endif

#ifdef TEST_WIFI_SERVER

    RUN_TEST_GROUP(wifi_server);

#endif


#ifdef TEST_WIFI_EXTRAS

    RUN_TEST_GROUP(wifi_extras);

#endif

#ifdef TEST_WIFI_EXCEPTIONS

    RUN_TEST_GROUP(wifi_exceptions);

#endif

#ifdef TEST_WIFI_UDP_SERVER

    RUN_TEST_GROUP(wifi_udp_server);

#endif

#ifdef TEST_WIFI_UDP_CLIENT

    RUN_TEST_GROUP(wifi_udp_client);

#endif

#ifdef TEST_SPI_CONNECTED1_LOOPBACK

    RUN_TEST_GROUP(spi_connected1_loopback);

#endif

#ifdef TEST_SPI_CONNECTED2_MASTERPINGPONG

    RUN_TEST_GROUP(spi_connected2_masterpingpong);

#endif

#ifdef TEST_SPI_CONNECTED2_SLAVEPINGPONG

    RUN_TEST_GROUP(spi_connected2_slavepingpong);

#endif

}


void setup() {
    Serial.begin(115200);
    Serial.println("setup done.");
}


void loop() {
    Serial.println("\n");

    const int   argc       = 2;
    const char *argv[argc] = { "", "-v" };

    (void) UnityMain(argc, argv, RunAllTests);
    delay(3000);
}