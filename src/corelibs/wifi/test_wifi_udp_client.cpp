#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiUdp.h>

TEST_GROUP(wifi_udp_client);

static TEST_SETUP(wifi_udp_client) {
    // Setup code for the client
}

static TEST_TEAR_DOWN(wifi_udp_client) {
    // Teardown code for the client
}

WiFiUDP udpClient;

TEST_IFX(wifi_udp_client, wifi_connect_to_ap) {
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_GROUP_RUNNER(wifi_udp_client) {
    RUN_TEST_CASE(wifi_udp_client, wifi_connect_to_ap);
}