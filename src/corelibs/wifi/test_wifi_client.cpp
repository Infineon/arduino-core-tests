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
    /* This will be replaced for gateway address.*/
    IPAddress ip(192, 168, 0, 1);
    TEST_ASSERT_FALSE(client.connected());
    TEST_ASSERT_TRUE(client.connect(ip, 80));
    TEST_ASSERT_TRUE(client.connected());
}

TEST_IFX(wifi_client, client_write) {
    const char msg[] = "hello server!";

    size_t written_bytes = client.write(msg[0]);
    TEST_ASSERT_EQUAL_INT(1, written_bytes);
    
    written_bytes = client.write((const uint8_t *)&msg[1], strlen(msg) - 1);
    TEST_ASSERT_EQUAL_INT(strlen(msg) - 1, written_bytes);
}

TEST_IFX(wifi_client, client_available) {
    /* Wait for the client to have available data */
    while(!client.available()) { }
    /* Assertion not required. 
    Pass upon when blocking wait ends.*/
}

TEST_IFX(wifi_client, client_read) {
    char expected_msg[] = "yo yo client!";
    char rcvd_msg[16] = {0};

    rcvd_msg[0] = client.read();
    TEST_ASSERT_EQUAL_CHAR(expected_msg[0],rcvd_msg[0]);

    int read_bytes = client.read((uint8_t *)&rcvd_msg[1], strlen(expected_msg) - 1);
    
    TEST_ASSERT_EQUAL_INT(strlen(expected_msg) - 1, read_bytes);
    TEST_ASSERT_EQUAL_STRING(expected_msg, rcvd_msg);
}

TEST_IFX(wifi_client, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_client) {
    RUN_TEST_CASE(wifi_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_client, check_local_ip);
    RUN_TEST_CASE(wifi_client, client_connect);
    RUN_TEST_CASE(wifi_client, client_write);
    RUN_TEST_CASE(wifi_client, client_available);
    RUN_TEST_CASE(wifi_client, client_read);
    /* We cannot yet end, otherwise the other end will
    not be able to complete the transaction.*/
    while(true) {};
    RUN_TEST_CASE(wifi_client, wifi_end);
}