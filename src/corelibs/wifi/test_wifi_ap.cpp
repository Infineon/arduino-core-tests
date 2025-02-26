#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_ap);

static TEST_SETUP(wifi_ap) {
}

static TEST_TEAR_DOWN(wifi_ap) {
}

TEST_IFX(wifi_ap, begin_ap) {
    int result = WiFi.beginAP("arduino-wifi-ap", "wifi-ap-password", 1);
    TEST_ASSERT_EQUAL_INT(WL_AP_CONNECTED, result);
}

TEST_IFX(wifi_ap, is_status_connected) {
    TEST_ASSERT_EQUAL_INT(WL_AP_CONNECTED, WiFi.status());
}

TEST_IFX(wifi_ap, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(1, ip[3]);
}

TEST_IFX(wifi_ap, check_gateway_ip) {
    IPAddress ip = WiFi.gatewayIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(1, ip[3]);
}

TEST_IFX(wifi_ap, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_ap) {
    RUN_TEST_CASE(wifi_ap, begin_ap);
    RUN_TEST_CASE(wifi_ap, is_status_connected);
    RUN_TEST_CASE(wifi_ap, check_local_ip);
    RUN_TEST_CASE(wifi_ap, check_gateway_ip);
    /* Wait forever for now. */
    /* This allows to check the sta
    test manually. */
    while(true) {};
    RUN_TEST_CASE(wifi_ap, wifi_end);
}


