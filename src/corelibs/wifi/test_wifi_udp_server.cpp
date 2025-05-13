#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiUdp.h>

TEST_GROUP(wifi_udp_server);

static TEST_SETUP(wifi_udp_server) {
    // Setup code for the server
}

static TEST_TEAR_DOWN(wifi_udp_server) {
    // Teardown code for the server
}

WiFiUDP udpServer;
Socket socket;

TEST_IFX(wifi_udp_server, begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, result);
}

TEST_IFX(wifi_udp_server, udp_begin) {
    uint16_t port = 1234;
    TEST_ASSERT_EQUAL_INT(SOCKET_STATUS_BOUND, udpServer.begin(port)); // Bind to port 1234
    Serial.print("UDP server listening on port: ");
    Serial.println(port);
}

TEST_GROUP_RUNNER(wifi_udp_server) {
    RUN_TEST_CASE(wifi_udp_server, begin_ap);
    RUN_TEST_CASE(wifi_udp_server, udp_begin);
    while (true) {}; // Keep the server running
}