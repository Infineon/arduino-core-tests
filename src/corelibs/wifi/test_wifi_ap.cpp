#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_ap);

static TEST_SETUP(wifi_ap) {
}

static TEST_TEAR_DOWN(wifi_ap) {
    WiFi.end();
}

TEST(wifi_ap, begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_CONNECTED, result);

    /* Wait forever for now. */
    while(true) {};
}

TEST_GROUP_RUNNER(wifi_ap) {
    RUN_TEST_CASE(wifi_ap, begin_ap);
}


