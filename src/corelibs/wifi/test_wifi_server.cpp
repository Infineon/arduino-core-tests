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
    TEST_ASSERT_EQUAL_INT(WL_AP_CONNECTED, result);
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
}

WiFiClient client;

TEST_IFX(wifi_server, server_available) {
    /* The boolean operator of a client object
    is true when the client is connected and it has 
    data available to be read.*/
    while(!client) {
        /* The available returns a client which is connected
        and has available data*/
        client = server.available();
    }
    TEST_ASSERT_TRUE(client);
}

TEST_IFX(wifi_server, server_client_read) {
    char expected_msg[] = "hello server!";
    char rcvd_msg[16] = {0};

    rcvd_msg[0] = client.read();
    TEST_ASSERT_EQUAL_CHAR('h',rcvd_msg[0]);

    int read_bytes = client.read((uint8_t *)&rcvd_msg[1], strlen(expected_msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg) - 1, read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);
}

TEST_IFX(wifi_server, server_client_write) {
    const char msg[] = "yo yo client!";

    size_t written_bytes = client.write(msg[0]);
    TEST_ASSERT_EQUAL_INT(1, written_bytes);
    
    written_bytes = client.write((const uint8_t *)&msg[1], strlen(msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(msg) - 1, written_bytes);
}

TEST_IFX(wifi_server, server_end) {
    server.end();
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
    /* We cannot yet end, otherwise the other end will
    not be able to complete the transaction.*/
    while(true) {};
    RUN_TEST_CASE(wifi_server, server_end);
    RUN_TEST_CASE(wifi_server, wifi_end);
    /* Wait forever for now. */
    while(true) {};

}