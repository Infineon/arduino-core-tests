#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiClient.h>

TEST_GROUP(wifi_client);

static TEST_SETUP(wifi_client) {
}

static TEST_TEAR_DOWN(wifi_client) {
}

TEST_IFX(wifi_client, wifi_connect_to_ap) {
   /* This AP is created by the test_wifi_ap.
    Currently the test tools does provide a way to and synch multitest. 
    It has only be validated manually. */
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_IFX(wifi_client, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(2, ip[3]);
}

WiFiClient client;

TEST_IFX(wifi_client, client_connect) {
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

TEST_IFX(wifi_client, client_write) {
    const char msg[] = "hello server!";

    size_t written_bytes = client.write(msg[0]);
    TEST_ASSERT_EQUAL_INT(1, written_bytes);
    
    written_bytes = client.write((const uint8_t *)&msg[1], strlen(msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(msg) - 1, written_bytes);
}

TEST_IFX(wifi_client, client_read) {
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

TEST_IFX(wifi_client, client_print) {
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

TEST_IFX(wifi_client, client2_connect) {
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

TEST_IFX(wifi_client, client_read_all_clients) {
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

TEST_IFX(wifi_client, client_peek_flush) {
    /* 4 bytes should be left unread from the previous reception */
    TEST_ASSERT_EQUAL_INT(4, client2.available());
    TEST_ASSERT_EQUAL_CHAR('s', client2.peek());   
    TEST_ASSERT_EQUAL_INT(4, client2.available());   
    client2.flush();
    TEST_ASSERT_EQUAL_INT(0, client2.available());
}

TEST_IFX(wifi_client, client_stop) {
    client.stop();
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, client.status());
}

TEST_IFX(wifi_client, client_server_disconnect) {

    TEST_ASSERT_TRUE(client2.connected());
    /**  
     * Wait for the server to disconnect.
     * The disconnect callback will stop the client
     */
    while(client2.status() != SOCKET_STATUS_DELETED) { }
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, client2.status());
}

TEST_IFX(wifi_client, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_client) {
    RUN_TEST_CASE(wifi_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_client, check_local_ip);
    RUN_TEST_CASE(wifi_client, client_connect);
    RUN_TEST_CASE(wifi_client, client_write);
    RUN_TEST_CASE(wifi_client, client_read);
    RUN_TEST_CASE(wifi_client, client_print);
    RUN_TEST_CASE(wifi_client, client2_connect);
    RUN_TEST_CASE(wifi_client, client_read_all_clients);
    RUN_TEST_CASE(wifi_client, client_peek_flush);
    RUN_TEST_CASE(wifi_client, client_stop);
    RUN_TEST_CASE(wifi_client, client_server_disconnect);
    RUN_TEST_CASE(wifi_client, wifi_end);
    /** 
     * TODO: Remove when this is added to cicd 
     * and more tests are added to the suite together with the 
     * WiFi ones.
     * For now it is more controllable like this to run tests 
     * manually. 
     */
    while(true) {};
}