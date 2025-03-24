#include "test_common_includes.h"

#include <WiFi.h>

TEST_GROUP(wifi_sta);

static TEST_SETUP(wifi_sta) {
}

static TEST_TEAR_DOWN(wifi_sta) {
}

TEST_IFX(wifi_sta, connect_to_ap) {
    /* This AP is created by the test_wifi_ap.
       Currently the test tools does provide a way to and synch multitest. 
       It has only be validated manually. */
    int result = WiFi.begin("arduino-wifi-ap", "wifi-ap-password");
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, result);
}

TEST_IFX(wifi_sta, is_status_connected) {
    TEST_ASSERT_EQUAL_INT(WL_CONNECTED, WiFi.status());
}

TEST_IFX(wifi_sta, check_local_ip) {
    IPAddress ip = WiFi.localIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(2, ip[3]);
}

TEST_IFX(wifi_sta, check_gateway_ip) {
    IPAddress ip = WiFi.gatewayIP();
    TEST_ASSERT_EQUAL_INT(192, ip[0]);
    TEST_ASSERT_EQUAL_INT(168, ip[1]);
    TEST_ASSERT_EQUAL_INT(0, ip[2]);
    TEST_ASSERT_EQUAL_INT(1, ip[3]);
}

TEST_IFX(wifi_sta, check_ssid) {
    const char *ssid = WiFi.SSID();
    TEST_ASSERT_EQUAL_STRING("arduino-wifi-ap", ssid);
}

TEST_IFX(wifi_sta, check_rssi) {
    int32_t rssi = WiFi.RSSI();
    TEST_ASSERT_TRUE(rssi > INT32_MIN);
    /* Assuming the boards are a few cm separate from each other. */
    TEST_ASSERT_TRUE(-50 < rssi && rssi < 50);
}

TEST_IFX(wifi_sta, disconnect) {
    WiFi.disconnect();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_STA_DISCONNECTED, WiFi.status());
}

TEST_IFX(wifi_sta, wifi_end) {
    WiFi.end();
    TEST_ASSERT_EQUAL_INT(WIFI_STATUS_UNINITED, WiFi.status());
}

TEST_GROUP_RUNNER(wifi_sta) {
    RUN_TEST_CASE(wifi_sta, connect_to_ap);
    RUN_TEST_CASE(wifi_sta, is_status_connected);
    RUN_TEST_CASE(wifi_sta, check_local_ip); 
    RUN_TEST_CASE(wifi_sta, check_gateway_ip);
    RUN_TEST_CASE(wifi_sta, check_ssid);
    RUN_TEST_CASE(wifi_sta, check_rssi);
    RUN_TEST_CASE(wifi_sta, disconnect);
    RUN_TEST_CASE(wifi_sta, wifi_end);  
    while(true) {};
}


