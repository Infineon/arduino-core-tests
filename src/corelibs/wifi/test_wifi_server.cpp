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

WiFiServer server;

TEST_IFX(wifi_server, server_begin) {
    server.begin(80);   
}

TEST_IFX(wifi_server, server_end) {
    server.end();
}

TEST_IFX(wifi_server, wifi_end) {
    WiFi.end();
}

TEST_GROUP_RUNNER(wifi_server) {
    RUN_TEST_CASE(wifi_server, wifi_begin_ap);
    RUN_TEST_CASE(wifi_server, server_begin);
    /* Wait forever for now. */
    /* This allows to check the sta
    test manually. */
    while(true) {};
    RUN_TEST_CASE(wifi_server, server_end);
    RUN_TEST_CASE(wifi_server, wifi_end);

}