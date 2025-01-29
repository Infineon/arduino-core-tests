#include "test_common_includes.h"

#include <WiFi.h>
#include <WiFiServer.h>

TEST_GROUP(wifi_server);

static TEST_SETUP(wifi_server) {
}

static TEST_TEAR_DOWN(wifi_server) {
}

TEST_IFX(wifi_server, begin_ap) {
}

TEST_IFX(wifi_server, is_status_connected) {
}

TEST_IFX(wifi_server, check_local_ip) {
}

TEST_IFX(wifi_server, wifi_end) {
}

TEST_GROUP_RUNNER(wifi_server) {
    RUN_TEST_CASE(wifi_server, begin_ap);
    RUN_TEST_CASE(wifi_server, is_status_connected);
    RUN_TEST_CASE(wifi_server, check_local_ip);
    RUN_TEST_CASE(wifi_server, wifi_end);
}