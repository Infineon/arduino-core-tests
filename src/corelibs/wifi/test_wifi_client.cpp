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

TEST_IFX(wifi_client, client_begin) {
}

TEST_IFX(wifi_client, client_connect) {
}

TEST_IFX(wifi_client, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_client) {
    RUN_TEST_CASE(wifi_client, wifi_connect_to_ap);
    RUN_TEST_CASE(wifi_client, client_begin);
    RUN_TEST_CASE(wifi_client, client_connect);
    RUN_TEST_CASE(wifi_client, wifi_end);
}