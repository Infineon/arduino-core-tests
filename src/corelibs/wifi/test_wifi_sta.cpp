#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_sta);

static TEST_SETUP(wifi_sta) {
}

static TEST_TEAR_DOWN(wifi_sta) {
    WiFi.end();
}

TEST(wifi_sta, connect_to_ap) {
    /* This AP is created by the test_wifi_ap.
       Currently the test tools does provide a way to and synch multitest. 
       It has only be validated manually. */
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_GROUP_RUNNER(wifi_sta) {
    RUN_TEST_CASE(wifi_sta, connect_to_ap);
}


