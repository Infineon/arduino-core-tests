#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiServer.h>

TEST_GROUP(wifi_server);

static TEST_SETUP(wifi_server) {
}

static TEST_TEAR_DOWN(wifi_server) {
}


TEST_IFX(wifi_server, wifi_begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_LISTENING, result);
}

TEST_IFX(wifi_server, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(1, ip[3]);
    TEST_ASSERT_EQUAL_STRING("192.168.0.1", ip.toString().c_str());
}

WiFiServer server;

TEST_IFX(wifi_server, server_begin) {
    server.begin(80);   
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_LISTENING, server.status());
}

WiFiClient client;

TEST_IFX(wifi_server, server_available) {
    /* The boolean operator of a client object
    is true when the client is connected and it has 
    data available to be read.*/
    do {
        /* The available returns a client which is connected
        and has available data*/
         client = server.available();
    } while(!client);

    TEST_ASSERT_TRUE(client);
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
    TEST_ASSERT_TRUE(client.remoteIP() == IPAddress(192, 168, 0, 2));
    TEST_ASSERT_EQUAL_UINT16(80, client.remotePort());
}

TEST_IFX(wifi_server, server_client_read) {
    const char expected_msg[] = "hello server!";
    char rcvd_msg[16] = {0};

    /* Wait for the client to have available data */
    while(client.available() < (int)strlen(expected_msg)) { }

    rcvd_msg[0] = client.read();
    TEST_ASSERT_EQUAL_CHAR('h',rcvd_msg[0]);

    int read_bytes = client.read((uint8_t *)&rcvd_msg[1], strlen(expected_msg));
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg) - 1, read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);

    TEST_ASSERT_EQUAL_UINT8(0, client.available());
    /* Check the client is still connected */
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
}

TEST_IFX(wifi_server, server_client_write) {
    const char msg[] = "yo yo client!";

    size_t written_bytes = client.write(msg[0]);
    TEST_ASSERT_EQUAL_INT(1, written_bytes);
    
    written_bytes = client.write((const uint8_t *)&msg[1], strlen(msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(msg) - 1, written_bytes);

    /* Check the client is still connected */
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
}

TEST_IFX(wifi_server, server_client_read_print) {
    const char expected_prompt[] = ">> "; 
    const char expected_msg[] = "print from client!\r\n";
    char rcvd_msg[32] = {0};

    while(client.available() < (int)(strlen(expected_prompt) + strlen(expected_msg))) { }

    int read_bytes = client.read((uint8_t *)rcvd_msg, strlen(expected_prompt));
    TEST_ASSERT_EQUAL_INT(strlen(expected_prompt), read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_prompt, rcvd_msg);

    memset(rcvd_msg, 0, sizeof(rcvd_msg));
    
    read_bytes = client.read((uint8_t *)rcvd_msg, strlen(expected_msg));
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg), read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);

    /* Check the client is still connected */
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_CONNECTED, client.status());
}

TEST_IFX(wifi_server, server_print_to_all_clients) {
    const char msg_a[] = "konnichiwa ";
    const char msg_b[] = "all clients!";

    /* Wait for the server to have more than 1 client connected */
    while(server.connectedSize() < 2) {}

    size_t written_bytes = server.print(msg_a);
    TEST_ASSERT_EQUAL_INT(strlen(msg_a), written_bytes);
    
    written_bytes = server.println(msg_b);
    TEST_ASSERT_EQUAL_INT(strlen(msg_b) + strlen("\r\n"), written_bytes);
}

TEST_IFX(wifi_server, server_client_stop) {
    /* Wait until the first client is disconnected
    from the other side */
    while(server.connectedSize() > 1) { }

    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, client.status());
}

TEST_IFX(wifi_server, server_end) {
    server.end();
    TEST_ASSERT_EQUAL_UINT8(SOCKET_STATUS_DELETED, server.status());
}

TEST_IFX(wifi_server, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_server) {
    RUN_TEST_CASE(wifi_server, wifi_begin_ap);
    RUN_TEST_CASE(wifi_server, check_local_ip);
    RUN_TEST_CASE(wifi_server, server_begin);
    RUN_TEST_CASE(wifi_server, server_available);
    RUN_TEST_CASE(wifi_server, server_client_read);
    RUN_TEST_CASE(wifi_server, server_client_write);
    RUN_TEST_CASE(wifi_server, server_client_read_print);
    RUN_TEST_CASE(wifi_server, server_print_to_all_clients);
    RUN_TEST_CASE(wifi_server, server_client_stop);
    RUN_TEST_CASE(wifi_server, server_end);
    RUN_TEST_CASE(wifi_server, wifi_end);
    /** 
     * TODO: Remove when this is added to cicd 
     * and more tests are added to the suite together with the 
     * WiFi ones.
     * For now it is more controllable like this to run tests 
     * manually. 
     */
    while(true) {};
}