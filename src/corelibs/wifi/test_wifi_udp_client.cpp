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

TEST_IFX(wifi_udp_client, udp_beginpacket) {
    IPAddress ip(127, 0, 0, 1);
    uint16_t port = 80;
    TEST_ASSERT_TRUE(udpClient.beginPacket(ip, port)); // Start a packet to send
}

TEST_IFX(wifi_udp_client, udp_write){
    const char *message = "Hello, UDP!";
    size_t bytes_written = udpClient.write(message[0]); // Write a single byte
    TEST_ASSERT_EQUAL_INT(1, bytes_written); // Check if all bytes were written

    bytes_written = udpClient.write((const uint8_t *)message, strlen(message));
    TEST_ASSERT_EQUAL_INT(strlen(message), bytes_written); // Check if all bytes were written
}

TEST_GROUP_RUNNER(wifi_udp_client) {
    RUN_TEST_CASE(wifi_udp_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_udp_client, udp_beginpacket);
    RUN_TEST_CASE(wifi_udp_client, udp_write);
    while (true) {}; // Keep the client running
}