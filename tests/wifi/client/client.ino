/**
 * @brief This test creates a WiFi (TCP) client and connects to a WiFi server 
 * and communicates with testing the WiFiClient API functions.
 * 
 * @details The tests runs the following sequence:
 * - Connect to the access point created by the test_wifi_ap.cpp test
 * - Check the local IP address
 * - Connect to the server
 * - Write to the server
 * - Read from the server
 * - Print to the server
 * - Connect a second client to the server
 * - Read from the server with both clients
 * - Peek and flush the second client
 * - Stop the first client
 * - Waits for the server to disconnect
 * - Disconnect the wifi connection and end the WiFi
 * 
 * This test is meant as a happy path verification. Exception and corner/edge cases
 * are not covered in this test. 
 * 
 * This test is paired in the "server.ino" test, which needs to be executed in 
 * a second board to provide the server to which the client connects to.
 * 
 * @note This test must be run after the "server.ino" test.
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include "ottu.hpp"

OTTUNITY_TEST(wifi_connect_to_ap) {
   /* This AP is created by the test_wifi_ap.
    Currently the test tools does provide NOT a way to and synch multitest. 
    It has only be validated manually. */
    int result = WL_IDLE_STATUS;
    const uint32_t start_ms = millis();
    /* The server board may still be starting its AP. */
    do {
        result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
        if (result != WL_CONNECTED) {
            delay(100);
        }
    } while (result != WL_CONNECTED && (millis() - start_ms) < 10000);
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

OTTUNITY_TEST(check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(2, ip[3]);
}

WiFiClient client;

OTTUNITY_TEST(client_connect) {
    /** 
     * TODO: This will be replaced for gateway address.
     */
    IPAddress ip(192, 168, 0, 1);
    uint16_t port = 80;
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_UNINITED, client.status());
    TEST_ASSERT_TRUE(client.connect(ip, port));
    TEST_ASSERT_TRUE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
    TEST_ASSERT_TRUE(ip == client.remoteIP());
    TEST_ASSERT_EQUAL_UINT16(port, client.remotePort());
}

OTTUNITY_TEST(client_write) {
    const char msg[] = "hello server!";

    size_t written_bytes = client.write(msg[0]);
    TEST_ASSERT_EQUAL_INT(1, written_bytes);
    
    written_bytes = client.write((const uint8_t *)&msg[1], strlen(msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(msg) - 1, written_bytes);
}

OTTUNITY_TEST(client_read) {
    char expected_msg[] = "yo yo client!";
    char rcvd_msg[16] = {0};

    /* Wait for the client to have available data */
    while(client.available() < (int)strlen(expected_msg)) { }

    rcvd_msg[0] = client.read();
    TEST_ASSERT_EQUAL_CHAR(expected_msg[0],rcvd_msg[0]);

    int read_bytes = client.read((uint8_t *)&rcvd_msg[1], strlen(expected_msg) - 1);
    
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg) - 1, read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);

    /* Check the client is still connected */
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status()); 
}

OTTUNITY_TEST(client_print) {
    const char prompt[] = ">> ";
    const char msg[] = "print from client!";

    size_t written_bytes = client.print(prompt);
    TEST_ASSERT_EQUAL_INT(strlen(prompt), written_bytes);

    written_bytes = client.println(msg);
    TEST_ASSERT_EQUAL_INT(strlen(msg) + strlen("\r\n"), written_bytes);

    /* Check the client is still connected */
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
}

WiFiClient client2;

OTTUNITY_TEST(client2_connect) {
    /** 
     * TODO: This will be replaced for gateway address.
     */
    IPAddress ip(192, 168, 0, 1);
    TEST_ASSERT_FALSE(client2.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_UNINITED, client2.status());
    TEST_ASSERT_TRUE(client2.connect(ip, 80));
    TEST_ASSERT_TRUE(client2.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client2.status());
}

OTTUNITY_TEST(client_read_all_clients) {
    const char expected_msg[] = "konnichiwa all clients!\r\n";
    char rcvd_msg[32] = {0};

    /* Wait for the client to have available data */
    while(client.available() < (int)strlen(expected_msg)) { }

    int read_bytes = client.read((uint8_t *)rcvd_msg, strlen(expected_msg));
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg), read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);

    memset(rcvd_msg, 0, sizeof(rcvd_msg));

    /* Wait for the client to have available data */
    while(client2.available() < (int)strlen(expected_msg)) { }
    
    /* We leave 4 bytes left unread for the next tests */
    const char expected_msg2[] = "konnichiwa all client";
    read_bytes = client2.read((uint8_t *)rcvd_msg, strlen(expected_msg2));
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg2), read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg2, rcvd_msg);
}

OTTUNITY_TEST(client_peek_flush) {
    /* 4 bytes should be left unread from the previous reception */
    TEST_ASSERT_EQUAL_INT(4, client2.available());
    TEST_ASSERT_EQUAL_CHAR('s', client2.peek());   
    TEST_ASSERT_EQUAL_INT(4, client2.available());   
    client2.flush();
    TEST_ASSERT_EQUAL_INT(0, client2.available());
}

OTTUNITY_TEST(client_stop) {
    client.stop();
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, client.status());
}

OTTUNITY_TEST(client_server_disconnect) {

    TEST_ASSERT_TRUE(client2.connected());
    /**  
     * Wait for the server to disconnect.
     * The disconnect callback will stop the client
     */
    while(client2.status() != SOCKET_STATUS_DELETED) { }
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, client2.status());
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