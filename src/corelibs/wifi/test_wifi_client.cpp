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
    TEST_ASSERT_TRUE(client.connect(ip, 80));
}


TEST_IFX(wifi_client, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_client) {
    RUN_TEST_CASE(wifi_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_client, check_local_ip);
    RUN_TEST_CASE(wifi_client, client_connect);
    RUN_TEST_CASE(wifi_client, wifi_end);
}