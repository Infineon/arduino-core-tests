/**
 * @file test_wifi_udp_client.cpp
 * @brief This test creates a WiFi (UDP) client and connects to a WiFi server
 * and communicates with testing the WiFiUDP API functions.
 * 
 * @details The tests runs the following sequence:
 * - Connect to the access point created by the test_wifi_ap.cpp test
 * - Starts WiFiUDP socket, listening at local port PORT.
 * - Starts a connection to write UDP data to the remote connection
 * - Writes UDP data to the remote connection
 * - finishes off the packet and send it.
 * - Write data packet and send it to the server, change the local IP and send another packet.
 * - Receives a packet from the server and checks if the data is correct.
 * - Peek and flush the socket.
 * - Check the senders ip 
 * - Check the senders port
 * - Disconnect from the server and stop the UDP client.
 * 
 *  This test is paired with the "test_wifi_udp_server.cpp" test, which needs to be executed in another 
 *  board to provide the server to which the client connects to.
 * 
 * @note This test must be run after the "test_wifi_udp_server.cpp" test.
 */

#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiUdp.h>

TEST_GROUP(wifi_udp_client);

static TEST_SETUP(wifi_udp_client) {
    
}

static TEST_TEAR_DOWN(wifi_udp_client) {
    
}

WiFiUDP udpClient;

TEST_IFX(wifi_udp_client, wifi_connect_to_ap) {
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_IFX(wifi_udp_client, udp_begin) {
    uint16_t port = 1234;
    TEST_ASSERT_EQUAL_INT(SOCKET_STATUS_BOUND, udpClient.begin(port)); 
}

TEST_IFX(wifi_udp_client, udp_beginpacket) {
    IPAddress ip(192, 168, 0, 1);
    uint16_t port = 1234;
    TEST_ASSERT_TRUE(udpClient.beginPacket(ip, port));
}

TEST_IFX(wifi_udp_client, udp_write){
    const char *message = "Hello, UDP Server!";
    size_t bytes_written = udpClient.write(message[0]); 
    TEST_ASSERT_EQUAL_INT(1, bytes_written); 

    bytes_written = udpClient.write((const uint8_t *)message+1, strlen(message)-1);
    TEST_ASSERT_EQUAL_INT(strlen(message)-1, bytes_written); 
}

TEST_IFX(wifi_udp_client, udp_endpacket) {
    TEST_ASSERT_TRUE(udpClient.endPacket());
    delay(100); // Wait for the packet to be sent
}

TEST_IFX(wifi_udp_client, write_multiple_packets) {
    const char *message1 = "Hello, UDP Server!";
    const char *message2 = "Hey, Guess Who am I!";
    IPAddress ip(192, 168, 0, 1);
    uint16_t port = 1234;

    TEST_ASSERT_TRUE(udpClient.beginPacket(ip, port)); 
    udpClient.write(message1);
    TEST_ASSERT_TRUE(udpClient.endPacket()); 

    // Reconfigure the WiFi client to send another packet
    WiFi.disconnect();
    WiFi.end();
    IPAddress local_ip(192, 168, 0, 5);
    WiFi.config(local_ip);
    WiFi.begin("arduino-wifi-ap", "wifi-ap-password");

    TEST_ASSERT_TRUE(udpClient.beginPacket(ip, port)); 
    udpClient.write(message2);
    TEST_ASSERT_TRUE(udpClient.endPacket()); 
}

TEST_IFX(wifi_udp_client, udp_client_receive) {
    const char* expectedMessage = "Hello, UDP Client!";
    char rcvd_msg[19] = {0};

    // Wait for the client to receive a packet
    int packetSize = 0;
    while ((packetSize = udpClient.parsePacket()) != (int)strlen(expectedMessage)) {
    }
    TEST_ASSERT_EQUAL_INT(strlen(expectedMessage), packetSize); 

   /* We leave 7 bytes left unread for the next tests */
   const char* expectedMessage2 = "Hello, UDP ";
   int read_bytes = udpClient.read((uint8_t *)rcvd_msg, strlen(expectedMessage2));
   TEST_ASSERT_EQUAL_INT(strlen(expectedMessage2), read_bytes); 
   TEST_ASSERT_EQUAL_STRING(expectedMessage2, rcvd_msg); 
}

TEST_IFX(wifi_udp_client , client_peek_flush) {
    /* 7 bytes should be left unread from the previous reception */
    TEST_ASSERT_EQUAL_INT(7, udpClient.available());
    TEST_ASSERT_EQUAL_CHAR('C', udpClient.peek());   
    TEST_ASSERT_EQUAL_INT(7, udpClient.available());   
    udpClient.flush();
    TEST_ASSERT_EQUAL_INT(0, udpClient.available());
}

TEST_IFX(wifi_udp_client, remote_ip)
{
    IPAddress expectedIP(192, 168, 0, 1);
    IPAddress remoteIP = udpClient.remoteIP();
    TEST_ASSERT_EQUAL(expectedIP, remoteIP); 
}

TEST_IFX(wifi_udp_client, remote_port)
{
    uint16_t expectedPort = 1234;
    uint16_t remotePort = udpClient.remotePort();
    TEST_ASSERT_EQUAL(expectedPort, remotePort); 
}

TEST_IFX(wifi_udp_client, udp_client_end) {
    udpClient.stop(); 
}

TEST_IFX(wifi_udp_client, wifi_disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

TEST_IFX(wifi_udp_client, wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_udp_client) {
    RUN_TEST_CASE(wifi_udp_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_udp_client, udp_begin);
    RUN_TEST_CASE(wifi_udp_client, udp_beginpacket);
    RUN_TEST_CASE(wifi_udp_client, udp_write);
    RUN_TEST_CASE(wifi_udp_client, udp_endpacket);
    RUN_TEST_CASE(wifi_udp_client, write_multiple_packets);
    RUN_TEST_CASE(wifi_udp_client, udp_client_receive);
    RUN_TEST_CASE(wifi_udp_client, client_peek_flush);
    RUN_TEST_CASE(wifi_udp_client, remote_ip);
    RUN_TEST_CASE(wifi_udp_client, remote_port);
    RUN_TEST_CASE(wifi_udp_client, udp_client_end);
    RUN_TEST_CASE(wifi_udp_client, wifi_disconnect);
    RUN_TEST_CASE(wifi_udp_client, wifi_end);
    while (true) {}; // Keep the client running
}