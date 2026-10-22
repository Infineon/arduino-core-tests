/**
 * @file test_wifi_udp_client.cpp
 * @brief This test creates a WiFi (UDP) Server to which a WiFi (UDP) Client can connect 
 * and communicate with testing the WiFiUDP API functions.
 * 
 * @details The tests runs the following sequence:
 * - Creates an WiFi access point to which the client connects
 * - Starts WiFiUDP socket, listening at local port PORT.
 * - Starts processing the next available incoming packet, checks for the presence of a UDP packet
 * - Reads the packet and checks if the data is correct.
 * - Receives multiple packets from different senders and checks if the data and IP are correct.
 * - Writes a packet to the client
 * - Handles packet loss by not sending a packet
 * - Joins a multicast group and receives a packet from it
 * - Stops the UDP server
 * - Disconnect from the client and stop the access point.
 * 
 * This test is paired with the "udp_client.ino" test, which needs to be executed
 * in another board to operate the client connecting to this server.
 * 
 * @note This test must be run before the "udp_client.ino" test.
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include "ottu.hpp"

WiFiUDP udpServer;

OTTUNITY_TEST(begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, result);
}

OTTUNITY_TEST(udp_begin) {
    uint16_t port = 1234;
    TEST_ASSERT_EQUAL_INT(SOCKET_STATUS_BOUND, udpServer.begin(port)); 
}

OTTUNITY_TEST(udp_parsePacket) {
    const char* expectedMessage = "Hello, UDP Server!";
    int packetSize = 0;

    // Wait for the server to receive a packet of the expected size
    while ((packetSize = udpServer.parsePacket()) != (int)strlen(expectedMessage)) {
    }
    // Verify the packet size matches the expected message length
    TEST_ASSERT_EQUAL_INT(strlen(expectedMessage), packetSize);
}

OTTUNITY_TEST(udp_receive) {
    const char* expectedMessage = "Hello, UDP Server!";
    char rcvd_msg[19] = {0};

    rcvd_msg[0] = udpServer.read(); // Read a single byte
    TEST_ASSERT_EQUAL_CHAR('H', rcvd_msg[0]); 

    // Read the rest of the message
    int read_bytes = udpServer.read((uint8_t *)rcvd_msg + 1, strlen(expectedMessage) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(expectedMessage) - 1, read_bytes); 
    TEST_ASSERT_EQUAL_STRING(expectedMessage, rcvd_msg); 
}

OTTUNITY_TEST(receive_multiple_packets) {
    const char* exp_message1 = "Hello, UDP Server!";
    const char* exp_message2 = "Hey, Guess Who am I!";
    IPAddress expectedIP1(192, 168, 0, 2);
    IPAddress expectedIP2(192, 168, 0, 5);
    char rcvd_msg[50] = {0};

    // Process the first packet
    int packetSize1 = 0;
    while ((packetSize1 = udpServer.parsePacket()) != (int)strlen(exp_message1)) {}
    TEST_ASSERT_EQUAL_INT(strlen(exp_message1), packetSize1);
    udpServer.read((uint8_t *)rcvd_msg, packetSize1);
    TEST_ASSERT_EQUAL_STRING(exp_message1, rcvd_msg);
    TEST_ASSERT_EQUAL(expectedIP1, udpServer.remoteIP());

    // Clear the buffer
    memset(rcvd_msg, 0, sizeof(rcvd_msg));

    // Process the second packet from different sender
    int packetSize2 = 0;
    while ((packetSize2 = udpServer.parsePacket()) != (int)strlen(exp_message2)) {}
    TEST_ASSERT_EQUAL_INT(strlen(exp_message2), packetSize2);
    udpServer.read((uint8_t *)rcvd_msg, packetSize2);
    TEST_ASSERT_EQUAL_STRING(exp_message2, rcvd_msg);
    TEST_ASSERT_EQUAL(expectedIP2, udpServer.remoteIP());
}

OTTUNITY_TEST(udp_server_write) {
    IPAddress remoteIP = udpServer.remoteIP();
    uint16_t remotePort = udpServer.remotePort();
    TEST_ASSERT_TRUE(udpServer.beginPacket(remoteIP, remotePort)); 

    const char* message = "Hello, UDP Client!";
    size_t bytes_written = udpServer.write(message[0]); 
    TEST_ASSERT_EQUAL_INT(1, bytes_written); 

    bytes_written = udpServer.write((const uint8_t *)message + 1, strlen(message) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(message) - 1, bytes_written); 

    TEST_ASSERT_TRUE(udpServer.endPacket()); 
}

OTTUNITY_TEST(handle_packet_loss) {
    // Simulate packet loss by not sending a packet
    TEST_ASSERT_EQUAL_INT(0, udpServer.parsePacket()); 
}

OTTUNITY_TEST(joinmulticast_receive) {
    IPAddress multicastIP(239, 0, 0, 1);
    uint16_t port = 1234;

    udpServer.stop(); // Stop any previous instance
    TEST_ASSERT_TRUE(udpServer.beginMulticast(multicastIP, port));

    const char* expected_msg = "Hello, Multicast!";
    char rcvd_msg[50] = {0};

    int packetSize = 0;
    while ((packetSize = udpServer.parsePacket()) != (int)strlen(expected_msg)) {}
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg), packetSize);
    udpServer.read((uint8_t *)rcvd_msg, packetSize);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);
}

OTTUNITY_TEST(udp_server_end) {
    udpServer.stop();
}

OTTUNITY_TEST(wifi_disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

OTTUNITY_TEST(wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

OTTUNITY_RUN();